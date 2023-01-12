/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	        Chong Kai Xiong <kaixiong@codeleft.sg>
 *	        Eric Anholt <anholt@freebsd.org>
 *
 * Extra Credits: MPlayer cpudetect hackers.
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
#define _DEFAULT_SOURCE

#include "config.h"
#include "lv_cpu.h"
#include "lv_common.h"
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

typedef struct {
	VisCPUType	type;
	int		num_cores;
	int		x86cpuType;
	int		cacheline;

	int		hasMMX;
	int		hasMMX2;
	int		hasSSE;
	int		hasSSE2;
	int		has3DNow;
	int		has3DNowExt;
	int		hasAltiVec;
	int		hasARMv7;
	int		hasVFPv3;
	int		hasNeon;
	int		hasLDREX_STREX;
} VisCPU;

static VisCPU cpu_caps;
static int cpu_initialized = FALSE;

/* The sigill handlers */
#if defined(VISUAL_ARCH_X86) //x86 (linux katmai handler check thing)
#if defined(VISUAL_OS_LINUX)
static void sigill_handler_sse (int signal, struct sigcontext sc)
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

	cpu_caps.hasSSE = FALSE;
}

static void sigfpe_handler_sse (int signal, struct sigcontext sc)
{
	if (sc.fpstate->magic != 0xffff) {
		/* Our signal context has the extended FPU state, so reset the
		 * divide-by-zero exception mask and clear the divide-by-zero
		 * exception bit. */
		sc.fpstate->mxcsr |= 0x00000200;
		sc.fpstate->mxcsr &= 0xfffffffb;
	} else {
		/* If we ever get here, we're completely hosed. */
	}
}
#endif
#endif /* VISUAL_OS_LINUX */

