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

typedef enum {
	VISUAL_CPU_TYPE_X86,
	VISUAL_CPU_TYPE_POWERPC,
	VISUAL_CPU_TYPE_OTHER
} VisCPUType;

typedef struct _VisCPU VisCPU;

/**
 * This VisCPU structure contains information regarding the processor.
 *
 * @see visual_cpu_get_info
 */
struct _VisCPU {
	VisCPUType	type;	/**< Contains the CPU/arch type. */
	uint32_t	flags;	/**< Contains feature flags for the CPU. */
	int		nrcpu;	/**< Contains the number of CPUs in the system. */
};

void visual_cpu_initialize (void);
VisCPU *visual_cpu_get_info (void);

int visual_cpu_feature_disable (int flag);
int visual_cpu_feature_enable (int flag);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_CPU_H */
