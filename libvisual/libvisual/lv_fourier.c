/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * The FFT implementation found in this file is based upon the NULLSOFT
 * Milkdrop FFT implementation.
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_fourier.c,v 1.3 2006-01-11 06:06:41 synap Exp $
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

#define FFT_CACHEENTRY(obj)				(VISUAL_CHECK_CAST ((obj), FourierCacheEntry))

typedef struct _FourierCacheEntry FourierCacheEntry;

struct _FourierCacheEntry {
	VisObject	 object;

	int		 spectrum_size;

	float		*bitrevtable;

	float		*sintable;
	float		*costable;
};

static VisCache __lv_fourier_cache;
static int __lv_fourier_initialized = FALSE;


static int fourier_dtor (VisObject *object);

static void fft_table_bitrev_init (FourierCacheEntry *fcache, VisFourier *fourier);
static void fft_table_cossin_init (FourierCacheEntry *fcache, VisFourier *fourier);
static void dft_table_cossin_init (FourierCacheEntry *fcache, VisFourier *fourier);

static int fourier_cache_destroyer (VisObject *object);
static FourierCacheEntry *fourier_cache_get (VisFourier *fourier);

static void perform_dft_brute_force (VisFourier *fourier, float *input, float *output);
static void perform_fft_radix2_dit (VisFourier *fourier, float *input, float *output);

static int fourier_dtor (VisObject *object)
{
	VisFourier *fourier = VISUAL_FOURIER (object);

	/* FIXME not all object elements are freed, destroyed, whatever */

	if (fourier->real != NULL)
		visual_mem_free (fourier->real);

	if (fourier->imag != NULL)
		visual_mem_free (fourier->imag);

	fourier->real = NULL;
	fourier->imag = NULL;

	return VISUAL_OK;
}

