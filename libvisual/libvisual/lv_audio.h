#ifndef _LV_AUDIO_H
#define _LV_AUDIO_H

#include <libvisual/lv_fft.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _VisAudio VisAudio;

/**
 * The VisAudio structure contains the sample and extra information
 * about the sample like a 256 bands analyzer, sound energy and
 * in the future BPM detection.
 *
 * @see visual_audio_new
 */
struct _VisAudio {
	short plugpcm[2][512];		/**< PCM data that comes from the input plugin
					 * or a callback function. */
	short pcm[3][512];		/**< PCM data that should be used within plugins
					 * pcm[0][x] is the left channel, pcm[1][x] is the right
					 * channel and pcm[2][x] is an average of both channels. */
	short freq[3][256];		/**< Frequency data as a 256 bands analyzer, with the channels
					 * like with the pcm element. */
	short freqnorm[3][256];		/**< Frequency data like the freq member, however this time the bands
					 * are normalized. */
	VisFFTState *fft_state;		/**< Private member that contains context information for the FFT engine. */

	short int bpmhistory[1024][6];	/**< Private member for BPM detection, not implemented right now. */
	short int bpmdata[1024][6];	/**< Private member for BPM detection, not implemented right now. */
	short int bpmenergy[6];		/**< Private member for BPM detection, not implemented right now. */
	int energy;			/**< Audio energy level. */
};

VisAudio *visual_audio_new (void);
int visual_audio_free (VisAudio *audio);
int visual_audio_analyze (VisAudio *audio);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_AUDIO_H */
