/*
 * EEex_mach.c -- Mac OSX version of the EEex loader.
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
 * Time-stamp: </Users/nico/BG_modding/EEexMacLoader_dev/src/EEex_mach.c, 2019-07-12 Friday 16:20:21 nico>
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <mach/mach_vm.h>
#include <mach/mach_init.h>
#include <mach/mach_error.h>
#include "EEex_mach.h"
#include "EEex_Logger.h"
/* todo:
 * clean up the _DEBUGs off this file... ideally... at some point...
 * shup up -Wincompatible-pointer-types going ham on vm_region calls
 * remove the memmove so that I can compile with -O3 and not -O0
 */
int EEex_protect(void* paddr, vm_prot_t prot, bool max)
{
    mach_vm_size_t vmsize;
    vm_region_basic_info_data_64_t info;
    mach_msg_type_number_t info_count = VM_REGION_BASIC_INFO_COUNT_64;
    memory_object_name_t object;
    mach_vm_address_t* addr = NULL;
    /* this is so that paddr isn't modified by vm_region */
    /* also check whether it's actually necessary or not */
    memmove(addr, paddr, sizeof(mach_vm_address_t));
    
    mach_error_t e = mach_vm_region(mach_task_self(), (mach_vm_address_t*)&addr, &vmsize, VM_REGION_BASIC_INFO_64, (vm_region_info_64_t)&info, &info_count, &object);

    if (e != KERN_SUCCESS)
    {
	EEex_Log(0, "error: vm_region failed: %s\n", mach_error_string(e));
	return 1;
    }
    if ((info.protection == prot && !max) || (info.max_protection == prot && max))
	return 0; /* nothing to do (right?) */
    else
	e = mach_vm_protect(mach_task_self(), (mach_vm_address_t)addr, vmsize, max, prot);
    /* should try raising max prot if real prot fails to get settled */
    if (e != KERN_SUCCESS)
    {
	EEex_Log(0, "error: vm_protect failed: %s\n", mach_error_string(e));
	return 1;
    }
    else
	return 0;
}
/* maybe add a soft failure system and call vm_protect if necessary? */
static int EEex_write(void* wraddr, void* data, uint64_t dsz)
{
    kern_return_t e = mach_vm_write(mach_task_self(), (mach_vm_address_t)wraddr, (pointer_t)data, (mach_msg_type_number_t)dsz);
    if (e != KERN_SUCCESS)
    {
	EEex_Log(0, "error: vm_write(%u) failed: %s\n", dsz, mach_error_string(e));
	return 1;
    }
    else
	return 0;
}

extern inline int EEex_write_byte(void* wraddr, int8_t byte)
{
    return EEex_write(wraddr, &byte, 1);
}

extern inline int EEex_write_word(void* wraddr, int16_t word)
{
    return EEex_write(wraddr, &word, 2);
}

extern inline int EEex_write_dword(void* wraddr, int32_t dword)
{
    return EEex_write(wraddr, &dword, 4);
}

extern inline int EEex_write_qword(void* wraddr, int64_t qword)
{
    return EEex_write(wraddr, &qword, 8);
}
