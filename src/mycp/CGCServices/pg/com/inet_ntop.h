
#ifndef HAVE_INET_NTOP

#ifndef __COM_INET_NTOP_H__
#define __COM_INET_NTOP_H__

#if defined(__cplusplus)
extern "C" {
#endif

const char *inet_ntopxp(int af, const void *src, char *dst, size_t size);

#if defined(__cplusplus)
}
#endif

#endif /* __COM_INET_NTOP_H__ */


#endif /* HAVE_INET_NTOP */
