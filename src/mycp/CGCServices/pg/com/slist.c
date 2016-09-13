#include "slist.h"
#include "list.h"
#include <stdlib.h>
/**
 * Function: slist_alloc
 * 
 * Description: Creates a new single-linked list
 *
 * Return value: a new list.
 **/

SList* slist_alloc()
{
    SList *list;

    list = (SList *)malloc(sizeof(SList));
    list->data = 0;
    list->next = 0;
    return list;
}

/**
 * Function: slist_free
 * 
 * Description: Frees a list.
 *
 * Input parameters:
 *
 *   list            List that want to be freed. It always the list head, so 
 *                   that the whole list will be freed. If it isn't the head 
 *                   pointer, the list from the current list node to the end of
 *                   the list will be freed.
 * 
 **/

void slist_free (SList *list)
{
    SList *last;
    while (list) {
	last = list;
	list = list->next;
	free(last);
    }
}

/**
 * Function: slist_free_1
 * 
 * Description: Frees a list node.
 *
 * Input parameters:
 *
 *   list            List nodethat want to be freed. 
 * 
 **/

void slist_free_1 (SList *list)
{
    free(list);
}

/**
 * Function: slist_append
 * 
 * Description: Appends data to end of list.
 *
 * Input parameters:
 *
 *   list            List that want to be appended to.
 *
 *   data            The pointer that to be appended. 
 * 
 * Return value: list head.
 **/

SList* slist_append (SList   *list, void *data)
{
    SList *new_list;
    SList *last;
    
    new_list = slist_alloc ();
    new_list->data = data;

    if (list) {
	last = slist_last (list);
	/* assert (last != 0); */
	last->next = new_list;

	return list;
    } 
    
    return new_list;
}
/**
 * Function: slist_prepend
 * 
 * Description: Insert data before list head.
 *
 * Input parameters:
 *
 *   list            List that want to be insert before.
 *
 *   data            The pointer that to be insert. 
 * 
 * Return value: list head.
 **/

SList* slist_prepend (SList   *list, void *  data)
{
    SList *new_list;

    new_list = slist_alloc ();
    new_list->data = data;
    new_list->next = list;

    return new_list;
}

/**
 * Function: slist_insert
 * 
 * Description: Insert data at <position> of list.
 *
 * Input parameters:
 *
 *   list            List that want to be insert to.
 *
 *   data            The pointer that to be insert. 
 *
 *   position        The postion the data will be insert to.
 * 
 * Return value: list head.
 **/

SList* slist_insert (SList   *list, void * data, int position)
{
    SList *prev_list;
    SList *tmp_list;
    SList *new_list;
    
    if (position < 0)
	return slist_append (list, data);
    else if (position == 0)
	return slist_prepend (list, data);

    new_list = slist_alloc ();
    new_list->data = data;

    if (!list)
	return new_list;

    prev_list = 0;
    tmp_list = list;

    while ((position-- > 0) && tmp_list) {
	prev_list = tmp_list;
	tmp_list = tmp_list->next;
    }

    if (prev_list){
	new_list->next = prev_list->next;
	prev_list->next = new_list;
    } else {
	new_list->next = list;
	list = new_list;
    }

    return list;
}

/**
 * Function: slist_concat
 * 
 * Description: Concat 2 list.
 *
 * Input parameters:
 *
 *   list1           The list be concated.
 *
 *   list2           The list be concated.
 *
 *   position        The postion the data will be insert to.
 * 
 * Return value: new list head.
 **/
SList *slist_concat (SList *list1, SList *list2)
{
    if (list2)
    {
	if (list1)
	    slist_last (list1)->next = list2;
	else
	    list1 = list2;
    }

    return list1;
}

/**
 * Function: slist_remove
 * 
 * Description: Remove user pointer from list. This function will serach the
 * whole list and compare the saved user pointer with pointer <data>, if they
 * are equal, then remove the list node.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   data            The user pointer that be removed from list.
 * 
 * Return value: new list head.
 **/
SList* slist_remove (SList *list, const void *data)
{
    SList *tmp, *prev = 0;

    tmp = list;
    while (tmp) {
	if (tmp->data == data) {
	    if (prev)
		prev->next = tmp->next;
	    else
		list = tmp->next;

	    slist_free_1 (tmp);
	    break;
	}
	prev = tmp;
	tmp = prev->next;
    }

    return list;
}

/**
 * Function: slist_remove_all
 * 
 * Description: Remove all matched userr pointers from list. This function 
 * will serach the whole list and compare the saved user pointer with pointer
 * <data>, if they are equal, then remove the list node.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   data            The user pointer that be removed from list.
 * 
 * Return value: new list head.
 **/
