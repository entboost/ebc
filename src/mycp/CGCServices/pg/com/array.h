/*
 * $Id: array.h,v 1.11 2003/10/16 18:50:54 jasta Exp $
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

#ifndef __ARRAY_H
#define __ARRAY_H

/*****************************************************************************/

/**
 * @file array.h
 *
 * @brief Array subsystem that mimics the flexibility and ease of Perl-style
 *        arrays.
 *
 * This is the only libgift subsystem which doesn't prefix it's name for
 * function namespacing.  This is done to provide the absolute shortest
 * possible function names.
 *
 * Example:
 *
 * \code
 *
 * Array *a = array ("foo", "bar", NULL);
 *
 * array_splice (&a, 0, 1, "FOO", NULL);
 * array_splice (&a, 1, 1, "Bar", NULL);
 *
 * array_push (&a, "baz");
 * array_push (&a, "test");
 * array_push (&a, "ing");
 *
 * str1 = pop (&a);
 * str2 = pop (&a);
 *
 * printf ("the %s is work%s\n", str2, str1);
 *
 * array_unset (&a);
 *
 * \endcode
 */

/*****************************************************************************/

/**
 * Opaque array object.
 */
typedef struct
{
	/**
	 * @name Private
	 */
	size_t size;                       /**< Allocated size */
	size_t nmemb;                      /**< Number of elements */
	size_t offs;                       /**< Used to efficiently shift data off
	                                    *   the array */
	void **array;                      /**< Data array */
} Array;

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/**
 * Construct an array a'la PHP.  The parameter list is NULL terminated.
 *
 * @param first  Beginning of a list of elements to construct the array with.
 *               NULL terminate the following variable argument list.
 *
 * @return Allocate memory that must be cleaned up with ::unset.
 */

  Array *array_new (void *first, ...);

/**
 * Clear all memory used by the array object and set the value at the
 * object's reference to NULL.
 *
 * @param a  Address of the pointer returned from ::array.
 */

  void array_unset (Array **a);

/*****************************************************************************/

/**
 * Push a single element on to the end of the array.
 *
 * @return If successful, element is returned, otherwise NULL.
 */

  void *array_push (Array **a, void *element);

/**
 * Pop an element off the array.
 *
 * @return Pointer to the inserted element or NULL if there is no element
 *         available (the array is empty).
 */

  void *array_pop (Array **a);

/*****************************************************************************/

/**
 * Shift an element off the beginning of the array and return it.
 *
 * @return Identical to ::pop.
 */

  void *array_shift (Array **a);

/**
 * Add a new element to the beginning of the array.
 *
 * @return Identical to ::push.
 */

  void *array_unshift (Array **a, void *element);

/*****************************************************************************/

/**
 * Access the array member at the supplied index.  In effective this is the
 * same as calling:
 *
 * \code
 * array_splice (a, index, 0, NULL);
 * \endcode
 */

  void *array_index (Array **a, int index);

/**
 * Return the number of elements in the array.  This expensive can be
 * considered inexpensive.
 */

  size_t array_count (Array **a);

/*****************************************************************************/

/**
 * Splice an array.  It's probably best to lookup perldoc -f splice in order
 * to see what inspired this interface.  The implementation details may make
 * this interface more clear as well.
 *
 * @param a
 * @param offset  Array index to begin operation from.
 * @param length  Number of elements to remove following `offset'.
 * @param first   First element to add to the offset position (after removal).
 *                The following variable argument list should be NULL
 *                terminated if this parameter is non-NULL.
 *
 * @return The last affected element is returned.  This is inconsistent with
 *         perl's splice implementation.
 */

  void *array_splice (Array **a, int offset, int length, void *first, ...);

/*****************************************************************************/

/**
 * Access the elements in the list sequentially with a single function call.
 * This replaces the need to call splice multiple times in order to access
 * incremental values in the list.  Example:
 *
 * \code
 * Array *ar = array ("a", "b", "c", NULL);
 * char  *a, *b, *c;
 *
 * array_list (&ar, &a, &b, &foo, NULL);
 *
 * ...
 * \endcode
 *
 * @param a
 * @param ...  NULL-terminated list of addresses to set.  It is safe to
 *             supply a list of a different size (including one larger) than
 *             the number of elements in the supplied array.  All additional
 *             values will be set to NULL as a convenience to the user.
 *
 * @return Number of elements successfully accessed.  This is usually the
 *         number of elements supplied.
 */

  int array_list (Array **a, ...);

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* __ARRAY_H */
