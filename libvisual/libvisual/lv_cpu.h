/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	        Chong Kai Xiong <kaixiong@codeleft.sg>
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

#include <libvisual/lv_types.h>

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

LV_BEGIN_DECLS

/**
 * Returns the processor architecture.
 *
 * @return processor architecture
 */
LV_API VisCPUType visual_cpu_get_type (void);

/**
 * Returns the number of processor cores
 */
LV_API unsigned int visual_cpu_get_num_cores (void);

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
