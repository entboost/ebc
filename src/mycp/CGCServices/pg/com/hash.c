
#include <stdlib.h>
#include <string.h>

#include "primes.h"
#include "comtypes.h"
#include "hash.h"

#define HASH_TABLE_MIN_SIZE 11
#define HASH_TABLE_MAX_SIZE 13845163


typedef  struct _HashNode      HashNode;

struct _HashNode {
  void     *key;
  void     *value;
  HashNode *next;
};

struct _HashTable
{
    int             size;
    int             nnodes;
    HashNode      **nodes;
    HashFunc        hash_func;
    EqualFunc       key_equal_func;
    DestroyNotify   key_destroy_func;
    DestroyNotify   value_destroy_func;
};


#if !(defined (G_STMT_START) && defined (G_STMT_END))
#  if defined (__GNUC__) && !defined (__STRICT_ANSI__) && !defined (__cplusplus)
#    define G_STMT_START        (void)(
#    define G_STMT_END          )
#  else
#    if (defined (sun) || defined (__sun__))
#      define G_STMT_START      if (1)
#      define G_STMT_END        else (void)0
#    else
#      define G_STMT_START      do
#      define G_STMT_END        while (0)
#    endif
#  endif
#endif

#define HASH_TABLE_RESIZE(hash_table)				\
   G_STMT_START {						\
     if ((hash_table->size >= 3 * hash_table->nnodes &&	        \
	  hash_table->size > HASH_TABLE_MIN_SIZE) ||		\
	 (3 * hash_table->size <= hash_table->nnodes &&	        \
	  hash_table->size < HASH_TABLE_MAX_SIZE))		\
	   hash_table_resize (hash_table);			\
   } G_STMT_END

static void		hash_table_resize	(HashTable	  *hash_table);
static HashNode**	hash_table_lookup_node  (HashTable     *hash_table,
						 const void *   key);
static HashNode*	hash_node_new		  (void *	   key,
                                                   void *        value);
static void		hash_node_destroy	  (HashNode	  *hash_node,
                                                   DestroyNotify  key_destroy_func,
                                                   DestroyNotify  value_destroy_func);
static void		hash_nodes_destroy	  (HashNode	  *hash_node,
						   DestroyNotify   key_destroy_func,
						   DestroyNotify   value_destroy_func);
static unsigned int hash_table_foreach_remove_or_steal (HashTable     *hash_table,
							HRFunc	   func,
							void *	   user_data,
							int        notify);




/**
 * Function: hash_table_new
 * 
 * Description: Creates a new HashTable.
 *
 * Input parameters:
 *
 *   hash_func       a function to create a hash value from a key. Hash values
 *                   are used to determine where keys are stored within the 
 *                   HashTable data structure. The direct_hash(), int_hash() 
 *                   and str_hash() functions are provided for some common 
 *                   types of keys. If hash_func is 0, direct_hash() is used.
 *
 *   key_equal_func  a function to check two keys for equality.  This is used 
 *                   when looking up keys in the HashTable.  The 
 *                   direct_equal(), int_equal() and str_equal() functions are
 *                   provided for the most common types of keys. If 
 *                   key_equal_func is 0, keys are compared directly in a 
 *                   similar fashion to direct_equal(), but without the 
 *                   overhead of a function call.
 * 
 * Return value: a new HashTable.
 **/
HashTable* hash_table_new (HashFunc hash_func, EqualFunc key_equal_func)
{
    return hash_table_new_full (hash_func, key_equal_func, 0, 0);
}


/**
 * Function: hash_table_new_full
 *
 * Description: Creates a new HashTable like hash_table_new() and allows to
 * specify functions to free the memory allocated for the key and value that
 * get called when removing the entry from the HashTable.
 *
 * Input parameters:
 *   hash_func        a function to create a hash value from a key. 
 *
 *   key_equal_func   a function to check two keys for equality.
 *
 *   key_destroy_func a function to free the memory allocated for the key 
 *                    used when removing the entry from the HashTable or 0 if
 *                    you don't want to supply such a function.
 *
 *   value_destroy_func  a function to free the memory allocated for the 
 *                    value used when removing the entry from the HashTable or
 *                    0 if  you don't want to supply such a function.
 * 
 * Return value: a new HashTable.
 **/

