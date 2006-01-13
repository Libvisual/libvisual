// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_random.hpp,v 1.2 2006-01-13 07:44:44 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

#ifndef LVCPP_RANDOM_HPP
#define LVCPP_RANDOM_HPP

#include <libvisual/lv_random.h>
#include <libvisual-cpp/lv_object.hpp>

namespace Lv
{

  class RandomContext
      : public Object
  {
  public:

      explicit RandomContext (uint32_t seed = 0)
          : Object (vis_random_context_to_object (visual_random_context_new (seed)))
      {}

      inline void set_seed (uint32_t seed)
      {
          visual_random_context_set_seed (&vis_random_context (), seed);
      }

      inline uint32_t get_seed () const
      {
          return visual_random_context_get_seed (const_cast<VisRandomContext *> (&vis_random_context ()));
      }

      inline uint32_t get_seed_state () const
      {
          return visual_random_context_get_seed_state (const_cast<VisRandomContext *> (&vis_random_context ()));
      }

      template <typename Value>
      Value value ();

      template <typename Value>
      Value value_range (Value min, Value max);

      inline bool decide (float x)
      {
          return visual_random_context_decide (&vis_random_context (), x);
      }

      inline const VisRandomContext& vis_random_context () const
      {
          return reinterpret_cast<const VisRandomContext&> (vis_object ());
      }

      inline VisRandomContext& vis_random_context ()
      {
          return reinterpret_cast<VisRandomContext&> (vis_object ());
      }

  private:

      static VisObject *vis_random_context_to_object (VisRandomContext *context)
      {
          return reinterpret_cast<VisObject *> (context);
      }

      // hide copy constructor and assignment operator for the moment
      RandomContext (const RandomContext& other);
      const RandomContext& operator = (const RandomContext& other);
  };


  // template member specializations

  template <>
  inline int RandomContext::value<int> ()
  {
      return visual_random_context_int (&vis_random_context ());
  }

  template <>
  inline float RandomContext::value<float> ()
  {
      return visual_random_context_float (&vis_random_context ());
  }

  template <>
  inline double RandomContext::value<double> ()
  {
      return visual_random_context_double (&vis_random_context ());
  }

  template <>
  inline int RandomContext::value_range<int> (int min, int max)
  {
      return visual_random_context_int_range (&vis_random_context (), min, max);
  }

} // namespace Lv

#endif // #ifndef LVCPP_RANDOM_HPP
