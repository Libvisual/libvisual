#ifndef _LV_RANDOM_H
#define _LV_RANDOM_H

#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_RANDOM_MAX	4294967295U

void visual_random_set_seed (uint32_t seed);
uint32_t visual_random_get_seed (void);
uint32_t visual_random_int (void);
uint32_t visual_random_int_range (int min, int max);
int visual_random_decide (float a);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_RANDOM_H */