HashTable *hash_table_new_full (HashFunc       hash_func,
				EqualFunc      key_equal_func,
				DestroyNotify  key_destroy_func,
				DestroyNotify  value_destroy_func)
{
    HashTable *hash_table;
    int i;
  
    hash_table = (HashTable*)malloc(sizeof(HashTable));

    hash_table->size               = HASH_TABLE_MIN_SIZE;
    hash_table->nnodes             = 0;
    hash_table->hash_func          = hash_func ? hash_func : direct_hash;
    hash_table->key_equal_func     = key_equal_func;
    hash_table->key_destroy_func   = key_destroy_func;
    hash_table->value_destroy_func = value_destroy_func;
    hash_table->nodes              = (HashNode**)malloc(sizeof(HashNode*) * hash_table->size);
  
    for (i = 0; i < hash_table->size; i++)
	hash_table->nodes[i] = 0;
  
    return hash_table;
}

/**
 * Function: hash_table_destroy
 * 
 * Description: Destroys the HashTable. If keys and/or values are dynamically 
 * allocated, you should either free them first or create the HashTable using 
 * hash_table_new_full(). In the latter case the destroy functions you supplied
 * will be called on all keys and values before destroying  the HashTable.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 * 
 **/
void hash_table_destroy (HashTable *hash_table)
{
    int i;
    if(!hash_table)
	return;
  
    for (i = 0; i < hash_table->size; i++)
	hash_nodes_destroy (hash_table->nodes[i], 
			    hash_table->key_destroy_func,
			    hash_table->value_destroy_func);
  
    free (hash_table->nodes);
    free (hash_table);
}

static /*inline*/ HashNode** hash_table_lookup_node (HashTable  *hash_table,
						 const void *key)
{
    HashNode **node;
  
    node = &hash_table->nodes
	[(* hash_table->hash_func) (key) % hash_table->size];
  
    /* Hash table lookup needs to be fast.
     *  We therefore remove the extra conditional of testing
     *  whether to call the key_equal_func or not from
     *  the inner loop.
     */
    if (hash_table->key_equal_func)
	while (*node && !(*hash_table->key_equal_func) ((*node)->key, key))
	    node = &(*node)->next;
    else
	while (*node && (*node)->key != key)
	    node = &(*node)->next;
  
    return node;
}

/**
 * Function: hash_table_lookup
 * 
 * Description: Looks up a key in a HashTable.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   key             the key to look up.
 *
 * Return value: the associated value, or 0 if the key is not found.
 * 
 **/
void *hash_table_lookup (HashTable  *hash_table,
			 const void *key)
{
    HashNode *node;
  
    if(!hash_table)
	return 0;
  
    node = *hash_table_lookup_node (hash_table, key);
  
    return node ? node->value : 0;
}

/**
 * Function: hash_table_lookup_extended
 * 
 * Description: Looks up a key in the HashTable, returning the original key and
 * the associated value and a int which is 1 if the key was found. This is 
 * useful if you need to free the memory allocated for the original key, for 
 * example before calling hash_table_remove().
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *  lookup_key       the key to look up.
 *
 *  orikey           returns the original key.
 *
 *  value            returns the value associated with the key.
 *
 * Return value: 1 if the key was found in the HashTable.
 * 
 **/
int hash_table_lookup_extended (HashTable   *hash_table,
				const void  *lookup_key,
				void       **orikey,
				void       **value)
{
    HashNode *node;
    
    if(!hash_table)
	return 0;
  
    node = *hash_table_lookup_node (hash_table, lookup_key);
  
    if (node)
    {
	if (orikey)
	    *orikey = node->key;
	if (value)
	    *value = node->value;
	return 1;
    }
    else
	return 0;
}

