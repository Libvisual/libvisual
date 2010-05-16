/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Chong Kai Xiong <descender@phreaker.net>
 *	    Eric Anholt <anholt@FreeBSD.org>
 *
 * Extra Credits: MPlayer cpudetect hackers.
 *
 * $Id: lv_cpu.c,v 1.27 2006/01/22 13:23:37 synap Exp $
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

/* FIXME: clean this entire file up */

#include "lvconfig.h"

#if defined(VISUAL_ARCH_POWERPC)
#if defined(VISUAL_OS_DARWIN)
#include <sys/sysctl.h>
#else
#include <signal.h>
#include <setjmp.h>
#endif
#endif

#if defined(VISUAL_OS_NETBSD) || defined(VISUAL_OS_OPENBSD)
#include <sys/param.h>
#include <sys/sysctl.h>
#include <machine/cpu.h>
#endif

#if defined(VISUAL_OS_FREEBSD)
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

#if defined(VISUAL_OS_LINUX)
#include <signal.h>
#endif

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "gettext.h"

#include "lv_log.h"
#include "lv_error.h"
#include "lv_cpu.h"

static VisCPU __lv_cpu_caps;
static int __lv_cpu_initialized = FALSE;

static int has_cpuid (void);
static int cpuid (unsigned int ax, unsigned int *p);

/* The sigill handlers */
#if defined(VISUAL_ARCH_X86) //x86 (linux katmai handler check thing)
#if defined(VISUAL_OS_LINUX) && defined(_POSIX_SOURCE) && defined(X86_FXSR_MAGIC)
static void sigill_handler_sse( int signal, struct sigcontext sc )
{
	/* Both the "xorps %%xmm0,%%xmm0" and "divps %xmm0,%%xmm1"
	 * instructions are 3 bytes long.  We must increment the instruction
	 * pointer manually to avoid repeated execution of the offending
	 * instruction.
	 *
	 * If the SIGILL is caused by a divide-by-zero when unmasked
	 * exceptions aren't supported, the SIMD FPU status and control
	 * word will be restored at the end of the test, so we don't need
	 * to worry about doing it here.  Besides, we may not be able to...
	 */
	sc.eip += 3;

	__lv_cpu_caps.hasSSE=0;
}

static void sigfpe_handler_sse( int signal, struct sigcontext sc )
{
	if ( sc.fpstate->magic != 0xffff ) {
		/* Our signal context has the extended FPU state, so reset the
		 * divide-by-zero exception mask and clear the divide-by-zero
		 * exception bit.
		 */
		sc.fpstate->mxcsr |= 0x00000200;
		sc.fpstate->mxcsr &= 0xfffffffb;
	} else {
		/* If we ever get here, we're completely hosed.
		*/
	}
}
#endif
#endif /* VISUAL_OS_LINUX && _POSIX_SOURCE && X86_FXSR_MAGIC */

