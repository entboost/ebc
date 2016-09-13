#ifndef __SLIST_H__
#define __SLIST_H__
#include "comtypes.h"

#ifdef __cplusplus 
    extern "C" {
#endif
	typedef struct _SList SList;

	struct _SList
	{
	    void  *data;
	    SList *next;
	};

/* Singly linked lists
 */
	SList*  slist_alloc          (void);
	void    slist_free           (SList  *list);
	SList*  slist_append         (SList          *list,
				      void           *data);
	SList*  slist_prepend        (SList          *list,
				      void           *data);
	SList*  slist_insert         (SList          *list,
				      void           *data,
				      int             position);
	SList*  slist_insert_sorted  (SList          *list,
				      void           *data,
				      CompareFunc     func);
	SList*  slist_concat         (SList          *list1,
				      SList          *list2);
	SList*  slist_remove         (SList          *list,
				      const void     *data);
	SList*  slist_remove_all     (SList          *list,
				      const void     *data);
	SList*  slist_remove_link    (SList          *list,
				      SList          *link);
	SList*  slist_delete_link    (SList          *list,
				      SList          *link);
	SList*  slist_reverse        (SList          *list);
	SList*  slist_copy           (SList          *list);
	SList*  slist_nth            (SList          *list,
				      unsigned int    n);
	SList*  slist_find           (SList          *list,
				      const void     *data);
	SList*  slist_find_custom    (SList          *list,
				      const void     *data,
				      CompareFunc     func);
	int     slist_position       (SList          *list,
				      SList          *llink);
	int     slist_index          (SList          *list,
				      const void     *data);
	SList*  slist_last           (SList          *list);
	unsigned int    slist_length (SList          *list);
	void     slist_foreach       (SList          *list,
				      Func            func,
				      void           *user_data);
	SList*  slist_sort           (SList          *list,
				      CompareFunc     compare_func);
	SList*  slist_sort_with_data (SList          *list,
				      CompareDataFunc  compare_func,
				      void           *u);
	void * slist_nth_data        (SList          *list,
				      unsigned int    n);
#define  slist_next(slist)	((slist) ? (((SList *)(slist))->next) : NULL)

#ifdef __cplusplus 
    }
#endif

#endif /* __SLIST_H__ */

    
