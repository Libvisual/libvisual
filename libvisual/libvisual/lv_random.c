#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lv_random.h"

/* Thanks Burkhard Plaum <plaum@ipf.uni-stuttgart.de> for these values and some other hints */
#define val_a 1664525L		/* as suggested by Knuth */
#define val_c 1013904223L	/* as suggested by H.W. Lewis and is a prime close to 2^32 * (sqrt(5) - 2)) */

static uint32_t _lv_randseed;

/**
 * @defgroup VisList VisList
 * @{
 */

void visual_random_set_seed (uint32_t seed)
{
	_lv_randseed = seed;
}

uint32_t visual_random_get_seed ()
{
	return _lv_randseed;
}

uint32_t visual_random_int ()
{
	return (_lv_randseed = val_a * _lv_randseed + val_c);
}

uint32_t visual_random_int_range (int min, int max)
{
	return (visual_random_int () / (VISUAL_RANDOM_MAX / (max - min + 1))) + min;
}

int visual_random_decide (float a)
{
	float x = (float) visual_random_int () / VISUAL_RANDOM_MAX;

	return x <= a;
}

VisRandomContext *visual_random_context_new (uint32_t seed)
{
	VisRandomContext *rcontext;

	rcontext = visual_mem_new0 (VisRandomContext, 1);

	return rcontext;
}

int visual_random_context_free (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	visual_mem_free (rcontext);

	return 0;
}

int visual_random_context_set_seed (VisRandomContext *rcontext, uint32_t seed)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	rcontext->seed = seed;
	rcontext->seed_state = seed;

	return 0;
}

int visual_random_context_get_seed (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	return rcontext->seed;
}

int visual_random_context_get_seed_state (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	return rcontext->seed_state;
}

uint32_t visual_random_context_int (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	return (rcontext->seed_state = val_a * rcontext->seed_state + val_c);
}

uint32_t visual_random_context_int_range (VisRandomContext *rcontext, int min, int max)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	return (visual_random_context_int (rcontext) / (VISUAL_RANDOM_MAX / (max - min + 1))) + min;
}

int visual_random_context_decide (VisRandomContext *rcontext, float a)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	float x = (float) visual_random_context_int (rcontext) / VISUAL_RANDOM_MAX;

	return x <= a;
}

/**
 * @}
 */


/*
void visual_plugin_set_seed(VisPluginInfo*, uint32_t seed);
uint32_t visual_plugin_get_seed(VisPluginInfo*);
void visual_plugin_random(VisPluginInfo*);
*/


