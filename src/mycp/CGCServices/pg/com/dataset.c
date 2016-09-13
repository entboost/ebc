/*
 * $Id: dataset.c,v 1.36 2003/10/30 08:46:42 jasta Exp $
 *
 * Copyright (C) 2001-2003 giFT project (gift.sourceforge.net)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 */

#ifdef WIN32
#include "config_win32.h"
#else
#include "config_unix.h"
#endif

#include "comtypes.h"
#include "parse.h"

#include "dataset.h"

/*****************************************************************************/

void *dateset_memdup (const void *ptr, size_t len)
{
	void *dup;
	if (len == 0)
		return NULL;

	if (!(dup = malloc (len)))
		return NULL;

	memcpy (dup, ptr, len);
	return dup;
}




/**
 * Dataset structure.
 */
struct dataset
{
	DatasetType type;                  /**< Backend implementation */

	union
	{
		struct hash_table *hash;
		Array *array;
		List *list;
	} tdata;
} /* Dataset */;

/* real dataset_foreach_ex prototype, see ::dataset_foreach_ex for more
 * information */
typedef int (*ForeachExFn) (ds_data_t *key, ds_data_t *value, void *udata,
                            Dataset *d, DatasetNode *node);

#define DS_RETURN(ret) ((ret) & 0x03)

/*****************************************************************************/

typedef unsigned long (*DatasetHashFunc) (Dataset *d, ds_data_t *key);
typedef int (*HashCmpFunc) (Dataset *d, DatasetNode *node, ds_data_t *key);

/* TODO: we need to add a separate hash_table.c for this */
struct hash_table
{
	/**
	 * @name readonly
	 * Read-only public variables
	 */
	size_t        size;                /**< Total no buckets in nodes */
	unsigned long items;               /**< Total no items currently inserted */

	/**
	 * @name private
	 * Private variables
	 */
	unsigned char frozen;              /**< Do not resize hash table */

	DatasetNode **nodes;               /**< Bucket array */

	DatasetHashFunc    hash;                  /**< Hashing routine */
	HashCmpFunc cmp;                   /**< Hash/value comparison */
};

/*****************************************************************************/

static const unsigned int primes[] =
{
	11,       19,       37,       73,       109,      163,
	251,      367,      557,      823,      1237,     1861,
	2777,     4177,     6247,     9371,     14057,    21089,
	31627,    47431,    71143,    106721,   160073,   240101,
	360163,   540217,   810343,   1215497,  1823231,  2734867,
	4102283,  6153409,  9230113,  13845163,
};

static size_t nprimes = sizeof (primes) / sizeof (primes[0]);

#define HASH_TABLE_MIN_SIZE 11
#define HASH_TABLE_MAX_SIZE 13845163

/*****************************************************************************/

void ds_data_init (ds_data_t *dsdata, const void *data, size_t len, int flags)
{
	assert (dsdata != NULL);

	dsdata->data  = (void *)data;
	dsdata->len   = len;
	dsdata->flags = (uint8_t)flags;
}

static void ds_data_free (ds_data_t *data)
{
	assert (data != NULL);

	/* free the copy if we made one */
	if (!(data->flags & DS_NOCOPY))
		free (data->data);

	free (data);
}

static void free_node (DatasetNode *node)
{
	assert (node != NULL);

	ds_data_free (node->key);
	ds_data_free (node->value);

	free (node);
}

static ds_data_t *ds_data_dup (ds_data_t *data)
{
	ds_data_t *dup;

	assert (data != NULL);

	if (!(dup = malloc (sizeof (ds_data_t))))
		return NULL;

	if (data->flags & DS_NOCOPY)
		dup->data = data->data;
	else
	{
		/* the caller MUST set DS_NOCOPY if they do not describe the size
		 * of the data element */
		assert (data->len > 0);

		if (!(dup->data = dateset_memdup (data->data, data->len)))
		{
			free (dup);
			return NULL;
		}
	}

	dup->len   = data->len;
	dup->flags = data->flags;

	return dup;
}

