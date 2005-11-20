/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * The FFT implementation found in this file is based upon the NULLSOFT
 * Milkdrop FFT implementation.
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#define FFT_PI 3.141592653589793238462643383279502884197169399f

#include <config.h>
#include <stdlib.h>
#include <math.h>

#include <string.h>

#include "lv_cache.h"
#include "lv_fft.h"

#define FFT_CACHEENTRY(obj)				(VISUAL_CHECK_CAST ((obj), FFTCacheEntry))


typedef struct _FFTCacheEntry FFTCacheEntry;

struct _FFTCacheEntry {
	VisObject	 object;

	int		 spectrum_size;

	float		*bitrevtable;

	float		*sintable;
	float		*costable;
};


static VisCache __lv_fft_cache;
static int __lv_fft_initialized = FALSE;


static int fft_dtor (VisObject *object);

static void fft_table_bitrev_init (FFTCacheEntry *fcache, VisFFT *fft);
static void fft_table_cossin_init (FFTCacheEntry *fcache, VisFFT *fft);

static int fft_cache_destroyer (VisObject *object);
static FFTCacheEntry *fft_cache_get (VisFFT *fft);

static int fft_dtor (VisObject *object)
{
	VisFFT *fft = VISUAL_FFT (object);

	/* FIXME not all object elements are freed, destroyed, whatever */

	if (fft->real != NULL)
		visual_mem_free (fft->real);

	if (fft->imag != NULL)
		visual_mem_free (fft->imag);

	fft->real = NULL;
	fft->imag = NULL;


	return VISUAL_OK;
}

static void fft_table_bitrev_init (FFTCacheEntry *fcache, VisFFT *fft)
{
	int i, m, temp;
	int j = 0;

	fcache->bitrevtable = visual_mem_malloc0 (sizeof (int) * fft->spectrum_size);

	for (i = 0; i < fft->spectrum_size; i++)
		fcache->bitrevtable[i] = i;

	for (i = 0; i < fft->spectrum_size; i++) {
		if (j > i) {
			temp = fcache->bitrevtable[i];
			fcache->bitrevtable[i] = fcache->bitrevtable[j];
			fcache->bitrevtable[j] = temp;
		}

		m = fft->spectrum_size >> 1;

		while (m >= 1 && j >= m) {
			j -= m;
			m >>= 1;
		}

		j += m;
	}
}

static void fft_table_cossin_init (FFTCacheEntry *fcache, VisFFT *fft)
{
	int i, dftsize, tabsize;
	float theta;

	dftsize = 2;
	tabsize = 0;
	while (dftsize <= fft->spectrum_size) {
		tabsize++;

		dftsize <<= 1;
	}

	fcache->sintable = visual_mem_malloc0 (sizeof (float) * tabsize);
	fcache->costable = visual_mem_malloc0 (sizeof (float) * tabsize);

	dftsize = 2;
	i = 0;
	while (dftsize <= fft->spectrum_size) {
		theta = (float) (-2.0f * FFT_PI / (float) dftsize);

		fcache->sintable[i] = (float) cosf (theta);
		fcache->costable[i] = (float) sinf (theta);

		i++;

		dftsize <<= 1;
	}
}

static int fft_cache_destroyer (VisObject *object)
{
	FFTCacheEntry *fcache = FFT_CACHEENTRY (object);

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

static FFTCacheEntry *fft_cache_get (VisFFT *fft)
{
	FFTCacheEntry *fcache;
	char key[16];

	visual_log_return_val_if_fail (__lv_fft_initialized == TRUE, NULL);

	snprintf (key, 16, "%d", fft->spectrum_size);
	fcache = visual_cache_get (&__lv_fft_cache, key);

	if (fcache == NULL) {
		fcache = visual_mem_new0 (FFTCacheEntry, 1);

		visual_object_initialize (VISUAL_OBJECT (fcache), TRUE, fft_cache_destroyer);

		fft_table_bitrev_init (fcache, fft);
		fft_table_cossin_init (fcache, fft);

		visual_cache_put (&__lv_fft_cache, key, fcache);
	}

	return fcache;
}


/**
 * @defgroup VisFFT VisFFT
 * @{
 */

int visual_fft_initialize ()
{
	visual_cache_init (&__lv_fft_cache, visual_object_collection_destroyer, 50, NULL, TRUE);

	__lv_fft_initialized = TRUE;

	return VISUAL_OK;
}

int visual_fft_is_initialized ()
{
	return __lv_fft_initialized;
}

int visual_fft_deinitialize ()
{
	if (__lv_fft_initialized == FALSE)
		return -VISUAL_ERROR_FFT_NOT_INITIALIZED;

	visual_object_unref (VISUAL_OBJECT (&__lv_fft_cache));

	__lv_fft_initialized = FALSE;

	return VISUAL_OK;
}

/**
 * Function to create a new VisFFT Fast Fourier Transform context used 
 * to calculate spectrums over audio data.
 *
 * @param samples_in The number of samples provided every visual_fft_perform() as input.
 * @param samples_out The size of the output spectrum.
 *
 * @return A newly created VisFFT.
 */
VisFFT *visual_fft_new (int samples_in, int samples_out)
{
	VisFFT *fft;

	fft = visual_mem_new0 (VisFFT, 1);

	visual_fft_init (fft, samples_in, samples_out);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (fft), TRUE);
	visual_object_ref (VISUAL_OBJECT (fft));

	return fft;
}

