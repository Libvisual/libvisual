#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <alsa/asoundlib.h>

#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE 16384

typedef struct {
	short fakebuf[PCM_BUF_SIZE];
	snd_pcm_t *chandle;
	int loaded;
	int fakebufloaded;
	int clearcount;
} alsaPrivate;

static int inp_alsa_init (VisPluginData *plugin);
static int inp_alsa_cleanup (VisPluginData *plugin);
static int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio);

static const int  inp_alsa_var_btmul      = sizeof (short);
static const char *inp_alsa_var_cdevice   = "hw:0,0";
static const int  inp_alsa_var_samplerate = 44100;
static const int  inp_alsa_var_channels   = 2;

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisInputPlugin input[] = {{
		.upload = inp_alsa_upload
	}};
	
	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "alsa",
		.name = "alsa",
		.author = "Vitaly V. Bursov <vitalyb@mail333.com>",
		.version = "0.1",
		.about = "The ALSA capture plugin.",
		.help = "Use this plugin to capture PCM data from the ALSA record device",

		.init = inp_alsa_init,
		.cleanup = inp_alsa_cleanup,
		
		.plugin = (void *) &input[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int inp_alsa_init (VisPluginData *plugin)
{
	snd_pcm_hw_params_t *hwparams = NULL;
	alsaPrivate *priv;
	int rate = inp_alsa_var_samplerate;
	int exact_rate;
	int dir;
	int err;
	int tmp;

	visual_log_return_val_if_fail(plugin != NULL, -1);
	
	priv = visual_mem_new0 (alsaPrivate, 1);
	visual_log_return_val_if_fail(priv != NULL, -1);
	
	plugin->priv = priv;

	if ((err = snd_pcm_open(&priv->chandle, strdup(inp_alsa_var_cdevice),
			SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0) {
  	        visual_log(VISUAL_LOG_CRITICAL, 
			    "Record open error: %s\n", snd_strerror(err));
		return -1;
	}

	snd_pcm_hw_params_malloc(&hwparams);
	visual_log_return_val_if_fail(hwparams != NULL, -1);

	if (snd_pcm_hw_params_any(priv->chandle, hwparams) < 0) {
 	        visual_log(VISUAL_LOG_CRITICAL, 
			   "Can not configure this PCM device.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

	if (snd_pcm_hw_params_set_access(priv->chandle, hwparams, 
					 SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
 	        visual_log(VISUAL_LOG_CRITICAL, "Error setting access.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

#ifdef VISUAL_LITTLE_ENDIAN
	if (snd_pcm_hw_params_set_format(priv->chandle, hwparams,
					 SND_PCM_FORMAT_S16_LE) < 0) {
#else
	if (snd_pcm_hw_params_set_format(priv->chandle, hwparams,
					 SND_PCM_FORMAT_S16_BE) < 0) {
#endif
 	        visual_log(VISUAL_LOG_CRITICAL, "Error setting format.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

	exact_rate = rate;

	if (snd_pcm_hw_params_set_rate_near(priv->chandle, hwparams,
					    &exact_rate, &dir) < 0) {
 	        visual_log(VISUAL_LOG_CRITICAL, "Error setting rate.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}
	if (exact_rate != rate) {
 	        visual_log(VISUAL_LOG_WARNING, 
			   "The rate %d Hz is not supported by your " \
			   "hardware.\n" \
			   "==> Using %d Hz instead.\n", rate, exact_rate);
	}
	rate = exact_rate;

	if (snd_pcm_hw_params_set_channels(priv->chandle, hwparams,
					   inp_alsa_var_channels) < 0) {
	        visual_log(VISUAL_LOG_CRITICAL, "Error setting channels.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

       /* Setup a large buffer */

	tmp = 1000000;
	if (snd_pcm_hw_params_set_period_time_near(priv->chandle, hwparams, &tmp, &dir) < 0){
		visual_log(VISUAL_LOG_CRITICAL, "Error setting period time.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

	tmp = 1000000*4;
	if (snd_pcm_hw_params_set_buffer_time_near(priv->chandle, hwparams, &tmp, &dir) < 0){
		visual_log(VISUAL_LOG_CRITICAL, "Error setting buffer time.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}


	if (snd_pcm_hw_params(priv->chandle, hwparams) < 0) {
 	        visual_log(VISUAL_LOG_CRITICAL, "Error setting HW params.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

	if (snd_pcm_prepare(priv->chandle) < 0) {
 	        visual_log(VISUAL_LOG_CRITICAL, "Failed to prepare interface.\n");
		snd_pcm_hw_params_free(hwparams);
		return(-1);
	}

	snd_pcm_hw_params_free(hwparams);

	priv->loaded = 1;

	return 0;
}

int inp_alsa_cleanup (VisPluginData *plugin)
{
	alsaPrivate *priv = NULL;

	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = plugin->priv;
	visual_log_return_val_if_fail(priv != NULL, -1);

	if (priv->loaded == 1)
		snd_pcm_close(priv->chandle);

	visual_mem_free (priv);
	plugin->priv = NULL;
	return 0;
}

int inp_alsa_upload (VisPluginData *plugin, VisAudio *audio)
{
	short data[PCM_BUF_SIZE];
	alsaPrivate *priv = NULL;
	int rcnt;
	int i;

	visual_log_return_val_if_fail(audio != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = plugin->priv;
	visual_log_return_val_if_fail(priv != NULL, -1);


	rcnt = snd_pcm_readi(priv->chandle, data, PCM_BUF_SIZE/2);

	if (rcnt < 0) {
		if (rcnt == -EPIPE){
#if 0
		  visual_log(VISUAL_LOG_ERROR, "=========BUFFER OVERRUN\n");
#endif
			if (snd_pcm_prepare(priv->chandle) < 0) {
			        visual_log(VISUAL_LOG_ERROR, 
					   "Failed to prepare interface.\n");
				return(-1);
			}
		}

		if (priv->fakebufloaded == 1) {
			priv->clearcount++;

			if (priv->clearcount > 100)
				memset (priv->fakebuf, 0, 
					PCM_BUF_SIZE * inp_alsa_var_btmul);

			memcpy (data, priv->fakebuf, 
				PCM_BUF_SIZE * inp_alsa_var_btmul);
		} else {
			memset (data, 0, sizeof (data));
		}
	} else {
		priv->clearcount = 0;
	}
	
	priv->fakebufloaded = 1;

	memcpy (priv->fakebuf, data, PCM_BUF_SIZE * inp_alsa_var_btmul);

	for (i = 0; i < PCM_BUF_SIZE && i < 1024; i += 2) {
		audio->plugpcm[0][i >> 1] = priv->fakebuf[i];
		audio->plugpcm[1][i >> 1] = priv->fakebuf[i + 1];
	}

	return 0;
}

