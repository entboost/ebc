
#ifndef __COM_MEMMEM_H__
#define __COM_MEMMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

void *memmem (const void *const haystack, 
	      const size_t      haystack_len,
	      const void *const needle, 
	      const size_t      needle_len);


#ifdef __cplusplus
}
#endif

#endif /*__COM_MEMMEM_H__ */


