#include "config.h"
#include "lv_fourier.h"
#include "lv_common.h"

extern "C" {

  VisDFT *visual_dft_new (unsigned int samples_out, unsigned int samples_in)
  {
      return new LV::DFT (samples_out, samples_in);
  }

  void visual_dft_free (VisDFT *dft)
  {
      delete dft;
  }

  void visual_dft_perform (VisDFT *self, float *output, float const *input)
  {
      visual_return_if_fail (self != nullptr);

      self->perform (output, input);
  }

  void visual_dft_log_scale (float *output, float const *input, unsigned int size)
  {
      LV::DFT::log_scale (output, input, size);
  }

  void visual_dft_log_scale_standard (float *output, float const *input, unsigned int size)
  {
      LV::DFT::log_scale (output, input, size);
  }

  void visual_dft_log_scale_custom (float *output, float const *input, unsigned int size, float log_scale_divisor)
  {
      LV::DFT::log_scale_custom (output, input, size, log_scale_divisor);
  }

} // C extern

