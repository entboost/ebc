#ifndef __SET_H__
#define __SET_H__
#include <com/comtypes.h>

#ifdef  __cplusplus
extern "C" {
#endif

    typedef struct _Set  Set;
  

    
/* Set tables
 */
    Set*  set_new();
    Set*  set_new_full(CompareFunc func, DestroyNotify notify);
    void  set_destroy (Set *set);
    void  set_insert (Set *set, void *value);
    void* set_remove(Set *set, void *value);
    int   set_has (Set *set, const void *value);
    void  set_foreach (Set *set,
		       HRFunc  func,
		       void *user_data);

    unsigned int set_size (Set *set);
    


/* 
 * Set Functions
 */
    int    str_cmp(const void *v, const void *v2);
    int    int_cmp(const void *v, const void *v2);
    int    direct_cmp(const void *v, const void *v2);

#ifdef  __cplusplus
}
#endif



#endif /* __SET_H__ */

