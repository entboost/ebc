
#ifndef HAVE_VSNPRINTF

#ifndef __COM_VSNPRINTF_H__
#define __COM_VSNPRINTF_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdio.h>
#include <stdarg.h>
int vsnprintf(char *s, size_t buf_size, const char *format, va_list ap);

#if defined(__cplusplus)
}
#endif


#endif /*__COM_VSNPRINTF_H__ */

#endif /* HAVE_VSNPRINTF */

