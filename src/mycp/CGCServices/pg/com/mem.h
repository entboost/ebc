/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __GLIB_MEM_H__
#define __GLIB_MEM_H__

#include "comtypes.h"
#include <sys/types.h>


typedef struct _Allocator Allocator;
typedef struct _MemChunk  MemChunk;

//should be checked in configure.in file
#define MEM_ALIGN	4

#ifdef __cplusplus
extern "C" {
#endif

#define mem_chunk_create(type, pre_alloc, alloc_type)	( \
  mem_chunk_new (#type " mem chunks (" #pre_alloc ")", \
		   sizeof (type), \
		   sizeof (type) * (pre_alloc), \
		   (alloc_type)) \
)
#define chunk_new(type, chunk)	( \
  (type *) mem_chunk_alloc (chunk) \
)
#define chunk_new0(type, chunk)	( \
  (type *) mem_chunk_alloc0 (chunk) \
)
#define chunk_free(mem, mem_chunk)	STMT_START { \
  mem_chunk_free ((mem_chunk), (mem)); \


/* "mem_chunk_new" creates a new memory chunk.
 * Memory chunks are used to allocate pieces of memory which are
 *  always the same size. Lists are a good example of such a data type.
 * The memory chunk allocates and frees blocks of memory as needed.
 *  Just be sure to call "mem_chunk_free" and not "free" on data
 *  allocated in a mem chunk. ("free" will most likely cause a seg
 *  fault...somewhere).
 *
 * Oh yeah, MemChunk is an opaque data type. (You don't really
 *  want to know what's going on inside do you?)
 */

/* ALLOC_ONLY MemChunk's can only allocate memory. The free operation
 *  is interpreted as a no op. ALLOC_ONLY MemChunk's save 4 bytes per
 *  atom. (They are also useful for lists which use MemChunk to allocate
 *  memory but are also part of the MemChunk implementation).
 * ALLOC_AND_FREE MemChunk's can allocate and free memory.
 */

 MemChunk* mem_chunk_new     (const char *name,
			      int         atom_size,
			      unsigned long   number);
 void       mem_chunk_destroy (MemChunk   *mem_chunk);
 void*      mem_chunk_alloc   (MemChunk   *mem_chunk);
 void       mem_chunk_free    (MemChunk   *mem_chunk,
			       void*     mem);
 
 void mem_init();

#ifdef __cplusplus
}
#endif

#endif /* __GLIB_MEM_H__ */
