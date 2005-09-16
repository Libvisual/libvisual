// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_fft.hpp,v 1.2 2005-09-16 20:09:53 descender Exp $
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

#ifndef LVCPP_FFT_HPP
#define LVCPP_FFT_HPP

#include <libvisual/lv_fft.h>
#include <lv_object.hpp>

namespace Lv
{
  
  class FFT
      : public Object
  {
  public:

      static inline int init ()
      {
          return visual_fft_initialize ();
      }

      static inline bool is_init ()
      {
          return visual_fft_is_initialized ();
      }

      static inline void deinit ()
      {
          visual_fft_deinitialize ();
      }

      FFT (int samples_in, int samples_out)
          : Object (vis_fft_to_object (visual_fft_new (samples_in, samples_out)))
      {}

      inline void perform (const float *input, float *output)
      {
          visual_fft_perform (&vis_fft (), const_cast<float *> (input), output);
      }

      inline int get_spectrum_size () const
      {
          return vis_fft ().spectrum_size;
      }

      inline int get_samples_in () const
      {
          return vis_fft ().samples_in;
      }

      inline const VisFFT &vis_fft () const
      {
          return *reinterpret_cast<const VisFFT *> (&vis_object ());
      }

      inline VisFFT &vis_fft ()
      {
          return *reinterpret_cast<VisFFT *> (&vis_object ());
      }

  private:

      static VisObject *vis_fft_to_object (VisFFT *fft)
      {
          return reinterpret_cast<VisObject *> (fft);
      }

      // hide copy constructor and assignment operator for the moment
      FFT (const FFT& other);
      const FFT& operator = (const FFT& other);
  };

} // namespace Lv

#endif // #ifndef LVCPP_FFT_HPP
