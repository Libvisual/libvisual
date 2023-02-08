/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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

#include <libvisual/lv_defines.h>

/**
 * @defgroup VisDFT VisDFT
 * @{
 */

#ifdef __cplusplus

#include <memory>

namespace LV {

  //! Computes a Discrete Fourier Transform
  class LV_API DFT
  {
  public:

      /**
       * Creates a DFT object used to calculate amplitude spectrums over audio data.
       *
       * @note For optimal performance, use a power-of-2 spectrum
       * size. The current implementation does not use the Fast
       * Fourier Transform for non powers of 2.
       *
       * @note If samples_in is smaller than 2 * samples_out, the input
       * will be padded with zeroes.
       *
       * @param samples_in  The number of samples provided to every
       *                    call to DFT::perform() as input.
       *
       * @param samples_out Size of output spectrum (number of output
       *                    samples).
       */
      DFT (unsigned int samples_out, unsigned int samples_in);

      DFT (DFT const&) = delete;

      /**
       * Move constructor
       */
      DFT (DFT&& rhs);

      /**
       * Destructor
       */
      ~DFT ();

      DFT& operator= (DFT const&) = delete;

      /**
       * Move assignment operator
       */
      DFT& operator= (DFT&& rhs);

      /**
       * Returns the output size of the DFT.
       *
       * @return Output size
       */
      unsigned int get_spectrum_size () const;

      /**
       * Performs a DFT over a set of input samples.
       *
       * @param output Array of output samples
       * @param input  Array of input samples with values in [-1.0, 1.0]
       */
      void perform (float *output, float const* input);

      /**
       * Logarithmically scales an amplitude spectrum.
       *
       * @note Scaled values are guaranteed to be in [0.0, 1.0].
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

      std::unique_ptr<Impl> m_impl;
  };

}  // LV namespace

#endif /* __cplusplus */


/* C API bindings */

#ifdef __cplusplus
typedef ::LV::DFT VisDFT;
#else
typedef struct _VisDFT VisDFT;
struct _VisDFT;
#endif

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisDFT *visual_dft_new  (unsigned int samples_out, unsigned int samples_in);

LV_API void visual_dft_free (VisDFT *dft);

LV_API void visual_dft_perform (VisDFT *dft, float *output, float const *input);

LV_API void visual_dft_log_scale (float *output, float const *input, unsigned int size);
LV_API void visual_dft_log_scale_standard (float *output, float const *input, unsigned int size);
LV_API void visual_dft_log_scale_custom (float *output, float const *input, unsigned int size, float log_scale_divisor);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_FOURIER_H */