int visual_fft_init (VisFFT *fft, int samples_in, int samples_out)
{
	visual_log_return_val_if_fail (fft != NULL, -VISUAL_ERROR_FFT_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (fft));
	visual_object_set_dtor (VISUAL_OBJECT (fft), fft_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (fft), FALSE);

	/* Set the VisFFT data */
	fft->samples_in = samples_in;
	fft->spectrum_size = samples_out * 2;

	/* Initialize the VisFFT */
	fft_cache_get (fft);

	fft->real = visual_mem_malloc0 (sizeof (float) * fft->spectrum_size);
	fft->imag = visual_mem_malloc0 (sizeof (float) * fft->spectrum_size);

	return VISUAL_OK;
}

/**
 * Function to perform a Fast Fourier Transform over a set of most likely audio data.
 * 
 * @param fft Pointer to the VisFFT context for this transform.
 * @param input Pointer to the input samples.
 * @param output Pointer to the output spectrum buffer.
 * @param normalised TRUE to normalise the values, FALSE to not normalise the values.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_FFT_NULL or -VISUAL_ERROR_NULL on failure.
 */
int visual_fft_perform (VisFFT *fft, float *input, float *output, int normalised)
{
	FFTCacheEntry *fcache;
	int j, m, i, dftsize, hdftsize, t;
	float wr, wi, wpi, wpr, wtemp, tempr, tempi;

	visual_log_return_val_if_fail (fft != NULL, -VISUAL_ERROR_FFT_NULL);
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (output != NULL, -VISUAL_ERROR_NULL);

	fcache = fft_cache_get (fft);
	visual_object_ref (VISUAL_OBJECT (fcache));

	for (i = 0; i < fft->spectrum_size; i++) {
		int idx = fcache->bitrevtable[i];

		if (idx < fft->samples_in)
			fft->real[i] = input[idx] / 32768.00f;
		else
			fft->real[i] = 0;
	}

	visual_mem_set (fft->imag, 0, sizeof (float) * fft->spectrum_size);

	dftsize = 2;
	t = 0;
	while (dftsize <= fft->spectrum_size) {
		wpr = fcache->sintable[t];
		wpi = fcache->costable[t];
		wr = 1.0f;
		wi = 0.0f;
		hdftsize = dftsize >> 1;

		for (m = 0; m < hdftsize; m += 1) {
			for (i = m; i < fft->spectrum_size; i+=dftsize) {
				j = i + hdftsize;
				tempr = wr * fft->real[j] - wi * fft->imag[j];
				tempi = wr * fft->imag[j] + wi * fft->real[j];
				fft->real[j] = fft->real[i] - tempr;
				fft->imag[j] = fft->imag[i] - tempi;
				fft->real[i] += tempr;
				fft->imag[i] += tempi;
			}

			wr = (wtemp = wr) * wpr - wi * wpi;
			wi = wi * wpr + wtemp * wpi;
		}

		dftsize <<= 1; /* FIXME not right, make so that a start and end freq can be given and
				  that a log scale will be calculated for this */
		t++;
	}

	visual_object_unref (VISUAL_OBJECT (fcache));

	for (i = 0; i < fft->spectrum_size / 2; i++)
		output[i] = sqrtf (fft->real[i] * fft->real[i] + fft->imag[i] * fft->imag[i]);

	return VISUAL_OK;
}

int visual_fft_normalise (float *spectrum, int size)
{

}

/**
 * @}
 */

