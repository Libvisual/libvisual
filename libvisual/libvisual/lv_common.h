#ifndef _LV_COMMON_H
#define _LV_COMMON_H

#include <sys/types.h>

#define uint8_t		u_int8_t
#define uint16_t	u_int16_t
#define uint32_t	u_int32_t

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef NULL
/**
 * NULL define.
 */
#define NULL	(0)
#endif

#ifndef FALSE
/**
 * FALSE define.
 */
#define FALSE	(0)
#endif

#ifndef TRUE
/**
 * TRUE define.
 */
#define TRUE	(!FALSE)
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */
	
#endif /* _LV_COMMON_H */
