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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
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

/* 
 * MT safe
 */

#include <stdlib.h>
#include "comtypes.h"
#include "mem.h"
#include "tree.h"

#ifdef WIN32
#include <Windows.h>
#endif

#include "lock_def.h"
//#include <pthread.h>


typedef struct _TreeNode  TreeNode;

struct _Tree
{
  TreeNode        *root;
  CompareDataFunc  key_compare;
  DestroyNotify    key_destroy_func;
  DestroyNotify    value_destroy_func;
  void            *key_compare_data;
};

struct _TreeNode
{
  int       balance;  /* height (left) - height (right) */
  TreeNode *left;     /* left subtree                   */
  TreeNode *right;    /* right subtree                  */
  void     *key;      /* key for this node              */
  void     *value;    /* value stored at this node      */
};

static TreeNode* tree_node_new(void *key, void *value);
static void      tree_node_destroy(TreeNode *node, 
				   DestroyNotify key_destroy_func, 
				   DestroyNotify  value_destroy_func);
static TreeNode* tree_node_insert(Tree            *tree,
				  TreeNode        *node,
				  void            *key,
				  void            *value,
				  int              replace,
				  int             *inserted);

static TreeNode* tree_node_remove(Tree            *tree,
				  TreeNode        *node,
				  const void*     key,
				  int             notify);

static TreeNode* tree_node_balance(TreeNode        *node);
static TreeNode* tree_node_remove_leftmost(TreeNode        *node,
					   TreeNode       **leftmost);
static TreeNode* tree_node_restore_left_balance(TreeNode        *node,
						int              old_balance);
static TreeNode* tree_node_restore_right_balance(TreeNode        *node,
						 int              old_balance);
static TreeNode* tree_node_lookup(TreeNode        *node,
				  CompareDataFunc  compare,
				  void            *comp_data,
				  const void      * key);
static int       tree_node_count   (TreeNode        *node);
static int       tree_node_pre_order(TreeNode       *node,
				     TraverseFunc    traverse_func,
				     void           *data);
static int       tree_node_in_order (TreeNode       *node,
				     TraverseFunc   traverse_func,
				     void          *data);
static int       tree_node_post_order        (TreeNode        *node,
					      TraverseFunc     traverse_func,
					      void            *data);
static void *   tree_node_search  (TreeNode        *node,
				   CompareFunc      search_func,
				   const void      *data);
static int       tree_node_height                (TreeNode        *node);
static TreeNode* tree_node_rotate_left           (TreeNode        *node);
static TreeNode* tree_node_rotate_right          (TreeNode        *node);
static void       tree_node_check                (TreeNode        *node);

/*
LOCK_DEFINE_STATIC (tree_global);
*/
//pthread_mutex_t   tree_global_lock = PTHREAD_MUTEX_INITIALIZER;
LOCK_T tree_global_lock;

static MemChunk *node_mem_chunk = 0;
static TreeNode *node_free_list = 0;


static TreeNode*
tree_node_new (void * key,
		 void * value)
{
  TreeNode *node;

  LOCK_INIT (&tree_global_lock);

  //pthread_mutex_lock(&tree_global_lock);
  LOCK (&tree_global_lock);

  if (node_free_list)
    {
      node = node_free_list;
      node_free_list = node->right;
    }
  else
    {
      if (!node_mem_chunk)
	node_mem_chunk = mem_chunk_new ("GLib TreeNode mem chunk",
					  sizeof (TreeNode),
					500);

      node = chunk_new (TreeNode, node_mem_chunk);
   }

  //pthread_mutex_unlock(&tree_global_lock);
  UNLOCK (&tree_global_lock);

  node->balance = 0;
  node->left = 0;
  node->right = 0;
  node->key = key;
  node->value = value;

  return node;
}

static void
tree_node_destroy (TreeNode      *node,
		     DestroyNotify  key_destroy_func,
		     DestroyNotify  value_destroy_func)
{
  if (node)
    {
      tree_node_destroy (node->right,
			   key_destroy_func, value_destroy_func);
      tree_node_destroy (node->left,
			   key_destroy_func, value_destroy_func);

      if (key_destroy_func)
	key_destroy_func (node->key);
      if (value_destroy_func)
	value_destroy_func (node->value);
      
#ifdef ENABLE_C_FRIENDLY
      node->left = 0;
      node->key = 0;
      node->value = 0;
#endif /* ENABLE_C_FRIENDLY */

      //pthread_mutex_lock(&tree_global_lock);
	  LOCK (&tree_global_lock);

      node->right = node_free_list;
      node_free_list = node;

      //pthread_mutex_unlock(&tree_global_lock);
	  UNLOCK (&tree_global_lock);

   }
}