static DatasetNode *new_node (Dataset *d, ds_data_t *key, ds_data_t *data)
{
	DatasetNode *node;

	if (!(node = malloc (sizeof (DatasetNode))))
		return NULL;

	/* the ds_data_t objects provided by the caller are not required to
	 * stay around, so we must make our own local copies here */
	if (!(node->key = ds_data_dup (key)) ||
		!(node->value = ds_data_dup (data)))
	{
		if (node->key)
		{
			ds_data_free (node->key);
			assert (node->value == NULL);
		}

		free (node);

		return NULL;
	}

	/* this is just easier than using a switch to zero each individual
	 * union member... */
	memset (&node->tdata, 0, sizeof (node->tdata));

	return node;
}

static int cmp_node (DatasetNode *node, ds_data_t *key)
{
	/* if the lengths dont match, there's no way the data matches... */
	if (node->key->len != key->len)
		return INTCMP (node->key->len, key->len);

	return memcmp (node->key->data, key->data, key->len);
}

/* TODO: opt using realloc or memcpy under appropriate conditions */
static ds_data_t *ds_data_reassign (ds_data_t *old_data, ds_data_t *new_data)
{
	ds_data_free (old_data);

	return ds_data_dup (new_data);
}

static void set_node (Dataset *d, DatasetNode *node,
                      ds_data_t *key, ds_data_t *data)
{
	/* if the keys are equal in size and data, do nothing */
	if (cmp_node (node, key))
		node->key = ds_data_reassign (node->key, key);

	node->value = ds_data_reassign (node->value, data);
}

/*****************************************************************************/

static unsigned long hash_int (Dataset *d, ds_data_t *key)
{
	unsigned long  hash = 0;
	unsigned char *data = key->data;
	size_t i;

	for (i = 0; i < key->len; i++)
		hash = (hash * 33) + *data++;

	hash = hash + (hash >> 5);

	return hash;
}

static int hash_cmp (Dataset *d, DatasetNode *node, ds_data_t *key)
{
	return cmp_node (node, key);
}

static unsigned int closest_prime (unsigned int num)
{
	unsigned int i;

	for (i = 0; i < nprimes; i++)
		if (primes[i] > num)
			return primes[i];

	return primes[i - 1];
}

static void d_hash_resize (Dataset *d)
{
	struct hash_table *table = d->tdata.hash;
	DatasetNode **new_nodes;
	DatasetNode *node;
	DatasetNode *next;
	float nodes_per_list;
	unsigned int hash_val;
	int new_size;
	unsigned int i;

	nodes_per_list = (float) table->items / (float) table->size;

	if ((nodes_per_list > 0.3 || table->size <= HASH_TABLE_MIN_SIZE) &&
		(nodes_per_list < 3.0 || table->size >= HASH_TABLE_MAX_SIZE))
		return;

	new_size = closest_prime (table->items);

	if (new_size < HASH_TABLE_MIN_SIZE)
		new_size = HASH_TABLE_MIN_SIZE;

	if (new_size > HASH_TABLE_MAX_SIZE)
		new_size = HASH_TABLE_MAX_SIZE;

	new_nodes = calloc (sizeof (DatasetNode *), new_size);

	for (i = 0; i < table->size; i++)
	{
		for (node = table->nodes[i]; node; node = next)
		{
			next = node->tdata.hash_next;

			hash_val = table->hash (d, node->key) % new_size;

			node->tdata.hash_next = new_nodes[hash_val];
			new_nodes[hash_val] = node;
		}
	}

	free (table->nodes);
	table->nodes = new_nodes;
	table->size  = new_size;
}

static void d_hash_new (Dataset *d)
{
	struct hash_table *table;
	size_t tsize;

	if (!(table = malloc (sizeof (struct hash_table))))
		return;

	table->size   = HASH_TABLE_MIN_SIZE;
	table->items  = 0;
	table->frozen = FALSE;

	tsize = (table->size * (sizeof (DatasetNode *)));

	if (!(table->nodes = malloc (tsize)))
	{
		free (table);
		return;
	}

	memset (table->nodes, 0, tsize);

	table->hash = hash_int;
	table->cmp  = hash_cmp;

	d->tdata.hash = table;
}

