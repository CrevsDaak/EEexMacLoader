/*
 * EEex_Init.c -- Mac OSX version of the EEex loader.
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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Init.c, 2019-07-16 Tuesday 23:09:03 nico>
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <dlfcn.h>
#include <sysexits.h>
#include <sys/_types/_ptrdiff_t.h>
#include <mach/mach_vm.h>

#include "EEex_Mach.h"
#include "EEex_Lua.h"
#include "EEex_Init.h"
#include "EEex_Logger.h"

/* maybe add error checking for dl* functions? */

int EEex_init(void* L, const char* s)
{
    EEex_Log(0, "EEex_init called\n");
    void* h = dlopen(NULL, RTLD_NOW);

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
    EEex_lua.tointegerx = dlsym(h, "lua_tointegerx");
    EEex_lua.error = dlsym(h, "luaL_error");
    EEex_lua.pushnumber = dlsym(h, "lua_pushnumber");
    EEex_lua.loadstring = dlsym(h, "luaL_loadstring");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_init, 0);
    EEex_lua.setglobal(L, "EEex_Init");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_dump_stack, 0);
    EEex_lua.setglobal(L, "EEex_DumpLuaStack");

    return EEex_lua.loadstring(L, s);
}

__attribute__((constructor)) static void EEex_ctor(void)
{
    if (access("../../../override/M__EEex.lua", F_OK)) /* assume we're in MacOS/ sitting next to the game's binary */
    {
	EEex_Log(0, "error: M__EEex.lua does not exist: %s\n", strerror(errno));
	exit(EX_DATAERR);
    }

    if (EEex_protect(NULL, VM_PROT_EXECUTE|VM_PROT_READ|VM_PROT_WRITE, false))
    {
	EEex_Log(0, "error: EEex_protect call on game memory failed: exiting!\n");
	exit(EX_OSERR);
    }

    void* btLua = dlsym(dlopen(NULL, RTLD_NOW), "_Z12bootstrapLuav");

    int32_t off_bt = (int64_t)btLua + 322 - (int64_t)&EEex_init;
    int64_t inst = 0xE8 | off_bt << 8;
    EEex_Log(0, "off_bt\t%p\nbtLua\t%p\nwroff\t%p\nEEex_init\t%p\n", off_bt, btLua, (btLua + 318), &EEex_init);

    if (EEex_write((void*)(btLua + 318), &inst, 5))
    {
	EEex_Log(0, "error: EEex_write failed to patch into bootstrapLua: exiting!\n");
	//exit(EX_OSERR); /* maybe use EX_NOPERM instead?? */
    }
}
