/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
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
 *
 * @return number of processor cores
 */
LV_API unsigned int visual_cpu_get_num_cores (void);

/**
 * Returns whether processor supports MMX instructions.
 *
 * @note Only valid for x86 processors.
 *
 * @return TRUE if MMX is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_mmx (void);

/**
 * Returns whether processor supports MMX2 instructions.
 *
 * @note Only valid for x86 processors.
 *
 * @return TRUE if MMX2 is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_mmx2 (void);

/**
 * Returns whether processor supports SSE instructions.
 *
 * @note Only valid for x86 processors.
 *
 * @return TRUE if SSE is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_sse (void);

/**
 * Returns whether processor supports SSE2 instructions.
 *
 * @note Only valid for x86 processors.
 *
 * @return TRUE if SSE2 is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_sse2 (void);

/**
 * Returns whether processor supports 3DNow!.
 *
 * @note Only valid for x86 processors.
 *
 * @return TRUE if 3DNow! is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_3dnow (void);

/**
 * Returns whether processor supports 3DNow! extensions.
 *
 * @note Only valid for x86 processors
 *
 * @return TRUE if 3DNow! extensions are supported, FALSE otherwise
 */
LV_API int visual_cpu_has_3dnow_ext (void);

/**
 * Returns whether processor supports AltiVec instructions.
 *
 * @note Only valid for PowerPC processors.
 *
 * @return TRUE if AltiVec is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_altivec (void);

/**
 * Returns whether processor supports ARMv7.
 *
 * @note Only valid for ARM processors.
 *
 * @return TRUE if ARMv7 is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_armv7 (void);

/**
 * Returns whether processor supports the ARM VFPv3 extension.
 *
 * @note Only valid for ARM processors.
 *
 * @return TRUE if VFPv3 is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_vfpv3 (void);

/**
 * Returns whether processor supports the ARM NEON extension.
 *
 * @note Only valid for ARM processors.
 *
 * @return TRUE if NEON is supported, FALSE otherwise
 */
LV_API int visual_cpu_has_neon (void);

/**
 * Function to retrieve if ARM LDREX and STREX instructions are supported
 *
 * @note Only valid for ARM processors.
 *
 * @return TRUE if LDREX and STREX are supported, FALSE otherwise
 */
LV_API int visual_cpu_has_ldrex_strex (void);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_CPU_H */
