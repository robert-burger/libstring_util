#ifndef STRING_UTIL_OS_H
#define STRING_UTIL_OS_H

#ifndef NO_CONFIG_H
#include "config.h"
#endif

#ifdef __FUNCTION__
#define __func__ __FUNCTION__
#endif

#ifndef HAVE_SNPRINTF
int string_util_vsnprintf(char* str, size_t size, const char* format, va_list ap);
int string_util_snprintf(char* str, size_t size, const char* format, ...);

#define snprintf string_util_snprintf
#define vsnprintf string_util_vsnprintf
#endif

#endif
