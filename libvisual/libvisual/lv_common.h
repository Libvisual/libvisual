#ifndef _LV_COMMON_H
#define _LV_COMMON_H

#include <libvisual/lv_mem.h>
#include <libvisual/lv_log.h>
#include <libvisual/lv_error.h>
#include <libvisual/lv_types.h>

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