#if defined(VISUAL_OS_WIN32)
LONG CALLBACK win32_sig_handler_sse(EXCEPTION_POINTERS* ep)
{
	if(ep->ExceptionRecord->ExceptionCode==EXCEPTION_ILLEGAL_INSTRUCTION){
		ep->ContextRecord->Eip +=3;
		__lv_cpu_caps.hasSSE=0;       
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
#endif /* VISUAL_OS_WIN32 */


#if defined(VISUAL_ARCH_POWERPC) && !defined(VISUAL_OS_DARWIN)
static sigjmp_buf __lv_powerpc_jmpbuf;
static volatile sig_atomic_t __lv_powerpc_canjump = 0;

static void sigill_handler (int sig);

static void sigill_handler (int sig)
{
	if (!__lv_powerpc_canjump) {
		signal (sig, SIG_DFL);
		raise (sig);
	}

	__lv_powerpc_canjump = 0;
	siglongjmp (__lv_powerpc_jmpbuf, 1);
}

static void check_os_altivec_support( void )
{
#if defined(VISUAL_OS_DARWIN)
	int sels[2] = {CTL_HW, HW_VECTORUNIT};
	int has_vu = 0;
	visual_size_t len = sizeof(has_vu);
	int err;

	err = sysctl (sels, 2, &has_vu, &len, NULL, 0);   

	if (err == 0)
		if (has_vu != 0)
			__lv_cpu_caps.hasAltiVec = 1;
#else /* !VISUAL_OS_DARWIN */
	/* no Darwin, do it the brute-force way */
	/* this is borrowed from the libmpeg2 library */
	signal (SIGILL, sigill_handler);
	if (sigsetjmp (__lv_powerpc_jmpbuf, 1)) {
		signal (SIGILL, SIG_DFL);
	} else {
		__lv_powerpc_canjump = 1;

		asm volatile
			("mtspr 256, %0\n\t"
			 "vand %%v0, %%v0, %%v0"
			 :
			 : "r" (-1));

		signal (SIGILL, SIG_DFL);
		__lv_cpu_caps.hasAltiVec = 1;
	}
#endif
}
#endif

/* If we're running on a processor that can do SSE, let's see if we
 * are allowed to or not.  This will catch 2.4.0 or later kernels that
 * haven't been configured for a Pentium III but are running on one,
 * and RedHat patched 2.2 kernels that have broken exception handling
 * support for user space apps that do SSE.
 */
static void check_os_katmai_support( void )
{
//	printf ("omg\n");
#if defined(VISUAL_ARCH_X86)
#if defined(VISUAL_OS_FREEBSD)
	int has_sse=0, ret;
	visual_size_t len=sizeof(has_sse);

	ret = sysctlbyname("hw.instruction_sse", &has_sse, &len, NULL, 0);
	if (ret || !has_sse)
		__lv_cpu_caps.hasSSE=0;

#elif defined(VISUAL_OS_NETBSD) || defined(VISUAL_OS_OPENBSD)
	int has_sse, has_sse2, ret, mib[2];
	visual_size_t varlen;

	mib[0] = CTL_MACHDEP;
	mib[1] = CPU_SSE;
	varlen = sizeof(has_sse);

	ret = sysctl(mib, 2, &has_sse, &varlen, NULL, 0);
	if (ret < 0 || !has_sse) {
		__lv_cpu_caps.hasSSE=0;
	} else {
		__lv_cpu_caps.hasSSE=1;
	}

	mib[1] = CPU_SSE2;
	varlen = sizeof(has_sse2);
	ret = sysctl(mib, 2, &has_sse2, &varlen, NULL, 0);
	if (ret < 0 || !has_sse2) {
		__lv_cpu_caps.hasSSE2=0;
	} else {
		__lv_cpu_caps.hasSSE2=1;
	}
	__lv_cpu_caps.hasSSE = 0; /* FIXME ?!?!? */

#elif defined(VISUAL_OS_WIN32)
	LPTOP_LEVEL_EXCEPTION_FILTER exc_fil;
	if ( __lv_cpu_caps.hasSSE ) {
		exc_fil = SetUnhandledExceptionFilter(win32_sig_handler_sse);
		__asm __volatile ("xorps %xmm0, %xmm0");
		SetUnhandledExceptionFilter(exc_fil);
	}
#elif defined(VISUAL_OS_LINUX)
//	printf ("omg1\n");
//	printf ("omg2\n");
	struct sigaction saved_sigill;
	struct sigaction saved_sigfpe;

	/* Save the original signal handlers.
	*/
	sigaction( SIGILL, NULL, &saved_sigill );
	sigaction( SIGFPE, NULL, &saved_sigfpe );

	signal( SIGILL, (void (*)(int))sigill_handler_sse );
	signal( SIGFPE, (void (*)(int))sigfpe_handler_sse );

	/* Emulate test for OSFXSR in CR4.  The OS will set this bit if it
	 * supports the extended FPU save and restore required for SSE.  If
	 * we execute an SSE instruction on a PIII and get a SIGILL, the OS
	 * doesn't support Streaming SIMD Exceptions, even if the processor
	 * does.
	 */
	if ( __lv_cpu_caps.hasSSE ) {
		__asm __volatile ("xorps %xmm1, %xmm0");
	}

	/* Emulate test for OSXMMEXCPT in CR4.  The OS will set this bit if
	 * it supports unmasked SIMD FPU exceptions.  If we unmask the
	 * exceptions, do a SIMD divide-by-zero and get a SIGILL, the OS
	 * doesn't support unmasked SIMD FPU exceptions.  If we get a SIGFPE
	 * as expected, we're okay but we need to clean up after it.
	 *
	 * Are we being too stringent in our requirement that the OS support
	 * unmasked exceptions?  Certain RedHat 2.2 kernels enable SSE by
	 * setting CR4.OSFXSR but don't support unmasked exceptions.  Win98
	 * doesn't even support them.  We at least know the user-space SSE
	 * support is good in kernels that do support unmasked exceptions,
	 * and therefore to be safe I'm going to leave this test in here.
	 */
	if ( __lv_cpu_caps.hasSSE ) {
		//      test_os_katmai_exception_support();
	}

	/* Restore the original signal handlers.
	*/
	sigaction( SIGILL, &saved_sigill, NULL );
	sigaction( SIGFPE, &saved_sigfpe, NULL );

#else
//	printf ("hier dan3\n");
	/* We can't use POSIX signal handling to test the availability of
	 * SSE, so we disable it by default.
	 */
	__lv_cpu_caps.hasSSE=0;
#endif /* __linux__ */
//	printf ("hier dan\n");
#endif
//	printf ("hier dan ha\n");
}


static int has_cpuid (void)
{
#if defined(VISUAL_ARCH_X86)
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
#if defined(VISUAL_ARCH_X86)
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
	return -VISUAL_ERROR_CPU_INVALID_CODE;
#endif
}

/**
 * @defgroup VisCPU VisCPU
 * @{
 */

/**
 * Initializes the VisCPU caps structure by detecting the CPU features and flags.
 *
 * This is normally called by visual_init() and is needed by visual_mem_initialize() in order to
 * detect the most optimal mem_copy and mem_set functions.
 */
void visual_cpu_initialize ()
{
	uint32_t cpu_flags;
	unsigned int regs[4];
	unsigned int regs2[4];

	int mib[2], ncpu;
	visual_size_t len;

	visual_mem_set (&__lv_cpu_caps, 0, sizeof (VisCPU));

	/* Check for arch type */
#if defined(VISUAL_ARCH_MIPS)
	__lv_cpu_caps.type = VISUAL_CPU_TYPE_MIPS;
#elif defined(VISUAL_ARCH_ALPHA)
	__lv_cpu_caps.type = VISUAL_CPU_TYPE_ALPHA;
#elif defined(VISUAL_ARCH_SPARC)
	__lv_cpu_caps.type = VISUAL_CPU_TYPE_SPARC;
#elif defined(VISUAL_ARCH_X86)
	__lv_cpu_caps.type = VISUAL_CPU_TYPE_X86;
#elif defined(VISUAL_ARCH_POWERPC)
	__lv_cpu_caps.type = VISUAL_CPU_TYPE_POWERPC;
#else
	__lv_cpu_caps.type = VISUAL_CPU_TYPE_OTHER;
#endif

	/* Count the number of CPUs in system */
#if !defined(VISUAL_OS_WIN32) && !defined(VISUAL_OS_UNKNOWN) && defined(_SC_NPROCESSORS_ONLN)
	__lv_cpu_caps.nrcpu = sysconf (_SC_NPROCESSORS_ONLN);
	if (__lv_cpu_caps.nrcpu == -1)
		__lv_cpu_caps.nrcpu = 1;

#elif defined(VISUAL_OS_NETBSD) || defined(VISUAL_OS_FREEBSD) || defined(VISUAL_OS_OPENBSD)

	mib[0] = CTL_HW;
	mib[1] = HW_NCPU; 

	len = sizeof (ncpu);
	sysctl (mib, 2, &ncpu, &len, NULL, 0);
	__lv_cpu_caps.nrcpu = ncpu;

#else
	__lv_cpu_caps.nrcpu = 1;
#endif
	
#if defined(VISUAL_ARCH_X86)
	/* No cpuid, old 486 or lower */
	if (has_cpuid () == 0)
		return;

	__lv_cpu_caps.cacheline = 32;
	
	/* Get max cpuid level */
	cpuid (0x00000000, regs);
	
	if (regs[0] >= 0x00000001) {
		unsigned int cacheline;

		cpuid (0x00000001, regs2);

		__lv_cpu_caps.x86cpuType = (regs2[0] >> 8) & 0xf;
		if (__lv_cpu_caps.x86cpuType == 0xf)
		    __lv_cpu_caps.x86cpuType = 8 + ((regs2[0] >> 20) & 255); /* use extended family (P4, IA64) */
		
		/* general feature flags */
		__lv_cpu_caps.hasTSC  = (regs2[3] & (1 << 8  )) >>  8; /* 0x0000010 */
		__lv_cpu_caps.hasMMX  = (regs2[3] & (1 << 23 )) >> 23; /* 0x0800000 */
		__lv_cpu_caps.hasSSE  = (regs2[3] & (1 << 25 )) >> 25; /* 0x2000000 */
		__lv_cpu_caps.hasSSE2 = (regs2[3] & (1 << 26 )) >> 26; /* 0x4000000 */
		__lv_cpu_caps.hasMMX2 = __lv_cpu_caps.hasSSE; /* SSE cpus supports mmxext too */

		cacheline = ((regs2[1] >> 8) & 0xFF) * 8;
		if (cacheline > 0)
			__lv_cpu_caps.cacheline = cacheline;
	}
	
	cpuid (0x80000000, regs);
	
	if (regs[0] >= 0x80000001) {

		cpuid (0x80000001, regs2);
		
		__lv_cpu_caps.hasMMX  |= (regs2[3] & (1 << 23 )) >> 23; /* 0x0800000 */
		__lv_cpu_caps.hasMMX2 |= (regs2[3] & (1 << 22 )) >> 22; /* 0x400000 */
		__lv_cpu_caps.has3DNow    = (regs2[3] & (1 << 31 )) >> 31; /* 0x80000000 */
		__lv_cpu_caps.has3DNowExt = (regs2[3] & (1 << 30 )) >> 30;
	}

	if (regs[0] >= 0x80000006) {
		cpuid (0x80000006, regs2);
		__lv_cpu_caps.cacheline = regs2[2] & 0xFF;
	}


#if defined(VISUAL_OS_LINUX) || defined(VISUAL_OS_FREEBSD) || defined(VISUAL_OS_NETBSD) || defined(VISUAL_OS_CYGWIN) || defined(VISUAL_OS_OPENBSD)
	if (__lv_cpu_caps.hasSSE)
		check_os_katmai_support ();

	if (!__lv_cpu_caps.hasSSE)
		__lv_cpu_caps.hasSSE2 = 0;
#else
	__lv_cpu_caps.hasSSE=0;
	__lv_cpu_caps.hasSSE2 = 0;
#endif
#endif /* VISUAL_ARCH_X86 */

#if defined(VISUAL_ARCH_POWERPC)
	check_os_altivec_support ();
#endif /* VISUAL_ARCH_POWERPC */

	/* Synchronizing enabled flags with has flags */
	__lv_cpu_caps.enabledTSC	= __lv_cpu_caps.hasTSC;
	__lv_cpu_caps.enabledMMX	= __lv_cpu_caps.hasMMX;
	__lv_cpu_caps.enabledMMX2	= __lv_cpu_caps.hasMMX2;
	__lv_cpu_caps.enabledSSE	= __lv_cpu_caps.hasSSE;
	__lv_cpu_caps.enabledSSE2	= __lv_cpu_caps.hasSSE2;
	__lv_cpu_caps.enabled3DNow	= __lv_cpu_caps.has3DNow;
	__lv_cpu_caps.enabled3DNowExt	= __lv_cpu_caps.has3DNowExt;
	__lv_cpu_caps.enabledAltiVec	= __lv_cpu_caps.hasAltiVec;
	
	visual_log (VISUAL_LOG_DEBUG, "CPU: Number of CPUs: %d", __lv_cpu_caps.nrcpu);
	visual_log (VISUAL_LOG_DEBUG, "CPU: type %d", __lv_cpu_caps.type);
	visual_log (VISUAL_LOG_DEBUG, "CPU: X86 type %d", __lv_cpu_caps.x86cpuType);
	visual_log (VISUAL_LOG_DEBUG, "CPU: cacheline %d", __lv_cpu_caps.cacheline);
	visual_log (VISUAL_LOG_DEBUG, "CPU: TSC %d", __lv_cpu_caps.hasTSC);
	visual_log (VISUAL_LOG_DEBUG, "CPU: MMX %d", __lv_cpu_caps.hasMMX);
	visual_log (VISUAL_LOG_DEBUG, "CPU: MMX2 %d", __lv_cpu_caps.hasMMX2);
	visual_log (VISUAL_LOG_DEBUG, "CPU: SSE %d", __lv_cpu_caps.hasSSE);
	visual_log (VISUAL_LOG_DEBUG, "CPU: SSE2 %d", __lv_cpu_caps.hasSSE2);
	visual_log (VISUAL_LOG_DEBUG, "CPU: 3DNow %d", __lv_cpu_caps.has3DNow);
	visual_log (VISUAL_LOG_DEBUG, "CPU: 3DNowExt %d", __lv_cpu_caps.has3DNowExt);
	visual_log (VISUAL_LOG_DEBUG, "CPU: AltiVec %d", __lv_cpu_caps.hasAltiVec);

	__lv_cpu_initialized = TRUE;
}

/**
 * Function to get the VisCPU caps initialized by visual_cpu_initialize(), this contains information
 * regarding the CPU features and flags.
 *
 * @return The VisCPU caps structure.
 */
VisCPU *visual_cpu_get_caps ()
{
	if (__lv_cpu_initialized == FALSE)
		return NULL;

	return &__lv_cpu_caps;
}

/* The getters and setters for feature flags */
/**
 * Function to retrieve if the tsc CPU feature is enabled.
 *
 * @return Whether tsc is enabled or not.
 */
int visual_cpu_get_tsc ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabledTSC;
}

/**
 * Function to retrieve if the mmx CPU feature is enabled.
 *
 * @return Whether mmx is enabled or not.
 */
int visual_cpu_get_mmx ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabledMMX;
}

