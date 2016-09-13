#ifndef __CQUEUE_H__
#define __CQUEUE_H__

#include "comtypes.h"
#ifdef  __cplusplus
extern "C" {
#endif
    

typedef void * item;

typedef struct {
    int       sz;
    int       head;
    int       tail;
    int       circle;
    void     *link[1];
}CQueue;


  CQueue* cqueue_new (unsigned int size) ;
  void   cqueue_free_full(CQueue *q, FreeFunc f);
  void   cqueue_free(CQueue* q);
  int    cqueue_put(CQueue *q, void *p);
  void  *cqueue_get(CQueue *q);


#ifdef  __cplusplus
}
#endif


#endif /* __CQUEUE_H__ */