/**
 * tree_new:
 * @key_compare_func: the function used to order the nodes in the #Tree.
 *   It should return values similar to the standard 
 *   <function>strcmp()</function> function -
 *   0 if the two arguments are equal, a negative value if the first argument 
 *   comes before the second, or a positive value if the first argument comes 
 *   after the second.
 * 
 * Creates a new #Tree.
 * 
 * Return value: a new #Tree.
 **/
Tree*
tree_new (CompareFunc key_compare_func)
{
  if(!key_compare_func)
    return 0;

  return tree_new_full ((CompareDataFunc) key_compare_func, 0,
                          0, 0);
}

/**
 * tree_new_with_data:
 * @key_compare_func: <function>qsort()</function>-style comparison function.
 * @key_compare_data: data to pass to comparison function.
 * 
 * Creates a new #Tree with a comparison function that accepts user data.
 * See tree_new() for more details.
 * 
 * Return value: a new #Tree.
 **/
Tree*
tree_new_with_data (CompareDataFunc key_compare_func,
 		      void *         key_compare_data)
{
  if(!key_compare_func)
    return 0;
  
  return tree_new_full (key_compare_func, key_compare_data, 
 			  0, 0);
}

/**
 * tree_new_full:
 * @key_compare_func: <function>qsort()</function>-style comparison function.
 * @key_compare_data: data to pass to comparison function.
 * @key_destroy_func: a function to free the memory allocated for the key 
 *   used when removing the entry from the #Tree or %0 if you don't
 *   want to supply such a function.
 * @value_destroy_func: a function to free the memory allocated for the 
 *   value used when removing the entry from the #Tree or %0 if you 
 *   don't want to supply such a function.
 * 
 * Creates a new #Tree like tree_new() and allows to specify functions 
 * to free the memory allocated for the key and value that get called when 
 * removing the entry from the #Tree.
 * 
 * Return value: a new #Tree.
 **/
Tree*	 
tree_new_full (CompareDataFunc key_compare_func,
 		 void *         key_compare_data, 		 
                 DestroyNotify   key_destroy_func,
 		 DestroyNotify   value_destroy_func)
{
  Tree *tree;
  
  if(!key_compare_func)
    return 0;
  
  tree = (Tree*)malloc(sizeof(Tree));
  tree->root               = 0;
  tree->key_compare        = key_compare_func;
  tree->key_destroy_func   = key_destroy_func;
  tree->value_destroy_func = value_destroy_func;
  tree->key_compare_data   = key_compare_data;
  
  return tree;
}

/**
 * tree_destroy:
 * @tree: a #Tree.
 * 
 * Destroys the #Tree. If keys and/or values are dynamically allocated, you 
 * should either free them first or create the #Tree using tree_new_full().
 * In the latter case the destroy functions you supplied will be called on 
 * all keys and values before destroying the #Tree.
 **/
void
tree_destroy (Tree *tree)
{
  if(!tree)
    return;

  tree_node_destroy (tree->root,
                       tree->key_destroy_func,
 		       tree->value_destroy_func);

  free (tree);
}

/**
 * tree_insert:
 * @tree: a #Gtree.
 * @key: the key to insert.
 * @value: the value corresponding to the key.
 * 
 * Inserts a key/value pair into a #Tree. If the given key already exists 
 * in the #Tree its corresponding value is set to the new value. If you 
 * supplied a value_destroy_func when creating the #Tree, the old value is 
 * freed using that function. If you supplied a @key_destroy_func when 
 * creating the #Tree, the passed key is freed using that function.
 *
 * The tree is automatically 'balanced' as new key/value pairs are added,
 * so that the distance from the root to every leaf is as small as possible.
 **/
void
tree_insert (Tree    *tree,
	       void *  key,
	       void *  value)
{
  int   inserted;

  if(!tree)
    return;

  inserted = 0;
  tree->root = tree_node_insert (tree,
                                   tree->root,
				   key, value, 
				   0, &inserted);
}

