/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Chong Kai Xiong <descender@phreaker.net>
 *
 * Extra Credits: MPlayer cpudetect hackers.
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

static VisCPU _lv_cpu_caps;
static int _lv_cpu_initialized = FALSE;

static int has_cpuid (void);
static int cpuid (unsigned int ax, unsigned int *p);

static int has_cpuid (void)
{
#ifdef VISUAL_ARCH_X86
	int a, c;

	__asm __volatile
		("pushf\n"
		 "popl %0\n"
		 "movl %0, %1\n"
		 "xorl $0x200000, %0\n"
		 "push %0\n"
		 "popf\n"
		 "pushf\n"
		 "popl %0\n"
		 : "=a" (a), "=c" (c)
		 :
		 : "cc");

	return a != c;
#else
	return 0;
#endif
}

static int cpuid (unsigned int ax, unsigned int *p)
{
#ifdef VISUAL_ARCH_X86
	uint32_t flags;

	__asm __volatile
		("movl %%ebx, %%esi\n\t"
		 "cpuid\n\t"
		 "xchgl %%ebx, %%esi"
		 : "=a" (p[0]), "=S" (p[1]),
		 "=c" (p[2]), "=d" (p[3])
		 : "0" (ax));

	return VISUAL_OK;
#else
	return VISUAL_ERROR_CPU_INVALID_CODE;
#endif
}

/**
 * @defgroup VisCPU VisCPU
 * @{
 */

void visual_cpu_initialize ()
{
	uint32_t cpu_flags;
	unsigned int regs[4];
	unsigned int regs2[4];

	memset (&_lv_cpu_caps, 0, sizeof (VisCPU));

	// FIXME clean this code up a great deal.
	// FIXME check for os katmai support.
	// FIXME try to find the author of the original mplayer code and ask
	// 	him if we can use this code. (check with eric and Michael)
	// FIXME add support for altivec check/ mac os X (darwin)
	

	/* Check for arch type */
#if defined(VISUAL_ARCH_MIPS)
	_lv_cpu_caps.type = VISUAL_CPU_TYPE_MIPS;
#elif defined(VISUAL_ARCH_ALPHA)
	_lv_cpu_caps.type = VISUAL_CPU_TYPE_ALPHA;
#elif defined(VISUAL_ARCH_SPARC)
	_lv_cpu_caps.type = VISUAL_CPU_TYPE_SPARC;
#elif defined(VISUAL_ARCH_X86)
	_lv_cpu_caps.type = VISUAL_CPU_TYPE_X86;
#elif defined(VISUAL_ARCH_POWERPC)
	_lv_cpu_caps.type = VISUAL_CPU_TYPE_POWERPC;
#else
	_lv_cpu_caps.type = VISUAL_CPU_TYPE_OTHER;
#endif

	/* Not X86 */
	if (_lv_cpu_caps.type != VISUAL_CPU_TYPE_X86)
		return;

	/* No cpuid, old 486 or lower */
	if (has_cpuid () == 0)
		return;

	_lv_cpu_caps.cacheline = 32;
	
	/* Get max cpuid level */
	cpuid (0x00000000, regs);
	
	if (regs[0] >= 0x00000001) {
		unsigned int cacheline;

		cpuid (0x00000001, regs2);

		_lv_cpu_caps.cpuType = (regs2[0] >> 8) & 0xf;
		if (_lv_cpu_caps.cpuType == 0xf)
		    _lv_cpu_caps.cpuType = 8 + ((regs2[0] >> 20) & 255); /* use extended family (P4, IA64) */
		
		/* general feature flags */
		_lv_cpu_caps.hasTSC  = (regs2[3] & (1 << 8  )) >>  8; /* 0x0000010 */
		_lv_cpu_caps.hasMMX  = (regs2[3] & (1 << 23 )) >> 23; /* 0x0800000 */
		_lv_cpu_caps.hasSSE  = (regs2[3] & (1 << 25 )) >> 25; /* 0x2000000 */
		_lv_cpu_caps.hasSSE2 = (regs2[3] & (1 << 26 )) >> 26; /* 0x4000000 */
		_lv_cpu_caps.hasMMX2 = _lv_cpu_caps.hasSSE; /* SSE cpus supports mmxext too */

		cacheline = ((regs2[1] >> 8) & 0xFF) * 8;
		if (cacheline > 0)
			_lv_cpu_caps.cacheline = cacheline;
	}
	
	cpuid (0x80000000, regs);
	
	if (regs[0] >= 0x80000001) {
		printf ("extended cpuid-level: %d\n",regs[0]&0x7FFFFFFF);

		cpuid (0x80000001, regs2);
		
		_lv_cpu_caps.hasMMX  |= (regs2[3] & (1 << 23 )) >> 23; /* 0x0800000 */
		_lv_cpu_caps.hasMMX2 |= (regs2[3] & (1 << 22 )) >> 22; /* 0x400000 */
		_lv_cpu_caps.has3DNow    = (regs2[3] & (1 << 31 )) >> 31; /* 0x80000000 */
		_lv_cpu_caps.has3DNowExt = (regs2[3] & (1 << 30 )) >> 30;
	}

	if (regs[0] >= 0x80000006) {
		cpuid (0x80000006, regs2);
		_lv_cpu_caps.cacheline = regs2[2] & 0xFF;
	}

#if 0	

		/* FIXME: Does SSE2 need more OS support, too? */
#if defined(__linux__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__CYGWIN__) || defined(__OpenBSD__)
		if (_lv_cpu_caps.hasSSE)
			check_os_katmai_support();
		if (!_lv_cpu_caps.hasSSE)
			_lv_cpu_caps.hasSSE2 = 0;
#else
		_lv_cpu_caps.hasSSE=0;
		_lv_cpu_caps.hasSSE2 = 0;
#endif

#endif

	printf ("DEBUG CPU: type %d\n", _lv_cpu_caps.cpuType);
	printf ("DEBUG CPU: TSC %d\n", _lv_cpu_caps.hasTSC);
	printf ("DEBUG CPU: MMX %d\n", _lv_cpu_caps.hasMMX);
	printf ("DEBUG CPU: MMX2 %d\n", _lv_cpu_caps.hasMMX2);
	printf ("DEBUG CPU: SSE %d\n", _lv_cpu_caps.hasSSE);
	printf ("DEBUG CPU: SSE2 %d\n", _lv_cpu_caps.hasSSE2);
	printf ("DEBUG CPU: 3DNow %d\n", _lv_cpu_caps.has3DNow);
	printf ("DEBUG CPU: 3DNowExt %d\n", _lv_cpu_caps.has3DNowExt);
	
	_lv_cpu_caps.nrcpu = 1;

	_lv_cpu_initialized = TRUE;
}

VisCPU *visual_cpu_get_caps ()
{
	if (_lv_cpu_initialized == FALSE)
		return NULL;

	return &_lv_cpu_caps;
}

/**
 * @}
 */

