/*
 * EEex_Logger.m -- Mac OSX version of the EEex loader.
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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Logger.m, 2019-11-15 Friday 20:45:04 nico>
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#include <Foundation/Foundation.h>

#include "EEex_Init.h"
#include "EEex_Logger.h"

static int
EEex_Logv(int lvl, const char* restrict a, va_list ap)
{
    switch (lvl)
    {
    case -1:
	return vfprintf(logfile, a, ap);
    case 0:
	vfprintf(logfile, a, ap);
	/* fallthrough */
    case 1:
	return vprintf(a, ap);
    default:
	return 0;
    }
}

int
EEex_Log(int s, const char* fmt, ...)
{
    va_list w;
    va_start(w, fmt);
    int r = EEex_Logv(s, fmt, w);
    va_end(w);
    return r;
}

static void
EEex_interpose_Log(NSString* restrict a, ...)
{
    va_list w;
    va_start(w, a);
    NSString* b = [[NSString alloc] initWithFormat:a arguments:w];
    EEex_Logv(6, [[NSString stringWithFormat:@"%@\n", b] UTF8String], w);
    va_end(w);
}

DYLD_INTERPOSE(EEex_interpose_Log, NSLog)
