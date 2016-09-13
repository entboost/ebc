/*
 * $Id: list.h,v 1.15 2003/12/23 03:51:31 jasta Exp $
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

#ifndef __GIFT_LIST_H__
#define __GIFT_LIST_H__

#include "comtypes.h"
#include <sys/types.h>

/*****************************************************************************/

/**
 * @file list.h
 *
 * @brief Singly linked list routines
 *
 * Implements a standard doubly linked list.
 */

/*****************************************************************************/

#define list_next(n) ((n) ? (n)->next : NULL)
#define list_prev(n) ((n) ? (n)->prev : NULL)

/*****************************************************************************/



/**
 * Linked list structure.  This represents all chains in the list.
 */
typedef struct listlink
{
	void         *data;                /**< Data inserted */

	struct listlink *prev;             /**< Previous chain */
	struct listlink *next;             /**< Next chain */
} List;

/**************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/**
 * Foreach iteration function.  Return value depends on the usage.
 *
 * @param data  List data at this chain
 * @param udata Arbitrary data passed along
 */
typedef int (*ListForeachFunc) (void *data, void *udata);
#define LIST_FOREACH(func) ((ListForeachFunc)func)

/*****************************************************************************/

/**
 * Add a new link to the end of the linked list pointed to by `head'.
 */
 
  List *list_append (List *head, void *udata);

#if 1
/**
 * Inserts the previously allocated link chain at the end of the list pointed
 * to by `head'.  Be sure to use list_unlink_link first to make sure that this
 * link doesn't already belong to another list.
 */
 
  List *list_append_link (List *head, List *link);
#endif

/**
 * Similar to ::list_append, but uses the prepend operation.
 */
 
  List *list_prepend (List *head, void *udata);

#if 0
/**
 * Similar to ::list_append_link, but uses the prepend operation.
 */
 
  List *list_prepend_link (List *head, List *link);
#endif

/*****************************************************************************/

/**
 * Insert a new link immediately preceding the link that currently exists at
 * the `nth' position.
 */
 
  List *list_insert (List *head, int nth, void *udata);

/**
 * Similar to ::list_insert, but determines `nth' based on the result of the
 * comparison function `func'.
 */
 
  List *list_insert_sorted (List *head, CompareFunc func, void *udata);

  List *list_unite_sorted (List *list1, CompareFunc func, List *list2);
/*****************************************************************************/

/**
 * Search for and remove the element inserted with the data described by
 * `udata'.
 */
 
  List *list_remove (List *head, void *udata);

/**
 * Similar to ::list_remove, but will not search for the links position.  The
 * memory pointed to by `link' will be freed by this call.
 */
 
  List *list_remove_link (List *head, List *link);

#if 0
/**
 * Similar to ::list_remove_link, but the memory pointed to by `link' will not
 * be freed.
 */
 
  List *list_unlink_link (List *head, List *link);
#endif

/*****************************************************************************/

/**
 * Walk through the list pointed to by `head' freeing each individual link
 * pointer.  This will not affect the data added by the user, which you will
 * need to manage yourself.  Consider ::list_foreach_remove for an alternate
 * interface.
 */
 
  List *list_free (List *head);

/*****************************************************************************/

/**
 * Search for the first list link found with the user data element matching
 * `udata'.
 */
 
  List *list_find (List *head, void *udata);

/**
 * Similar to ::list_find, except that comparison of your search data against
 * the data in the list will be subject to the comparison function `func',
 * instead of the default pointer comparison.
 */
 
  List *list_find_custom (List *head, void *udata, CompareFunc func);

/*****************************************************************************/

/**
 * Iterate through each node in the list described by `head' calling the
 * iterator function `func' for each link chain encountered.
 */
 
  void list_foreach (List *head, ListForeachFunc func, void *udata);

/**
 * Combination of ::list_foreach and ::list_free, allowing the user the
 * chance to manage the memory of each individual link chain and the
 * associated user data.  See ::ListForeachFunc for more information on the
 * various options you have.
 */
 
  List *list_foreach_remove (List *head, ListForeachFunc func, void *udata);

/*****************************************************************************/

/**
 * Access the link chain at the nth position in the list described by `head'.
 */
 
  List *list_nth (List *head, int nth);

/**
 * Similar to ::list_nth, except that the link chains user data element is
 * returned instead of the link chain itself.
 */
 
  void *list_nth_data (List *head, int nth);

/**
 * Iterate through the list described by `head' until the tail is reached, in
 * which case it will be returned to the caller.
 */
 
  List *list_last (List *head);

/*****************************************************************************/

/**
 * Determine the length of the list described by `head' through iteration.
 */
 
  int list_length (List *head);

/**
 * Sort the list described by `head' according to the comparison function
 * `func'.  If NULL is used for the comparison function, the default string
 * comparison will be used via strcmp.
 */
 
  List *list_sort (List *head, CompareFunc func);

/**
 * Allocate a copy of each individual node in the list decsribed by `head'.
 * This will not affect the data added by the user.
 */
 
  List *list_copy (List *head);

/**************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* __GIFT_LIST_H__ */



