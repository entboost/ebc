
#include <stdlib.h>
#include "cqueue.h"

CQueue* cqueue_new (unsigned int size) 
{
    CQueue *q;
    if(size == 0)
	return 0;


    q = (CQueue*)malloc(sizeof(item) * (size - 1) + sizeof(CQueue));
    q->head   = q->tail = 0;
    q->circle = 0;
    q->sz     = size;
    return q;
}

void cqueue_free(CQueue* q)
{
    free(q);
}

void cqueue_free_full(CQueue *q, FreeFunc f)
{
    int circle;

    if(f) {
	int end;
	if(q->head > q->tail || (q->head == q->tail && q->circle == 1)) {
	    end = q->sz;
	    circle = 1;
	} else {
	    end = q->tail;
	    circle = 0;
	}


	while(q->head < end) {
	    f(q->link[q->head]);
	    q->head++;
	}

	if(circle) {
	    q->head = 0;
	    while(q->head < q->tail) {
		free(q->link[q->head]);
		q->head++;
	    }
	}
    }
    cqueue_free(q);
}
	    
int cqueue_put(CQueue *q, void *p)
{
    if(q->circle && q->head == q->tail)
	return -1;
    q->link[q->tail] = (char*)p;
    q->tail++;
    if(q->tail == q->sz){
	q->tail = 0;
	q->circle = 1;
    }	
    return 0;
}
	
void  *cqueue_get(CQueue *q)
{ 
    void *ret;
    if(q->head == q->tail && !q->circle)
	return 0;

    ret = q->link[q->head];
    q->head++;
    if(q->head == q->sz){
	q->circle = 0;
	q->head = 0;
    }	
    return ret;
}	

