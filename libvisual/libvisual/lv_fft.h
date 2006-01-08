/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * The FFT implementation found in this file is based upon the NULLSOFT
 * Milkdrop FFT implementation.
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_fft.h,v 1.17 2006-01-08 15:43:30 descender Exp $
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

#ifndef _LV_FFT_H
#define _LV_FFT_H

#include <libvisual/lv_common.h>

VISUAL_BEGIN_DECLS

#define VISUAL_FFT(obj)					(VISUAL_CHECK_CAST ((obj), VisFFT))

typedef struct _VisFFT VisFFT;

/**
 * Private structure to contain Fast Fourier Transform states in.
 */
struct _VisFFT {
	VisObject	 object;			/**< The VisObject data. */
	int		 samples_in;			/**< The number of input samples. */
	int		 spectrum_size;			/**< The size of the spectrum (power of two). */
	float		*real;				/**< Private data that is used by the FFT engine. */
	float		*imag;				/**< Private data that is used by the FFT engine. */
	int		 brute_force;			/**< Private data that is used by the FFT engine. */
};

/* prototypes */
int visual_fft_initialize (void);
int visual_fft_is_initialized (void);
int visual_fft_deinitialize (void);

VisFFT *visual_fft_new (int samples_in, int samples_out);
int visual_fft_init (VisFFT *fft, int samples_in, int samples_out);

int visual_fft_perform (VisFFT *fft, float *input, float *output, int normalised);
int visual_fft_normalise (float *spectrum, int size);

VISUAL_END_DECLS

#endif /* _LV_FFT_H */
