/*
 * $Id: array.c,v 1.14 2003/07/02 23:06:10 jasta Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#include "array.h"

/*****************************************************************************/

#define INITIAL_SIZE 32                /* initial supported members */
#define UNSHIFT_BUF  8                 /* padding to optimize unshift */
#define DEADBEEF ((void *)0xdeadbeef)  /* debugging */

/*****************************************************************************/

static size_t set_size (Array **a, size_t nmemb)
{
	size_t size;                       /* total allocated size */
	size_t reqsize;                    /* total requested size */
	void **newar;

	/* assume that all offset elements must be preserved */
	reqsize = ((nmemb + (*a)->offs) * (sizeof (void *)));

	if (reqsize > (*a)->size)
	{
		/* double the real request size until we reach one large enough to
		 * hold reqsize elements */
		for (size = (*a)->size; size < reqsize; size *= 2)
		{
			if (size < 1)
				size = 1;
		}

		if (!(newar = realloc ((*a)->array, size)))
			return 0;

		(*a)->array = newar;
		(*a)->size = size;
	}

	return (*a)->size;
}

static Array *new_array (void)
{
	Array *a;

	if (!(a = calloc (1, sizeof (Array))))
		return NULL;

	if (!set_size (&a, INITIAL_SIZE))
	{
		array_unset (&a);
		return NULL;
	}

	return a;
}

Array *array_new (void *first, ...)
{
	Array  *a;
	va_list args;
	void   *element;

	if (!(a = new_array ()))
		return NULL;

	if (!first)
		return a;

	array_push (&a, first);

	va_start (args, first);

	for (;;)
	{
		if (!(element = va_arg (args, void *)))
			break;

		array_push (&a, element);
	}

	va_end (args);

	return a;
}

void array_unset (Array **a)
{
	if (!a || !(*a))
		return;

	free ((*a)->array);
	free ((*a));
	*a = NULL;
}

/*****************************************************************************/

void *array_push (Array **a, void *element)
{
	if (!a)
		return NULL;

	if (!(*a))
	{
		*a = array_new (element, NULL);
		return element;
	}

	if (!set_size (a, (*a)->nmemb + 1))
		return NULL;

	/* optimization used to quickly clean out old entries that have been
	 * completely shifted off and use their space for new insertions */
	if ((*a)->offs > 0 && array_count (a) == 0)
		(*a)->offs = 0;

	(*a)->array[(*a)->offs + (*a)->nmemb] = element;
	(*a)->nmemb++;

	return element;
}

void *array_pop (Array **a)
{
	void  *element;
	void **aptr;

	if (array_count (a) == 0)
		return NULL;

	(*a)->nmemb--;

	aptr = (*a)->array + (*a)->offs + (*a)->nmemb;
	element = *aptr;

#ifdef LIBGIFT_DEBUG
	assert (element != DEADBEEF);
	*aptr = DEADBEEF;
#endif /* LIBGIFT_DEBUG */

	return element;
}

/*****************************************************************************/

void *array_shift (Array **a)
{
	void  *element;
	void **aptr;

	if (array_count (a) == 0)
		return NULL;

	aptr = (*a)->array + (*a)->offs;
	element = *aptr;

#ifdef LIBGIFT_DEBUG
	assert (element != DEADBEEF);
	*aptr = DEADBEEF;
#endif /* LIBGIFT_DEBUG */

	(*a)->offs++;
	(*a)->nmemb--;

	return element;
}

void *array_unshift (Array **a, void *element)
{
	if (!a || !(*a))
		return array_push (a, element);

	if (!(*a)->offs)
	{
		/* allocate enough data for the extra padding we will be doing */
		if (!set_size (a, (*a)->nmemb + UNSHIFT_BUF))
			return NULL;

		/* we must make room in the buffer for this data */
		memmove ((*a)->array + UNSHIFT_BUF, (*a)->array,
		         (((*a)->nmemb + UNSHIFT_BUF) * sizeof (void *)));
		(*a)->offs = UNSHIFT_BUF;
	}

	(*a)->offs--;
	(*a)->array[(*a)->offs] = element;
	(*a)->nmemb++;

	return element;
}

/*****************************************************************************/

void *array_index (Array **a, int index)
{
	if (!a || !(*a))
		return NULL;

	if (index < 0 || index >= ((*a)->nmemb))
		return NULL;

	return ((*a)->array[(*a)->offs + index]);
}

size_t array_count (Array **a)
{
	if (!a || !(*a))
		return 0;

	return ((*a)->nmemb);
}

