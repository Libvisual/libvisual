#ifndef _LV_RANDOM_H
#define _LV_RANDOM_H

#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_RANDOM_MAX	4294967295U

typedef struct _VisRandomContext VisRandomContext;

struct _VisRandomContext {
	uint32_t	seed;
	uint32_t	seed_state;
};

/* Non context random functions */
void visual_random_set_seed (uint32_t seed);
uint32_t visual_random_get_seed (void);
uint32_t visual_random_int (void);
uint32_t visual_random_int_range (int min, int max);
int visual_random_decide (float a);

/* Context management */
VisRandomContext *visual_random_context_new (uint32_t seed);
int visual_random_context_free (VisRandomContext *rcontext);
int visual_random_context_set_seed (VisRandomContext *rcontext, uint32_t seed);
int visual_random_context_get_seed (VisRandomContext *rcontext);
int visual_random_context_get_seed_state (VisRandomContext *rcontext);

/* Context random functions */
uint32_t visual_random_context_int (VisRandomContext *rcontext);
uint32_t visual_random_context_int_range (VisRandomContext *rcontext, int min, int max);
int visual_random_context_decide (VisRandomContext *rcontext, float a);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_RANDOM_H */
