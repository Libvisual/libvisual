/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Chong Kai Xiong <descender@phreaker.net>
 *
 * $Id:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lvconfig.h"
#include "lv_log.h"
#include "lv_cpu.h"

static VisCPU cpu_info;
static int cpu_initialized = FALSE;

enum {
	CPU_FLAGS_TSC	= 1 << 4,
	CPU_FLAGS_MMX	= 1 << 23,
	CPU_FLAGS_SSE	= 1 << 25,
	CPU_FLAGS_SSE2	= 1 << 26,
	CPU_FLAGS_HT	= 1 << 28,
};

/* FIXME: Can't check this on intel, on intel 30 is ia64... */
enum {
	/* 
	 * FIXME this is only true on AMD machines!
	 * i.e. on Intel machines bit 30 says if this
	 * is an IA64 or not.
	 */
	CPU_FLAGS_EXT_3DNOWEXT	= 1 << 30,
	CPU_FLAGS_EXT_3DNOW	= 1 << 31
};

uint32_t cpuid (uint32_t type);

uint32_t cpuid (uint32_t type)
{
	uint32_t flags;

/* 
 * FIXME See what to do if we are on a 386 or 486 machine,
 * which doesn't have the 'cpuid' instruction.
 */
#ifdef VISUAL_ARCH_Ix86
	__asm__ ("movl %1, %%eax \n"
			"cpuid          \n"
			"movl %%edx, %0 \n"
			:"=m" (flags)
			:"m" (type)
			:"%eax"
		);
#else
	flags = 0;
#endif

	return flags;
}
/**
 * @defgroup VisCPU VisCPU
 * @{
 */

/* FIXME: Should be called from visual_init, when it's safe to do this... */
void visual_cpu_initialize ()
{
	uint32_t cpu_flags;

	memset (&cpu_info, 0, sizeof (VisCPU));
	
	cpu_info.type = VISUAL_CPU_TYPE_X86;
	cpu_info.nrcpu = 1;

	cpu_flags = cpuid(1);

	if (cpu_flags & CPU_FLAGS_MMX)
		cpu_info.flags |= VISUAL_CPU_FLAG_MMX;

	if (cpu_flags & CPU_FLAGS_SSE)
		cpu_info.flags |= VISUAL_CPU_FLAG_SSE;

	if (cpu_flags & CPU_FLAGS_SSE2)
		cpu_info.flags |= VISUAL_CPU_FLAG_SSE2;

	cpu_initialized = TRUE;
}

VisCPU *visual_cpu_get_info ()
{
	if (cpu_initialized == FALSE)
		return NULL;

	return &cpu_info;
}

/* FIXME: Forcefully disable features, like 'mmx' or 'sse' (for example, using mmx in a multithreaded environment
 * where there is also floating point action can be quite harmful
 */
int visual_cpu_feature_disable (int flag)
{

	return VISUAL_OK;
}

int visual_cpu_feature_enable (int flag)
{
	
	return VISUAL_OK;
}

/**
 * @}
 */