static void d_hash_free (Dataset *d)
{
	free (d->tdata.hash->nodes);
	free (d->tdata.hash);
}

/*****************************************************************************/

static void d_list_free (Dataset *d)
{
	list_free (d->tdata.list);
}

/*****************************************************************************/

static void d_array_free (Dataset *d)
{
	array_unset (&(d->tdata.array));
}

/*****************************************************************************/

Dataset *dataset_new (DatasetType type)
{
	Dataset *d;

	if (!(d = calloc (1, sizeof (Dataset))))
		return NULL;

	d->type = type;

	switch (d->type)
	{
	 case DATASET_HASH:      d_hash_new (d);               break;
	 default:                                              break;
	}

	return d;
}

static int clear (ds_data_t *key, ds_data_t *value, void *udata)
{
	return DS_CONTINUE | DS_REMOVE;
}

void dataset_clear (Dataset *d)
{
	if (!d)
		return;

	dataset_foreach_ex (d, DS_FOREACH_EX(clear), NULL);

	switch (d->type)
	{
	 case DATASET_HASH:
		d_hash_free (d);
		break;
	 case DATASET_LIST:
		d_list_free (d);
		break;
	 case DATASET_ARRAY:
		d_array_free (d);
		break;
	 default:
		abort ();
		break;
	}

	free (d);
}

/*****************************************************************************/

static DatasetNode *d_list_lookup_node (Dataset *d, ds_data_t *key)
{
	List *link;

	link = list_find_custom (d->tdata.list, key, (CompareFunc)cmp_node);

	return list_nth_data (link, 0);
}

static DatasetNode *d_array_lookup_node (Dataset *d, ds_data_t *key)
{
	Array       *a;
	DatasetNode *node;
	size_t       nmemb;
	size_t       i;

	if (!(a = d->tdata.array))
		return NULL;

	nmemb = array_count (&a);

	for (i = 0; i < nmemb; i++)
	{
		if (!(node = array_splice (&a, i, 0, NULL)))
			continue;

		if (cmp_node (node, key) == 0)
			return node;
	}

	return NULL;
}

static DatasetNode **d_hash_lookup_node (Dataset *d, ds_data_t *key)
{
	DatasetNode **node;
	unsigned long hash;

	hash = d->tdata.hash->hash (d, key);
	node = &d->tdata.hash->nodes [hash % d->tdata.hash->size];

	while (*node && d->tdata.hash->cmp (d, *node, key))
		node = & (*node)->tdata.hash_next;

#if 0
	/* if we're attempting to lookup data we don't want to find a useless
	 * node */
	if (lookup && !(*node))
		return NULL;
#endif

	return node;
}

DatasetNode *dataset_lookup_node_ex (Dataset *d, ds_data_t *key)
{
	DatasetNode *node = NULL;

	if (!d || !key)
		return NULL;

	switch (d->type)
	{
	 case DATASET_LIST:
		node = d_list_lookup_node (d, key);
		break;
	 case DATASET_ARRAY:
		node = d_array_lookup_node (d, key);
		break;
	 case DATASET_HASH:
		node = *d_hash_lookup_node (d, key);
		break;
	 default:
		abort ();
		break;
	}

	return node;
}

DatasetNode *dataset_lookup_node (Dataset *d, const void *key, size_t key_len)
{
	ds_data_t dskey;

	ds_data_init (&dskey, key, key_len, 0);

	return dataset_lookup_node_ex (d, &dskey);
}

/*****************************************************************************/

static void d_list_insert (Dataset *d, DatasetNode *node)
{
	d->tdata.list = list_prepend (d->tdata.list, node);
	node->tdata.list_link = d->tdata.list;
}

static void d_array_insert (Dataset *d, DatasetNode *node)
{
	void *ret;

	node->tdata.array_offs = array_count (&(d->tdata.array));

	ret = array_push (&(d->tdata.array), node);
	assert (ret != NULL);
}

static void d_hash_insert (Dataset *d, DatasetNode *node)
{
	DatasetNode **placement;

	placement = d_hash_lookup_node (d, node->key);
	*placement = node;

	d->tdata.hash->items++;

	if (!d->tdata.hash->frozen)
		d_hash_resize (d);
}

