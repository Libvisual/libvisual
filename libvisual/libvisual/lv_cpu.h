#ifndef _LV_CPU_H
#define _LV_CPU_H

#include <libvisual/lvconfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_CPU_FLAG_ALTIVEC	0x1
#define VISUAL_CPU_FLAG_64BITS	0x2
#define VISUAL_CPU_FLAG_MMX	0x4
#define VISUAL_CPU_FLAG_XMMX	0x8
#define VISUAL_CPU_FLAG_SSE	0x10
#define VISUAL_CPU_FLAG_SSE2	0x20
#define VISUAL_CPU_FLAG_SSE3	0x40
#define VISUAL_CPU_FLAG_3DNOW	0x80

#define VISUAL_CPU_TYPE_X86	0x1
#define VISUAL_CPU_TYPE_POWERPC	0x2
#define VISUAL_CPU_TYPE_OTHER	0x3

typedef struct _VisCPU VisCPU;

struct _VisCPU {
	int		type;
	unsigned int	flags;
	int		nrcpu;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_CPU_H */
