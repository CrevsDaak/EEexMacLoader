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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Init.c, 2019-07-13 Saturday 19:19:34 nico>
 *
 */

#include <dlfcn.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>
#include <sysexits.h>
#include <mach/mach_vm.h>
#include <AudioUnit/AudioUnit.h>
#include "EEex_mach.h"
#include "EEex_Lua.h"
#include "EEex_Init.h"
#include "EEex_Logger.h"

__attribute__((constructor)) static void EEex_ctor(void)
{
    if (access("../../../override/M__EEex.lua", F_OK)) /* assume we're in MacOS/ sitting next to the game's binary */
    {
	EEex_Log(0, "error: M__EEex.lua does not exist: %s\n", strerror(errno));
	exit(EX_DATAERR);
    }
}

int EEex_init(AudioUnit au)
{
    EEex_lua.handle = dlopen(NULL, RTLD_NOW);

    if (EEex_protect(NULL, VM_PROT_EXECUTE|VM_PROT_READ|VM_PROT_WRITE, false))
    {
	EEex_Log(0, "error: EEex_protect call on game memory failed: exiting!\n");
	exit(EX_OSERR);
    }

    EEex_lua.pushcclosure = dlsym(EEex_lua.handle, "lua_pushcclosure");
    EEex_lua.getint = dlsym(EEex_lua.handle, "luaH_getint");
    EEex_lua.gettop = dlsym(EEex_lua.handle, "lua_gettop");
    EEex_lua.type = dlsym(EEex_lua.handle, "lua_type");
    EEex_lua.tostring = dlsym(EEex_lua.handle, "lua_tostring");
    EEex_lua.toboolean = dlsym(EEex_lua.handle, "lua_toboolean");
    EEex_lua.tonumber = dlsym(EEex_lua.handle, "lua_tonumber");
    EEex_lua.newlstr = dlsym(EEex_lua.handle, "luaS_newlstr");
    EEex_lua.typename = dlsym(EEex_lua.handle, "lua_typename");
    EEex_lua.setglobal = dlsym(EEex_lua.handle, "lua_setglobal");

    void** aux_L = dlsym(EEex_lua.handle, "g_lua");
    EEex_lua.State = (*aux_L);

    EEex_lua.pushcclosure(EEex_lua.State, (lua_CFunction)&EEex_lua_init, 0);
    EEex_lua.setglobal(EEex_lua.State, "EEex_Init");

    EEex_lua.pushcclosure(EEex_lua.State, (lua_CFunction)&EEex_lua_dump_stack, 0);
    EEex_lua.setglobal(EEex_lua.State, "EEex_DumpLuaStack");

    return AudioUnitInitialize(au);
}

DYLD_INTERPOSE(EEex_init, AudioUnitInitialize)