SList* slist_remove_all (SList *list, const void *data)
{
    SList *tmp, *prev = 0;

    tmp = list;
    while (tmp) {
	if (tmp->data == data) {
	    SList *next = tmp->next;

	    if (prev)
		prev->next = next;
	    else
		list = next;
	  
	    slist_free_1 (tmp);
	    tmp = next;
	} else {
	    prev = tmp;
	    tmp = prev->next;
	}
    }

    return list;
}

static /*inline*/ SList* _slist_remove_link (SList *list, SList *link)
{
    SList *tmp;
    SList *prev;

    prev = 0;
    tmp = list;

    while (tmp) {
	if (tmp == link) {
	    if (prev)
		prev->next = tmp->next;
	    if (list == tmp)
		list = list->next;

	    tmp->next = 0;
	    break;
	}

	prev = tmp;
	tmp = tmp->next;
    }

    return list;
}

/**
 * Function: slist_remove_link
 * 
 * Description: Remove one list node without free it. The removed node's next
 * link is set to 0, so that it becomes a self-contained list with one node.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   link            list node.
 * 
 * Return value: new list head.
 **/
SList* slist_remove_link (SList *list,  SList *link)
{
    return _slist_remove_link (list, link);
}

/**
 * Function: slist_delete_link
 * 
 * Description: Delete one list node from list and free the memory space
 * occupied by the list node.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   link            list node.
 * 
 * Return value: new list head.
 **/
SList* slist_delete_link (SList *list, SList *link)
{
    list = _slist_remove_link (list, link);
    slist_free (link);

    return list;
}

/**
 * Function: slist_copy
 * 
 * Description: Makes a shadow clone of the list.
 *
 * Input parameters:
 *
 *   list            The list.
 * 
 * Return value: new list.
 **/

SList* slist_copy (SList *list)
{
    SList *new_list = 0;

    if (list)
    {
	SList *last;

	new_list = slist_alloc ();
	new_list->data = list->data;
	last = new_list;
	list = list->next;
	while (list)
	{
	    last->next = slist_alloc ();
	    last = last->next;
	    last->data = list->data;
	    list = list->next;
	}
    }

    return new_list;
}
/**
 * Function: slist_reverse
 * 
 * Description: Reverses a list.
 *
 * Input parameters:
 *
 *   list            The list.
 * 
 * Return value: list head of reversed list.
 **/

SList* slist_reverse (SList *list)
{
    SList *prev = 0;
  
    while (list)
    {
	SList *next = list->next;

	list->next = prev;
      
	prev = list;
	list = next;
    }
  
    return prev;
}

/**
 * Function: slist_nth
 * 
 * Description: Gets the list node at given position in a list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   n               The position of the list.
 * 
 * Return value: the list node, or 0 if no such position in list.
 **/
SList* slist_nth (SList *list, unsigned int n)
{
    while (n-- > 0 && list)
	list = list->next;

    return list;
}

/**
 * Function: slist_nth_data
 * 
 * Description: Gets the user pointer at given position in a list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   n               The position of the list.
 * 
 * Return value: the user pointer, or 0 if no such position in list.
 **/
void *slist_nth_data (SList *list, unsigned int n)
{
    while (n-- > 0 && list)
	list = list->next;

    return list ? list->data : 0;
}

/**
 * Function: slist_find
 * 
 * Description: Gets the list node which contains the given user pointer.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   data            The user pointer.
 * 
 * Return value: the list node, or 0 if it isn't found.
 **/
SList* slist_find (SList *list, const void *data)
{
    while (list)
    {
	if (list->data == data)
	    break;
	list = list->next;
    }

    return list;
}

/**
 * Function: slist_find_custom
 * 
 * Description: Find the list node whose user pointer are matched with user 
 * pointer <data> by compared them with <func> function.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   data            The user pointer.
 * 
 *   func            CompareFunc function.
 *
 * Return value: the list node, or 0 if it isn't found.
 **/
SList* slist_find_custom (SList *list, const void *data, CompareFunc func)
{
    if(func == 0)
	return list;

    while (list) {
	if (! func (list->data, data))
	    return list;
	list = list->next;
    }

    return 0;
}
/**
 * Function: slist_position
 * 
 * Description: Gets the postion of given list node in one list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   link            Supplied list node.
 *
 * Return value: 
 *   positive intger: the position of the list node in list.
 *   -1             : have no such list node in given list.
 **/

int slist_position (SList *list, SList *link)
{
    int i;

    i = 0;
    while (list) {
	if (list == link)
	    return i;
	i++;
	list = list->next;
    }

    return -1;
}

/**
 * Function: slist_index
 * 
 * Description: Gets the index (begin with 0) of given user pointer in one 
 * list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   data            The user pointer.
 *
 * Return value: 
 *   non negtive intger: the index of the list node in list.
 *   -1                : have no found the user pointer in given list.
 **/
