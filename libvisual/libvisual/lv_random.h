/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 * 	    Vitaly V. Bursov <vitalyvb@ukr.net>
 *
 * $Id: lv_random.h,v 1.15 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_RANDOM_H
#define _LV_RANDOM_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_types.h>
#include <libvisual/lv_defines.h>

/**
 * @defgroup VisRandom VisRandom
 * @{
 */


/**
 * The highest random nummer.
 */
#define VISUAL_RANDOM_MAX	4294967295U

#ifdef __cplusplus

#include <libvisual/lv_scoped_ptr.hpp>

namespace LV {

  typedef uint32_t RandomSeed;

  class RandomContext
  {
  public:

	typedef RandomSeed Seed;

	/**
	 * Creates a new VisRandomContext data structure.
	 *
	 * @param seed The seed to be used to initialize the VisRandomContext with.
	 *
	 * @return A newly allocated VisRandomContext, or NULL on failure.
	 */
	RandomContext (Seed seed);

	~RandomContext ();

	/**
	 * Set the seed to for a VisRandomContext.
	 *
	 * @param rcontext Pointer to the VisRandomContext for which the seed it set.
	 * @param seed The seed which is set in the VisRandomContext.
	 *
	 * @return VISUAL_OK on success, -VISUAL_ERROR_RANDOM_CONTEXT_NULL on failure.
	 */
	void set_seed (uint32_t seed);

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
	Seed get_seed () const;

	/**
	 * Get the current state seed for the VisRandomContext.
	 *
	 * @see visual_random_context_get_seed
	 *
	 * @param rcontext The pointer to the VisRandomContext of which the state seed is requested.
	 *
	 * @return The current state seed for the randomizer.
	 */
	Seed get_seed_state () const;

	/**
	 * Gives a random integer using the VisRandomContext as context for the randomizer.
	 *
	 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
	 *
	 * @return A pseudo random integer.
	 */
	uint32_t get_int ();

	/**
	 * Gives a random integer ranging between min and max using the
	 * VisRandomContext as context for the randomizer.  This function may
	 * use floating point instructions. Remeber, this will break things if
	 * used inside of MMX code.
	 *
	 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
	 * @param min The minimum for the output.
	 * @param max The maximum for the output.
	 *
	 * @return A pseudo random integer confirm to the minimum and maximum.
	 */
	uint32_t get_int (unsigned int min, unsigned int max);

	/**
	 * Gives a random double precision floating point value
	 * using the VisRandomContext as context for the randomizer.
	 *
	 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
	 *
	 * @return A pseudo random integer.
	 */
	double get_double ();

	/**
	 * Gives a random single precision floating point value
	 * using the VisRandomContext as context for the randomizer.
	 *
	 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
	 *
	 * @return A pseudo random integer.
	 */
	float get_float ();

	/**
	 * Function which returns 1 with a propability of p (0.0 <= p <= 1.0)
	 * using the VisRandomContext as context for the randomizer.
	 *
	 * @param rcontext The pointer to the VisRandomContext in which the state of the randomizer is set.
	 * @param p The float to be used in the decide.
	 *
	 * @returns 0 or 1, -VISUAL_ERROR_RANDOM_CONTEXT_NULL on failure.
	 */
	bool decide (float p);

  private:

	class Impl;

	ScopedPtr<Impl> m_impl;
  };

} // LV namespace

#endif /* __cplusplus */

/* LV C API */

#ifdef __cplusplus
typedef ::LV::RandomContext VisRandomContext;
#else
  typedef struct _VisRandomContext VisRandomContext;
  struct _VisRandomContext;
#endif

/**
 * The VisRandomContext data structure is used to keep track of
 * the randomizer it's state. When state tracking is used you need
 * to use the visual_random_context_* functions.
 */

VISUAL_BEGIN_DECLS

#define VISUAL_RANDOMCONTEXT(obj)			(VISUAL_CHECK_CAST ((obj), VisRandomContext))

/* Non context random macros */
extern VisRandomContext __lv_internal_random_context;

#define visual_random_set_seed(a) visual_random_context_set_seed(&__lv_internal_random_context, a)
#define visual_random_get_seed() visual_random_context_get_seed(&__lv_internal_random_context)
#define visual_random_int() visual_random_context_int(&__lv_internal_random_context)
#define visual_random_int_range(a, b) visual_random_context_int(&__lv_internal_random_context, a, b)
#define visual_random_double () visual_random_context_double(&__lv_internal_random_context);
#define visual_random_float () visual_random_context_float(&__lv_internal_random_context);
#define visual_random_decide(a) visual_random_int(&__lv_internal_random_context, a)

VisRandomContext *visual_random_context_new (uint32_t seed);
void visual_random_context_free (VisRandomContext *rcontext);

void visual_random_context_set_seed (VisRandomContext *rcontext, uint32_t seed);
uint32_t visual_random_context_get_seed (VisRandomContext *rcontext);
uint32_t visual_random_context_get_seed_state (VisRandomContext *rcontext);
uint32_t visual_random_context_int (VisRandomContext *rcontext);
uint32_t visual_random_context_int_range (VisRandomContext *rcontext, unsigned int min, unsigned int max);
double visual_random_context_double (VisRandomContext *rcontext);
float visual_random_context_float (VisRandomContext *rcontext);
int visual_random_context_decide (VisRandomContext *rcontext, float a);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_RANDOM_H */