static DatasetNode *insert (Dataset *d, ds_data_t *key, ds_data_t *data)
{
	DatasetNode *node;

	/*
	 * We must first attempt to locate an existing node entry in the
	 * dataset that would collid with this insertion.  If we find something,
	 * we must override its entries rather than insert a new value.
	 */
	if ((node = dataset_lookup_node_ex (d, key)))
	{
		set_node (d, node, key, data);
		return node;
	}

	/* construct a new node for insertion */
	if (!(node = new_node (d, key, data)))
		return NULL;

	/* insert into the backend data structure */
	switch (d->type)
	{
	 case DATASET_LIST:      d_list_insert (d, node);      break;
	 case DATASET_ARRAY:     d_array_insert (d, node);     break;
	 case DATASET_HASH:      d_hash_insert (d, node);      break;
	 default:                abort ();                     break;
	}

	return node;
}

DatasetNode *dataset_insert_ex (Dataset **d, ds_data_t *key, ds_data_t *data)
{
	if (!d || !key || !data)
		return NULL;

	assert (key->len > 0);

	/* if the data pointed to is NULL we are being asked (as a special
	 * convenience) to construct the dataset using the default type, so handle
	 * that here */
	if (!(*d))
	{
		if (!(*d = dataset_new (DATASET_DEFAULT)))
			return NULL;
	}

	return insert (*d, key, data);
}

DatasetNode *dataset_insert (Dataset **d,
                             const void *key, size_t key_len,
                             const void *value, size_t value_len)
{
	ds_data_t dskey;
	ds_data_t dsdata;

	assert (key != NULL);
	assert (key_len > 0);

	ds_data_init (&dskey, key, key_len, 0);
	ds_data_init (&dsdata, value, value_len, (value_len == 0) ? DS_NOCOPY : 0);

	return dataset_insert_ex (d, &dskey, &dsdata);
}

DatasetNode *dataset_insertstr (Dataset **d, const char *key, const char *value)
{
	assert (key != NULL);
	assert (value != NULL);

	return dataset_insert (d,
	                       key, STRLEN_0(key),
	                       value, STRLEN_0(value));
}

/*****************************************************************************/

static void d_list_remove (Dataset *d, DatasetNode *node)
{
	if (!node->tdata.list_link)
		d->tdata.list = list_remove (d->tdata.list, node);
	else
	{
		d->tdata.list = list_remove_link (d->tdata.list, node->tdata.list_link);
		node->tdata.list_link = NULL;
	}
}

static void d_array_remove (Dataset *d, DatasetNode *node)
{
	array_splice (&(d->tdata.array), node->tdata.array_offs, 1, NULL);
}

static void d_hash_remove (Dataset *d, DatasetNode *node)
{
	DatasetNode *dest;
	DatasetNode **placement;

	/* this is very inefficient, fix later */
	placement = d_hash_lookup_node (d, node->key);

	dest = *placement;
	*placement = dest->tdata.hash_next;

	d->tdata.hash->items--;

	if (!d->tdata.hash->frozen)
		d_hash_resize (d);
}

void dataset_remove_node (Dataset *d, DatasetNode *node)
{
	if (!d || !node)
		return;

	switch (d->type)
	{
	 case DATASET_LIST:      d_list_remove (d, node);      break;
	 case DATASET_ARRAY:     d_array_remove (d, node);     break;
	 case DATASET_HASH:      d_hash_remove (d, node);      break;
	 default:                abort ();                     break;
	}

	free_node (node);
}

void dataset_remove_ex (Dataset *d, ds_data_t *key)
{
	DatasetNode *node;

	if (!(node = dataset_lookup_node_ex (d, key)))
		return;

	dataset_remove_node (d, node);
}

void dataset_remove (Dataset *d, const void *key, size_t key_len)
{
	ds_data_t dskey;

	ds_data_init (&dskey, key, key_len, 0);

	dataset_remove_ex (d, &dskey);
}

void dataset_removestr (Dataset *d, const char *key)
{
	dataset_remove (d, key, STRLEN_0 (key));
}