/**
 * Function: hash_table_insert
 * 
 * Description: Inserts a new key and value into a HashTable.  If the key 
 * already exists in the HashTable its current value is replaced with the new
 * value. If you supplied a value_destroy_func when creating the HashTable, 
 * the old value is freed using that function. If you supplied a 
 * key_destroy_func when creating the HashTable, the passed key is freed 
 * using that function.

 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   key             the key to look up.
 *
 *   value           the value to associate with the key.
 **/
void hash_table_insert (HashTable *hash_table, void *key, void *value)
{
    HashNode **node;
  
    if(!hash_table)
	return;
  
    node = hash_table_lookup_node (hash_table, key);
  
    if (*node) {
	/* do not reset node->key in this place, keeping
	 * the old key is the intended behaviour. 
	 * hash_table_replace() can be used instead.
	 */

	/* free the passed key */
	if (hash_table->key_destroy_func)
	    hash_table->key_destroy_func (key);
      
	if (hash_table->value_destroy_func)
	    hash_table->value_destroy_func ((*node)->value);

	(*node)->value = value;
    } else {
	*node = hash_node_new (key, value);
	hash_table->nnodes++;
	HASH_TABLE_RESIZE (hash_table);
    }
}

/**
 * Function: hash_table_replace
 * 
 * Description: Inserts a new key and value into a HashTable similar to 
 * hash_table_insert(). The difference is that if the key already exists 
 * in the HashTable, it gets replaced by the new key. If you supplied a 
 * value_destroy_func when creating the HashTable, the old value is freed 
 * using that function. If you supplied a key_destroy_func when creating the 
 * HashTable, the old key is freed using that function. 
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   key             the key to look up.
 *
 *   value           the value to associate with the key.
 **/
void hash_table_replace (HashTable *hash_table, void *key, void *value)
{
    HashNode **node;
  
    if(!hash_table)
	return;
  
    node = hash_table_lookup_node (hash_table, key);
  
    if (*node)
    {
	if (hash_table->key_destroy_func)
	    hash_table->key_destroy_func ((*node)->key);
      
	if (hash_table->value_destroy_func)
	    hash_table->value_destroy_func ((*node)->value);

	(*node)->key   = key;
	(*node)->value = value;
    }
    else
    {
	*node = hash_node_new (key, value);
	hash_table->nnodes++;
	HASH_TABLE_RESIZE (hash_table);
    }
}

/**

 * Function: hash_table_remove
 * 
 * Description: Removes a key and its associated value from a HashTable.
 *
 * If the HashTable was created using hash_table_new_full(), the
 * key and value are freed using the supplied destroy functions, otherwise
 * you have to make sure that any dynamically allocated values are freed 
 * yourself.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   key             the key to look up.
 *
 * Return value: 1 if the key was found and removed from the HashTable.
 **/
int hash_table_remove (HashTable *hash_table, const void *key)
{
    HashNode **node, *dest;

    if(!hash_table)
	return 0;
  
    node = hash_table_lookup_node (hash_table, key);
    if (*node)
    {
	dest = *node;
	(*node) = dest->next;
	hash_node_destroy (dest, 
			   hash_table->key_destroy_func,
			   hash_table->value_destroy_func);
	hash_table->nnodes--;
  
	HASH_TABLE_RESIZE (hash_table);

	return 1;
    }

    return 0;
}

/**
 * Function: hash_table_steal
 * 
 * Description: Removes a key and its associated value from a HashTable
 * without calling the key and value destroy functions.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   key             the key to look up.
 *
 * Return value: 1 if the key was found and removed from the HashTable.
 **/
int hash_table_steal (HashTable *hash_table, const void *key)
{
    HashNode **node, *dest;
 
    if(!hash_table)
	return 0;
  
    node = hash_table_lookup_node (hash_table, key);
    if (*node)
    {
	dest = *node;
	(*node) = dest->next;
	hash_node_destroy (dest, 0, 0);
	hash_table->nnodes--;
  
	HASH_TABLE_RESIZE (hash_table);

	return 1;
    }

    return 0;
}


