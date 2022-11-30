/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 8          Dennis Smit <ds@nerds-incorporated.org>
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

#include "config.h"
#include "lv_random.h"
#include "lv_common.h"
#include <random>

namespace LV {

  typedef std::minstd_rand Generator;

  class RandomContext::Impl
  {
  public:

      Generator generator;
  };

  RandomContext::RandomContext (Seed seed)
      : m_impl (new Impl)
  {
      set_seed (seed);
  }

  RandomContext::RandomContext (RandomContext&& rhs)
      : m_impl {std::move (rhs.m_impl)}
  {
      // empty
  }

  RandomContext::~RandomContext ()
  {
      // empty
  }

  RandomContext& RandomContext::operator= (RandomContext&& rhs)
  {
      m_impl.swap (rhs.m_impl);
      return *this;
  }

  void RandomContext::set_seed (Seed seed)
  {
      m_impl->generator.seed (seed);
  }

  uint32_t RandomContext::get_int ()
  {
      return m_impl->generator ();
  }

  uint32_t RandomContext::get_int (unsigned int min, unsigned int max)
  {
  #if VISUAL_RANDOM_FAST_FP_RND
      // FIXME: Test this to see if this is really faster or produce
      // better distributed numbers

      // Uses fast floating number generator and two divisions elimitated.
      // More than 2 times faster than original.
      float fm = min; // +10% speedup...

      return get_float () * (max - min + 1) + fm;
  #else
      return get_int () % (max - min + 1) + min;
  #endif
  }

  double RandomContext::get_double ()
  {
      uint32_t irnd = m_impl->generator ();

  #if VISUAL_RANDOM_FAST_FP_RND
      union {
          unsigned int i[2];
          double d;
      } value;

      // This saves floating point division (20 clocks on AXP, 38 on P4)
      // and introduces store-to-load data size mismatch penalty and
      // substraction op.
      // Faster on AXP anyway :)
      value.i[0] = (irnd << 20);
      value.i[1] = 0x3ff00000 | (irnd >> 12);
      return value.d - 1.0;
  #else
      return double (irnd) / Generator::max ();
  #endif
  }

  float RandomContext::get_float ()
  {
      uint32_t irnd = m_impl->generator ();

  #if VISUAL_RANDOM_FAST_FP_RND
      // Saves floating point division. Introduces substraction. Yet faster! :)

      union {
          unsigned int i;
          float f;
      } value;

      value.i = 0x3f800000 | (t >> 9);
      return value.f - 1.0f;
  #else
      return float (irnd) / float(Generator::max ());
  #endif
  }

} // LV namespace
