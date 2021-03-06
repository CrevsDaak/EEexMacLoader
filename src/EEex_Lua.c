/*
 * EEex_Lua.c -- Mac OSX version of the EEex loader.
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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Lua.c, 2019-09-19 Thursday 17:42:42 nico>
 *
 */

#include <mach/mach_vm.h>
#include <sys/_types/_ptrdiff_t.h>

#include <dlfcn.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "EEex_Init.h"
#include "EEex_Lua.h"
#include "EEex_Logger.h"
#include "EEex_Mach.h"

int
EEex_lua_init(void* L)
{
    EEex_Log(3, "EEex_Init called! (from Lua)\n");
    uint64_t r = (uint64_t)malloc(4096);
    if (!r)
    {
        EEex_Log(0, "error: malloc failed: %s\n", strerror(errno));
        EEex_lua.pushnil(L);
    }
    else
        EEex_lua.pushnumber(L, (double)r);

    return 1;
}

int
EEex_lua_dump_stack(void* L)
{
    int top = EEex_lua.gettop(L);
    for (int i = 1; i <= top; i++)
    {
        int t = EEex_lua.type(L, i);
        switch (t)
        {
        case 4:
            EEex_Log(3, "[%d] `%s'\n", EEex_lua.tostring(L, i, NULL), i);
            break;

        case 1:
            EEex_Log(3, EEex_lua.toboolean(L, i) ? "[%d] true\n" : "[%d] false\n", i);
            break;

        case 3:
            EEex_Log(3, "[%d] %g\n", EEex_lua.tonumber(L, i, NULL), i);
            break;

        default: /* other values */
            EEex_Log(3, "[%d] %s\n", EEex_lua.typename(L, t), i);
            break;
        }
    }
    return 0;
}

int
EEex_lua_read_dword(void* L)
{
    ptrdiff_t raddr = EEex_lua.tointegerx(L, -1, NULL);
    int32_t dword;
    if (EEex_read((void*)raddr, &dword, 4))
    {
        EEex_Log(0, "error: failed to read dword at %p\n", raddr);
        EEex_lua.pushnil(L);
    }
    else
        EEex_lua.pushnumber(L, (double)dword);

    return 1;
}

int
EEex_lua_write_byte(void* L)
{
    ptrdiff_t wraddr = EEex_lua.tointegerx(L, -2, NULL);
    int8_t byte = EEex_lua.tointegerx(L, -1, NULL);
    if (EEex_write((void*)wraddr, &byte, 1))
        EEex_Log(0, "error: failed to write %d on byte at %p\n", byte, wraddr);

    return 0;
}

int
EEex_lua_dlsym(void* L)
{
    const char* sym = EEex_lua.tostring(L, -1, NULL);
    uint64_t temp = (uint64_t)dlsym(RTLD_MAIN_ONLY, sym);
    EEex_lua.pushnumber(L, (double)temp);
    return 1;
}

int
EEex_lua_expose_cfunc(void* L)
{
    ptrdiff_t faddr = EEex_lua.tointegerx(L, -2, NULL);
    const char* name = EEex_lua.tostring(L, -1, NULL);
    EEex_lua.pushcclosure(L, (lua_CFunction)faddr, 0);
    EEex_lua.setglobal(L, name);
    return 0;
}

int
EEex_lua_write_string(void* L)
{
    ptrdiff_t wraddr = EEex_lua.tointegerx(L, -2, NULL);
    const char* wrs = EEex_lua.tostring(L, -1, NULL);
    if (EEex_write((void*)wraddr, (void*)wrs, strlen(wrs) + 1))
        EEex_Log(0, "error: failed to write %u bytes at %p\n", strlen(wrs) + 1, wraddr);

    return 0;
}