/**
 * tree_replace:
 * @tree: a #Gtree.
 * @key: the key to insert.
 * @value: the value corresponding to the key.
 * 
 * Inserts a new key and value into a #Tree similar to tree_insert(). 
 * The difference is that if the key already exists in the #Tree, it gets 
 * replaced by the new key. If you supplied a @value_destroy_func when 
 * creating the #Tree, the old value is freed using that function. If you 
 * supplied a @key_destroy_func when creating the #Tree, the old key is 
 * freed using that function. 
 *
 * The tree is automatically 'balanced' as new key/value pairs are added,
 * so that the distance from the root to every leaf is as small as possible.
 **/
void
tree_replace (Tree    *tree,
		void *  key,
		void *  value)
{
  int   inserted;
  if(!tree)
    return;

  inserted = 0;
  tree->root = tree_node_insert (tree,
                                   tree->root,
				   key, value, 
				   1, &inserted);
}

/**
 * tree_remove:
 * @tree: a #Gtree.
 * @key: the key to remove.
 * 
 * Removes a key/value pair from a #Tree.
 *
 * If the #Tree was created using tree_new_full(), the key and value 
 * are freed using the supplied destroy functions, otherwise you have to 
 * make sure that any dynamically allocated values are freed yourself.
 **/
void
tree_remove (Tree         *tree,
	       const void*  key)
{
  if(!tree)
    return;

  tree->root = tree_node_remove (tree, tree->root, key, 1);
}

/**
 * tree_steal:
 * @tree: a #Gtree.
 * @key: the key to remove.
 * 
 * Removes a key and its associated value from a #Tree without calling 
 * the key and value destroy functions.
 **/
void
tree_steal (Tree         *tree,
              const void*  key)
{
  if(!tree)
    return;

  tree->root = tree_node_remove (tree, tree->root, key, 0);
}

/**
 * tree_lookup:
 * @tree: a #Tree.
 * @key: the key to look up.
 * 
 * Gets the value corresponding to the given key. Since a #Tree is 
 * automatically balanced as key/value pairs are added, key lookup is very 
 * fast.
 *
 * Return value: the value corresponding to the key.
 **/
void *
tree_lookup (Tree         *tree,
	       const void*  key)
{
  TreeNode *node;

  if(!tree)
    return 0;

  node = tree_node_lookup (tree->root, 
                             tree->key_compare, tree->key_compare_data, key);

  return node ? node->value : 0;
}

/**
 * tree_lookup_extended:
 * @tree: a #Tree.
 * @lookup_key: the key to look up.
 * @orikey: returns the original key.
 * @value: returns the value associated with the key.
 * 
 * Looks up a key in the #Tree, returning the original key and the
 * associated value and a #int which is %1 if the key was found. This 
 * is useful if you need to free the memory allocated for the original key, 
 * for example before calling tree_remove().
 * 
 * Return value: %1 if the key was found in the #Tree.
 **/
int
tree_lookup_extended (Tree         *tree,
                        const void*  lookup_key,
                        void *      *orikey,
                        void *      *value)
{
  TreeNode *node;

  if(!tree)
    return 0;
  
  node = tree_node_lookup (tree->root, 
                             tree->key_compare, tree->key_compare_data, lookup_key);

  if (node)
    {
      if (orikey)
        *orikey = node->key;
      if (value)
        *value = node->value;
      return 1;
    }
  else
    return 0;
}

/**
 * tree_foreach:
 * @tree: a #Tree.
 * @func: the function to call for each node visited. If this function
 *   returns %1, the traversal is stopped.
 * @user_data: user data to pass to the function.
 * 
 * Calls the given function for each of the key/value pairs in the #Tree.
 * The function is passed the key and value of each pair, and the given
 * @data parameter. The tree is traversed in sorted order.
 *
 * The tree may not be modified while iterating over it (you can't 
 * add/remove items). To remove all items matching a predicate, you need 
 * to add each item to a list in your #TraverseFunc as you walk over 
 * the tree, then walk the list and remove each item.
 **/
void
tree_foreach (Tree         *tree,
                TraverseFunc  func,
                void *       user_data)
{
  if(tree && tree->root) 
    tree_node_in_order (tree->root, func, user_data);
}