/*****************************************************************************/

ds_data_t *dataset_lookup_ex (Dataset *d, ds_data_t *key)
{
	DatasetNode *node;

	if (!(node = dataset_lookup_node_ex (d, key)))
		return NULL;

	return node->value;
}

void *dataset_lookup (Dataset *d, const void *key, size_t key_len)
{
	ds_data_t  dskey;
	ds_data_t *data;

	ds_data_init (&dskey, key, key_len, 0);

	if (!(data = dataset_lookup_ex (d, &dskey)))
		return NULL;

	return data->data;
}

void *dataset_lookupstr (Dataset *d, const char *key)
{
	return dataset_lookup (d, key, STRLEN_0(key));
}

/*****************************************************************************/

/*
 * NOTE: We cannot use list_foreach_remove here because of complications with
 * the DatasetForeachExFn return value requirements
 */
static void d_list_foreach_remove (Dataset *d, ForeachExFn func, void *udata)
{
	DatasetNode *node;
	List        *ptr;
	List        *next;
	int          ret;

	if (!d->tdata.list)
		return;

	for (ptr = d->tdata.list; ptr; )
	{
		next = ptr->next;

		node = ptr->data;
		assert (node != NULL);

		ret = (*func) (node->key, node->value, udata, d, node);

		if (ret & DS_REMOVE)
		{
			free_node (node);
			d->tdata.list = list_remove_link (d->tdata.list, ptr);
		}

		if (DS_RETURN(ret) == DS_BREAK)
			break;

		ptr = next;
	}
}

static void d_array_foreach_remove (Dataset *d, ForeachExFn func, void *udata)
{
	Array       *a;
	DatasetNode *node;
	size_t       i;
	int          ret;

	if (!(a = d->tdata.array))
		return;

	/* execute count after each iteration to ensure that removals do not cause
	 * us to go too far */
	for (i = 0; i < array_count (&a); i++)
	{
		/* access the element at this offset */
		if (!(node = array_splice (&a, i, 0, NULL)))
			continue;

		/* readjust the array offset if necessary, the code block below will
		 * explain more about why we're doing this */
		if (node->tdata.array_offs != i)
			node->tdata.array_offs = i;

		ret = func (node->key, node->value, udata, d, node);

		if (ret & DS_REMOVE)
		{
			/* destroy the current node element */
			free_node (node);

			/* remove the current element _AND_ decrement index value as to
			 * produce a "redo" effect */
			array_splice (&a, i--, 1, NULL);
		}

		if (DS_RETURN(ret) == DS_BREAK)
			break;
	}
}

static int d_hash_foreach_remove (Dataset *d, ForeachExFn func, void *udata)
{
	DatasetNode *node;
	DatasetNode *prev, *next;
	unsigned int i;
	int          ret;

	for (i = 0; i < d->tdata.hash->size; i++)
	{
restart:
		prev = NULL;

		for (node = d->tdata.hash->nodes[i]; node; )
		{
			next = node->tdata.hash_next;

			ret = func (node->key, node->value, udata, d, node);

			if (ret & DS_REMOVE)
			{
				d->tdata.hash->items--;

				if (prev)
				{
					prev->tdata.hash_next = node->tdata.hash_next;
					free_node (node);
					node = prev;
				}
				else
				{
					d->tdata.hash->nodes[i] = node->tdata.hash_next;
					free_node (node);
					goto restart;
				}
			}

			if (DS_RETURN(ret) == DS_BREAK)
				goto bail;

			prev = node;
			node = next;
		}
	}

bail:

	if (!d->tdata.hash->frozen)
		d_hash_resize (d);

	return TRUE;
}

void dataset_foreach_ex (Dataset *d, DatasetForeachExFn api_fn, void *udata)
{
	ForeachExFn hackfn;

	if (!d || !api_fn)
		return;

	/*
	 * We are actually passing two more arguments than the external API is
	 * aware of as it will be used internally by dataset_find.  Yes, this is
	 * a very disgusting hack, and I apologize to anyone who has to read this
	 * garbage.
	 */
	hackfn = (ForeachExFn)api_fn;

	switch (d->type)
	{
	 case DATASET_LIST:   d_list_foreach_remove (d, hackfn, udata);   break;
	 case DATASET_ARRAY:  d_array_foreach_remove (d, hackfn, udata);  break;
	 case DATASET_HASH:   d_hash_foreach_remove (d, hackfn, udata);   break;
	 default:             abort ();                                   break;
	}
}

