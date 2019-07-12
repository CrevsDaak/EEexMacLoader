#ifndef __EEEX_LOGGER_H__
#define __EEEX_LOGGER_H__

#include <stdarg.h> /* to shut up -pedantic -Wall */

/* stole this bit directly from the dfhack source, thanks I guess */
#define DYLD_INTERPOSE(_replacement,_replacee) \
    __attribute__((used)) static struct{ const void* replacment; const void* replacee; } \
    _interpose_##_replacee __attribute__ ((section ("__DATA,__interpose"))) = \
    { (const void*)(unsigned long)&_replacement, (const void*)(unsigned long)&_replacee };

int EEex_Logv(int, const char*, va_list);
int EEex_Log(int, const char*, ...);

#endif /* !__EEEX_LOGGER_H__ */
