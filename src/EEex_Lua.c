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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Lua.c, 2019-07-13 Saturday 19:22:53 nico>
 *
 */

#include <stdint.h>
#include <mach/mach_vm.h>
#include <stdbool.h>
#include "EEex_mach.h"
#include "EEex_Logger.h"
#include "EEex_Init.h"

int EEex_lua_test(void* L)
{
    uint64_t addr = EEex_lua.tonumber(L, -1);
    (void)addr;
    return 0;
}

int EEex_lua_init(void* L)
{
    (void)L;
    EEex_Log(0, "EEex_Init called!\n");
    return 0;
}

void EEex_lua_dump_stack(void *L)
{
    int top = EEex_lua.gettop(L);
    for (int i = 1; i <= top; i++)
    {
        int t = EEex_lua.type(L, i);
        switch (t)
	{
	case 4:
	    EEex_Log(3, "[%d] `%s'\n", EEex_lua.tostring(L, i), i);
	    break;
	    
	case 1:
	    EEex_Log(3, EEex_lua.toboolean(L, i) ? "[%d] true\n" : "[%d] false\n", i);
	    break;
	    
	case 3:
	    EEex_Log(3, "[%d] %g\n", EEex_lua.tonumber(L, i), i);
	    break;
	    
	default:  /* other values */
            EEex_Log(3, "[%d] %s\n", EEex_lua.typename(L, t), i);
            break;
        }
    }
}