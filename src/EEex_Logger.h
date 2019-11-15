#ifndef __EEEX_LOGGER_H__
#define __EEEX_LOGGER_H__

/* from dfhack */
#define DYLD_INTERPOSE(_replacement, _replacee)                                                              \
    __attribute__((used)) static struct                                                                      \
    {                                                                                                        \
        const void* replacment;                                                                              \
        const void* replacee;                                                                                \
    } _interpose_##_replacee                                                                                 \
        __attribute__((section("__DATA,__interpose"))) = {(const void*)(unsigned long)&_replacement,         \
                                                          (const void*)(unsigned long)&_replacee};

int EEex_Log(int, const char*, ...);

#endif /* !__EEEX_LOGGER_H__ */
