#ifndef __COMTYPES_H
#define __COMTYPES_H

typedef unsigned int   (*HashFunc) (const void *v);

typedef int            (*CompareFunc)     (const void *a,
					   const void *b);

typedef int            (*CompareDataFunc) (const void *a,
					   const void *b,
					   void       *u);
typedef int            (*EqualFunc)       (const void *a,
					   const void *b);
typedef void           (*DestroyNotify)   (void *data);
typedef void           (*Func)            (void *data,
					   void *u);
typedef void           (*HFunc)           (void *key,
					   void *value,
					   void *u);
typedef void           (*FreeFunc)        (void *data);
    typedef int            (*HRFunc)          (void *key, 
					       void *value, 
					       void *user_data);


#undef	MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))

#undef	MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#undef	ABS
#define ABS(a)	   (((a) < 0) ? -(a) : (a))

#undef	CLAMP
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

#ifndef INTCMP
# define INTCMP(x,y)           (((x)>(y))?1:(((x)<(y))?-1:0))
#endif

#ifndef BOOL_EXPR
# define BOOL_EXPR(expr)       ((expr)?(1):(0))
#endif

#endif
