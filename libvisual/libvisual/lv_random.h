#ifndef _LV_RANDOM_H
#define _LV_RANDOM_H

#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The highest random nummer.
 */
#define VISUAL_RANDOM_MAX	4294967295U

typedef struct _VisRandomContext VisRandomContext;

/**
 * The VisRandomContext data structure is used to keep track of
 * the randomizer it's state. When state tracking is used you need
 * to use the visual_random_context_* functions.
 */
struct _VisRandomContext {
	uint32_t	seed;		/**< The initial random seed. */
	uint32_t	seed_state;	/**< The current state seed. */
};

/* Non context random macros */
extern VisRandomContext __lv_internal_random_context;
#define visual_random_set_seed(a) visual_random_context_set_seed(__lv_internal_random_context, a)
#define visual_random_get_seed() visual_random_context_get_seed(__lv_internal_random_context)
#define visual_random_int() visual_random_context_int(__lv_internal_random_context)
#define visual_random_int_range(a, b) visual_random_context_int(__lv_internal_random_context, a, b)
#define visual_random_double () visual_random_context_double(__lv_internal_random_context);
#define visual_random_float () visual_random_context_float(__lv_internal_random_context);
#define visual_random_decide(a) visual_random_int(__lv_internal_random_context, a)

/* Context management */
VisRandomContext *visual_random_context_new (uint32_t seed);
int visual_random_context_free (VisRandomContext *rcontext);
int visual_random_context_set_seed (VisRandomContext *rcontext, uint32_t seed);
uint32_t visual_random_context_get_seed (VisRandomContext *rcontext);
uint32_t visual_random_context_get_seed_state (VisRandomContext *rcontext);

/* Context random functions */
uint32_t visual_random_context_int (VisRandomContext *rcontext);
uint32_t visual_random_context_int_range (VisRandomContext *rcontext, int min, int max);
double visual_random_context_double (VisRandomContext *rcontext);
float visual_random_context_float (VisRandomContext *rcontext);
int visual_random_context_decide (VisRandomContext *rcontext, float a);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_RANDOM_H */