int slist_index (SList *list, const void *data)
{
    int i;

    i = 0;
    while (list)
    {
	if (list->data == data)
	    return i;
	i++;
	list = list->next;
    }

    return -1;
}

/**
 * Function: slist_last
 * 
 * Description: Gets the last list node of one list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 * Return value: the last list node.
 **/
SList* slist_last (SList *list)
{
    if (list)
    {
	while (list->next)
	    list = list->next;
    }

    return list;
}

/**
 * Function: slist_length
 * 
 * Description: Gets length of one list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 * Return value: the length of the list.
 **/
unsigned int slist_length (SList *list)
{
    unsigned int length;

    length = 0;
    while (list)
    {
	length++;
	list = list->next;
    }

    return length;
}

/**
 * Function: slist_foreach
 * 
 * Description: Calls a function for each list node in a list.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   func            The user function.
 *
 *   user_data       The user data to pass to function <func>.
 *
 **/
void slist_foreach (SList   *list, Func func, void *user_data)
{
    while (list) {
	SList *next = list->next;
	(*func) (list->data, user_data);
	list = next;
    }
}

/**
 * Function: slist_insert_sorted
 * 
 * Description: Inserts a list node into the list, using the given comparison
 * function to determine its position.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   data            The user pointer for list node.
 *
 *   func            The comparison function, which returns a number > 0 if 
 *                   the first list node come after the second parameter in the
 *                   sort order.
 * Return value: the list head.
 **/

SList* slist_insert_sorted (SList       *list,
			    void        *data,
			    CompareFunc  func)
{
    SList *tmp_list = list;
    SList *prev_list = 0;
    SList *new_list;
    int cmp;
  
    if(func == 0)
	return list;

    if (!list)
    {
	new_list = slist_alloc ();
	new_list->data = data;
	return new_list;
    }
 
    cmp = (*func) (data, tmp_list->data);
 
    while ((tmp_list->next) && (cmp > 0)) {
	prev_list = tmp_list;
	tmp_list = tmp_list->next;
	cmp = (*func) (data, tmp_list->data);
    }

    new_list = slist_alloc ();
    new_list->data = data;

    if ((!tmp_list->next) && (cmp > 0)) {
	tmp_list->next = new_list;
	return list;
    }
  
    if (prev_list)
    {
	prev_list->next = new_list;
	new_list->next = tmp_list;
	return list;
    }
    else
    {
	new_list->next = list;
	return new_list;
    }
}

static SList *slist_sort_merge (SList   *l1, 
				SList   *l2,
				Func     compare_func,
				int      use_data,
				void    *user_data)
{
    SList list, *l;
    int cmp;

    l=&list;

    while (l1 && l2)
    {
	if (use_data)
	    cmp = ((CompareDataFunc) compare_func) (l1->data, l2->data, user_data);
	else
	    cmp = ((CompareFunc) compare_func) (l1->data, l2->data);

	if (cmp <= 0)
        {
	    l=l->next=l1;
	    l1=l1->next;
        } 
	else 
	{
	    l=l->next=l2;
	    l2=l2->next;
        }
    }
    l->next= l1 ? l1 : l2;
  
    return list.next;
}

static SList *slist_sort_real (SList   *list,
			       Func     compare_func,
			       int      use_data,
			       void    *user_data)
{
    SList *l1, *l2;

    if (!list) 
	return 0;
    if (!list->next) 
	return list;

    l1 = list; 
    l2 = list->next;

    while ((l2 = l2->next) != 0)
    {
	if ((l2 = l2->next) == 0) 
	    break;
	l1=l1->next;
    }
    l2 = l1->next; 
    l1->next = 0;

    return slist_sort_merge (slist_sort_real (list, compare_func, use_data, user_data),
			     slist_sort_real (l2, compare_func, use_data, user_data),
			     compare_func,
			     use_data,
			     user_data);
}

/**
 * Function: slist_sort
 * 
 * Description: Sorts a list using the given comparison function.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   func            The comparison function, which returns a number > 0 if 
 *                   the first list node come after the second parameter in the
 *                   sort order.
 *
 * Return value: the list head.
 **/
SList *slist_sort (SList       *list,
		   CompareFunc  func)
{
  return slist_sort_real (list, (Func)func, 0, 0);
}

/**
 * Function: slist_sort_with_data
 * 
 * Description: Sorts a list using the given comparison function. Like the
 * function of slist_sort, but the sort function accepts a user data argument.
 *
 * Input parameters:
 *
 *   list            The list.
 *
 *   func            The comparison function, which returns a number > 0 if 
 *                   the first list node come after the second parameter in the
 *                   sort order.
 *
 * Return value: the list head.
 **/
SList *slist_sort_with_data (SList           *list,
			     CompareDataFunc  compare_func,
			     void            *user_data)
{
    return slist_sort_real (list, (Func) compare_func, 1, user_data);
}
