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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader/src/EEex_Logger.m, 2019-07-13 Saturday 15:37:37 nico>
 *
 */

#include <stdio.h> /* no need for stdio.h after using asl.h is implemented */
#include <Foundation/NSObjCRuntime.h>
#include "EEex_Logger.h"
/* hopefully not too far in the future, this file will get a complete rework,
 * mainly to make use of the Apple System Log and not be as messy
*/

static int loglevel = 5; /* make this configurable */

/* values for p:
 0 is for ''critical failures'' and friends
 1 is for user messages
 2 is for game messages (everything re-routed from NSLog)
 3 is EEex dev debugging
 4 is for in-game EEex functions debugging
 5 is always saved to the log but never printed unless loglevel=6
*/
/* currently, logging to a file is not implemented */
/* the way loglevel is being checked wastes about 3 instructions, improve it. */
int EEex_Logv(int p, const char* restrict a, va_list ap)
{
    if ((loglevel - p) > 0)
	return vprintf(a, ap); /* okay... */
    return 0;
}

int EEex_Log(int s, const char* fmt, ...)
{
    va_list w;
    va_start(w, fmt);
    return EEex_Logv(s, fmt, w);
    va_end(w);
}

void EEex_interpose_Log(NSString* restrict a, ...)
{
    va_list w;
    va_start(w, [a UTF8String]);
    /* this gives out a LOT of warnings, but I don't really know ObjC so not fixing */
    NSString *b = [[NSString alloc] initWithFormat:a arguments:w];
    EEex_Logv(6, [[NSString stringWithFormat: @"%@\n", b] UTF8String], w); /* changed game loglevel req to 6 for debugging, should be 2 */
    va_end(w);
}

DYLD_INTERPOSE(EEex_interpose_Log, NSLog)
