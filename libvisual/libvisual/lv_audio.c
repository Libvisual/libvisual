#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lv_common.h"
#include "lv_audio.h"

static int audio_band_total (VisAudio *audio, int begin, int end);
static int audio_band_energy (VisAudio *audio, int band, int length);

static int audio_band_total (VisAudio *audio, int begin, int end)
{
	int bpmtotal = 0;
	int i;

	for (i = begin; i < end; i++)
		bpmtotal += audio->freq[2][i];

	if (bpmtotal > 0)
		return bpmtotal / (end - begin);
	else
		return 0;
}

static int audio_band_energy (VisAudio *audio, int band, int length)
{
	int energytotal = 0;
	int i;

	for (i = 0; i < length; i++)
		energytotal += audio->bpmhistory[i][band];

	if (energytotal > 0)
		return energytotal / length;
	else
		return 0;
}

/**
 * @defgroup VisAudio VisAudio
 * @{
 */

/**
 * Creates a new VisAudio structure.
 *
 * @return A newly allocated VisAudio.
 */
VisAudio *visual_audio_new ()
{
	VisAudio *audio;

	audio = visual_mem_new0 (VisAudio, 1);

	return audio;
}

/**
 * Frees the VisAudio. This frees the VisAudio data structure.
 *
 * @param audio Pointer to a VisAudio that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_audio_free (VisAudio *audio)
{
	visual_log_return_val_if_fail (audio != NULL, -1);

	_lv_fft_close (audio->fft_state);
	
	visual_mem_free (audio);

	return 0;
}

/**
 * This function analyzes the VisAudio, the FFT frequency magic gets done here, also
 * the audio energy is calculated and some other magic to provide the developer more
 * information about the current sample and the stream.
 *
 * For every sample that is being retrieved this needs to be called. However keep in mind
 * that the VisBin runs it automaticly.
 *
 * @param audio Pointer to a VisAudio that needs to be analyzed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_audio_analyze (VisAudio *audio)
{
        float tmp_out[256];
	int i;

	/* Load the pcm data */
	for (i = 0; i < 512; i++) {
		audio->pcm[0][i] = audio->plugpcm[0][i];
		audio->pcm[1][i] = audio->plugpcm[1][i];
		audio->pcm[2][i] = (audio->plugpcm[0][i] + audio->plugpcm[1][i]) >> 1;
	}

	/* Initialize fft if not yet initialized */
	if (audio->fft_state == NULL)
		audio->fft_state = _lv_fft_init ();

	/* FFT analyze the pcm data */
	_lv_fft_perform (audio->plugpcm[0], tmp_out, audio->fft_state);
		
	for (i = 0; i < 256; i++)
		audio->freq[0][i] = ((int) sqrt (tmp_out[i + 1])) >> 8;

	_lv_fft_perform (audio->plugpcm[1], tmp_out, audio->fft_state);

	for (i = 0; i < 256; i++)
		audio->freq[1][i] = ((int) sqrt (tmp_out[i + 1])) >> 8;

	for (i = 0; i < 256; i++)
		audio->freq[2][i] = (audio->freq[0][i] + audio->freq[1][i]) >> 1;

	/* BPM stuff, used for the audio energy only right now */
	for (i = 1023; i > 0; i--) {
		memcpy (&audio->bpmhistory[i], &audio->bpmhistory[i - 1], 6 * sizeof (short int));
		memcpy (&audio->bpmdata[i], &audio->bpmdata[i - 1], 6 * sizeof (short int));
	}

	/* Calculate the audio energy */
	audio->energy = 0;
	
	for (i = 0; i < 6; i++)	{
		audio->bpmhistory[0][i] = audio_band_total (audio, i * 2, (i * 2) + 3);
		audio->bpmenergy[i] = audio_band_energy (audio, i, 10);

		audio->bpmdata[0][i] = audio->bpmhistory[0][i] - audio->bpmenergy[i];

		audio->energy += audio_band_energy(audio, i, 50);
	}

	audio->energy >>= 7;

	if (audio->energy > 100)
		audio->energy = 100;

	return 0;
}

/**
 * @}
 */
