/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * The FFT implementation found in this file is based upon the NULLSOFT
 * Milkdrop FFT implementation.
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *	    Chong Kai Xiong <descender@phreaker.net>
 *
 * $Id: lv_fourier.c,v 1.7 2006-01-15 00:15:14 synap Exp $
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

#define FOURIER_PI 3.141592653589793238462643383279502884197169399f

#include <config.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>

#include "lv_cache.h"
#include "lv_fourier.h"
#include "lv_utils.h"

#define DFT_CACHE_ENTRY(obj)				(VISUAL_CHECK_CAST ((obj), DFTCacheEntry))

typedef struct _DFTCacheEntry DFTCacheEntry;

struct _DFTCacheEntry {
	VisObject	 object;

	int		 spectrum_size;

	float		*bitrevtable;

	float		*sintable;
	float		*costable;
};

static VisCache __lv_dft_cache;
static int __lv_fourier_initialized = FALSE;


static int dft_dtor (VisObject *object);

static void fft_table_bitrev_init (DFTCacheEntry *fcache, VisDFT *fourier);
static void fft_table_cossin_init (DFTCacheEntry *fcache, VisDFT *fourier);
static void dft_table_cossin_init (DFTCacheEntry *fcache, VisDFT *fourier);

static int dft_cache_destroyer (VisObject *object);
static DFTCacheEntry *dft_cache_get (VisDFT *dft);

static void perform_dft_brute_force (VisDFT *fourier, float *input, float *output);
static void perform_fft_radix2_dit (VisDFT *fourier, float *input, float *output);

static int dft_dtor (VisObject *object)
{
	VisDFT *dft = VISUAL_DFT (object);

	/* FIXME not all object elements are freed, destroyed, whatever */

	if (dft->real != NULL)
		visual_mem_free (dft->real);

	if (dft->imag != NULL)
		visual_mem_free (dft->imag);

	dft->real = NULL;
	dft->imag = NULL;

	return VISUAL_OK;
}

static void fft_table_bitrev_init (DFTCacheEntry *fcache, VisDFT *fourier)
{
	unsigned int i, m, temp;
	unsigned int j = 0;

	fcache->bitrevtable = visual_mem_malloc0 (sizeof (unsigned int) * fourier->spectrum_size);

	for (i = 0; i < fourier->spectrum_size; i++)
		fcache->bitrevtable[i] = i;

	for (i = 0; i < fourier->spectrum_size; i++) {
		if (j > i) {
			temp = fcache->bitrevtable[i];
			fcache->bitrevtable[i] = fcache->bitrevtable[j];
			fcache->bitrevtable[j] = temp;
		}

		m = fourier->spectrum_size >> 1;

		while (m >= 1 && j >= m) {
			j -= m;
			m >>= 1;
		}

		j += m;
	}
}

static void fft_table_cossin_init (DFTCacheEntry *fcache, VisDFT *fourier)
{
	unsigned int i, dftsize, tabsize;
	float theta;

	dftsize = 2;
	tabsize = 0;
	while (dftsize <= fourier->spectrum_size) {
		tabsize++;

		dftsize <<= 1;
	}

	fcache->sintable = visual_mem_malloc0 (sizeof (float) * tabsize);
	fcache->costable = visual_mem_malloc0 (sizeof (float) * tabsize);

	dftsize = 2;
	i = 0;
	while (dftsize <= fourier->spectrum_size) {
		theta = (float) (-2.0f * FOURIER_PI / (float) dftsize);

		fcache->costable[i] = (float) cosf (theta);
		fcache->sintable[i] = (float) sinf (theta);

		i++;

		dftsize <<= 1;
	}
}

static void dft_table_cossin_init (DFTCacheEntry *fcache, VisDFT *fourier)
{
	unsigned int i, tabsize;
	float theta;

	tabsize = fourier->spectrum_size / 2;
	fcache->sintable = visual_mem_malloc0 (sizeof (float) * tabsize);
	fcache->costable = visual_mem_malloc0 (sizeof (float) * tabsize);

	for (i = 0; i < tabsize; i++) {
		theta = (-2.0f * FOURIER_PI * i) / fourier->spectrum_size;

		fcache->costable[i] = cosf (theta);
		fcache->sintable[i] = sinf (theta);
	}
}