#if defined(VISUAL_OS_WIN32)
LONG CALLBACK win32_sig_handler_sse (EXCEPTION_POINTERS* ep)
{
	if (ep->ExceptionRecord->ExceptionCode == EXCEPTION_ILLEGAL_INSTRUCTION) {
#if defined(VISUAL_OS_WIN64)
		ep->ContextRecord->Rip +=3;
#else
		ep->ContextRecord->Eip +=3;
#endif
		cpu_caps.hasSSE = FALSE;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_CONTINUE_SEARCH;
}
#endif /* VISUAL_OS_WIN32 */


#if defined(VISUAL_ARCH_POWERPC) && !defined(VISUAL_OS_DARWIN)
static sigjmp_buf powerpc_jmpbuf;
static volatile sig_atomic_t powerpc_canjump = 0;

static void sigill_handler (int sig)
{
	if (!powerpc_canjump) {
		signal (sig, SIG_DFL);
		raise (sig);
	}

	powerpc_canjump = 0;
	siglongjmp (powerpc_jmpbuf, 1);
}

static void check_os_altivec_support (void)
{
#if defined(VISUAL_OS_DARWIN)
	int sels[2] = {CTL_HW, HW_VECTORUNIT};
	int has_vu = 0;
	visual_size_t len = sizeof(has_vu);
	int err;

	err = sysctl (sels, 2, &has_vu, &len, NULL, 0);

	if (err == 0)
		if (has_vu != 0)
			cpu_caps.hasAltiVec = 1;
#else /* !VISUAL_OS_DARWIN */
	/* no Darwin, do it the brute-force way */
	/* this is borrowed from the libmpeg2 library */
	signal (SIGILL, sigill_handler);
	if (sigsetjmp (powerpc_jmpbuf, 1)) {
		signal (SIGILL, SIG_DFL);
	} else {
		powerpc_canjump = 1;

		asm volatile
			("mtspr 256, %0\n\t"
			 "vand %%v0, %%v0, %%v0"
			 :
			 : "r" (-1));

		signal (SIGILL, SIG_DFL);
		cpu_caps.hasAltiVec = 1;
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
static void check_os_katmai_support (void)
{
#if defined(VISUAL_OS_FREEBSD)
	int has_sse = 0, ret;
	visual_size_t len = sizeof(has_sse);

	ret = sysctlbyname ("hw.instruction_sse", &has_sse, &len, NULL, 0);
	if (ret || !has_sse)
		cpu_caps.hasSSE = FALSE;

#elif defined(VISUAL_OS_NETBSD) || defined(VISUAL_OS_OPENBSD)
	int has_sse, has_sse2, ret, mib[2];
	visual_size_t varlen;

	mib[0] = CTL_MACHDEP;
	mib[1] = CPU_SSE;
	varlen = sizeof(has_sse);

	ret = sysctl (mib, 2, &has_sse, &varlen, NULL, 0);
	if (ret < 0 || !has_sse) {
		cpu_caps.hasSSE = FALSE;
	} else {
		cpu_caps.hasSSE = TRUE;
	}

	mib[1] = CPU_SSE2;
	varlen = sizeof (has_sse2);
	ret = sysctl (mib, 2, &has_sse2, &varlen, NULL, 0);
	if (ret < 0 || !has_sse2) {
		cpu_caps.hasSSE2 = FALSE;
	} else {
		cpu_caps.hasSSE2 = TRUE;
	}
	cpu_caps.hasSSE = FALSE; /* FIXME ?!?!? */

#elif defined(VISUAL_OS_WIN32)
	LPTOP_LEVEL_EXCEPTION_FILTER exc_fil;
	if (cpu_caps.hasSSE) {
		exc_fil = SetUnhandledExceptionFilter (win32_sig_handler_sse);
		__asm __volatile ("xorps %xmm0, %xmm0");
		SetUnhandledExceptionFilter (exc_fil);
	}
#elif defined(VISUAL_OS_LINUX)
	struct sigaction saved_sigill;
	struct sigaction saved_sigfpe;

	/* Save the original signal handlers.
	*/
	sigaction (SIGILL, NULL, &saved_sigill);
	sigaction (SIGFPE, NULL, &saved_sigfpe);

	signal (SIGILL, (void (*)(int)) sigill_handler_sse);
	signal (SIGFPE, (void (*)(int)) sigfpe_handler_sse);

	/* Emulate test for OSFXSR in CR4.  The OS will set this bit if it
	 * supports the extended FPU save and restore required for SSE.  If
	 * we execute an SSE instruction on a PIII and get a SIGILL, the OS
	 * doesn't support Streaming SIMD Exceptions, even if the processor
	 * does.
	 */
	if (cpu_caps.hasSSE) {
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
	//if (cpu_caps.hasSSE) {
	//    test_os_katmai_exception_support();
	//}

	/* Restore the original signal handlers. */
	sigaction (SIGILL, &saved_sigill, NULL);
	sigaction (SIGFPE, &saved_sigfpe, NULL);

#else
	/* We can't use POSIX signal handling to test the availability of
	 * SSE, so we disable it by default.
	 */
	cpu_caps.hasSSE = FALSE;
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
static void cpuid (unsigned int ax, unsigned int *p)
{
	__asm __volatile
		("movl %%ebx, %%esi\n\t"
		 "cpuid\n\t"
		 "xchgl %%ebx, %%esi"
		 : "=a" (p[0]), "=S" (p[1]),
		 "=c" (p[2]), "=d" (p[3])
		 : "0" (ax));
}
#endif

static unsigned int get_number_of_cores (void)
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

	return android_getCpuCount ();

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
	visual_log (VISUAL_LOG_DEBUG, "CPU: Number of CPUs: %d", cpu_caps.num_cores);
	visual_log (VISUAL_LOG_DEBUG, "CPU: type %d", cpu_caps.type);

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	visual_log (VISUAL_LOG_DEBUG, "CPU: X86 type %d", cpu_caps.x86cpuType);
	visual_log (VISUAL_LOG_DEBUG, "CPU: cacheline %d", cpu_caps.cacheline);
	visual_log (VISUAL_LOG_DEBUG, "CPU: MMX %d", cpu_caps.hasMMX);
	visual_log (VISUAL_LOG_DEBUG, "CPU: MMX2 %d", cpu_caps.hasMMX2);
	visual_log (VISUAL_LOG_DEBUG, "CPU: SSE %d", cpu_caps.hasSSE);
	visual_log (VISUAL_LOG_DEBUG, "CPU: SSE2 %d", cpu_caps.hasSSE2);
	visual_log (VISUAL_LOG_DEBUG, "CPU: 3DNow %d", cpu_caps.has3DNow);
	visual_log (VISUAL_LOG_DEBUG, "CPU: 3DNowExt %d", cpu_caps.has3DNowExt);
#elif defined(VISUAL_ARCH_POWERPC)
	visual_log (VISUAL_LOG_DEBUG, "CPU: AltiVec %d", cpu_caps.hasAltiVec);
#elif defined(VISUAL_ARCH_ARM)
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM v7 %d", cpu_caps.hasARMv7);
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM VFPv3 %d", cpu_caps.hasVFPv3);
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM NEON %d", cpu_caps.hasNeon);
	visual_log (VISUAL_LOG_DEBUG, "CPU: ARM LDREX_STREX %d", cpu_caps.hasLDREX_STREX);
#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */
}

__attribute__((no_sanitize("address")))
void visual_cpu_initialize ()
{
	visual_mem_set (&cpu_caps, 0, sizeof (VisCPU));

	cpu_caps.type      = get_cpu_type ();
	cpu_caps.num_cores = get_number_of_cores ();

#if defined(VISUAL_ARCH_ARM)
# if defined(VISUAL_OS_ANDROID)
	if (android_getCpuFamily() == ANDROID_CPU_FAMILY_ARM) {
		uint64_t type = android_getCpuFeatures ();

		if (type & ANDROID_CPU_ARM_FEATURE_ARMv7)
			cpu_caps.hasARMv7 = TRUE;

		if (type & ANDROID_CPU_ARM_FEATURE_VFPv3)
			cpu_caps.hasVFPv3 = TRUE;

		if (type & ANDROID_CPU_ARM_FEATURE_NEON)
			cpu_caps.hasNeon = TRUE;

		if(type & ANDROID_CPU_ARM_FEATURE_LDREX_STREX)
			cpu_caps.hasLDREX_STREX = TRUE;
	}
# endif /* VISUAL_OS_ANDROID */
#endif /* VISUAL_ARCH_ARM */

#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
	/* No cpuid, old 486 or lower */
	if (!has_cpuid ()) {
		return;
	}

	cpu_caps.cacheline = 32;

	unsigned int regs[4];
	unsigned int regs2[4];

	/* Get max cpuid level */
	cpuid (0x00000000, regs);

	if (regs[0] >= 0x00000001) {
		unsigned int cacheline;

		cpuid (0x00000001, regs2);

		cpu_caps.x86cpuType = (regs2[0] >> 8) & 0xf;
		if (cpu_caps.x86cpuType == 0xf)
		    cpu_caps.x86cpuType = 8 + ((regs2[0] >> 20) & 255); /* use extended family (P4, IA64) */

		/* general feature flags */
		cpu_caps.hasMMX  = TEST_BIT (regs2[3], 23); /* 0x0800000 */
		cpu_caps.hasSSE  = TEST_BIT (regs2[3], 25); /* 0x2000000 */
		cpu_caps.hasSSE2 = TEST_BIT (regs2[3], 26); /* 0x4000000 */
		cpu_caps.hasMMX2 = cpu_caps.hasSSE; /* SSE cpus supports mmxext too */

		cacheline = ((regs2[1] >> 8) & 0xFF) * 8;
		if (cacheline > 0)
			cpu_caps.cacheline = cacheline;
	}

	cpuid (0x80000000, regs);

	if (regs[0] >= 0x80000001) {

		cpuid (0x80000001, regs2);

		cpu_caps.hasMMX  |= TEST_BIT (regs2[3], 23); /* 0x0800000 */
		cpu_caps.hasMMX2 |= TEST_BIT (regs2[3], 22); /* 0x400000 */
		cpu_caps.has3DNow    = TEST_BIT (regs2[3], 31); /* 0x80000000 */
		cpu_caps.has3DNowExt = TEST_BIT (regs2[3], 30);
	}

	if (regs[0] >= 0x80000006) {
		cpuid (0x80000006, regs2);
		cpu_caps.cacheline = regs2[2] & 0xFF;
	}

#if defined(VISUAL_ARCH_X86)
	if (cpu_caps.hasSSE)
		check_os_katmai_support ();

	if (!cpu_caps.hasSSE)
		cpu_caps.hasSSE2 = FALSE;
#endif

#endif /* VISUAL_ARCH_X86 || VISUAL_ARCH_X86_64 */

#if defined(VISUAL_ARCH_POWERPC)
	check_os_altivec_support ();
#endif /* VISUAL_ARCH_POWERPC */

	print_cpu_info ();

	cpu_initialized = TRUE;
}

VisCPUType visual_cpu_get_type ()
{
	visual_return_val_if_fail (cpu_initialized, VISUAL_CPU_TYPE_OTHER);

	return cpu_caps.type;
}

unsigned int visual_cpu_get_num_cores ()
{
	visual_return_val_if_fail (cpu_initialized, 1);

	return cpu_caps.num_cores;
}

int visual_cpu_has_mmx ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasMMX;
}

int visual_cpu_has_mmx2 ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasMMX2;
}

int visual_cpu_has_sse ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasSSE;
}
int visual_cpu_has_sse2 ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasSSE2;
}

int visual_cpu_has_3dnow ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.has3DNow;
}

int visual_cpu_has_3dnow_ext ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.has3DNowExt;
}

int visual_cpu_has_altivec ()
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasAltiVec;
}

int visual_cpu_has_armv7 (void)
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasARMv7;
}

int visual_cpu_has_vfpv3 (void)
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasVFPv3;
}

int visual_cpu_has_neon (void)
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasNeon;
}

int visual_cpu_has_ldrex_strex (void)
{
	visual_return_val_if_fail (cpu_initialized, FALSE);

	return cpu_caps.hasLDREX_STREX;
}
