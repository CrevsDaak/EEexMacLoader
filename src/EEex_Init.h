#ifndef __EEEX_INIT_H_
#define __EEEX_INIT_H_

typedef int (*lua_CFunction)(void*);

struct __eeex_lua_f_list
{
    void* State;
    void* handle;
    int (*pushcclosure)(void*, lua_CFunction, int);
    void* (*getint)(void*, int);
    void* (*newlstr)(void*, const char*, size_t);
    void (*settable)(void*, const void*, void*, void*);
    void (*setglobal)(void*, const char*);
    int (*gettop)(void*);
    int (*toboolean)(void*, int);
    int (*type)(void*, int);
    char* (*tostring)(void*, int);
    float (*tonumber)(void*, int);
    char* (*typename)(void*, int);
};

struct __eeex_lua_f_list EEex_lua;

#endif /* !__EEEX_INIT_H_ */
