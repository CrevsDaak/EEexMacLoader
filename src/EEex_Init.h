#ifndef __EEEX_INIT_H_
#define __EEEX_INIT_H_

typedef int (*lua_CFunction)(void*);

struct __eeex_lua_f_list
{
    int (*pushcclosure)(void*, lua_CFunction, int);
    void* (*getint)(void*, int);
    void* (*newlstr)(void*, const char*, size_t);
    void (*settable)(void*, const void*, void*, void*);
    void (*setglobal)(void*, const char*);
    int (*gettop)(void*);
    int (*toboolean)(void*, int);
    int (*type)(void*, int);
    float (*tonumber)(void*, int, int*);
    char* (*typename)(void*, int);
    ptrdiff_t (*tointegerx)(void*, int, int*);
    void (*pushnumber)(void*, double);
    int (*loadstring)(void*, const char*);
    const char* (*tostring)(void*, int, unsigned long*);
    void (*pushnil)(void*);
};

struct __eeex_lua_f_list EEex_lua;

#endif /* !__EEEX_INIT_H_ */