/*****************************************************************************/

static int list_foreach_node (DatasetNode *node, void **args)
{
#if 0
	Dataset         *d     = args[0];
#endif
	DatasetForeachFn func  = args[1];
	void            *udata = args[2];

	(*func) (node->key, node->value, udata);

	return TRUE;
}

static void d_list_foreach (Dataset *d, DatasetForeachFn func, void *udata)
{
	void *args[] = { d, func, udata };

	list_foreach (d->tdata.list, (ListForeachFunc)list_foreach_node, args);
}

static void d_array_foreach (Dataset *d, DatasetForeachFn func, void *udata)
{
	Array       *a;
	DatasetNode *node;
	size_t       i;
	size_t       nmemb;

	if (!(a = d->tdata.array))
		return;

	nmemb = array_count (&a);

	for (i = 0; i < nmemb; i++)
	{
		if (!(node = array_splice (&a, i, 0, NULL)))
			continue;

		(*func) (node->key, node->value, udata);
	}
}

static void d_hash_foreach (Dataset *d, DatasetForeachFn func, void *udata)
{
	DatasetNode *node;
	unsigned int i;

	for (i = 0; i < d->tdata.hash->size; i++)
	{
		for (node = d->tdata.hash->nodes[i]; node;
		     node = node->tdata.hash_next)
		{
			(*func) (node->key, node->value, udata);
		}
	}
}

void dataset_foreach (Dataset *d, DatasetForeachFn func, void *udata)
{
	if (!d || !func)
		return;

	switch (d->type)
	{
	 case DATASET_LIST:      d_list_foreach (d, func, udata);   break;
	 case DATASET_ARRAY:     d_array_foreach (d, func, udata);  break;
	 case DATASET_HASH:      d_hash_foreach (d, func, udata);   break;
	 default:                abort ();                          break;
	}
}

/*****************************************************************************/

static int find_wrap (ds_data_t *key, ds_data_t *value, void **args, Dataset *d, DatasetNode *node)
{
	DatasetFindFn  func  = args[0];
	void          *udata = args[1];
	int            ret;

	/* this will happen if DS_BREAK doesn't function properly */
	assert (args[2] == NULL);

	ret = func (key, value, udata);

	if (ret)
	{
		args[2] = node;
		return DS_BREAK;
	}

	return DS_CONTINUE;
}

DatasetNode *dataset_find_node (Dataset *d, DatasetFindFn func, void *udata)
{
	void *args[] = { func, udata, NULL };

	if (!d || !func)
		return NULL;

	dataset_foreach_ex (d, DS_FOREACH_EX(find_wrap), args);

	return args[2];
}

void *dataset_find (Dataset *d, DatasetFindFn func, void *udata)
{
	DatasetNode *node;

	if (!(node = dataset_find_node (d, func, udata)))
		return NULL;

	assert (node->value != NULL);
	return node->value->data;
}

/*****************************************************************************/

unsigned int dataset_length (Dataset *d)
{
	unsigned int cnt = 0;

	if (!d)
		return cnt;

	switch (d->type)
	{
	 case DATASET_LIST:
		cnt = (unsigned int)(list_length (d->tdata.list));
		break;
	 case DATASET_ARRAY:
		cnt = (unsigned int)(array_count (&(d->tdata.array)));
		break;
	 case DATASET_HASH:
		cnt = (unsigned int)(d->tdata.hash->items);
		break;
	 default:
		abort ();
		break;
	}

	return cnt;
}

/*****************************************************************************/

static void flatten_node (ds_data_t *key, ds_data_t *value, List **list)
{
	*list = list_prepend (*list, value->data);
}

List *dataset_flatten (Dataset *d)
{
	List *list = NULL;

	dataset_foreach (d, DS_FOREACH(flatten_node), &list);

	return list;
}

