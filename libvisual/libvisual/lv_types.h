#ifndef _LV_TYPES_H
#define _LV_TYPES_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_CHECK_CAST(uiobj, cast_type, cast)    ((cast*) (uiobj))

#define uint8_t		u_int8_t
#define uint16_t	u_int16_t
#define uint32_t	u_int32_t

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_TYPES_H */
