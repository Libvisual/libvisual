/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *          Vitaly V. Bursov <vitalyvb@ukr.net>
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

#ifdef __cplusplus

#include <memory>

namespace LV {

  typedef uint32_t RandomSeed;

  /**
   * Pseudorandom number generator class.
   */
  class LV_API RandomContext
  {
  public:

      typedef RandomSeed Seed;

      /**
       * Creates a new RandomContext with a given seed.
       *
       * @param seed Initial seed for generating random number sequences
       */
      explicit RandomContext (Seed seed);

      RandomContext (RandomContext const&) = delete;

      /**
       * Move constructor
       */
      RandomContext (RandomContext&& rhs);

      /**
       * Destructor
       */
      ~RandomContext ();

      RandomContext& operator= (RandomContext const&) = delete;

      /*
       * Move assignment operator
       */
      RandomContext& operator= (RandomContext&& rhs);

      /**
       * Sets the seed.
       *
       * @param seed New seed
       */
      void set_seed (uint32_t seed);

      /**
       * Returns a random integer.
       *
       * @return A random integer
       */
      uint32_t get_int ();

      /**
       * Returns a random integer in a given range.
       *
       * @param min Lower bound
       * @param max Upper bound
       *
       * @return A random integer between min and max inclusive
       */
      uint32_t get_int (unsigned int min, unsigned int max);

      /**
       * Returns a random double-precision floating point value between 0.0 and 1.0.
       *
       * @return A random value between 0.0 and 1.0
       */
      double get_double ();

      /**
       * Returns a random single-precision floating point value between 0.0 and 1.0.
       *
       * @return A random value between 0.0 and 1.0
       */
      float get_float ();

  private:

      class Impl;
      std::unique_ptr<Impl> m_impl;
  };

} // LV namespace

#endif /* __cplusplus */

/* LV C API */

#ifdef __cplusplus
typedef ::LV::RandomContext VisRandomContext;
typedef ::LV::RandomSeed    VisRandomSeed;
#else
  typedef struct _VisRandomContext VisRandomContext;
  struct _VisRandomContext;

  typedef uint32_t VisRandomSeed;
#endif

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisRandomContext *visual_random_context_new  (VisRandomSeed seed);

LV_API void visual_random_context_free (VisRandomContext *rcontext);

LV_API void     visual_random_context_set_seed  (VisRandomContext *rcontext, VisRandomSeed seed);
LV_API uint32_t visual_random_context_int       (VisRandomContext *rcontext);
LV_API uint32_t visual_random_context_int_range (VisRandomContext *rcontext, unsigned int min, unsigned int max);
LV_API double   visual_random_context_double    (VisRandomContext *rcontext);
LV_API float    visual_random_context_float     (VisRandomContext *rcontext);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_RANDOM_H */
