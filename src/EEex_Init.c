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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Init.c, 2019-08-02 Friday 17:11:17 nico>
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
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
    /* might be worth moving all these to EEex_ctor */
    EEex_lua.pushcclosure = dlsym(RTLD_MAIN_ONLY, "lua_pushcclosure");
    EEex_lua.loadstring = dlsym(RTLD_MAIN_ONLY, "luaL_loadstring");
    EEex_lua.tointegerx = dlsym(RTLD_MAIN_ONLY, "lua_tointegerx");
    EEex_lua.pushnumber = dlsym(RTLD_MAIN_ONLY, "lua_pushnumber");
    EEex_lua.toboolean = dlsym(RTLD_MAIN_ONLY, "lua_toboolean");
    EEex_lua.setglobal = dlsym(RTLD_MAIN_ONLY, "lua_setglobal");
    EEex_lua.tostring = dlsym(RTLD_MAIN_ONLY, "lua_tolstring");
    EEex_lua.tonumber = dlsym(RTLD_MAIN_ONLY, "lua_tonumberx");
    EEex_lua.typename = dlsym(RTLD_MAIN_ONLY, "lua_typename");
    EEex_lua.newlstr = dlsym(RTLD_MAIN_ONLY, "luaS_newlstr");
    EEex_lua.pushnil = dlsym(RTLD_MAIN_ONLY, "lua_pushnil");
    EEex_lua.getint = dlsym(RTLD_MAIN_ONLY, "luaH_getint");
    EEex_lua.gettop = dlsym(RTLD_MAIN_ONLY, "lua_gettop");
    EEex_lua.type = dlsym(RTLD_MAIN_ONLY, "lua_type");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_init, 0);
    EEex_lua.setglobal(L, "EEex_Init");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_dump_stack, 0);
    EEex_lua.setglobal(L, "EEex_DumpLuaStack");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_read_dword, 0);
    EEex_lua.setglobal(L, "EEex_ReadDword");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_write_byte, 0);
    EEex_lua.setglobal(L, "EEex_WriteByte");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_dlsym, 0);
    EEex_lua.setglobal(L, "EEex_Label");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_expose_cfunc, 0);
    EEex_lua.setglobal(L, "EEex_ExposeToLua");

    EEex_lua.pushcclosure(L, (lua_CFunction)&EEex_lua_write_string, 0);
    EEex_lua.setglobal(L, "EEex_WriteString");

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

    void* btLua = dlsym(RTLD_MAIN_ONLY, "_Z12bootstrapLuav");
    if (btLua) /* this check is so that I can test this on lldb, not needed if not debugging */
    {
	int32_t off_bt = (int32_t)&EEex_init - ((int32_t)btLua + 323); /* 323, 319 are hardcoded offsets: don't do this!! */
	if (EEex_write((void*)(btLua + 319), &off_bt, 4))
	{
	    EEex_Log(0, "error: EEex_write failed to patch EEex_init call into bootstrapLua: exiting!\n");
	    exit(EX_OSERR); /* maybe use EX_NOPERM instead?? */
	}
    }
}
