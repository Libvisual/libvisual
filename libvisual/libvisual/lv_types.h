#ifndef _LV_TYPES_H
#define _LV_TYPES_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_CHECK_CAST(uiobj, cast_type, cast)    ((cast*) (uiobj))

#ifndef uint8_t
#define uint8_t		u_int8_t
#endif
	
#ifndef uint16_t
#define uint16_t	u_int16_t
#endif

#ifndef uint32_t
#define uint32_t	u_int32_t
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_TYPES_H */
