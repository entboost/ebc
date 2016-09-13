/*
 * $Id: list.c,v 1.17 2003/12/23 03:51:31 jasta Exp $
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

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"


/*****************************************************************************/

List *list_nth (List *list, int n)
{
	int dir;

	if (!list || n == 0)
		return list;

	/*
	 * Translate n to a direction component indicating the stepping for n to
	 * reach 0.  That is, if we need to proceed forward in the list (n > 0),
	 * dir must be -1.
	 */
	dir = 0 - (CLAMP (n, -1, 1));
	assert (dir != 0);

	for (; n != 0; n += dir)
	{
		/* iterate the list according to `dir' making sure to bail if n
		 * corresponds to a list index which is not present in this list */
		if (!(list = ((dir < 0) ? (list_next (list)) : (list_prev (list)))))
			break;
	}

	return list;
}

void *list_nth_data (List *list, int n)
{
	List *nth;

	if (!(nth = list_nth (list, n)))
		return NULL;

	return nth->data;
}

List *list_last (List *list)
{
	List *ptr;

	if (!list)
		return NULL;

	for (ptr = list; ptr->next; ptr = ptr->next);

	return ptr;
}

int list_length (List *list)
{
	int len;

	if (!list)
		return 0;

	for (len = 0; list; list = list_next (list))
		len++;

	return len;
}

/*****************************************************************************/

static List *list_new_entry (List *list, void *data)
{
	List *entry;

	if ((entry = malloc (sizeof (List))))
	{
		entry->data = data;
		entry->prev = NULL;
		entry->next = NULL;
	}

	return entry;
}

List *list_append_link (List *list, List *link)
{
	List *tail;

	if (!link)
		return list;

	if (!list)
		return link;

	tail = list_last (list);
	assert (tail != NULL);

	link->prev = tail;
	tail->next = link;

	return list;
}

List *list_append (List *list, void *data)
{
	List *entry;

	entry = list_new_entry (list, data);
	assert (entry != NULL);

	return list_append_link (list, entry);
}

static List *list_prepend_link (List *list, List *link)
{
	if (!link)
		return list;

	if (!list)
		return link;

	list->prev = link;
	link->next = list;

	return link;
}

List *list_prepend (List *list, void *data)
{
	List *entry;

	entry = list_new_entry (list, data);
	assert (entry != NULL);

	return list_prepend_link (list, entry);
}

List *list_insert (List *list, int index, void *data)
{
	List *entry;
	List *nth;

	/* handle the easy conditions :) */
	if (!list || index <= 0)
		return list_prepend (list, data);

	nth = list_nth (list, index);

	if (nth && !nth->prev)             /* beginning of list */
		return list_prepend (list, data);
	else if (!nth)                     /* end of list */
		return list_append (list, data);

	/* middle of list */
	entry = list_new_entry (list, data);
	assert (entry != NULL);

	entry->next = nth;
	entry->prev = nth->prev;

	nth->prev->next = entry;
	nth->prev       = entry;

	return list;
}

/*****************************************************************************/

static int list_sort_default (char *a, char *b)
{
	return strcmp (a, b);
}

List *list_insert_sorted (List *list, CompareFunc func, void *data)
{
	List *ptr;
	int   index = 0;

	if (!list)
		return list_insert (list, 0, data);

	if (!func)
		func = (CompareFunc) list_sort_default;

	for (ptr = list; ptr; ptr = list_next (ptr), index++)
	{
		if (func (ptr->data, data) < 0)
			continue;

		return list_insert (list, index, data);
	}

	return list_append (list, data);
}

List *list_unite_sorted (List *list1, CompareFunc func, List *list2)
{
	List *head, *ptr;

	if (!list1)
		return list2;
	if (!list2)
		return list1;

	if (!func)
		func = (CompareFunc)list_sort_default;

	if (func(list1->data, list2->data) < 0) {
		head = ptr = list1;
		list1 = list1->next;
	} else {
		head = ptr = list2;
		list2 = list2->next;
	}

	while (list1 && list2) {
		if (func(list1->data, list2->data) < 0) {
			ptr->next = list1;
			list1->prev = ptr;
			list1 = list1->next;
		} else {
			ptr->next = list2;
			list2->prev = ptr;
			list2 = list2->next;
		}
		ptr = ptr->next;
	}

	if(list1) {
		ptr->next = list1;
		list1->prev = ptr;
	}
	else {
		ptr->next = list2;
		list2->prev = ptr;
	}

	return head;
}

/*****************************************************************************/

