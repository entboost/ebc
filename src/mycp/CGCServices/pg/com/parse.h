/*
 * $Id: parse.h,v 1.22 2003/10/16 18:50:54 jasta Exp $
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

#ifndef __GIFT_PARSE_H__
#define __GIFT_PARSE_H__

/*****************************************************************************/

/**
 * @file parse.h
 *
 * @brief Standard C library string wrappers and extensions to facilitate
 *        string parsing.
 */

/*****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/

/**
 * Similar to sprintf, except that the buffer is replaced by a static
 * character array.  This provides a more elegant high-level interface to
 * formatted text.
 *
 * @return Pointer to an internally managed static buffer or NULL on error.
 */
 
  char *stringf (const char *fmt, ...);

/**
 * Similar to ::stringf, except that the memory returned will be allocated
 * and management will be handed over to the caller.  This is useful when you
 * need multiple formatted buffers or you wish to be threadsafe (and too lazy
 * to use sprintf).
 */
 
  char *stringf_dup (const char *fmt, ...);

/*****************************************************************************/

/**
 * Apply the standard toupper function on each character in the provided
 * string.  The original calling argument `s' will be returned for your
 * convenience only.
 */
 
  char *string_upper (char *s);

/**
 * Identical to ::string_upper, except that tolower will be used, obviously.
 */
 
  char *string_lower (char *s);

/*****************************************************************************/

/**
 * Similar to ::gift_strdup, except that an exact number of bytes will be
 * copied from the source string.
 *
 * @param s
 * @param len  Number of characters to copy.
 *
 * @return Dynamically allocated NUL-terminated string of at least len + 1
 *         bytes.
 */
 
  char *gift_strndup (const char *s, size_t len);
#define STRDUP_N gift_strndup

/**
 * Wrapper for strncpy that does not suck.  Main difference includes the
 * guarantee that a NUL byte will be written at the end of the destination
 * string.
 *
 * @param dst
 * @param src
 * @param len  Total number of bytes to copy.  The buffer must be at least
 *             one byte larger than this value to write the NUL byte.
 *
 * @return Pointer to `dst'.
 */
 
  char *gift_strncpy (char *dst, const char *src, size_t len);
#define STRNCPY gift_strncpy

/*****************************************************************************/

/**
 * Prevents strlen(NULL) from causing a segmentation violation.  Usage is
 * recommended through the ::STRLEN macro.
 *
 * @return The length of the string according to strlen, or 0 if `s' is NULL.
 */
 
  size_t gift_strlen (const char *s);
#define STRLEN(str) gift_strlen(str)

/**
 * Similar to ::gift_strlen, except that the trailing NUL will be counted as
 * a character.  This is useful when you wish to determine the actual buffer
 * size, not the length, such as for binary protocols.
 */
 
  size_t gift_strlen0 (const char *s);
#define STRLEN_0(str) gift_strlen0(str)

/*****************************************************************************/

/**
 * Wrapper around strcmp which will not attempt to compare either calling
 * argument if passed as NULL.  s1 is considered greater than s2 if s2 is
 * NULL, and vice versa.  If both are NULL, s1 is considered greater, as a
 * convenience to the caller.
 */
 
  int gift_strcmp (const char *s1, const char *s2);
#define STRCMP gift_strcmp

/**
 * @see gift_strcmp
 */
 
  int gift_strcasecmp (const char *s1, const char *s2);
#define STRCASECMP gift_strcasecmp

/*****************************************************************************/

/**
 * ANSI C portable wrapper to the functionality that strtol provides.
 *
 * @return Computed signed integer value.  Zero is returned on error, and is
 *         indistinguishable from a successful conversion of the integer zero.
 */
 
  long gift_strtol (const char *str);
#define ATOI gift_strtol               /* grumble */
#define ATOL gift_strtol

/**
 * @see gift_strtol
 */
 
  unsigned long gift_strtoul (const char *str);
#define ATOUL gift_strtoul

/**
 * Very silly interface for reverse string-to-integer conversion.  Consider
 * using ::stringf and friends instead.
 */
 
  char *gift_ltostr (long integer);
#define ITOA gift_ltostr
#define LTOA gift_ltostr

/**
 * @see gift_ltostr
 */
 
  char *gift_ultostr (unsigned long integer);
#define ULTOA gift_ultostr

/*****************************************************************************/

/**
 * Mimic memmove's functionality with NUL-terminated strings.
 */
 
  char *gift_strmove (char *dst, const char *src);
#define string_move gift_strmove

/*****************************************************************************/

/**
 * Determines whether or not the contents of the calling argument contains
 * non-NUL or newline data.  This function is useful when examining strings
 * that may be "" or "\n", but that's about it.
 *
 * @return Boolean determination of the emptiness of the string.
 */
 
  int string_isempty (const char *s);

/**
 * Trims leading and trailing whitespace.  Please note that in order to avoid
 * returning an address into the supplied object, data is moved (via
 * ::gift_strmove) in order to begin the first non-whitespace character at
 * the 0th index.  The NUL terminated is then moved to one character beyond
 * the last non-whitespace character.
 *
 * @return Original pointer to `s', for convenience.
 */
 
  char *string_trim (char *s);

/**
 * Wrapper for strsep for portability and stability.  Please note that this
 * function is actually the one most similar to the actual strsep.
 */
 
  char *string_sep_set (char **string, const char *charset);

/**
 * Similar to ::string_sep_set, except that it is extended to allow for
 * delimiters greater than 1 character wide.  Multiple delimiters may not be
 * searched with this function, unfortunately.
 */
 
  char *string_sep (char **string, const char *delim);

/*****************************************************************************/

/**
 * Special macro provided that will check if `str' contains only a NUL
 * character, in which case it will evaluate to NULL instead of the argument.
 * There is a practical use for this somewhere, I'm sure.
 */
#define STRING_NULL(str) (((str) && *(str)) ? ((char *)(str)) : ((char *)NULL))

/**
 * Opposite of ::STRING_NULL.  The str will never evaluate to NULL, opting
 * for an "empty string" (single NUL character) instead.
 */
#define STRING_NOTNULL(str) ((str) ? ((char *)(str)) : "")

/*****************************************************************************/

#ifdef __cplusplus
}
#endif

/*****************************************************************************/

#endif /* __GIFT_PARSE_H__ */