/*****************************************************************************/

uint32_t dataset_uniq32 (Dataset *d, uint32_t *counter)
{
	uint32_t cnt = 0;

	if (counter)
		cnt = *counter;

	cnt++;

	while (cnt == 0 || dataset_lookup (d, &cnt, sizeof (cnt)))
		cnt++;

	/* store the counter for the next call */
	if (counter)
		*counter = cnt;

	return cnt;
}

/*****************************************************************************/

#ifdef DEBUG
static void dump_node (ds_data_t *key, ds_data_t *value, void *udata)
{
	fprintf (stderr, "%p(%p[%hu,%d]) => %p(%p[%hu,%d])\n",
	         key,   key->data, ((unsigned short)key->len), key->flags,
	         value, value->data, ((unsigned short)value->len), value->flags);
}

void dataset_dump (Dataset *d)
{
	dataset_foreach (d, DS_FOREACH(dump_node), NULL);
}
#endif /* DEBUG */

/*****************************************************************************/

#if 0
static void run_test2 (DatasetType type)
{
	Dataset *d = NULL;
	struct foo { int x; int y; } foo;
	char *bar = "bar";

	d = dataset_new (type);
	assert (d != NULL);

	foo.x = 10;
	foo.y = 15;
	dataset_insert (&d, &foo, sizeof (foo), bar, STRLEN_0 (bar));

	foo.x = 5;
	assert (dataset_lookup (d, &foo, sizeof (foo)) == NULL);

	foo.x = 10;
	bar = dataset_lookup (d, &foo, sizeof (foo));

	printf ("%i,%i = %s\n", foo.x, foo.y, bar);

	dataset_insert (&d, &foo, sizeof (foo), bar, 0);
	dataset_clear (d);
}

static int foreach (Dataset *d, DatasetNode *node, int *cmp)
{
	if (memcmp (node->key, cmp, sizeof (*cmp)))
		return FALSE;

	printf ("removing %i\n", *cmp);
	return TRUE;
}

static void run_test1 (DatasetType type)
{
	Dataset *d = dataset_new (type);
	int foo;

	foo = 1;
	dataset_insert (&d, &foo, sizeof (foo), "1", 2);

	foo = 2;
	dataset_insert (&d, &foo, sizeof (foo), "2", 2);

	foo = 3;
	dataset_insert (&d, &foo, sizeof (foo), "3", 2);

	foo = 2;
	dataset_foreach (d, DS_FOREACH (foreach), &foo);

	assert (dataset_lookup (d, &foo, sizeof (foo)) == NULL);
	dataset_clear (d);
}

static void run_test (char *name, void (*testfn) (DatasetType type))
{
	printf ("*** Entering test %s\n", name);

	testfn (DATASET_HASH);
	testfn (DATASET_LIST);
	testfn (DATASET_ARRAY);

	printf ("*** Leaving test %s\n", name);
}

#define RUN_TEST(testfn) run_test (#testfn, testfn)

int main ()
{
	RUN_TEST (run_test1);
	RUN_TEST (run_test2);

	return 0;
}
#endif

/*****************************************************************************/

#if 0
static void func1 (ds_data_t *key, ds_data_t *data, void *udata)
{
	printf ("%s = '%s'\n", (char *)key->data, (char *)data->data);
}

static int func2 (ds_data_t *key, ds_data_t *data, void *udata)
{
	if (strcmp (key->data, "foo") == 0)
		return DS_CONTINUE | DS_REMOVE;

	return DS_CONTINUE;
}

int main (int argc, char **argv)
{
	Dataset  *d = NULL;
	ds_data_t key;
	ds_data_t data;

	ds_data_init (&key,  "foo", 4, DS_NOCOPY);
	ds_data_init (&data, "bar", 4, DS_NOCOPY);

	dataset_insert_ex (&d, &key, &data);

	dataset_foreach (d, DS_FOREACH(func1), NULL);
	dataset_foreach_ex (d, DS_FOREACH_EX(func2), NULL);

	printf ("len=%u, should be 0\n", dataset_length (d));

	return 0;
}
#endif
