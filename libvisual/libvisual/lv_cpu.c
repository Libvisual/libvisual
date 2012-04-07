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

#define _POSIX_SOURCE
#define _BSD_SOURCE

#include "config.h"
#include "lv_cpu.h"
#include "lv_common.h"
#include "gettext.h"
#include <unistd.h>

#if defined(VISUAL_ARCH_POWERPC)
#if defined(VISUAL_OS_DARWIN)
#include <sys/sysctl.h>
#else
#include <signal.h>
#include <setjmp.h>
#endif
#endif

#if defined(VISUAL_OS_HPUX)
#include <sys/mpctl.h>
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

#if defined(VISUAL_OS_ANDROID)
#include <cpu-features.h>
#endif

#define TEST_BIT(x, n)  (((x) >> (n)) & 1)

static VisCPU __lv_cpu_caps;
static int __lv_cpu_initialized = FALSE;

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
static int has_cpuid (void);
static int cpuid (unsigned int ax, unsigned int *p);
#endif

#if defined(VISUAL_OS_WIN32)
LONG CALLBACK win32_sig_handler_sse(EXCEPTION_POINTERS* ep);
#endif

/* The sigill handlers */
#if defined(VISUAL_ARCH_X86) //x86 (linux katmai handler check thing)
#if defined(VISUAL_OS_LINUX)
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
#endif /* VISUAL_OS_LINUX */

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
#if defined(VISUAL_ARCH_X86)
static void check_os_katmai_support( void )
{
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
	/* We can't use POSIX signal handling to test the availability of
	 * SSE, so we disable it by default.
	 */
	__lv_cpu_caps.hasSSE=0;
#endif /* __linux__ */
}
#endif /* VISUAL_ARCH_X86 */


#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
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
#elif defined(VISUAL_ARCH_X86_64)
	return TRUE;
#else
	return FALSE;
#endif
}
#endif /* defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64) */

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
static int cpuid (unsigned int ax, unsigned int *p)
{
	__asm __volatile
		("movl %%ebx, %%esi\n\t"
		 "cpuid\n\t"
		 "xchgl %%ebx, %%esi"
		 : "=a" (p[0]), "=S" (p[1]),
		 "=c" (p[2]), "=d" (p[3])
		 : "0" (ax));

	return VISUAL_OK;
}
#endif

static int get_number_of_cores (void)
{
	/* See: http://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine */

#if defined(VISUAL_OS_LINUX) || defined(VISUAL_OS_SOLARIS) || defined(VISUAL_OS_AIX)

	int ncpus = sysconf (_SC_NPROCESSORS_ONLN);

	return ncpus != -1 ? ncpus : 1;

#elif defined(VISUAL_OS_NETBSD) || defined(VISUAL_OS_FREEBSD) || defined(VISUAL_OS_OPENBSD) || defined(VISUAL_OS_DARWIN)

	int ncpus;
	int mib[2];
	visual_size_t len = sizeof (ncpus);

	mib[0] = CTL_HW;
	mib[1] = HW_NCPU;

	sysctl (mib, 2, &ncpus, &len, NULL, 0);

	return ncpus >= 1 ? ncpus : 1;

#elif defined(VISUAL_OS_HPUX)

	return mpctl (MPC_GETNUMSPUS, NULL, NULL);

#elif defined(VISUAL_OS_IRIX)

	return sysconf (_SC_NPROC_ONLN);

#elif defined(VISUAL_OS_ANDROID)

	return android_getCpuCount();

#elif defined(VISUAL_OS_WIN32)

	SYSTEM_INFO system_info;

	GetSystemInfo (&system_info);

	return system_info.dwNumberOfProcessors;

#else

	return 1;

#endif
}

static VisCPUType get_cpu_type (void)
{
#if defined(VISUAL_ARCH_MIPS)
	return VISUAL_CPU_TYPE_MIPS;
#elif defined(VISUAL_ARCH_ALPHA)
	return VISUAL_CPU_TYPE_ALPHA;
#elif defined(VISUAL_ARCH_SPARC)
	return VISUAL_CPU_TYPE_SPARC;
#elif defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	return VISUAL_CPU_TYPE_X86;
#elif defined(VISUAL_ARCH_POWERPC)
	return VISUAL_CPU_TYPE_POWERPC;
#elif defined(VISUAL_ARCH_ARM)
	return VISUAL_CPU_TYPE_ARM;
#else
	return VISUAL_CPU_TYPE_OTHER;
#endif
}

static void print_cpu_info (void)
{
	visual_log (VISUAL_LOG_DEBUG, "CPU: Number of CPUs: %d", __lv_cpu_caps.nrcpu);
	visual_log (VISUAL_LOG_DEBUG, "CPU: type %d", __lv_cpu_caps.type);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	visual_log (VISUAL_LOG_DEBUG, "CPU: X86 type %d", __lv_cpu_caps.x86cpuType);
	visual_log (VISUAL_LOG_DEBUG, "CPU: cacheline %d", __lv_cpu_caps.cacheline);
	visual_log (VISUAL_LOG_DEBUG, "CPU: MMX %d", __lv_cpu_caps.hasMMX);
	visual_log (VISUAL_LOG_DEBUG, "CPU: MMX2 %d", __lv_cpu_caps.hasMMX2);
	visual_log (VISUAL_LOG_DEBUG, "CPU: SSE %d", __lv_cpu_caps.hasSSE);
	visual_log (VISUAL_LOG_DEBUG, "CPU: SSE2 %d", __lv_cpu_caps.hasSSE2);
	visual_log (VISUAL_LOG_DEBUG, "CPU: 3DNow %d", __lv_cpu_caps.has3DNow);
	visual_log (VISUAL_LOG_DEBUG, "CPU: 3DNowExt %d", __lv_cpu_caps.has3DNowExt);
#elif defined(VISUAL_ARCH_POWERPC)
	visual_log (VISUAL_LOG_DEBUG, "CPU: AltiVec %d", __lv_cpu_caps.hasAltiVec);
#elif defined(VISUAL_ARCH_ARM)
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM v7 %d", __lv_cpu_caps.hasARMv7);
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM VFPv3 %d", __lv_cpu_caps.hasVFPv3);
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM NEON %d", __lv_cpu_caps.hasNeon);
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM LDREX_STREX %d", __lv_cpu_caps.hasLDREX_STREX);
#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */
}

