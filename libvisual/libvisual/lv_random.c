/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 * 	    Vitaly V. Bursov <vitalyvb@ukr.net>
 *
 * $Id: lv_random.c,v 1.19 2006/01/22 13:23:37 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_random.h"
#include "lv_common.h"

/* Thanks Burkhard Plaum <plaum@ipf.uni-stuttgart.de> for these values and some other hints */
#define val_a 1664525L		/* As suggested by Knuth */
#define val_c 1013904223L	/* As suggested by H.W. Lewis and is a prime close to 2^32 * (sqrt(5) - 2)) */

VisRandomContext __lv_internal_random_context;

VisRandomContext *visual_random_context_new (uint32_t seed)
{
	VisRandomContext *rcontext;

	rcontext = visual_mem_new0 (VisRandomContext, 1);

	visual_random_context_init (rcontext, seed);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (rcontext), TRUE);
	visual_object_ref (VISUAL_OBJECT (rcontext));

	return rcontext;
}

int visual_random_context_init (VisRandomContext *rcontext, uint32_t seed)
{
	visual_return_val_if_fail (rcontext != NULL, -VISUAL_ERROR_RANDOM_CONTEXT_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (rcontext));
	visual_object_set_dtor (VISUAL_OBJECT (rcontext), NULL);
	visual_object_set_allocated (VISUAL_OBJECT (rcontext), FALSE);

	/* Set the VisRandomContext data */
	visual_random_context_set_seed (rcontext, seed);

	return VISUAL_OK;
}

int visual_random_context_set_seed (VisRandomContext *rcontext, uint32_t seed)
{
	visual_return_val_if_fail (rcontext != NULL, -VISUAL_ERROR_RANDOM_CONTEXT_NULL);

	rcontext->seed = seed;
	rcontext->seed_state = seed;

	return VISUAL_OK;
}

uint32_t visual_random_context_get_seed (VisRandomContext *rcontext)
{
	visual_return_val_if_fail (rcontext != NULL, 0);

	return rcontext->seed;
}

uint32_t visual_random_context_get_seed_state (VisRandomContext *rcontext)
{
	visual_return_val_if_fail (rcontext != NULL, 0);

	return rcontext->seed_state;
}

uint32_t visual_random_context_int (VisRandomContext *rcontext)
{
	visual_return_val_if_fail (rcontext != NULL, 0);

	return (rcontext->seed_state = val_a * rcontext->seed_state + val_c);
}

uint32_t visual_random_context_int_range (VisRandomContext *rcontext, int min, int max)
{
#if VISUAL_RANDOM_FAST_FP_RND
	/* Uses fast floating number generator and two divisions elimitated.
	 * More than 2 times faster than original.
	 */
	float fm = min; /* +10% speedup... */

	visual_return_val_if_fail (rcontext != NULL, 0);

	return visual_random_context_float (rcontext) * (max - min + 1) + fm;
#else
	visual_return_val_if_fail (rcontext != NULL, 0);

	return (visual_random_context_int (rcontext) / (VISUAL_RANDOM_MAX / (max - min + 1))) + min;
#endif
}

double visual_random_context_double (VisRandomContext *rcontext)
{
#if VISUAL_RANDOM_FAST_FP_RND
	union {
		unsigned int i[2];
		double d;
	} value;
#endif
	uint32_t irnd;

	visual_return_val_if_fail (rcontext != NULL, -1);

	irnd = (rcontext->seed_state = val_a * rcontext->seed_state + val_c);
#if VISUAL_RANDOM_FAST_FP_RND
	/* This saves floating point division (20 clocks on AXP,
	 * 38 on P4) and introduces store-to-load data size mismatch penalty
	 * and substraction op.
	 * Faster on AXP anyway :)
	 */

	value.i[0] = (irnd << 20);
	value.i[1] = 0x3ff00000 | (irnd >> 12);

	return value.d - 1.0;
#else
	return (double) irnd / VISUAL_RANDOM_MAX;
#endif
}

float visual_random_context_float (VisRandomContext *rcontext)
{
#if VISUAL_RANDOM_FAST_FP_RND
	union {
		unsigned int i;
		float f;
	} value;
#endif
	uint32_t irnd;

	visual_return_val_if_fail (rcontext != NULL, -1);

	irnd = (rcontext->seed_state = val_a * rcontext->seed_state + val_c);
#if VISUAL_RANDOM_FAST_FP_RND
	/* Saves floating point division. Introduces substraction.
	 * Yet faster! :)
	 */

	value.i = 0x3f800000 | (t >> 9);

	return value.f - 1.0f;
#else
	return (float) irnd / VISUAL_RANDOM_MAX;
#endif
}

int visual_random_context_decide (VisRandomContext *rcontext, float a)
{
	visual_return_val_if_fail (rcontext != NULL, -VISUAL_ERROR_RANDOM_CONTEXT_NULL);

	return visual_random_context_float (rcontext) <= a;
}