/**
 * tree_traverse:
 * @tree: a #Tree.
 * @traverse_func: the function to call for each node visited. If this 
 *   function returns %1, the traversal is stopped.
 * @traverse_type: the order in which nodes are visited, one of %IN_ORDER,
 *   %PRE_ORDER and %POST_ORDER.
 * @user_data: user data to pass to the function.
 * 
 * Calls the given function for each node in the #Tree. This function is
 * deprecated, since the order of a balanced tree is somewhat arbitrary.
 * If you just want to visit all nodes in sorted order, use tree_foreach() 
 * instead. If you really need to visit nodes in a different order, consider
 * using an <link linkend="glib-N-ary-Trees">N-ary Tree</link>.
 **/
void
tree_traverse (Tree         *tree,
		 TraverseFunc  traverse_func,
		 TraverseType  traverse_type,
		 void *       user_data)
{
  if(tree && tree->root) {
    switch (traverse_type)
      {
      case PRE_ORDER:
	tree_node_pre_order (tree->root, traverse_func, user_data);
	break;
	
      case IN_ORDER:
	tree_node_in_order (tree->root, traverse_func, user_data);
	break;
	
      case POST_ORDER:
	tree_node_post_order (tree->root, traverse_func, user_data);
	break;
	
      case LEVEL_ORDER:
	/*
	warning ("tree_traverse(): traverse type LEVEL_ORDER isn't implemented.");
	*/
	break;
      }
  }
}

/**
 * tree_search:
 * @tree: a #Tree.
 * @search_func: the comparison function used to search the #Tree. 
 * @user_data: the data passed as the second argument to the @search_func 
 * function.
 * 
 * Searches a #Tree using an alternative form of the comparison function.
 *
 * This function is not as useful as it sounds.
 * It allows you to use a different function for performing the lookup of
 * a key. However, since the tree is ordered according to the @key_compare_func
 * function passed to tree_new(), the function you pass to tree_search() 
 * must return exactly the same value as would be returned by the comparison 
 * function, for each pair of tree nodes, or the search will not work.
 * 
 * To search for a specific value, you can use tree_foreach().
 *
 * Return value: the value corresponding to the found key, or %0 if the key 
 * is not found.
 **/
void *
tree_search (Tree         *tree,
	       CompareFunc   search_func,
	       const void*  user_data)
{
  if(tree && tree->root) 
    return tree_node_search (tree->root, search_func, user_data);
  else
    return 0;
}

/**
 * tree_height:
 * @tree: a #Tree.
 * 
 * Gets the height of a #Tree.
 *
 * If the #Tree contains no nodes, the height is 0.
 * If the #Tree contains only one root node the height is 1.
 * If the root node has children the height is 2, etc.
 * 
 * Return value: the height of the #Tree.
 **/
int
tree_height (Tree *tree)
{
  if(tree && tree->root)
    return tree_node_height (tree->root);
  else
    return 0;
}

/**
 * tree_nnodes:
 * @tree: a #Tree.
 * 
 * Gets the number of nodes in a #Tree.
 * 
 * Return value: the number of nodes in the #Tree.
 **/
int
tree_nnodes (Tree *tree)
{

  if (tree && tree->root)
    return tree_node_count (tree->root);
  else
    return 0;
}

static TreeNode*
tree_node_insert (Tree     *tree,
                    TreeNode *node,
		    void *   key,
		    void *   value,
                    int   replace,
		    int  *inserted)
{
  int  old_balance;
  int  cmp;

  if (!node)
    {
      *inserted = 1;
      return tree_node_new (key, value);
    }

  cmp = tree->key_compare (key, node->key, tree->key_compare_data);
  if (cmp == 0)
    {
      *inserted = 0;

      if (tree->value_destroy_func)
	tree->value_destroy_func (node->value);

      node->value = value;
      
      if (replace)
	{
	  if (tree->key_destroy_func)
	    tree->key_destroy_func (node->key);

	  node->key = key;
	}
      else
	{
	  /* free the passed key */
	  if (tree->key_destroy_func)
	    tree->key_destroy_func (key);
	}

      return node;
    }

  if (cmp < 0)
    {
      if (node->left)
	{
	  old_balance = node->left->balance;
	  node->left = tree_node_insert (tree,
                                           node->left,
					   key, value,
					   replace, inserted);

	  if ((old_balance != node->left->balance) && node->left->balance)
	    node->balance -= 1;
	}
      else
	{
	  *inserted = 1;
	  node->left = tree_node_new (key, value);
	  node->balance -= 1;
	}
    }
  else if (cmp > 0)
    {
      if (node->right)
	{
	  old_balance = node->right->balance;
	  node->right = tree_node_insert (tree,
                                            node->right,
					    key, value, 
					    replace, inserted);

	  if ((old_balance != node->right->balance) && node->right->balance)
	    node->balance += 1;
	}
      else
	{
	  *inserted = 1;
	  node->right = tree_node_new (key, value);
	  node->balance += 1;
	}
    }

  if (*inserted)
    {
      if ((node->balance < -1) || (node->balance > 1))
	node = tree_node_balance (node);
    }

  return node;
}

