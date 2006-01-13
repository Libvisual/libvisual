// Libvisual-c++ - C++ bindings for Libvisual
//
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_fourier.hpp,v 1.1 2006-01-13 06:25:01 descender Exp $
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

#ifndef LVCPP_FOURIER_HPP
#define LVCPP_FOURIER_HPP

#include <libvisual/lv_fourier.h>
#include <lv_object.hpp>

namespace Lv
{
  class Fourier
  {
  public:

      static inline int init ()
      {
          return visual_fourier_initialize ();
      }

      static inline bool is_init ()
      {
          return visual_fourier_is_initialized ();
      }

      static inline void deinit ()
      {
          visual_fourier_deinitialize ();
      }
  };

  class DFT
      : public Object
  {
  public:

      DFT (int samples_in, int samples_out)
          : Object (vis_dft_to_object (visual_dft_new (samples_in, samples_out)))
      {}

      inline void perform (const float *input, float *output)
      {
          visual_dft_perform (&vis_dft (), const_cast<float *> (input), output);
      }

      inline int normalise (float *spectrum, int size)
      {
          return visual_dft_normalise (spectrum, size);
      }

      inline int get_spectrum_size () const
      {
          return vis_dft ().spectrum_size;
      }

      inline int get_samples_in () const
      {
          return vis_dft ().samples_in;
      }

      inline const VisDFT &vis_dft () const
      {
          return *reinterpret_cast<const VisDFT *> (&vis_object ());
      }

      inline VisDFT &vis_dft ()
      {
          return *reinterpret_cast<VisDFT *> (&vis_object ());
      }

  private:

      static VisObject *vis_dft_to_object (VisDFT *dft)
      {
          return reinterpret_cast<VisObject *> (dft);
      }

      // hide copy constructor and assignment operator for the moment
      DFT (const DFT& other);
      const DFT& operator = (const DFT& other);
  };

} // namespace Lv

#endif // #ifndef LVCPP_FOURIER_HPP
