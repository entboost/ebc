/*
 * $Id: dataset.h,v 1.28 2003/10/30 09:08:42 jasta Exp $
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

#ifndef __GIFT_DATASET_H__
#define __GIFT_DATASET_H__

/*****************************************************************************/

/**
 * @file dataset.h
 *
 * @brief Arbitrary key/data association data structure.
 *
 * Flexible data structure used for management of arbitrary sets of data.
 * Currently supports a hash table and a linked list backend, btree should
 * come soon.
 */

/*****************************************************************************/

#include "list.h"
#include "array.h"

/*****************************************************************************/

/* ds_data_t flags */
#define DS_NOCOPY   0x01

/**
 * Data type used to describe a single data entry in the dataset.  Two data
 * entries are used per 'row', one for key and one for value.
 */
typedef struct
{
	void   *data;
	size_t  len;
	uint8_t flags;
} ds_data_t;

/*****************************************************************************/

/**
 *  Describes a specific key/value set.
 */
struct dataset_node
{
	ds_data_t *key;
	ds_data_t *value;

	union
	{
		struct dataset_node *hash_next;
		size_t array_offs;
		List *list_link;
	} tdata;
} /* DatasetNode */;

/*****************************************************************************/

/**
 * Selects the backend data structure implementation for the dataset to use.
 */
typedef enum
{
	DATASET_LIST,                      /**< Linked list */
	DATASET_ARRAY,                     /**< Dynamically allocated array */
	DATASET_HASH                       /**< Hash table */
} DatasetType;

#define DATASET_DEFAULT DATASET_HASH

struct dataset;
typedef struct dataset Dataset;

struct dataset_node;
typedef struct dataset_node DatasetNode;

/*****************************************************************************/

#define DS_NOTFOUND 0x00               /**< DS_FIND: Continue iteration */
#define DS_FOUND    0x02               /**< DS_FIND: Found, halt iteration */

#define DS_CONTINUE 0x01               /**< DS_FOREACH: Continue iteration */
#define DS_BREAK    0x02               /**< DS_FOREACH: Halt iteration */
#define DS_REMOVE   0x04               /**< DS_FOREACH: Remove the node */

/**
 * Main dataset iterator callback.  See ::DatasetForeachExFn for the extended
 * type.
 */
typedef void (*DatasetForeachFn) (ds_data_t *key, ds_data_t *value,
                                  void *udata);

/**
 * Extended dataset iterator capable of basic flow control and removal of
 * entries as you pass by them.
 *
 * @return One of either DS_CONTINUE or DS_BREAK, optionally OR'd against
 *         DS_REMOVE.
 */
typedef int (*DatasetForeachExFn) (ds_data_t *key, ds_data_t *value,
                                   void *udata);

/**
 * Helper macro for properly typecasting iterator functions.
 */
#define DS_FOREACH(func) ((DatasetForeachFn)func)
#define DS_FOREACH_EX(func) ((DatasetForeachExFn)func)

typedef DatasetForeachExFn DatasetFindFn;
#define DS_FIND(func) ((DatasetFindFn)func)

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/**
 * Allocate a new dataset.  Using this function is optional, see
 * ::dataset_insert for more information.
 *
 * @param type  Backend implementation to use.
 */
 
  Dataset *dataset_new (DatasetType type);

/**
 * Unallocate a dataset and all internally allocated variables.
 */
 
  void dataset_clear (Dataset *d);

/*****************************************************************************/

/**
 * Initialize a ds_data_t element for use with the extended dataset
 * interface.  Example usage:
 *
 * \code
 * ds_data_t key;
 * ds_data_t data;
 *
 * ds_data_init (&key, "foo", 4, DS_NOCOPY);
 * ds_data_init (&data, "bar", 4, DS_NOCOPY);
 *
 * dataset_insert_ex (&d, &key, &data);
 * \endcode
 *
 * @param dsdata  Storage location to initialize.
 * @param data    Data to be attached with this element.
 * @param len     Length of the data segment, or 0 if you wish to leave
 *                undefined (must set DS_NOCOPY).
 * @param flags   Action-specific flags to use for the data element.
 */
 
  void ds_data_init (ds_data_t *dsdata, const void *data, size_t len,
                     int flags);

/*****************************************************************************/

/**
 * Base implementation for ::dataset_insert and ::dataset_insertstr.
 */
 
  DatasetNode *dataset_insert_ex (Dataset **d, ds_data_t *key,
                                  ds_data_t *value);