static TreeNode*
tree_node_remove (Tree         *tree,
                    TreeNode     *node,
		    const void*  key,
                    int       notify)
{
  TreeNode *new_root;
  int old_balance;
  int cmp;

  if (!node)
    return 0;

  cmp = tree->key_compare (key, node->key, tree->key_compare_data);
  if (cmp == 0)
    {
      TreeNode *garbage;

      garbage = node;

      if (!node->right)
	{
	  node = node->left;
	}
      else
	{
	  old_balance = node->right->balance;
	  node->right = tree_node_remove_leftmost (node->right, &new_root);
	  new_root->left = node->left;
	  new_root->right = node->right;
	  new_root->balance = node->balance;
	  node = tree_node_restore_right_balance (new_root, old_balance);
	}

      if (notify)
        {
          if (tree->key_destroy_func)
            tree->key_destroy_func (garbage->key);
          if (tree->value_destroy_func)
            tree->value_destroy_func (garbage->value);
        }

#ifdef ENABLE_C_FRIENDLY
      garbage->left = 0;
      garbage->key = 0;
      garbage->value = 0;
#endif /* ENABLE_C_FRIENDLY */

      //pthread_mutex_lock(&tree_global_lock);
	  LOCK (&tree_global_lock);

      garbage->right = node_free_list;
      node_free_list = garbage;
      
	  //pthread_mutex_unlock(&tree_global_lock);
	  UNLOCK (&tree_global_lock);

   }
  else if (cmp < 0)
    {
      if (node->left)
	{
	  old_balance = node->left->balance;
	  node->left = tree_node_remove (tree, node->left, key, notify);
	  node = tree_node_restore_left_balance (node, old_balance);
	}
    }
  else if (cmp > 0)
    {
      if (node->right)
	{
	  old_balance = node->right->balance;
	  node->right = tree_node_remove (tree, node->right, key, notify);
	  node = tree_node_restore_right_balance (node, old_balance);
	}
    }

  return node;
}

static TreeNode*
tree_node_balance (TreeNode *node)
{
  if (node->balance < -1)
    {
      if (node->left->balance > 0)
	node->left = tree_node_rotate_left (node->left);
      node = tree_node_rotate_right (node);
    }
  else if (node->balance > 1)
    {
      if (node->right->balance < 0)
	node->right = tree_node_rotate_right (node->right);
      node = tree_node_rotate_left (node);
    }

  return node;
}

static TreeNode*
tree_node_remove_leftmost (TreeNode  *node,
			     TreeNode **leftmost)
{
  int old_balance;

  if (!node->left)
    {
      *leftmost = node;
      return node->right;
    }

  old_balance = node->left->balance;
  node->left = tree_node_remove_leftmost (node->left, leftmost);
  return tree_node_restore_left_balance (node, old_balance);
}

static TreeNode*
tree_node_restore_left_balance (TreeNode *node,
				  int       old_balance)
{
  if (!node->left)
    node->balance += 1;
  else if ((node->left->balance != old_balance) &&
	   (node->left->balance == 0))
    node->balance += 1;

  if (node->balance > 1)
    return tree_node_balance (node);
  return node;
}

static TreeNode*
tree_node_restore_right_balance (TreeNode *node,
				   int       old_balance)
{
  if (!node->right)
    node->balance -= 1;
  else if ((node->right->balance != old_balance) &&
	   (node->right->balance == 0))
    node->balance -= 1;

  if (node->balance < -1)
    return tree_node_balance (node);
  return node;
}

static TreeNode *
tree_node_lookup (TreeNode        *node,
		    CompareDataFunc  compare,
		    void *          compare_data,
		    const void*     key)
{
  int cmp;

  if (!node)
    return 0;

  cmp = (* compare) (key, node->key, compare_data);
  if (cmp == 0)
    return node;

  if (cmp < 0)
    {
      if (node->left)
	return tree_node_lookup (node->left, compare, compare_data, key);
    }
  else if (cmp > 0)
    {
      if (node->right)
	return tree_node_lookup (node->right, compare, compare_data, key);
    }

  return 0;
}

