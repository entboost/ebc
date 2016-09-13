/* This file provides a replacement for the memmem library function.
   Copyright (C) 2004 Martin Dickopp

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This file is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this file; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
   USA.  */

#include <stddef.h>



/* This function implements the Horspool algorithm.
   It is assumed that chars have eight bits.  */
void *memmem (const void *const haystack, const size_t haystack_len,
	      const void *const needle, const size_t needle_len)
{
    const unsigned char *const haystack_endptr = (const unsigned char *)haystack + haystack_len;
    const unsigned char *const needle_endptr = (const unsigned char *)needle + needle_len;
    const unsigned char *haystack_shifting_ptr;
    size_t occurrence [256];


    /* Compute occurrence function.  */
    {
        const unsigned char *needle_ptr;
        size_t i;

        for (i = 0; i < sizeof occurrence / sizeof occurrence [0]; ++i)
            occurrence [i] = 0;
        for (i = 1, needle_ptr = needle; i < needle_len; ++i)
            occurrence [*needle_ptr++] = i;
    }


    /* Matching algorithm.  */
    haystack_shifting_ptr = (const unsigned char *)haystack;

    while ((size_t)(haystack_endptr - haystack_shifting_ptr) >= needle_len)
    {
        const unsigned char *haystack_ptr = haystack_shifting_ptr + needle_len;
        const unsigned char *needle_ptr = needle_endptr;
        size_t len = needle_len;

        while (len > 0 && *--haystack_ptr == *--needle_ptr)
            --len;

        if (len == 0)
            return (void *)haystack_ptr;

        haystack_shifting_ptr += needle_len - occurrence [haystack_shifting_ptr [needle_len - 1]];
    }


    return 0;
}