/**
 * Insert a new key/value set.  If the value at d is NULL, a new dataset will
 * be allocated using DATASET_DEFAULT.
 *
 * @param d
 * @param key        Storage location of the lookup key.
 * @param key_len    Length of \em key for copying.
 * @param value      Storage location of the value.
 * @param value_len
 *
 *   Length of \em value if you wish for the dataset to internally manage the
 *   values memory, otherwise 0 in which case dataset_clear will not free
 *   this address.
 *
 * @return Node handle that has just been inserted, or NULL on error.
 */
 
  DatasetNode *dataset_insert (Dataset **d,
                               const void *key, size_t key_len,
                               const void *value, size_t value_len);

/**
 * Wrapper around ::dataset_insert for string constants.  Copies both key and
 * value.
 */
 
  DatasetNode *dataset_insertstr (Dataset **d, const char *key,
                                  const char *value);

/*****************************************************************************/

/**
 * Base implementation for ::dataset_remove and ::dataset_removestr.
 */
 
  void dataset_remove_ex (Dataset *d, ds_data_t *key);

/**
 * Remove a key/value set by the key.
 */
 
  void dataset_remove (Dataset *d, const void *key, size_t key_len);

/**
 * Wrapped around ::dataset_remove for string constants.
 */
 
  void dataset_removestr (Dataset *d, const char *key);

/**
 * Perform a remove operation from the original lookup node.  No extra lookup
 * will be performed.
 */
 
  void dataset_remove_node (Dataset *d, DatasetNode *node);

/*****************************************************************************/

/**
 * Base implementation for ::dataset_lookup and ::dataset_lookupstr.
 */
 
  ds_data_t *dataset_lookup_ex (Dataset *d, ds_data_t *key);

/**
 * Lookup a value by the set's key.  If you need to know the value_len,
 * consider using ::dataset_lookup_node.
 */
 
  void *dataset_lookup (Dataset *d, const void *key, size_t key_len);

/**
 * Wrapper around ::dataset_lookup for aggregate string constants.
 */
 
  void *dataset_lookupstr (Dataset *d, const char *key);

/**
 * Base implementation for ::dataset_lookup_node.
 */
 
  DatasetNode *dataset_lookup_node_ex (Dataset *d, ds_data_t *key);

/**
 * Lookup a complete set by the set's key.  Similar to ::dataset_lookup
 * except that the full set is returned.
 */
 
  DatasetNode *dataset_lookup_node (Dataset *d, const void *key,
                                    size_t key_len);

/*****************************************************************************/

/**
 * Iterate an entire dataset.  This is the simplest form and does not have
 * any kind of flow control of extended features.
 *
 * @note This iterator is optimized under the assumption that the dataset
 *       integrity will not be affected during iteration.  If you can not
 *       guarantee this, you must use ::dataset_foreach_ex.
 */
 
  void dataset_foreach (Dataset *d, DatasetForeachFn func, void *udata);

/**
 * Extended iterator for the dataset capable of simple flow control and
 * removal of entries as you pass over them.  See ::dataset_foreach for more
 * details.
 */
 
  void dataset_foreach_ex (Dataset *d, DatasetForeachExFn func, void *udata);

/*****************************************************************************/

/**
 * Locate a set value through iteration.  See ::dataset_find_node.
 *
 * @return Pointer to the value entry found.
 */
 
  void *dataset_find (Dataset *d, DatasetFindFn func, void *udata);

/**
 * Locate a dataset node through iteration.  DatasetForeach returns TRUE when
 * the set has been found.
 */
 
  DatasetNode *dataset_find_node (Dataset *d, DatasetFindFn func, void *udata);

/*****************************************************************************/

/**
 * Calculate the dataset length (total number of sets currently inserted).
 * Please note that the DATASET_LIST backend requires iteration to perform
 * this task and as such is considered expensive.
 *
 * @retval Number of data entries in the set.
 */
 
  unsigned int dataset_length (Dataset *d);

/*****************************************************************************/

/**
 * Flatten a dataset to a reallocated list.  The values are not copied.
 */
 
  List *dataset_flatten (Dataset *d);

/*****************************************************************************/

/**
 * Find a unique, non-zero 32-bit value that is not contained in the
 * dataset.  Assumes that the dataset contains integers.
 *
 * @param d
 * @param counter  Hint variable for fast lookup.
 *
 * @retval A unique, non-zero 32-bit integer that is not a key of the dataset.
 */
 
  uint32_t dataset_uniq32 (Dataset *d, uint32_t *counter);

/*****************************************************************************/

 void *dateset_memdup (const void *ptr, size_t len);

 /*****************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* __GIFT_DATASET_H__ */