static void fft_table_bitrev_init (FourierCacheEntry *fcache, VisFourier *fourier)
{
	int i, m, temp;
	int j = 0;

	fcache->bitrevtable = visual_mem_malloc0 (sizeof (int) * fourier->spectrum_size);

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

static void fft_table_cossin_init (FourierCacheEntry *fcache, VisFourier *fourier)
{
	int i, dftsize, tabsize;
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

static void dft_table_cossin_init (FourierCacheEntry *fcache, VisFourier *fourier)
{
	int i, tabsize;
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

static int fourier_cache_destroyer (VisObject *object)
{
	FourierCacheEntry *fcache = FFT_CACHEENTRY (object);

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

static FourierCacheEntry *fourier_cache_get (VisFourier *fourier)
{
	FourierCacheEntry *fcache;
	char key[16];

	visual_log_return_val_if_fail (__lv_fourier_initialized == TRUE, NULL);

	snprintf (key, 16, "%d", fourier->spectrum_size);
	fcache = visual_cache_get (&__lv_fourier_cache, key);

	if (fcache == NULL) {
		fcache = visual_mem_new0 (FourierCacheEntry, 1);

		visual_object_initialize (VISUAL_OBJECT (fcache), TRUE, fourier_cache_destroyer);

		if (fourier->brute_force) {
			dft_table_cossin_init (fcache, fourier);
		} else {
			fft_table_bitrev_init (fcache, fourier);
			fft_table_cossin_init (fcache, fourier);
		}

		visual_cache_put (&__lv_fourier_cache, key, fcache);
	}

	return fcache;
}


/**
 * @defgroup VisFourier VisFourier
 * @{
 */

int visual_fourier_initialize ()
{
	visual_cache_init (&__lv_fourier_cache, visual_object_collection_destroyer, 50, NULL, TRUE);

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

	visual_object_unref (VISUAL_OBJECT (&__lv_fourier_cache));

	__lv_fourier_initialized = FALSE;

	return VISUAL_OK;
}

/**
 * Function to create a new VisFourier Fast Fourier Transform context used
 * to calculate spectrums over audio data.
 *
 * @param samples_in The number of samples provided every visual_fourier_perform() as input.
 * @param samples_out The size of the output spectrum.
 *
 * @return A newly created VisFourier.
 */
VisFourier *visual_fourier_new (int samples_in, int samples_out)
{
	VisFourier *fourier;

	fourier = visual_mem_new0 (VisFourier, 1);

	visual_fourier_init (fourier, samples_in, samples_out);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (fourier), TRUE);
	visual_object_ref (VISUAL_OBJECT (fourier));

	return fourier;
}

int visual_fourier_init (VisFourier *fourier, int samples_in, int samples_out)
{
	visual_log_return_val_if_fail (fourier != NULL, -VISUAL_ERROR_FOURIER_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (fourier));
	visual_object_set_dtor (VISUAL_OBJECT (fourier), fourier_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (fourier), FALSE);

	/* Set the VisFourier data */
	fourier->samples_in = samples_in;
	fourier->spectrum_size = samples_out * 2;
	fourier->brute_force = !visual_utils_is_power_of_2 (fourier->spectrum_size);

	/* Initialize the VisFourier */
	fourier_cache_get (fourier);

	fourier->real = visual_mem_malloc0 (sizeof (float) * fourier->spectrum_size);
	fourier->imag = visual_mem_malloc0 (sizeof (float) * fourier->spectrum_size);

	return VISUAL_OK;
}

static void perform_dft_brute_force (VisFourier *fourier, float *input, float *output)
{
	FourierCacheEntry *fcache;
	int i, j;
	float xr, xi, wr, wi, wtemp;

	fcache = fourier_cache_get (fourier);
	visual_object_ref (VISUAL_OBJECT (fcache));

	for (i = 0; i < fourier->spectrum_size / 2; i++) {
		xr = 0.0f;
		xi = 0.0f;

		wr = 1.0f;
		wi = 0.0f;

		for (j = 0; j < fourier->spectrum_size; j++) {
			xr += input[j] * wr;
			xi += input[j] * wi;

			wtemp = wr;
			wr = wr * fcache->costable[i] - wi * fcache->sintable[i];
			wi = wtemp * fcache->sintable[i] + wi * fcache->costable[i];
		}

		fourier->real[i] = xr;
		fourier->imag[i] = xi;
	}

	visual_object_unref (VISUAL_OBJECT (fcache));
}

static void perform_fft_radix2_dit (VisFourier *fourier, float *input, float *output)
{
	FourierCacheEntry *fcache;
	int j, m, i, dftsize, hdftsize, t;
	float wr, wi, wpi, wpr, wtemp, tempr, tempi;

	fcache = fourier_cache_get (fourier);
	visual_object_ref (VISUAL_OBJECT (fcache));

	for (i = 0; i < fourier->spectrum_size; i++) {
		int idx = fcache->bitrevtable[i];

		if (idx < fourier->samples_in)
			fourier->real[i] = input[idx];
		else
			fourier->real[i] = 0;
	}

	visual_mem_set (fourier->imag, 0, sizeof (float) * fourier->spectrum_size);

	dftsize = 2;
	t = 0;
	while (dftsize <= fourier->spectrum_size) {
		wpr = fcache->costable[t];
		wpi = fcache->sintable[t];

		wr = 1.0f;
		wi = 0.0f;

		hdftsize = dftsize >> 1;

		for (m = 0; m < hdftsize; m += 1) {
			for (i = m; i < fourier->spectrum_size; i+=dftsize) {
				j = i + hdftsize;

				tempr = wr * fourier->real[j] - wi * fourier->imag[j];
				tempi = wr * fourier->imag[j] + wi * fourier->real[j];

				fourier->real[j] = fourier->real[i] - tempr;
				fourier->imag[j] = fourier->imag[i] - tempi;

				fourier->real[i] += tempr;
				fourier->imag[i] += tempi;
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
 * Function to perform a Fourier Transform over a set of data.
 *
 * @param fourier Pointer to the VisFourier context for this transform.
 * @param input Pointer to the input samples.
 * @param output Pointer to the output fourier transformed buffer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_FOURIER_NULL or -VISUAL_ERROR_NULL on failure.
 */
int visual_fourier_perform (VisFourier *fourier, float *input, float *output)
{
	int i;

	visual_log_return_val_if_fail (fourier != NULL, -VISUAL_ERROR_FOURIER_NULL);
	visual_log_return_val_if_fail (input != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (output != NULL, -VISUAL_ERROR_NULL);

	if (fourier->brute_force)
		perform_dft_brute_force (fourier, input, output);
	else
		perform_fft_radix2_dit (fourier, input, output);

	/* FIXME SSEfy */
	for (i = 0; i < fourier->spectrum_size / 2; i++)
		output[i] = sqrtf (fourier->real[i] * fourier->real[i] +
				fourier->imag[i] * fourier->imag[i]) / fourier->spectrum_size;

	return VISUAL_OK;
}

int visual_fourier_normalise (float *spectrum, int size)
{

}

/**
 * @}
 */