/**
 * Function to retrieve if the mmx2 CPU feature is enabled.
 *
 * @return Whether mmx2 is enabled or not.
 */
int visual_cpu_get_mmx2 ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabledMMX2;
}

/**
 * Function to retrieve if the sse CPU feature is enabled.
 *
 * @return Whether sse is enabled or not.
 */
int visual_cpu_get_sse ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabledSSE;
}

/**
 * Function to retrieve if the sse2 CPU feature is enabled.
 *
 * @return Whether sse2 is enabled or not.
 */
int visual_cpu_get_sse2 ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabledSSE2;
}

/**
 * Function to retrieve if the 3dnow CPU feature is enabled.
 *
 * @return Whether 3dnow is enabled or not.
 */
int visual_cpu_get_3dnow ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabled3DNow;
}

/**
 * Function to retrieve if the 3dnowext CPU feature is enabled.
 *
 * @return Whether 3dnowext is enabled or not.
 */
int visual_cpu_get_3dnow2 ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabled3DNowExt;
}

/**
 * Function to retrieve if the altivec CPU feature is enabled.
 *
 * @return Whether altivec is enabled or not.
 */
int visual_cpu_get_altivec ()
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.enabledAltiVec;
}

/**
 * Function to set if the tsc feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */
int visual_cpu_set_tsc (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.hasTSC == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabledTSC = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the MMX feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */ 
int visual_cpu_set_mmx (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.hasMMX == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabledMMX = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the MMX2 feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */ 
int visual_cpu_set_mmx2 (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.hasMMX2 == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabledMMX2 = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the SSE feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */  
int visual_cpu_set_sse (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.hasSSE == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabledSSE = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the SSE2 feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */ 
int visual_cpu_set_sse2 (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.hasSSE2 == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabledSSE2 = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the 3DNow feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */ 
int visual_cpu_set_3dnow (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.has3DNow == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabled3DNow = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the 3dnowext feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */ 
int visual_cpu_set_3dnow2 (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.has3DNowExt == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabled3DNowExt = enabled;

	return VISUAL_OK;
}

/**
 * Function to set if the altivec feature should be enabled or not, this function will also check
 * if the feature is actually available.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED on failure.
 */ 
int visual_cpu_set_altivec (int enabled)
{
	if (__lv_cpu_initialized == FALSE)
		visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	if (__lv_cpu_caps.hasAltiVec == FALSE)
		return -VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED;

	__lv_cpu_caps.enabledAltiVec = enabled;

	return VISUAL_OK;
}

/**
 * @}
 */

