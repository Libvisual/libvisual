/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *      Chong Kai Xiong <descender@phreaker.net>
 *
 * $Id: lv_fourier.h,v 1.8 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_FOURIER_H
#define _LV_FOURIER_H

#include <libvisual/lv_object.h>

/**
 * @defgroup VisDFT VisDFT
 * @{
 */

#ifdef __cplusplus

#include <memory>
#include <libvisual/lv_singleton.hpp>

namespace LV {

  class Fourier
      : public Singleton<Fourier>
  {
  public:

      static void init ()
      {
          if (m_instance != 0)
              m_instance = new Fourier;
      }

      // FIXME: Hide this
      ~Fourier ();

  private:

      class Impl;

      std::auto_ptr<Impl> m_impl;

      Fourier ();
  };

  class DFT
  {
  public:

      /**
       * Function to create a new VisDFT Discrete Fourier Transform
       * context used to calculate amplitude spectrums over audio data.
       *
       * \note For optimal performance, use a power-of-2 spectrum
       * size. The current implementation does not use the Fast Fourier
       * Transform for non powers of 2.
       *
       * \note If samples_in is smaller than 2 * samples_out, the input
       * will be padded with zeroes.
       *
       * @param samples_in  The number of samples provided to every call to
       *                    DFT::perform() as input.
       *
       * @param samples_out Size of output spectrum (number of output samples).
       */
      DFT (unsigned int samples_out, unsigned int samples_in);

      unsigned int get_spectrum_size () const;

      /**
       * Function to perform a Discrete Fourier Transform over a set of data.
       *
       * \note Output samples are normalised to [0.0, 1.0] by dividing with the
       * spectrum size.
       *
       * @param output Array of output samples
       * @param input Array of input samples with values in [-1.0, 1.0]
       */
      void perform (float *output, float const* input);

      /**
       * Function to scale an ampltitude spectrum logarithmically.
       *
       * \note Scaled values are guaranteed to be in [0.0, 1.0].
       *
       * @param output Array of output samples
       * @param input  Array of input samples with values in [0.0, 1.0]
       * @param size Array size.
       *
       */
      static void log_scale (float* output, float const* input, unsigned int size);

      static void log_scale_standard (float* output, float const* input, unsigned int size);

      static void log_scale_custom (float* output, float const* input, unsigned int size, float log_scale_divisor);

  private:

      class Impl;

      std::auto_ptr<Impl> m_impl;
  };

}  // LV namespace

#endif /* __cplusplus */


/* C API bindings */

VISUAL_BEGIN_DECLS

#define VISUAL_DFT(obj)                 (VISUAL_CHECK_CAST ((obj), VisDFT))

#ifdef __cplusplus
typedef ::LV::DFT VisDFT;
#else
typedef struct _VisDFT VisDFT;
struct _VisDFT;
#endif

void visual_fourier_initialize (void);
int  visual_fourier_is_initialized (void);
void visual_fourier_deinitialize (void);

VisDFT *visual_dft_new (unsigned int samples_out, unsigned int samples_in);
void visual_dft_free (VisDFT *dft);

void visual_dft_perform (VisDFT *dft, float *output, float const *input);

void visual_dft_log_scale (float *output, float const *input, unsigned int size);
void visual_dft_log_scale_standard (float *output, float const *input, unsigned int size);
void visual_dft_log_scale_custom (float *output, float const *input, unsigned int size, float log_scale_divisor);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_FOURIER_H */
