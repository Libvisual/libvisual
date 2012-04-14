/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Chong Kai Xiong <descender@phreaker.net>
 *
 * $Id: lv_cpu.h,v 1.16 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_CPU_H
#define _LV_CPU_H

#include <libvisual/lv_object.h>

/**
 * @defgroup VisCPU VisCPU
 * @{
 */

/**
 * Enumerate containing the different architectual types.
 */
typedef enum {
	VISUAL_CPU_TYPE_MIPS,		/**< Running on the mips architecture. */
	VISUAL_CPU_TYPE_ALPHA,		/**< Running on the alpha architecture. */
	VISUAL_CPU_TYPE_SPARC,		/**< Running on the sparc architecture. */
	VISUAL_CPU_TYPE_X86,		/**< Running on the X86 architecture. */
	VISUAL_CPU_TYPE_POWERPC,	/**< Running on the PowerPC architecture. */
	VISUAL_CPU_TYPE_ARM,		/**< Running on the ARM architecture, */
	VISUAL_CPU_TYPE_OTHER		/**< Running on an architecture that is not specified. */
} VisCPUType;

typedef struct _VisCPU VisCPU;

/**
 * This VisCPU structure contains information regarding the processor.
 *
 * @see visual_cpu_get_info
 */
struct _VisCPU {
	VisObject	object;			/**< The VisObject data. */
	VisCPUType	type;			/**< Contains the CPU/arch type. */
	int		nrcpu;			/**< Contains the number of CPUs in the system. */

	/* Feature flags */
	int		x86cpuType;		/**< The x86 cpu family type. */
	int		cacheline;		/**< The size of the cacheline. */

	int		hasMMX;			/**< The CPU has the mmx feature. */
	int		hasMMX2;		/**< The CPU has the mmx2 feature. */
	int		hasSSE;			/**< The CPU has the sse feature. */
	int		hasSSE2;		/**< The CPU has the sse2 feature. */
	int		has3DNow;		/**< The CPU has the 3dnow feature. */
	int		has3DNowExt;		/**< The CPU has the 3dnowext feature. */
	int		hasAltiVec;     /**< The CPU has the altivec feature. */
	int		hasARMv7;       /**<The CPU has the ARM v7 feature. */
	int		hasVFPv3;       /**<The CPU has the ARM VFPv3 feature. */
	int		hasNeon;        /**<The CPU has the ARM Neon feature. */
	int		hasLDREX_STREX; /**<The CPU has ARM LDREX_STREX feature. */
};

LV_BEGIN_DECLS

/**
 * Initializes the VisCPU caps structure by detecting the CPU features
 * and flags.
 *
 * This is normally called by visual_init() and is needed by
 * visual_mem_initialize() in order to detect the most optimal
 * mem_copy and mem_set functions.
 */
LV_API void visual_cpu_initialize (void);

/**
 * Function to get the VisCPU caps initialized by
 * visual_cpu_initialize(), this contains information regarding the
 * CPU features and flags.
 *
 * @return The VisCPU caps structure.
 */
LV_API const VisCPU *visual_cpu_get_caps (void);

/**
 * Function to retrieve if the tsc CPU feature is enabled.
 *
 * @return Whether tsc is enabled or not.
 */
LV_API int visual_cpu_has_tsc (void);

/**
 * Function to retrieve if the MMX CPU feature is enabled.
 *
 * @return Whether MMX is enabled or not.
 */
LV_API int visual_cpu_has_mmx (void);

/**
 * Function to retrieve if the MMX2 CPU feature is enabled.
 *
 * @return Whether MMX2 is enabled or not.
 */
LV_API int visual_cpu_has_mmx2 (void);

/**
 * Function to retrieve if the SSE CPU feature is enabled.
 *
 * @return Whether SSE is enabled or not.
 */
LV_API int visual_cpu_has_sse (void);

/**
 * Function to retrieve if the SSE2 CPU feature is enabled.
 *
 * @return Whether SSE2 is enabled or not.
 */
LV_API int visual_cpu_has_sse2 (void);

/**
 * Function to retrieve if the 3dnow CPU feature is enabled.
 *
 * @return Whether 3dnow is enabled or not.
 */
LV_API int visual_cpu_has_3dnow (void);

/**
 * Function to retrieve if the 3dnowext CPU feature is enabled.
 *
 * @return Whether 3dnowext is enabled or not.
 */
LV_API int visual_cpu_has_3dnow2 (void);

/**
 * Function to retrieve if the altivec CPU feature is enabled.
 *
 * @return Whether altivec is enabled or not.
 */
LV_API int visual_cpu_has_altivec (void);

/**
 * Function to retrieve if the ARM v7 feature is enabled.
 *
 * @return Whether altivec is enabled or not.
 */
LV_API int visual_cpu_has_armv7 (void);

/**
 * Function to retrieve if the ARM VFPv3 feature is enabled.
 *
 * @return Whether altivec is enabled or not.
 */
LV_API int visual_cpu_has_vfpv3 (void);

/**
 * Function to retrieve if the ARM Neon feature is enabled.
 *
 * @return Whether altivec is enabled or not.
 */
LV_API int visual_cpu_has_neon (void);

/**
 * Function to retrieve if the ARM LDREX_STREX feature is enabled.
 *
 * @return Whether altivec is enabled or not.
 */
LV_API int visual_cpu_has_ldrex_strex (void);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_CPU_H */
