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
 * @defgroup VisRandom VisRandom
 * @{
 */

/**
 * Function to set the seed for the global randomizer.
 *
 * @param seed The seed to be used in the randomizer.
 */
void visual_random_set_seed (uint32_t seed)
{
	_lv_randseed = seed;
}

/**
 * Get the current seed of the global randomizer. This
 * is a state snapshot, and can be different from a
 * previous set seed.
 *
 * @return The current seed of the randomizer.
 */
uint32_t visual_random_get_seed ()
{
	return _lv_randseed;
}

/**
 * Gives a random intenger using libvisual it's randomizer.
 *
 * @return A pseudo random integer.
 */
uint32_t visual_random_int ()
{
	return (_lv_randseed = val_a * _lv_randseed + val_c);
}

/**
 * Gives a random integer ranging between min and max.
 *
 * @param min The minimum for the output.
 * @param max The maximum for the output.
 *
 * @return A pseudo random integer confirm to the minimum and maximum.
 */
uint32_t visual_random_int_range (int min, int max)
{
	return (visual_random_int () / (VISUAL_RANDOM_MAX / (max - min + 1))) + min;
}

/**
 * Function which returns 1 with a propability of p (0.0 <= p <= 1.0).
 *
 * @param a The float to be used in the decide.
 *
 * @return 0 or 1.
 */
int visual_random_decide (float a)
{
	float x = (float) visual_random_int () / VISUAL_RANDOM_MAX;

	return x <= a;
}

/**
 * Creates a new VisRandomContext data structure.
 *
 * @param seed The seed to be used to initialize the VisRandomContext with.
 *
 * @return A newly allocated VisRandomContext, or NULL on error.
 */
VisRandomContext *visual_random_context_new (uint32_t seed)
{
	VisRandomContext *rcontext;

	rcontext = visual_mem_new0 (VisRandomContext, 1);
	visual_random_context_set_seed (rcontext, seed);

	return rcontext;
}

/**
 * Frees the VisRandomContext. This frees the VisRandomContext data structure.
 *
 * @param rcontext Pointer to a VisRandomContext that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_random_context_free (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	visual_mem_free (rcontext);

	return 0;
}

/**
 * Set the seed to for a VisRandomContext.
 *
 * @param rcontext Pointer to the VisRandomContext for which the seed it set.
 * @param seed The seed which is set in the VisRandomContext.
 *
 * @return 0 on succes -1 on error.
 */
int visual_random_context_set_seed (VisRandomContext *rcontext, uint32_t seed)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	rcontext->seed = seed;
	rcontext->seed_state = seed;

	return 0;
}

/**
 * Get the seed that has been set to the VisRandomContext. This returns
 * the initial seed. Not the state seed.
 *
 * @see visual_random_context_get_seed_state
 *
 * @param rcontext The pointer to the VisRandomContext of which the initial random seed is requested.
 *
 * @return The initial random seed.
 */
uint32_t visual_random_context_get_seed (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, 0);

	return rcontext->seed;
}

/**
 * Get the current state seed for the VisRandomContext.
 *
 * @see visual_random_context_get_seed
 *
 * @param rcontext The pointer to the VisRandomContext of which the state seed is requested.
 *
 * @return The current state seed for the randomizer.
 */
uint32_t visual_random_context_get_seed_state (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, 0);

	return rcontext->seed_state;
}

/**
 * Gives a random integer using the VisRandomContext as context for the randomizer.
 *
 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
 *
 * @return A pseudo random integer.
 */
uint32_t visual_random_context_int (VisRandomContext *rcontext)
{
	visual_log_return_val_if_fail (rcontext != NULL, 0);

	return (rcontext->seed_state = val_a * rcontext->seed_state + val_c);
}

/**
 * Gives a random integer ranging between min and max using the VisRandomContext as context
 * for the randomizer.
 *
 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
 * @param min The minimum for the output.
 * @param max The maximum for the output.
 *   
 * @return A pseudo random integer confirm to the minimum and maximum.
 */
uint32_t visual_random_context_int_range (VisRandomContext *rcontext, int min, int max)
{
	visual_log_return_val_if_fail (rcontext != NULL, 0);

	return (visual_random_context_int (rcontext) / (VISUAL_RANDOM_MAX / (max - min + 1))) + min;
}

/**
 * Function which returns 1 with a propability of p (0.0 <= p <= 1.0) using the VisRandomContext
 * as context for the randomizer.
 *
 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
 * @param a The float to be used in the decide.
 *
 * @returns 0 or 1, or -1 on error.
 */
int visual_random_context_decide (VisRandomContext *rcontext, float a)
{
	visual_log_return_val_if_fail (rcontext != NULL, -1);

	float x = (float) visual_random_context_int (rcontext) / VISUAL_RANDOM_MAX;

	return x <= a;
}

/**
 * @}
 */

