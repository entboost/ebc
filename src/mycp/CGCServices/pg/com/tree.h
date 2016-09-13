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

#ifndef __GILB_TREE_H__
#define __GILB_TREE_H__

#include "comtypes.h"
//#include <com/tree.h>

typedef struct _Tree  Tree;

typedef enum
{
    IN_ORDER,
    PRE_ORDER,
    POST_ORDER,
    LEVEL_ORDER
} TraverseType;

typedef int (*TraverseFunc) (void *  key,
			     void *  value,
			     void *  data);

/* Balanced binary trees
 */
Tree*   tree_new             (CompareFunc      key_compare_func);
Tree*   tree_new_with_data   (CompareDataFunc  key_compare_func,
			      void *          key_compare_data);
Tree*   tree_new_full        (CompareDataFunc  key_compare_func,
			      void *          key_compare_data,
			      DestroyNotify    key_destroy_func,
			      DestroyNotify    value_destroy_func);
void     tree_destroy         (Tree            *tree);
void     tree_insert          (Tree            *tree,
			       void *          key,
			       void *          value);
void     tree_replace         (Tree            *tree,
			       void *          key,
			       void *          value);
void     tree_remove          (Tree            *tree,
			       const void*     key);
void     tree_steal           (Tree            *tree,
			       const void*     key);
void * tree_lookup          (Tree            *tree,
			     const void*     key);
int tree_lookup_extended (Tree            *tree,
			  const void*     lookup_key,
			  void *         *orikey,
			  void *         *value);
void     tree_foreach         (Tree            *tree,
			       TraverseFunc	   func,
			       void *	   user_data);

#ifndef DISABLE_DEPRECATED
void     tree_traverse        (Tree            *tree,
			       TraverseFunc     traverse_func,
			       TraverseType     traverse_type,
			       void *          user_data);
#endif /* DISABLE_DEPRECATED */

void * tree_search          (Tree            *tree,
			     CompareFunc      search_func,
			     const void*     user_data);
int     tree_height          (Tree            *tree);
int     tree_nnodes          (Tree            *tree);


#endif /* __GILB_TREE_H__ */

