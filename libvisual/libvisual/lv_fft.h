/* fft.h: Header for iterative implementation of a FFT
 * Copyright (C) 1999 Richard Boulton <richard@tartarus.org>
 */

#ifndef _LV_FFT_H
#define _LV_FFT_H

#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_FFTSTATE(obj)				(VISUAL_CHECK_CAST ((obj), 0, VisFFTState))

/**
 * Private FFT define that contains the log size.
 */
#define VISUAL_FFT_BUFFER_SIZE_LOG 9

/**
 * Private FFT define that contains the buffer size.
 */
#define VISUAL_FFT_BUFFER_SIZE (1 << VISUAL_FFT_BUFFER_SIZE_LOG)

typedef struct _VisFFTState VisFFTState;

/**
 * Private structure to contain Fast Fourier Transform states in.
 */
struct _VisFFTState {
	VisObject	object;				/**< The VisObject data. */
	/* Temporary data stores to perform FFT in. */
	float		real[VISUAL_FFT_BUFFER_SIZE];	/**< Private data that is used by the FFT engine. */
	float		imag[VISUAL_FFT_BUFFER_SIZE];	/**< Private data that is used by the FFT engine. */
};

VisFFTState *visual_fft_init (void);
void visual_fft_perform (const int16_t *input, float *output, VisFFTState *state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_FFT_H */
