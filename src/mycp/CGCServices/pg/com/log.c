/*
 * $Id: log.c,v 1.49 2004/01/14 10:00:24 hipnod Exp $
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
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

#ifdef WIN32
#include "config_win32.h"
#else 
#include "config_unix.h"
#endif

#include "time.h"
#include "vsnprintf.h"
#include "array.h"
#include "log.h"

/*****************************************************************************/

/* reduces code duplication in each log function */
#define LOG_FORMAT(pfx,fmt)                                           \
	char    buf[4096];                                                \
	size_t  buf_len = 0;                                              \
	va_list args;                                                     \
	                                                                  \
	assert (pfx != NULL);                                             \
	assert (fmt != NULL);                                             \
	                                                                  \
	if (pfx)                                                          \
		buf_len = snprintf (buf, sizeof (buf) - 1, "%s", pfx);        \
	                                                                  \
	va_start (args, fmt);                                             \
	vsnprintf (buf + buf_len, sizeof (buf) - buf_len - 1, fmt, args); \
	va_end (args);

/*****************************************************************************/

/* output log data */
static LogOptions   log_options = GLOG_STDERR
#ifdef _DEBUG
	| GLOG_DEBUGGER
#endif
	; /* for platform_init errors */
static Array       *log_fds     = NULL;
static FILE        *log_file_fd = NULL;

/* horribly non-threadsafe macro abuse
 * TODO: plugin/protocol.h has a better approach, but im too lazy to clean
 * this up */
static char *trace_pfx   = NULL;
static char *trace_file  = "";
static int   trace_line  = 0;
static char *trace_func  = "";
static char *trace_extra = NULL;

/*****************************************************************************/

int log_init (LogOptions options, char *ident, int syslog_option, int facility,
              char *log_file)
{
	/* unset previous settings */
	log_cleanup ();

	/* initialize/filter the opts */
	log_options = GLOG_NONE;

#ifdef DEBUG
	log_options |= GLOG_DEBUG;
#endif /* DEBUG */

	if (options)
		log_options |= options;

	/* apply the opts */
	if (log_options & GLOG_STDERR)
		array_push (&log_fds, stderr);

	if (log_options & GLOG_STDOUT)
		array_push (&log_fds, stdout);

	if (log_options & GLOG_SYSLOG)
	{
#ifdef HAVE_SYSLOG_H
		openlog (ident, syslog_option, facility);
#elif defined(WIN32)
		/* TODO -- add ugly windows code to log to Event Log */
#endif /* WIN32 */
	}

	if (log_options & GLOG_FILE)
	{
		assert (log_file != NULL);
		assert (log_file_fd == NULL);

		/* t=cr/lf in win32 */
		if (!(log_file_fd = fopen (log_file, "w+t")))
		{
			fprintf (stderr, "Can't open %s: %s", log_file, "platform_error!");
			return 0;
		}

		array_push (&log_fds, log_file_fd);
	}

	return 1;
}

void log_cleanup ()
{
	array_unset (&log_fds);

	if (log_file_fd)
	{
		fclose (log_file_fd);
		log_file_fd = NULL;
	}

	if (log_options & GLOG_SYSLOG)
	{
#ifdef HAVE_SYSLOG_H
		closelog ();
#elif defined(WIN32)
		/* TODO -- add ugly windows code to log to Event Log */
#endif /* WIN32 */
	}

	log_options = GLOG_STDERR;
}

/*****************************************************************************/

static void print_timestamp (FILE *f)
{
	time_t     t;
	struct tm *lt;
	char       buf[16];

	/* hi rossta :) */
	t  = time (NULL);
	lt = localtime (&t);

	if (strftime (buf, sizeof (buf), "[%H:%M:%S]", lt) > 0)
		fprintf (f, "%s ", buf);
}

static void print_fd (FILE *f, char *message)
{
	fprintf (f, "%s\n", message);
	fflush (f);
}

/* TODO -- if not initialized, log_fd should be emulated as stderr here */
void log_print (int priority, char *message)
{
	int err_or_out = 0;
	int i = 0;

	for (i = 0; i < array_count (&log_fds); i++)
	{
		FILE *f;

		if (!(f = array_index (&log_fds, i)))
			continue;

		if (f == stdout || f == stderr)
			err_or_out = 1;

		print_timestamp (f);
		print_fd (f, message);
	}

	/* if the priority is as or more severe than critical, ensure that we
	 * write the message to stderr on top of the other logging paths */
	if (priority <= LOG_CRIT && !err_or_out)
		print_fd (stderr, message);

	if (log_options & GLOG_SYSLOG)
	{
#ifdef HAVE_SYSLOG_H
		syslog (priority, "%s", message);
#else
		/* TODO -- add ugly windows code to log to Event Log */
		if (!err_or_out)
			print_fd (stderr, message);
#endif /* !HAVE_SYSLOG_H */
	}

#if defined(WIN32)
	if (log_options & GLOG_DEBUGGER)
	{
		OutputDebugString (message);
		OutputDebugString ("\n");
	}
#endif /* WIN32 */
}

