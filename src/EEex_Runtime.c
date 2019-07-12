/*
 * EEex_Runtime.c -- Mac OSX version of the EEex loader.
 *
 * Copyright (c) 2019 Nicolás Clotta <nicolas.clotta@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader_dev/src/EEex_Runtime.c, 2019-07-12 Friday 19:41:28 nico>
 *
 */

#include <dlfcn.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <mach/mach_vm.h>
#include <AudioUnit/AudioUnit.h>
#include "EEex_mach.h"
#include "EEex_Logger.h"

typedef int (*lua_CFunction)(void*);

struct __eeex_lua_f_list
{
    int (*pushcclosure)(lua_State*, lua_CFunction, int);
    TValue* (*getint)(struct Table*, int);
    void* (*newlstr)(lua_State*, const char*, size_t);
    void (*settable)(lua_State*, const TValue*, TValue*, TValue*);
    void (*setglobal)(lua_State*, const char*);
    int (*gettop)(lua_State*);
    int (*toboolean)(lua_State*, int);
    int (*type)(lua_State*, int);
    char* (*tostring)(lua_State*, int);
    float (*tonumber)(lua_State*, int);
    char* (*typename)(lua_State*, int);
};

struct __eeex_lua_f_list EEex_lua;

int EEex_lua_init(void* n)
{
    (void)n;
    return 0;
}

static void stackDump (lua_State *L)
{
    int i;
    int top = EEex_lua.gettop(L);
    for (i = 1; i <= top; i++)
    {
        int t = EEex_lua.type(L, i);
        switch (t)
	{
	case 4:
	    EEex_Log(0,"`%s'", EEex_lua.tostring(L, i));
	    break;
	    
	case 1:
             EEex_Log(0,EEex_lua.toboolean(L, i) ? "true" : "false");
             break;
	     
	 case 3:
             EEex_Log(0,"%g", EEex_lua.tonumber(L, i));
             break;
	     
	default:  /* other values */
            EEex_Log(0,"%s", EEex_lua.typename(L, t));
            break;
	    
        }
        EEex_Log(0,"  ");  /* put a separator */
    }
    EEex_Log(0,"\n");  /* end the listing */
}

int EEex_init(AudioUnit au)
{
    void* h = dlopen(NULL, RTLD_NOW);
    
    if (EEex_protect(NULL, VM_PROT_EXECUTE|VM_PROT_READ|VM_PROT_WRITE, false))
    {
	EEex_Log(0, "error: EEex_protect call on game memory failed: exiting!\n");
	exit(1);
    }

    EEex_lua.pushcclosure = dlsym(h, "lua_pushcclosure");
    EEex_lua.getint = dlsym(h, "luaH_getint");
    EEex_lua.gettop = dlsym(h, "lua_gettop");
    EEex_lua.type = dlsym(h, "lua_type");
    EEex_lua.tostring = dlsym(h, "lua_tostring");
    EEex_lua.toboolean = dlsym(h, "lua_toboolean");
    EEex_lua.tonumber = dlsym(h, "lua_tonumber");
    EEex_lua.newlstr = dlsym(h, "luaS_newlstr");
    EEex_lua.typename = dlsym(h, "lua_typename");
    EEex_lua.setglobal = dlsym(h, "lua_setglobal");

    void** aux_L = dlsym(h, "g_lua");
    void* L = (*aux_L);
    EEex_Log(0, "aux_L\t%p\nL\t%p\n",aux_L,L);

    stackDump(L);

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_init, 0);
    EEex_lua.setglobal(L, "EEex_Init");
    
    return AudioUnitInitialize(au);
}

DYLD_INTERPOSE(EEex_init, AudioUnitInitialize)

