/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *      Vitaly V. Bursov <vitalyvb@ukr.net>
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
#define val_a 1664525L      /* As suggested by Knuth */
#define val_c 1013904223L   /* As suggested by H.W. Lewis and is a prime close to 2^32 * (sqrt(5) - 2)) */


namespace LV {

  class RandomContext::Impl {
  public:
    Seed seed;
    Seed seed_state;
  };

  RandomContext::RandomContext (Seed seed)
    : m_impl (new Impl)
  {
    set_seed (seed);
  }

  RandomContext::~RandomContext ()
  {
    // empty
  }

  void RandomContext::set_seed (Seed seed)
  {
    m_impl->seed = seed;
    m_impl->seed_state = seed;
  }

  RandomContext::Seed RandomContext::get_seed () const
  {
    return m_impl->seed;
  }

  RandomContext::Seed RandomContext::get_seed_state () const
  {
    return m_impl->seed_state;
  }

  uint32_t RandomContext::get_int ()
  {
    return (m_impl->seed_state = val_a * m_impl->seed_state + val_c);
  }

  uint32_t RandomContext::get_int (unsigned int min, unsigned int max)
  {
#if VISUAL_RANDOM_FAST_FP_RND
    // Uses fast floating number generator and two divisions elimitated.
    // More than 2 times faster than original.
    float fm = min; // +10% speedup...

    return get_float () * (max - min + 1) + fm;
#else
    // FIXME: this looks wrong
    return (get_int () / (VISUAL_RANDOM_MAX / (max - min + 1))) + min;
#endif
  }

  double RandomContext::get_double ()
  {
#if VISUAL_RANDOM_FAST_FP_RND
    union {
        unsigned int i[2];
        double d;
    } value;
#endif
    uint32_t irnd;

    irnd = (m_impl->seed_state = val_a * m_impl->seed_state + val_c);

#if VISUAL_RANDOM_FAST_FP_RND
    // This saves floating point division (20 clocks on AXP, 38 on P4)
    // and introduces store-to-load data size mismatch penalty and
    // substraction op.
    // Faster on AXP anyway :)

    value.i[0] = (irnd << 20);
    value.i[1] = 0x3ff00000 | (irnd >> 12);

    return value.d - 1.0;
#else
    return double (irnd) / VISUAL_RANDOM_MAX;
#endif
}

  float RandomContext::get_float ()
  {
#if VISUAL_RANDOM_FAST_FP_RND
    union {
        unsigned int i;
        float f;
    } value;
#endif
    uint32_t irnd;

    irnd = (m_impl->seed_state = val_a * m_impl->seed_state + val_c);
#if VISUAL_RANDOM_FAST_FP_RND
    // Saves floating point division. Introduces substraction. Yet faster! :)

    value.i = 0x3f800000 | (t >> 9);

    return value.f - 1.0f;
#else
    return float (irnd) / VISUAL_RANDOM_MAX;
#endif
  }

  bool RandomContext::decide (float a)
  {
    return get_float () <= a;
  }

} // LV namespace


