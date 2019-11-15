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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Init.c, 2019-11-15 Friday 20:54:39 nico>
 *
 */

#include <mach/vm_prot.h>

#include <dlfcn.h>
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <time.h>
#include <unistd.h>

#include "EEex_Init.h"
#include "EEex_Logger.h"
#include "EEex_Lua.h"
#include "EEex_Mach.h"

/* maybe add error checking for dl* functions? */

static int
EEex_init(void* L, const char* s)
{
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

struct CString
{
    char* m_pchData;
};

static void (*msgboxcall)(char*, char*, struct CString*, int);

/* bug: won't log potential dialog choices. research and extend to cover them */
/* bug: does not clear the IE colour-codes from the string, output is _ugly_  */
static void
EEex_combat_log_hook(char* a1, char* a2, struct CString* text, int a3)
{
    EEex_Log(-1, "[MSGBOX]%s\n", text->m_pchData);
    msgboxcall(a1, a2, text, a3);
}

static void
EEex_close_log(void)
{
    if (logfile)
	fclose(logfile);
}

__attribute__((constructor)) static void
EEex_ctor(void)
{
    logfile = fopen("../../../EEex.log", "a");
    if (!logfile)
    {
	char buf[256];
	strerror_r(errno, buf, 256);
	EEex_Log(1, "error: failed to open log: %s\n!", buf);
        exit(EX_OSERR);
    }

    atexit(&EEex_close_log);

    time_t t = time(NULL);
    char* tt = ctime(&t);
    EEex_Log(-1, "EEex -- %s", tt);
    
    /* assume we're in MacOS/ sitting next to the game's binary */
    if (access("../../../override/M__EEex.lua", F_OK))
    {
	char buf[256];
	strerror_r(errno, buf, 256);
        EEex_Log(0, "error: M__EEex.lua does not exist: %s\n", buf);
        exit(EX_DATAERR);
    }

    if (EEex_protect(NULL, VM_PROT_EXECUTE | VM_PROT_READ | VM_PROT_WRITE, false))
    {
        EEex_Log(0, "error: EEex_protect call on game memory failed: exiting!\n");
        exit(EX_OSERR);
    }

    /* moved out of the hook function for whatever performance boost doing that could give us */
    msgboxcall = dlsym(RTLD_MAIN_ONLY, "_Z18AppendToMessageBox7CStringS_S_i");

    /* moved out of EEex_init because they're better off here with the rest of the dlsym calls :) */
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

    int8_t* btlua = (int8_t*)dlsym(RTLD_MAIN_ONLY, "_Z12bootstrapLuav");
    if (btlua) /* this check is so that I can test this on lldb, not needed if not debugging */
    {
        /* 323, 319 are hardcoded offsets: don't do this!! */
        int32_t off_e8 = (int32_t)&EEex_init - ((int32_t)btlua + 323);
        if (EEex_write((void*)(btlua + 319), &off_e8, 4))
        {
            EEex_Log(0, "error: EEex_write failed to patch EEex_init call into bootstrapLua: exiting!\n");
            exit(EX_OSERR); /* maybe use EX_NOPERM instead?? */
        }
    }

    int8_t* disptext = (int8_t*)dlsym(RTLD_MAIN_ONLY, "_ZN12CScreenWorld11DisplayTextERK7CStringS2_jjiiji");
    if (disptext) /* this check is so that I can test this on lldb, not needed if not debugging */
    {
        /* 680, 676 are hardcoded offsets: don't do this!! */
        int32_t off_e8 = (int32_t)&EEex_combat_log_hook - ((int32_t)disptext + 680);
        if (EEex_write((void*)(disptext + 676), &off_e8, 4))
        {
            EEex_Log(0, "error: EEex_write failed to patch EEex_init call into bootstrapLua: exiting!\n");
            exit(EX_OSERR); /* maybe use EX_NOPERM instead?? */
        }
    }
}