/**
 * Function: hash_table_foreach_remove
 * 
 * Description: Calls the given function for each key/value pair in the 
 * HashTable. If the function returns 1, then the key/value pair is removed
 * from the HashTable. If you supplied key or value destroy functions when 
 * creating the HashTable, they are used to free the memory allocated for the
 * removed  keys and values.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   func            the function to call for each key/value pair.
 *
 *   user_data       user data to pass to the function.
 *
 * Return value: the number of key/value pairs removed.
 **/
unsigned int hash_table_foreach_remove (HashTable *hash_table,
					HRFunc	   func,
					void *	   user_data)
{
    if((hash_table == 0) || (func == 0))
	return 0;
  
    return hash_table_foreach_remove_or_steal (hash_table, func, user_data, 1);
}

/**
 * Function: hash_table_foreach_steal
 * 
 * Description: Calls the given function for each key/value pair in the 
 * HashTable. If the function returns %1, then the key/value pair is removed 
 * from the HashTable, but no key or value destroy functions are called.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   func            the function to call for each key/value pair.
 *
 *   user_data       user data to pass to the function.
 *
 * Return value: the number of key/value pairs removed.
 **/
unsigned int hash_table_foreach_steal (HashTable *hash_table,
				       HRFunc	  func,
				       void *	  user_data)
{
    if(hash_table == 0 || func == 0)
	return 0;
  
    return hash_table_foreach_remove_or_steal (hash_table, func, user_data, 0);
}

static unsigned int hash_table_foreach_remove_or_steal (HashTable *hash_table,
							HRFunc	  func,
							void *	  user_data,
							int       notify)
{
    HashNode *node, *prev;
    int i;
    unsigned int deleted = 0;
  
    for (i = 0; i < hash_table->size; i++)
    {
    restart:
      
	prev = 0;
      
	for (node = hash_table->nodes[i]; node; prev = node, node = node->next)
	{
	    if ((* func) (node->key, node->value, user_data))
	    {
		deleted += 1;
	      
		hash_table->nnodes -= 1;
	      
		if (prev)
		{
		    prev->next = node->next;
		    hash_node_destroy (node,
				       notify ? hash_table->key_destroy_func : 0,
				       notify ? hash_table->value_destroy_func : 0);
		    node = prev;
		}
		else
		{
		    hash_table->nodes[i] = node->next;
		    hash_node_destroy (node,
				       notify ? hash_table->key_destroy_func : 0,
				       notify ? hash_table->value_destroy_func : 0);
		    goto restart;
		}
	    }
	}
    }
  
    HASH_TABLE_RESIZE (hash_table);
  
    return deleted;
}

/**
 * Function: hash_table_remove_all
 * 
 * Description: Removes all nodes in given hash table. 
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 * Return value: the number of key/value pairs removed.
 **/
unsigned int hash_table_remove_all(HashTable *hash_table)
{
    HashNode *node, *prev;
    int i;
    unsigned int deleted = 0;
  
    for (i = 0; i < hash_table->size; i++) {
	node = hash_table->nodes[i];
	while (node) {
	    prev = node;
	    node = node->next;
	    hash_node_destroy (prev, 
			       hash_table->key_destroy_func,
			       hash_table->value_destroy_func);
		deleted++;
	}
	hash_table->nodes[i] = 0;
    }
	
    HASH_TABLE_RESIZE (hash_table);
	hash_table->nnodes = 0;
    return deleted;
}
/**
 * Function: hash_table_foreach
 * 
 * Description: Calls the given function for each of the key/value pairs in the
 * HashTable.  The function is passed the key and value of each pair, and the
 * given user_data parameter.  The hash table may not be modified while 
 * iterating over it (you can't add/remove items). To remove all items matching
 * a predicate, use hash_table_remove().
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   func            the function to call for each key/value pair.
 *
 *   user_data       user data to pass to the function.
 *
 * Return value: the number of key/value pairs in the HashTable.
 **/