static int
tree_node_count (TreeNode *node)
{
  int count;

  count = 1;
  if (node->left)
    count += tree_node_count (node->left);
  if (node->right)
    count += tree_node_count (node->right);

  return count;
}

static int
tree_node_pre_order (TreeNode     *node,
		       TraverseFunc  traverse_func,
		       void *       data)
{
  if ((*traverse_func) (node->key, node->value, data))
    return 1;
  if (node->left)
    {
      if (tree_node_pre_order (node->left, traverse_func, data))
	return 1;
    }
  if (node->right)
    {
      if (tree_node_pre_order (node->right, traverse_func, data))
	return 1;
    }

  return 0;
}

static int
tree_node_in_order (TreeNode     *node,
		      TraverseFunc  traverse_func,
		      void *       data)
{
  if (node->left)
    {
      if (tree_node_in_order (node->left, traverse_func, data))
	return 1;
    }
  if ((*traverse_func) (node->key, node->value, data))
    return 1;
  if (node->right)
    {
      if (tree_node_in_order (node->right, traverse_func, data))
	return 1;
    }

  return 0;
}

static int
tree_node_post_order (TreeNode     *node,
			TraverseFunc  traverse_func,
			void *       data)
{
  if (node->left)
    {
      if (tree_node_post_order (node->left, traverse_func, data))
	return 1;
    }
  if (node->right)
    {
      if (tree_node_post_order (node->right, traverse_func, data))
	return 1;
    }
  if ((*traverse_func) (node->key, node->value, data))
    return 1;

  return 0;
}

static void *
tree_node_search (TreeNode     *node,
		    CompareFunc   search_func,
		    const void*  data)
{
  int dir;

  if (!node)
    return 0;

  do {
    dir = (* search_func) (node->key, data);
    if (dir == 0)
      return node->value;

    if (dir < 0)
      node = node->left;
    else if (dir > 0)
      node = node->right;
  } while (node);

  return 0;
}

static int
tree_node_height (TreeNode *node)
{
  int left_height;
  int right_height;

  if (node)
    {
      left_height = 0;
      right_height = 0;

      if (node->left)
	left_height = tree_node_height (node->left);

      if (node->right)
	right_height = tree_node_height (node->right);

      return MAX (left_height, right_height) + 1;
    }

  return 0;
}

static TreeNode*
tree_node_rotate_left (TreeNode *node)
{
  TreeNode *right;
  int a_bal;
  int b_bal;

  right = node->right;

  node->right = right->left;
  right->left = node;

  a_bal = node->balance;
  b_bal = right->balance;

  if (b_bal <= 0)
    {
      if (a_bal >= 1)
	right->balance = b_bal - 1;
      else
	right->balance = a_bal + b_bal - 2;
      node->balance = a_bal - 1;
    }
  else
    {
      if (a_bal <= b_bal)
	right->balance = a_bal - 2;
      else
	right->balance = b_bal - 1;
      node->balance = a_bal - b_bal - 1;
    }

  return right;
}

static TreeNode*
tree_node_rotate_right (TreeNode *node)
{
  TreeNode *left;
  int a_bal;
  int b_bal;

  left = node->left;

  node->left = left->right;
  left->right = node;

  a_bal = node->balance;
  b_bal = left->balance;

  if (b_bal <= 0)
    {
      if (b_bal > a_bal)
	left->balance = b_bal + 1;
      else
	left->balance = a_bal + 2;
      node->balance = a_bal - b_bal + 1;
    }
  else
    {
      if (a_bal <= -1)
	left->balance = b_bal + 1;
      else
	left->balance = a_bal + b_bal + 2;
      node->balance = a_bal + 1;
    }

  return left;
}

static void
tree_node_check (TreeNode *node)
{
  int left_height;
  int right_height;
  int balance;
  
  if (node)
    {
      left_height = 0;
      right_height = 0;
      
      if (node->left)
	left_height = tree_node_height (node->left);
      if (node->right)
	right_height = tree_node_height (node->right);
      
      balance = right_height - left_height;
      if (balance != node->balance) {
	/*
	log (LODOMAIN, LOLEVEL_INFO,
	       "tree_node_check: failed: %d ( %d )\n",
	       balance, node->balance);
	*/
      }
      if (node->left)
	tree_node_check (node->left);
      if (node->right)
	tree_node_check (node->right);
    }
}