static int dft_cache_destroyer (VisObject *object)
{
	DFTCacheEntry *fcache = DFT_CACHE_ENTRY (object);

	if (fcache->bitrevtable != NULL)
		visual_mem_free (fcache->bitrevtable);

	if (fcache->sintable != NULL)
		visual_mem_free (fcache->sintable);

	if (fcache->costable != NULL)
		visual_mem_free (fcache->costable);

	fcache->bitrevtable = NULL;
	fcache->sintable = NULL;
	fcache->costable = NULL;

	return VISUAL_OK;
}

static DFTCacheEntry *dft_cache_get (VisDFT *fourier)
{
	DFTCacheEntry *fcache;
	char key[16];

	visual_log_return_val_if_fail (__lv_fourier_initialized == TRUE, NULL);

	snprintf (key, 16, "%d", fourier->spectrum_size);
	fcache = visual_cache_get (&__lv_dft_cache, key);

	if (fcache == NULL) {
		fcache = visual_mem_new0 (DFTCacheEntry, 1);

		visual_object_initialize (VISUAL_OBJECT (fcache), TRUE, dft_cache_destroyer);

		if (fourier->brute_force) {
			dft_table_cossin_init (fcache, fourier);
		} else {
			fft_table_bitrev_init (fcache, fourier);
			fft_table_cossin_init (fcache, fourier);
		}

		visual_cache_put (&__lv_dft_cache, key, fcache);
	}

	return fcache;
}


/**
 * @defgroup VisDFT VisDFT
 * @{
 */

int visual_fourier_initialize ()
{
	visual_cache_init (&__lv_dft_cache, visual_object_collection_destroyer, 50, NULL, TRUE);

	__lv_fourier_initialized = TRUE;

	return VISUAL_OK;
}

int visual_fourier_is_initialized ()
{
	return __lv_fourier_initialized;
}

int visual_fourier_deinitialize ()
{
	if (__lv_fourier_initialized == FALSE)
		return -VISUAL_ERROR_FOURIER_NOT_INITIALIZED;

	visual_object_unref (VISUAL_OBJECT (&__lv_dft_cache));

	__lv_fourier_initialized = FALSE;

	return VISUAL_OK;
}

/**
 * Function to create a new VisDFT Discrete Fourier Transform context used
 * to calculate amplitude spectrums over audio data.
 *
 * \note For optimal performance, use a power-of-2 spectrum size. The
 * current implementation does not use the Fast Fourier Transform for
 * non powers of 2.
 *
 * \note If samples_in is smaller than 2 * samples_out, the input will be padded
 * with zeroes.
 *
 * @param samples_in The number of samples provided to every call to
 * visual_fourier_perform() as input.
 * @param samples_out Size of output spectrum (number of output samples).
 *
 * @return A newly created VisDFT.
 */
VisDFT *visual_dft_new (unsigned int samples_in, unsigned int samples_out)
{
	VisDFT *dft;

	dft = visual_mem_new0 (VisDFT, 1);

	visual_dft_init (dft, samples_in, samples_out);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (dft), TRUE);
	visual_object_ref (VISUAL_OBJECT (dft));

	return dft;
}

int visual_dft_init (VisDFT *dft, unsigned int samples_in, unsigned int samples_out)
{
	visual_log_return_val_if_fail (dft != NULL, -VISUAL_ERROR_FOURIER_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (dft));
	visual_object_set_dtor (VISUAL_OBJECT (dft), dft_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (dft), FALSE);

	/* Set the VisDFT data */
	dft->samples_in = samples_in;
	dft->spectrum_size = samples_out * 2;
	dft->brute_force = !visual_utils_is_power_of_2 (dft->spectrum_size);

	/* Initialize the VisDFT */
	dft_cache_get (dft);

	dft->real = visual_mem_malloc0 (sizeof (float) * dft->spectrum_size);
	dft->imag = visual_mem_malloc0 (sizeof (float) * dft->spectrum_size);

	return VISUAL_OK;
}

