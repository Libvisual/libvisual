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
	return (visual_random_int () % (max - min + 1)) + min;
}

int visual_random_decide (float a)
{
	float x = (float) visual_random_int () / VISUAL_RANDOM_MAX;

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