void log_info (const char *fmt, ...)
{
	LOG_FORMAT ("", fmt);
	log_print (LOG_INFO, buf);
}

void log_warn (const char *fmt, ...)
{
	LOG_FORMAT ("*** GIFT-WARNING: ", fmt);
	log_print (LOG_WARNING, buf);
}

void log_error (const char *fmt, ...)
{
	LOG_FORMAT ("*** GIFT-ERROR: ", fmt);
	log_print (LOG_ERR, buf);
}

void log_fatal (const char *fmt, ...)
{
	LOG_FORMAT ("*** GIFT-FATAL: ", fmt);
	log_print (LOG_CRIT, buf);
	log_print (LOG_CRIT,
	           "*** Often times more information can be found in the log "
	           "file or with the -v command line switch.");
}

/*****************************************************************************/

#ifdef DEBUG

void log_debug (const char *fmt, ...)
{
	LOG_FORMAT ("** gift-debug:    ", fmt);
	log_print (LOG_DEBUG, buf);
}

void log_trace_pfx (char *pfx, char *file, int line, char *func, char *extra)
{
	free (trace_pfx);
	trace_pfx   = strdup (pfx);

# ifndef WIN32
	trace_file  = file;
# else /* WIN32 */
	/* MSVC prepends the directory name */
	trace_file  = strrchr (file, '\\') ? strrchr (file, '\\') + 1 : file;
# endif /* !WIN32 */

	trace_line  = line;
	trace_func  = func;

	free (trace_extra);
	trace_extra = strdup (extra);
}

void log_trace (const char *fmt, ...)
{
	char     buf[4096];
	size_t   buf_len = 0;
	va_list  args;

	assert (fmt);

	if (trace_pfx)
	{
		buf_len += snprintf (buf + buf_len, sizeof (buf) - buf_len - 1,
		                     "%s", trace_pfx);
	}

	if (trace_extra)
	{
		buf_len += snprintf (buf + buf_len, sizeof (buf) - buf_len - 1,
		                     "[%s]: ", trace_extra);
	}

	buf_len += snprintf (buf + buf_len, sizeof (buf) - buf_len - 1,
	                     "%s:%i(%s): ", trace_file, trace_line, trace_func);

	va_start (args, fmt);
	vsnprintf (buf + buf_len, sizeof (buf) - buf_len - 1, fmt, args);
	va_end (args);

	log_print (LOG_DEBUG, buf);
}

void log_dump_memory (void *ptr, unsigned int len)
{
	unsigned int  i;
	char         *p = (char *)ptr;
	char          buf[256];
	char          hex[50];
	char          str[20];
	char         *p1;
	char         *p2;
	unsigned int  offset = 0;

#if 0
	static const char syms[256] =
		/*       0123456789ABCDEF  */
		/* 0 */ "................"
		/* 1 */ "................"
		/* 2 */ " !\"#$%&'()*+,-./"
		/* 3 */ "0123456789:;<=>?"
		/* 4 */ "@ABCDEFGHIJKLMNO"
		/* 5 */ "PQRSTUVWXYZ[\\]^_"
		/* 6 */ "abcdefghijklmno"
		/* 7 */ "pqrstuvwxyz{|}~."
		/* 8 */ "................"
		/* 9 */ "................"
		/* A */ "................"
		/* B */ "................"
		/* C */ "................"
		/* D */ "................"
		/* E */ "................"
		/* F */ "................";
#endif

	p1 = hex;
	p2 = str;

	for (i = 0; i < len; i++)
	{
		unsigned char c;

		if ((i % 16) == 0)
			offset = i;

		c = p[i];
		p1 += sprintf (p1, "%02x ", c);
		*p1 = '\0';

		/* *p2++ = syms[c]; */
		*p2++ = isprint (c) ? c : '.';
		*p2 = '\0';

		if (((i + 1) % 16) == 0)
		{
			sprintf (buf, "%04x: %-48.48s\t%-16.16s", offset, hex, str);
			log_print (LOG_DEBUG, buf);
			p1 = hex;
			p2 = str;
		}
	}

	if (p1 != hex)
	{
		sprintf (buf, "%04x: %-48.48s\t%-16.16s", offset, hex, str);
		log_print (LOG_DEBUG, buf);
	}
}

#endif /* DEBUG */

/*****************************************************************************/

#if 0
int main ()
{
	log_init (GLOG_STDERR, "log.c", 0, 0, NULL);
	log_trace ("%s %s", "foo", "bar", 0);
	log_cleanup ();
	return 0;
}
#endif
