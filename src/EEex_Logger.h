#ifndef __EEEX_LOGGER_H__
#define __EEEX_LOGGER_H__

enum EEex_log_level
{
    EEEX_LOG_DEBUG = 0x1,
    EEEX_LOG_ERROR = 0x2,
    EEEX_LOG_GAME_MSG = 0x4,
    EEEX_LOG_INFO = 0x8,
    EEEX_LOG_DIALOG = 0x10,
    EEEX_LOG_PRINT = 0x20
};

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