void visual_cpu_initialize ()
{
	unsigned int regs[4];
	unsigned int regs2[4];

	visual_mem_set (&__lv_cpu_caps, 0, sizeof (VisCPU));

	__lv_cpu_caps.type  = get_cpu_type ();
	__lv_cpu_caps.nrcpu = get_number_of_cores ();

#if defined(VISUAL_ARCH_ARM)
# if defined(VISUAL_OS_ANDROID)
	if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM) {
		uint64_t type = android_getCpuFeatures ();

		if (type & ANDROID_CPU_ARM_FEATURE_ARMv7)
			__lv_cpu_caps.hasARMv7 = 1;

		if (type & ANDROID_CPU_ARM_FEATURE_VFPv3)
			__lv_cpu_caps.hasVFPv3 = 1;

		if (type & ANDROID_CPU_ARM_FEATURE_NEON)
			__lv_cpu_caps.hasNeon = 1;

		if(type & ANDROID_CPU_ARM_FEATURE_LDREX_STREX)
			__lv_cpu_caps.hasLDREX_STREX = 1;
	}
# endif /* VISUAL_OS_ANDROID */
#endif /* VISUAL_ARCH_ARM */

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
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
		__lv_cpu_caps.hasMMX  = TEST_BIT (regs2[3], 23); /* 0x0800000 */
		__lv_cpu_caps.hasSSE  = TEST_BIT (regs2[3], 25); /* 0x2000000 */
		__lv_cpu_caps.hasSSE2 = TEST_BIT (regs2[3], 26); /* 0x4000000 */
		__lv_cpu_caps.hasMMX2 = __lv_cpu_caps.hasSSE; /* SSE cpus supports mmxext too */

		cacheline = ((regs2[1] >> 8) & 0xFF) * 8;
		if (cacheline > 0)
			__lv_cpu_caps.cacheline = cacheline;
	}

	cpuid (0x80000000, regs);

	if (regs[0] >= 0x80000001) {

		cpuid (0x80000001, regs2);

		__lv_cpu_caps.hasMMX  |= TEST_BIT (regs2[3], 23); /* 0x0800000 */
		__lv_cpu_caps.hasMMX2 |= TEST_BIT (regs2[3], 22); /* 0x400000 */
		__lv_cpu_caps.has3DNow    = TEST_BIT (regs2[3], 31); /* 0x80000000 */
		__lv_cpu_caps.has3DNowExt = TEST_BIT (regs2[3], 30);
	}

	if (regs[0] >= 0x80000006) {
		cpuid (0x80000006, regs2);
		__lv_cpu_caps.cacheline = regs2[2] & 0xFF;
	}

#if defined(VISUAL_ARCH_X86)
	if (__lv_cpu_caps.hasSSE)
		check_os_katmai_support ();

	if (!__lv_cpu_caps.hasSSE)
		__lv_cpu_caps.hasSSE2 = 0;
#endif

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

#if defined(VISUAL_ARCH_POWERPC)
	check_os_altivec_support ();
#endif /* VISUAL_ARCH_POWERPC */

	print_cpu_info ();

	__lv_cpu_initialized = TRUE;
}

const VisCPU *visual_cpu_get_caps ()
{
	if (!__lv_cpu_initialized)
		return NULL;

	return &__lv_cpu_caps;
}

int visual_cpu_has_mmx ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasMMX;
}

int visual_cpu_has_mmx2 ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasMMX2;
}

int visual_cpu_has_sse ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasSSE;
}
int visual_cpu_has_sse2 ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasSSE2;
}

int visual_cpu_has_3dnow ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.has3DNow;
}

int visual_cpu_has_3dnow2 ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.has3DNowExt;
}

int visual_cpu_has_altivec ()
{
	if (!__lv_cpu_initialized)
		visual_log (VISUAL_LOG_ERROR, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasAltiVec;
}

int visual_cpu_has_armv7 (void)
{
    if (!__lv_cpu_initialized)
	    visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasARMv7;
}

int visual_cpu_has_vfpv3 (void)
{
    if (!__lv_cpu_initialized)
	    visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasVFPv3;
}

int visual_cpu_has_neon (void)
{
    if (!__lv_cpu_initialized)
        visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasNeon;
}

int visual_cpu_has_ldrex_strex (void)
{
    if (!__lv_cpu_initialized)
	    visual_log (VISUAL_LOG_CRITICAL, _("The VisCPU system is not initialized."));

	return __lv_cpu_caps.hasLDREX_STREX;
}