List *list_copy (List *list)
{
	List *new_list = NULL;
	List *ptr;

	for (ptr = list; ptr; ptr = list_next (ptr))
		new_list = list_append (new_list, ptr->data);

	return new_list;
}

/*****************************************************************************/

static List *list_unlink_link (List *list, List *link)
{
	List *prev;
	List *next;

	if (!list || !link)
		return list;

	prev = link->prev;
	next = link->next;

	if (prev)
		prev->next = next;
	else
		list = next;

	if (next)
		next->prev = prev;

	return list;
}

List *list_remove_link (List *list, List *link)
{
	list = list_unlink_link (list, link);
	free (link);

	return list;
}

List *list_remove (List *list, void *data)
{
	List *link;

	if (!list)
		return NULL;

	link = list_find (list, data);

	return list_remove_link (list, link);
}

/*****************************************************************************/

List *list_free (List *list)
{
	List *ptr = list;
	List *next;

	if (!list)
		return NULL;

	while (ptr)
	{
		next = ptr->next;
		free (ptr);
		ptr = next;
	}

	return NULL;
}

/*****************************************************************************/

static int find_custom_default (void *a, void *b)
{
	return (a != b);
}

List *list_find_custom (List *list, void *data, CompareFunc func)
{
	if (!func)
		func = (CompareFunc) find_custom_default;

	for (; list; list = list_next (list))
		if (func (list->data, data) == 0)
			return list;

	return NULL;
}

List *list_find (List *list, void *data)
{
	return list_find_custom (list, data, NULL);
}

/*****************************************************************************/
/* copied from GLib w/ modifications */

static List *list_sort_merge (List *l1, List *l2, CompareFunc compare_func)
{
	List list, *l, *lprev;

	l = &list;
	lprev = NULL;

	while (l1 && l2)
    {
		if (compare_func (l1->data, l2->data) < 0)
        {
			l->next = l1;
			l = l->next;
			l->prev = lprev;
			lprev = l;
			l1 = l1->next;
        }
		else
		{
			l->next = l2;
			l = l->next;
			l->prev = lprev;
			lprev = l;
			l2 = l2->next;
        }
    }
	l->next = l1 ? l1 : l2;
	l->next->prev = l;

	return list.next;
}

List *list_sort (List *list, CompareFunc compare_func)
{
	List *l1, *l2;

	if (!list)
		return NULL;
	if (!list->next)
		return list;

	l1 = list;
	l2 = list->next;

	while ((l2 = l2->next) != NULL)
    {
		if ((l2 = l2->next) == NULL)
			break;
		l1 = l1->next;
    }
	l2 = l1->next;
	l1->next = NULL;

	return list_sort_merge (list_sort (list, compare_func),
	                        list_sort (l2,   compare_func),
	                        compare_func);
}

/*****************************************************************************/

void list_foreach (List *list, ListForeachFunc func, void *udata)
{
	List *ptr;

	for (ptr = list; ptr; ptr = list_next (ptr))
	{
		(*func) (ptr->data, udata);
	}
}

static int remove_free (void *data, void *udata)
{
	free (data);
	return 1;
}

List *list_foreach_remove (List *list, ListForeachFunc func, void *udata)
{
	List *ptr;
	List *next;

	if (!list)
		return list;

	if (!func)
		func = (ListForeachFunc) remove_free;

	for (ptr = list; ptr; )
	{
		next = ptr->next;

		if ((*func) (ptr->data, udata))
			list = list_remove_link (list, ptr);

		ptr = next;
	}

	return list;
}

/*****************************************************************************/

#if 0
static int rfunc (char *str, void *udata)
{
	printf ("str = %s\n", str);
	return TRUE;
}

int main (int argc, char **argv)
{
	List *list = NULL;

	list = list_append (list, "foo");
	list = list_append (list, "bar");
	list = list_append (list, "baz");

	list = list_foreach_remove (list, LIST_FOREACH(rfunc), NULL);
	assert (list == NULL);

	return 0;
}
#endif

#if 0
static int moveall (char *a, List **newlist)
{
	if (strncmp (a, "audio", 5) == 0)
		*newlist = list_prepend (*newlist, a);

	return TRUE;
}

static int removeall (char *a, void *udata)
{
	return TRUE;
}

int main (int argc, char **argv)
{
	List *list    = NULL;
	List *newlist = NULL;
	int   i       = 1000;

	while (i--)
		list = list_prepend (list, "audio/x-mpeg");

	list = list_foreach_remove (list, (ListForeachFunc)moveall, &newlist);
	assert (list == NULL);

	newlist = list_foreach_remove (newlist, (ListForeachFunc)removeall, NULL);
	assert (newlist == NULL);

	return 0;
}
#endif