/*****************************************************************************/

static int splice_rem (Array **a, int offset, int length)
{
	void **aptr;
	size_t movelen;

	if (length <= 0)
		return 0;

	/* assigned for convenience */
	aptr = (*a)->array + (*a)->offs + offset;

	/* decrement the total number of elements before hand to avoid
	 * duplicate subtraction */
	(*a)->nmemb -= length;

	/* prevent memmove (..., 0); */
	if ((movelen = ((*a)->nmemb - offset)))
		memmove (aptr, aptr + length, (movelen * sizeof (void *)));

	return length;
}

static int splice_add_elem (Array **a, int offset, void *element)
{
	void **aptr;
	size_t movelen;
	if (!set_size (a, (*a)->nmemb + 1))
		return 0;

	aptr = (*a)->array + (*a)->offs + offset;
	if ((movelen = ((*a)->nmemb - offset)))
		memmove (aptr + 1, aptr, (movelen * sizeof (void *)));

	(*a)->array[(*a)->offs + offset] = element;
	(*a)->nmemb++;

	return 1;
}

static int splice_add (Array **a, int offset, void *first, va_list args)
{
	int   soffset;
	void *element;

	if (!first)
		return 0;

	/* save a copy of the offset so that we can track how many items we
	 * inserted for the return value */
	soffset = offset;

	splice_add_elem (a, offset++, first);

	while ((element = va_arg (args, void *)))
		splice_add_elem (a, offset++, element);

	return (offset - soffset);
}

void *array_splice (Array **a, int offset, int length, void *first, ...)
{
	va_list args;

	if (!a)
		return NULL;

	if (!(*a))
	{
		/* we already know there's nothing to remove, so if the caller has
		 * supplied nothing to add, we know this entire operation will be
		 * a no-op */
		if (!first)
			return NULL;

		/* construct an empty array so that we dont have to worry about NULL
		 * conditions below */
		if (!(*a = array_new (NULL)))
			return NULL;
	}

	/* If OFFSET is negative then it starts that far from the end of the
	 * array */
	if (offset < 0)
		offset += (*a)->nmemb;

	/* If LENGTH is negative, leaves that many elements off the end of the
	 * array */
	if (length < 0)
		length += (*a)->nmemb - offset;

	/* prevent underflow */
	assert (offset >= 0);
	assert (length >= 0);

	/* prevent overflow */
	assert (offset < (*a)->nmemb);
	assert (length <= (*a)->nmemb - offset);

	splice_rem (a, offset, length);

	va_start (args, first);
	splice_add (a, offset, first, args);
	va_end (args);

	return ((*a)->array[(*a)->offs + offset]);
}

/*****************************************************************************/

int array_list (Array **a, ...)
{
	va_list args;
	int     ndata;
	void  **addr;

	va_start (args, a);

	for (ndata = 0 ;; ndata++)
	{
		if (!(addr = va_arg (args, void **)))
			break;

		if (ndata > (*a)->nmemb)
			*addr = NULL;
		else
			*addr = (*a)->array[(*a)->offs + ndata];
	}

	va_end (args);

	return ndata;
}

/*****************************************************************************/

#if 0
int main ()
{
	Array *a = array ("foo", "bar", NULL);
	char  *str1;
	char  *str2;

	printf ("FOO = %s\n", (char *)splice (&a, 0, 1, "FOO", NULL));
	printf ("Bar = %s\n", (char *)splice (&a, 1, 1, "Bar", NULL));

	printf ("FOO = %s\n", (char *)splice (&a, 0, 0, NULL));
	printf ("Bar = %s\n", (char *)splice (&a, 1, 0, NULL));

	array_splice (&a, 0, 2, "Foo", "BAR", NULL);

	array_push (&a, "baz");
	array_push (&a, "test");
	array_push (&a, "ing");

	str1 = pop (&a);
	str2 = pop (&a);

	printf ("the %s is work%s\n", str2, str1);

	array_unshift (&a, "prefoo");
	array_unshift (&a, "prebar");

	str1 = array_shift (&a);
	str2 = array_shift (&a);

	printf ("prebar = %s, prefoo = %s\n", str1, str2);

	array_push (&a, "postfoo");
	array_push (&a, "postbar");

	array_list (&a, &str1, &str2, NULL);
	printf ("Foo = %s, BAR = %s\n", str1, str2);

	while ((str1 = array_shift (&a)))
		printf ("%s\n", str1);

	array_unset (&a);

	return 0;
}
#endif
