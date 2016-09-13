#include <com/primes.h>
#include <com/comtypes.h>
#include <com/set.h>
#include <com/list.h>

#define SET_MIN_SIZE 100

struct _Set
{
    int             size;
    int             nnodes;
    List           *list;
    CompareFunc     value_cmp_func;
    DestroyNotify   value_destroy_func;
};


/**
 * Function: set_new_full
 *
 * Description: Creates a new Set like set_new() and allows to
 * specify functions to free the memory allocated for the key and value that
 * get called when removing the entry from the Set.
 *
 * Input parameters:
 *   set_func        a function to create a set value from a key. 
 *
 *   key_equal_func   a function to check two keys for equality.
 *
 *   key_destroy_func a function to free the memory allocated for the key 
 *                    used when removing the entry from the Set or 0 if
 *                    you don't want to supply such a function.
 *
 *   value_destroy_func  a function to free the memory allocated for the 
 *                    value used when removing the entry from the Set or
 *                    0 if  you don't want to supply such a function.
 * 
 * Return value: a new Set.
 **/

Set *set_new_full (CompareFunc cmp_func, DestroyNotify value_destroy_func)
{
    Set *set;
    unsigned int i;
  
    set = (Set*)malloc(sizeof(Set));
    set->size               = SET_MIN_SIZE;
    set->nnodes             = 0;
    set->value_cmp_func     = cmp_func;
    set->value_destroy_func = value_destroy_func;
    set->list               = 0;
  
    return set;
}


Set *set_new()
{
  return set_new_full(0, 0);
}


void  free_data(void *node, void *u)
{
  Set *set = (Set*)u;
  if(set->value_destroy_func)
    set->value_destroy_func(node);
}

/**
 * Function: set_destroy
 * 
 * Description: Destroys the Set. If keys and/or values are dynamically 
 * allocated, you should either free them first or create the Set using 
 * set_new_full(). In the latter case the destroy functions you supplied
 * will be called on all keys and values before destroying  the Set.
 *
 * Input parameters:
 *   set      a Set.
 * 
 **/
void set_destroy (Set *set)
{
    unsigned int i;
    if(!set)
	return;
  
    if(set->value_destroy_func)
      list_foreach(set->list, free_data, set);

    list_free(set->list);
    free(set);
}

static inline SetNode** set_lookup_node (Set  *set, const void *key)
{
    SetNode **node;
  
    node = &set->nodes
	[(* set->set_func) (key) % set->size];
  
    /* Set table lookup needs to be fast.
     *  We therefore remove the extra conditional of testing
     *  whether to call the key_equal_func or not from
     *  the inner loop.
     */
    if (set->key_equal_func)
	while (*node && !(*set->key_equal_func) ((*node)->key, key))
	    node = &(*node)->next;
    else
	while (*node && (*node)->key != key)
	    node = &(*node)->next;
  
    return node;
}

/**
 * set_lookup:
 * @set: a Set.
 * @key: the key to look up.
 * 
 * Looks up a key in a Set.
 * 
 * Return value: the associated value, or 0 if the key is not found.
 **/
void *set_lookup (Set  *set,
			 const void *key)
{
    SetNode *node;
  
    if(!set)
	return 0;
  
    node = *set_lookup_node (set, key);
  
    return node ? node->value : 0;
}

/**
 * set_insert:
 * @set: a Set.
 * @key: a key to insert.
 * @value: the value to associate with the key.
 * 
 * Inserts a new key and value into a Set.
 * 
 * If the key already exists in the Set its current value is replaced
 * with the new value. If you supplied a @value_destroy_func when creating the 
 * Set, the old value is freed using that function. If you supplied
 * a @key_destroy_func when creating the Set, the passed key is freed 
 * using that function.
 **/
void set_insert (Set *set, void *value)
{

  if(!set)
    return;
  if(list_find(set->list, value))
    return;

  set->list = list_prepend(set->list, value);
}

/**
 * set_remove:
 * @set: a Set.
 * @key: a key to insert.
 * @value: the value to associate with the key.
 * 
 * Inserts a new key and value into a Set similar to 
 * set_insert(). The difference is that if the key already exists 
 * in the Set, it gets replaced by the new key. If you supplied a 
 * @value_destroy_func when creating the Set, the old value is freed 
 * using that function. If you supplied a @key_destroy_func when creating the 
 * Set, the old key is freed using that function. 
 **/
void set_remove (Set *set, void *value)
{
    
    if(!set)
	return;

    set->list = list_remove(set->list, value);
}


/**
 * set_foreach_remove:
 * @set: a Set.
 * @func: the function to call for each key/value pair.
 * @user_data: user data to pass to the function.
 * 
 * Calls the given function for each key/value pair in the Set.
 * If the function returns %1, then the key/value pair is removed from the
 * Set. If you supplied key or value destroy functions when creating
 * the Set, they are used to free the memory allocated for the removed
 * keys and values.
 * 
 * Return value: the number of key/value pairs removed.
 **/
unsigned int set_foreach_remove (Set *set,
					HRFunc	   func,
					void *	   user_data)
{
    if(set == 0 | func == 0)
	return 0;
  
    return set_foreach_remove_or_steal (set, func, user_data, 1);
}


/**
 * set_foreach:
 * @set: a Set.
 * @func: the function to call for each key/value pair.
 * @user_data: user data to pass to the function.
 * 
 * Calls the given function for each of the key/value pairs in the
 * Set.  The function is passed the key and value of each
 * pair, and the given @user_data parameter.  The set table may not
 * be modified while iterating over it (you can't add/remove
 * items). To remove all items matching a predicate, use
 * set_remove().
 **/
void set_foreach (Set *set, HFunc func, void *user_data)
{
  if(!set)
    return;
  list_foreach(set->list, func, user_data);
}

/**
 * set_size:
 * @set: a Set.
 * 
 * Returns the number of elements contained in the Set.
 * 
 * Return value: the number of key/value pairs in the Set.
 **/
unsigned int set_size (Set *set)
{
    if(!set)
	return 0;
    return list_length(set->list);
}



int str_equal(const void *v1, const void *v2)
{
    const char *string1 = v1;
    const char *string2 = v2;
 
    return strcmp (string1, string2) == 0;
}

/* 31 bit set function */
unsigned int str_set (const void *key)
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
    return *((const int*) v1) == *((const int*) v2);
}

unsigned int int_set (const void *v)
{
    return *(const int*)v;
}


unsigned int direct_set (const void *v)
{
    return (unsigned int)v;
}

int direct_equal (const void *v1, const void *v2)
{
    return v1 == v2;
}
