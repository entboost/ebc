#ifndef __HASH_H__
#define __HASH_H__
#include "comtypes.h"

#ifdef  __cplusplus
extern "C" {
#endif



    typedef struct _HashTable HashTable;
    /* Hash tables
     */
    HashTable* hash_table_new (HashFunc	hash_func, EqualFunc key_equal_func);
    HashTable* hash_table_new_full (HashFunc hash_func,
				    EqualFunc key_equal_func,
				    DestroyNotify key_destroy_func,
				    DestroyNotify value_destroy_func);
    void hash_table_destroy (HashTable *hash_table);
    void hash_table_insert (HashTable *hash_table, void *key, void *value);
    void hash_table_replace (HashTable *hash_table, void *key, void * value);
    int  hash_table_remove (HashTable *hash_table, const void *key);
    int  hash_table_steal (HashTable *hash_table, const void *key);
    void *hash_table_lookup (HashTable *hash_table, const void *key);
    int  hash_table_lookup_extended (HashTable *hash_table,
				     const void *lookup_key,
				     void **orikey,
				     void **value);
    void hash_table_foreach (HashTable *hash_table,
			     HFunc	    func,
			     void *user_data);
    unsigned int hash_table_remove_all(HashTable *);
    unsigned int hash_table_foreach_remove (HashTable *hash_table,
					    HRFunc     func,
					    void       *user_data);
    unsigned int hash_table_foreach_steal (HashTable	   *hash_table,
					   HRFunc	    func,
					   void *	    user_data);
    unsigned int hash_table_size (HashTable *hash_table);
    


    /* 
     * Hash Functions
     */
    int          str_equal (const void *v, const void *v2);
    unsigned int str_hash  (const void *v);

    int          int_equal (const void *v, const void *v2);
    unsigned int int_hash  (const void *v);

    /* This "hash" function will just return the key's adress as an
     * unsigned integer. Useful for hashing on plain adresses or
     * simple integer values.
     * passing NULL into hash_table_new() as HashFunc has the
     * same effect as passing direct_hash().
     */
    unsigned int direct_hash  (const void *v);
    int          direct_equal (const void *v,  const void *v2);

#ifdef  __cplusplus
}
#endif



#endif /* __HASH_H__ */

