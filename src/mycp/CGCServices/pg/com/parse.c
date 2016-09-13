/*
 * $Id: parse.c,v 1.25 2003/11/15 19:07:52 jasta Exp $
 *
 * I admit this file is poorly named.  Most of the routines defined here are
 * actually string.h wrappers for portability or more convenient/reliable
 * interfaces.
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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef WIN32
#include "config_win32.h"
#endif

#include "vsnprintf.h"

#include "parse.h"

/*****************************************************************************/

/*
 * Of course I could just do = { 0 }, but where's the fun in that?
 * Plus, the standard doesn't guarantee they are equiv, IIRC.
 */
static unsigned char string_set[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*****************************************************************************/

#define STRINGF(buf,fmt) \
	char        buf[4096];                        \
	va_list     args;                             \
	                                              \
	va_start (args, fmt);                         \
	vsnprintf (buf, sizeof (buf) - 1, fmt, args); \
	va_end (args)

char *stringf (const char *fmt, ...)
{
	static STRINGF (buf, fmt);

	return buf;
}

char *stringfv (const char *fmt, va_list args)
{
	static char buf[4096];

	vsnprintf (buf, sizeof (buf) - 1, fmt, args);

	return buf;
}

char *stringf_dup (const char *fmt, ...)
{
	STRINGF (buf, fmt);

	/* wow, this is quite possibly the stupidest way i could've done this,
	 * sigh... oh well :) */
	return strdup (buf);
}

/*****************************************************************************/

static char *strctype (char *s, int (*cfunc) (int))
{
	char *ptr;

	if (!s || !cfunc)
		return NULL;

	for (ptr = s; *ptr; ptr++)
		*ptr = cfunc (*ptr);

	return s;
}

char *string_upper (char *s)
{
	return strctype (s, toupper);
}

char *string_lower (char *s)
{
	return strctype (s, tolower);
}

/*****************************************************************************/


char *gift_strndup (const char *str, size_t len)
{
	char *buf;

	if (!str || len == 0)
		return NULL;

	if (!(buf = malloc (len + 1)))
		return NULL;

	return gift_strncpy (buf, str, len);
}

char *gift_strncpy (char *dst, const char *src, size_t len)
{
	assert (dst != NULL);
	assert (src != NULL);
	assert (len > 0);

	strncpy (dst, src, len);
	dst[len] = 0;

	return dst;
}

static int string_cmp (int (*cmpfn) (const char *s1, const char *s2),
                       const char *s1, const char *s2)
{
	if (!s1)
		return 1;
	else if (!s2)
		return -1;

	return cmpfn (s1, s2);
}

int gift_strcmp (const char *s1, const char *s2)
{
	return string_cmp (strcmp, s1, s2);
}

int gift_strcasecmp (const char *s1, const char *s2)
{
#ifndef _MSC_VER
	return string_cmp (strcasecmp, s1, s2);
#else
	return string_cmp (_stricmp, s1, s2);
#endif
}

size_t gift_strlen (const char *str)
{
	if (!str)
		return 0;

	return strlen (str);
}

size_t gift_strlen0 (const char *str)
{
	if (!str)
		return 0;

	return (strlen (str) + 1);
}

/*****************************************************************************/

long gift_strtol (const char *str)
{
	if (!str)
		return 0;

	return strtol (str, NULL, 10);
}

unsigned long gift_strtoul (const char *str)
{
	if (!str)
		return 0;

	return strtoul (str, NULL, 10);
}

char *gift_ltostr (long integer)
{
	static char buf[32];

	snprintf (buf, sizeof (buf) - 1, "%li", integer);

	return buf;
}

char *gift_ultostr (unsigned long integer)
{
	static char buf[32];

	snprintf (buf, sizeof (buf) - 1, "%lu", integer);

	return buf;
}

/*****************************************************************************/

char *gift_strmove (char *dst, const char *src)
{
	if (!dst || !src)
		return dst;

	return memmove (dst, src, strlen (src) + 1);
}

/*****************************************************************************/

int string_isempty (const char *string)
{
	if (!string)
		return 1;

	/* this logic needs work */
	switch (*string)
	{
	 case '\r':
	 case '\n':
	 case '\0':
		return 1;
		break;
	}

	return 0;
}

char *string_trim (char *string)
{
	char *ptr;

	if (!string || !string[0])
		return string;

	/* skip leading whitespace */
	for (ptr = string; isspace (*ptr); ptr++);

	/* shift downward */
	if (ptr != string)
		string_move (string, ptr);

	if (!string[0])
		return string;

	/* look backwards */
	ptr = string + strlen (string) - 1;

	if (isspace (*ptr))
	{
		while (ptr >= string && isspace (*ptr))
			ptr--;

		ptr[1] = 0;
	}

	return string;
}

/*****************************************************************************/

typedef char* (*StringFunc) (char *string, const char *needle);

static char *string_sep_ex (char **string, const char *needle, size_t needle_len,
                            StringFunc search)
{
	char *iter, *str;

	if (!string || !*string || !**string)
		return NULL;

	str = *string;

	if ((iter = (*search) (str, needle)))
	{
		*iter = 0;
		iter += needle_len;
	}

	*string = iter;

	return str;
}

/*****************************************************************************/

static char *string_sep_set_func (char *string, const char *charset)
{
	unsigned char *str_ptr;
	unsigned char *ptr;

	for (ptr = (unsigned char *)charset; *ptr; ptr++)
		string_set[(int)*ptr] = 1;

	for (str_ptr = (unsigned char*)string; *str_ptr; str_ptr++)
	{
		if (string_set[(int)*str_ptr])
			break;
	}

	for (ptr = (unsigned char *)charset; *ptr; ptr++)
		string_set[(int)*ptr] = 0;

	if (!str_ptr[0])
		str_ptr = NULL;

	return (char*)str_ptr;
}

char *string_sep_set (char **string, const char *charset)
{
	return string_sep_ex (string, charset, sizeof (char), string_sep_set_func);
}

/*****************************************************************************/

char *string_sep (char **string, const char *delim)
{
	return string_sep_ex (string, delim, STRLEN(delim), (StringFunc)strstr);
}

/*****************************************************************************/

#if 0
int main ()
{
	char *test = strdup ("/music/paul_oakenfold!!!!/paul_oakenfold_-_perfecto_presents_'another_world'/cd1/08_-_paul_oakenfold_-_planet_perfecto_-_bullet_in_a_gun_[rabbit_in_the_moon_remix].mp3");
	char *token;

	while ((token = string_sep_set (&test, "\\/_-.[]()")))
	{
		char *ptr, *token_ptr;

		if (!token[0])
			continue;

		token_ptr = token;

		while ((ptr = string_sep_set (&token_ptr, ",`'!?*")))
		{
			if (!token_ptr)
				continue;

			strmove (token_ptr - 1, token_ptr);
			token_ptr--;
		}

		printf ("token = %s\n", token);
	}

	return 0;
}
#endif

#if 0
int main ()
{
	char *x = strdup ("GET /testing_this_stuff\r\n"
	                  "Key-Number-1: This is a sample value\r\n"
	                  "Key2: bla=bla\r\n"
	                  "Something: Something else\n"
	                  "User-Agent: Mozilla\r\n");
	char *token;

	while ((token = string_sep_set (&x, "\r\n")))
	{
		char *key;

		trim_whitespace (token);

		key = string_sep (&token, ": ");

		printf ("key = %s, value = %s\n", key, token);
	}

	return 0;
}
#endif
