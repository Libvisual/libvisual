#ifndef _LV_ENDIANESS_H
#define _LV_ENDIANESS_H

#include <libvisual/lvconfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Macros to convert LE <-> BE
 * 'I' stands for integer here.
 */
#define VISUAL_ENDIAN_LE_BE_I16(w) (\
	(((w) & 0xff00) >> 8) |\
	(((w) & 0x00ff) << 8) )

#define VISUAL_ENDIAN_LE_BE_I32(w) (\
	(((w) & 0x000000ff) << 24) | \
	(((w) & 0x0000ff00) << 8) | \
	(((w) & 0x00ff0000) >> 8) | \
	(((w) & 0xff000000) >> 24) )


#if VISUAL_BIG_ENDIAN & VISUAL_LITTLE_ENDIAN
#	error determining system endianess.
#endif

/**
 * Arch. dependant definitions
 */

#if VISUAL_BIG_ENDIAN
#	define VISUAL_ENDIAN_BEI16(x) (x)
#	define VISUAL_ENDIAN_BEI32(x) (x)
#	define VISUAL_ENDIAN_LEI16(x) VISUAL_ENDIAN_LE_BE_I16(x)
#	define VISUAL_ENDIAN_LEI32(x) VISUAL_ENDIAN_LE_BE_I32(x)
#endif

#if VISUAL_LITTLE_ENDIAN
#	define VISUAL_ENDIAN_LEI16(x) (x)
#	define VISUAL_ENDIAN_LEI32(x) (x)
#	define VISUAL_ENDIAN_BEI16(x) VISUAL_ENDIAN_LE_BE_I16(x)
#	define VISUAL_ENDIAN_BEI32(x) VISUAL_ENDIAN_LE_BE_I32(x)
#endif


#ifdef __cplusplus
}
#endif /* __cplusplus */   

#endif /* _LV_ENDIANESS_H */