static void perform_dft_brute_force (VisDFT *dft, float *input, float *output)
{
	DFTCacheEntry *fcache;
	unsigned int i, j;
	float xr, xi, wr, wi, wtemp;

	fcache = dft_cache_get (dft);
	visual_object_ref (VISUAL_OBJECT (fcache));

	for (i = 0; i < dft->spectrum_size / 2; i++) {
		xr = 0.0f;
		xi = 0.0f;

		wr = 1.0f;
		wi = 0.0f;

		for (j = 0; j < dft->spectrum_size; j++) {
			xr += input[j] * wr;
			xi += input[j] * wi;

			wtemp = wr;
			wr = wr * fcache->costable[i] - wi * fcache->sintable[i];
			wi = wtemp * fcache->sintable[i] + wi * fcache->costable[i];
		}

		dft->real[i] = xr;
		dft->imag[i] = xi;
	}

	visual_object_unref (VISUAL_OBJECT (fcache));
}

static void perform_fft_radix2_dit (VisDFT *dft, float *input, float *output)
{
	DFTCacheEntry *fcache;
	unsigned int j, m, i, dftsize, hdftsize, t;
	float wr, wi, wpi, wpr, wtemp, tempr, tempi;

	fcache = dft_cache_get (dft);
	visual_object_ref (VISUAL_OBJECT (fcache));

	for (i = 0; i < dft->spectrum_size; i++) {
		unsigned int idx = fcache->bitrevtable[i];

		if (idx < dft->samples_in)
			dft->real[i] = input[idx];
		else
			dft->real[i] = 0;
	}

	visual_mem_set (dft->imag, 0, sizeof (float) * dft->spectrum_size);

	dftsize = 2;
	t = 0;
	while (dftsize <= dft->spectrum_size) {
		wpr = fcache->costable[t];
		wpi = fcache->sintable[t];

		wr = 1.0f;
		wi = 0.0f;

		hdftsize = dftsize >> 1;

		for (m = 0; m < hdftsize; m += 1) {
			for (i = m; i < dft->spectrum_size; i+=dftsize) {
				j = i + hdftsize;

				tempr = wr * dft->real[j] - wi * dft->imag[j];
				tempi = wr * dft->imag[j] + wi * dft->real[j];

				dft->real[j] = dft->real[i] - tempr;
				dft->imag[j] = dft->imag[i] - tempi;

				dft->real[i] += tempr;
				dft->imag[i] += tempi;
			}

			wr = (wtemp = wr) * wpr - wi * wpi;
			wi = wi * wpr + wtemp * wpi;
		}

		dftsize <<= 1;
		t++;
	}

	visual_object_unref (VISUAL_OBJECT (fcache));
}


/**
 * Function to perform a Discrete Fourier Transform over a set of data.
 *
 * \note Output samples are normalised to [0.0, 1.0] by dividing with the
 * spectrum size.
 *
 * @param fourier Pointer to the VisDFT context for this transform.
 * @param input Array of input samples with values in [-1.0, 1.0]
 * @param output Array of output samples
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_FOURIER_NULL or -VISUAL_ERROR_NULL on failure.
 */
int visual_dft_perform (VisDFT *dft, float *input, float *output)
{
	unsigned int i;

	visual_log_return_val_if_fail (dft != NULL, -VISUAL_ERROR_FOURIER_NULL);
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (output != NULL, -VISUAL_ERROR_NULL);

	if (dft->brute_force)
		perform_dft_brute_force (dft, input, output);
	else
		perform_fft_radix2_dit (dft, input, output);

	/* FIXME SSEfy */
	for (i = 0; i < dft->spectrum_size / 2; i++)
		output[i] = sqrtf (dft->real[i] * dft->real[i] +
				dft->imag[i] * dft->imag[i]) / dft->spectrum_size;

	return VISUAL_OK;
}

int visual_fourier_normalise (float *spectrum, int size)
{

}

/**
 * @}
 */