void hash_table_foreach (HashTable *hash_table, HFunc func, void *user_data)
{
    HashNode *node;
    int i;
  
    if (hash_table == 0 || func == 0) {
	return;
    }
  
    for (i = 0; i < hash_table->size; i++)
	for (node = hash_table->nodes[i]; node; node = node->next)
	{
	    (* func) (node->key, node->value, user_data);
	}
}

/**
 * Function: hash_table_size
 * 
 * Description: Returns the number of elements contained in the HashTable.
 *
 * Input parameters:
 *   hash_table      a HashTable.
 *
 *   func            the function to call for each key/value pair.
 *
 *   user_data       user data to pass to the function.
 *
 * Return value: the number of key/value pairs in the HashTable.
 **/
unsigned int hash_table_size (HashTable *hash_table)
{
    if(!hash_table)
	return 0;
  
    return hash_table->nnodes;
}

static void hash_table_resize (HashTable *hash_table)
{
    HashNode **new_nodes;
    HashNode *node;
    HashNode *next;
    unsigned int hash_val;
    int new_size;
    int i;

    new_size = spaced_primes_closest (hash_table->nnodes);
    new_size = CLAMP (new_size, HASH_TABLE_MIN_SIZE, HASH_TABLE_MAX_SIZE);
 
    new_nodes =(HashNode**)malloc(sizeof(HashNode*) * new_size);

    for (i = 0; i < new_size; i++)
	new_nodes[i] = 0;

    for (i = 0; i < hash_table->size; i++) {
	for (node = hash_table->nodes[i]; node; node = next) {
	    next = node->next;

	    hash_val = (* hash_table->hash_func) (node->key) % new_size;

	    node->next = new_nodes[hash_val];
	    new_nodes[hash_val] = node;
	}
    }  
    free (hash_table->nodes);
    hash_table->nodes = new_nodes;
    hash_table->size  = new_size;
}

static HashNode* hash_node_new (void *key, void *value)
{
    HashNode *hash_node;
    hash_node = (HashNode*)malloc(sizeof(HashNode));
    hash_node->key = key;
    hash_node->value = value;
    hash_node->next = 0;
  
    return hash_node;
}

static void hash_node_destroy (HashNode      *hash_node,
			       DestroyNotify  key_destroy_func,
			       DestroyNotify  value_destroy_func)
{
    if (key_destroy_func)
	key_destroy_func (hash_node->key);
    if (value_destroy_func)
	value_destroy_func (hash_node->value);
  
    hash_node->key = 0;
    hash_node->value = 0;
    free(hash_node);

}

static void hash_nodes_destroy (HashNode *hash_node,
				FreeFunc  key_destroy_func,
				FreeFunc  value_destroy_func)
{
  HashNode *last;
  while (hash_node) {
    last = hash_node;
    hash_node = hash_node->next;
    hash_node_destroy(last, key_destroy_func, value_destroy_func);
  }
}
/****************************************************************************/

int str_equal(const void *v1, const void *v2)
{
    const char *string1 = v1;
    const char *string2 = v2;
 
    return strcmp (string1, string2) == 0;
}

/* 31 bit hash function */
unsigned int str_hash (const void *key)
{
    const char  *p = key;
    unsigned int h = *p;
    
    if (h) {
	for (p += 1; *p != '\0'; p++)
	    h = (h << 5) - h + *p;
    }
    return h;
}



int int_equal (const void *v1, const void *v2)
{
    return ((const int) v1) == ((const int) v2);
}

unsigned int int_hash (const void *v)
{
    return (const int)v;
}


unsigned int direct_hash (const void *v)
{
    return (unsigned int)v;
}

int direct_equal (const void *v1, const void *v2)
{
    return v1 == v2;
}
