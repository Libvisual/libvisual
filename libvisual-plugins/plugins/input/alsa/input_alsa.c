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

static int inp_alsa_init (VisInputPlugin *plugin);
static int inp_alsa_cleanup (VisInputPlugin *plugin);
static int inp_alsa_upload (VisInputPlugin *plugin, VisAudio *audio);

static const int btmul = sizeof (short);
static const char *cdevice = "hw:0,0";
static const int samplerate = 44100;
static const int channels = 2;

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisInputPlugin *alsa_input;
	alsaPrivate *priv;
	
	plugin = visual_plugin_new ();
	alsa_input = visual_plugin_input_new ();
	
	alsa_input->name = "alsa";
	alsa_input->info = visual_plugin_info_new (
		"alsa",
		"Vitaly V. Bursov <vitalyb@mail333.com>",
		"0.1",
		"The ALSA capture plugin.\n"
		"Based on ESD capture plugin by Dennis Smit.",
		"Use this plugin to capture PCM data from the ALSA record device");

	alsa_input->init =	inp_alsa_init;
	alsa_input->cleanup =	inp_alsa_cleanup;
	alsa_input->upload =	inp_alsa_upload;

	priv = malloc (sizeof (alsaPrivate));
	memset (priv, 0, sizeof (alsaPrivate));

	alsa_input->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_INPUT;
	plugin->plugin.inputplugin = alsa_input;

	return plugin;
}

int inp_alsa_init (VisInputPlugin *plugin)
{
	snd_pcm_hw_params_t *hwparams;
	alsaPrivate *priv = plugin->private;
	int rate = samplerate;
	int exact_rate;
	int dir;
	int err;


	if ((err = snd_pcm_open(&priv->chandle, strdup(cdevice),
			SND_PCM_STREAM_CAPTURE, SND_PCM_NONBLOCK)) < 0) {
		fprintf(stderr, "Record open error: %s\n", snd_strerror(err));
		return -1;
	}

	snd_pcm_hw_params_alloca(&hwparams);

	if (snd_pcm_hw_params_any(priv->chandle, hwparams) < 0) {
		fprintf(stderr, "Can not configure this PCM device.\n");
		return(-1);
	}

	if (snd_pcm_hw_params_set_access(priv->chandle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
		fprintf(stderr, "Error setting access.\n");
		return(-1);
	}

	if (snd_pcm_hw_params_set_format(priv->chandle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
		fprintf(stderr, "Error setting format.\n");
		return(-1);
	}

	exact_rate = rate;

	if (snd_pcm_hw_params_set_rate_near(priv->chandle, hwparams, &exact_rate, &dir) < 0) {
		fprintf(stderr, "Error setting rate.\n");
		return(-1);
	}
	if (dir != 0) {
		fprintf(stderr, "The rate %d Hz is not supported by your hardware.\n"
	                    "==> Using %d Hz instead.\n", rate, exact_rate);
	}

	if (snd_pcm_hw_params_set_channels(priv->chandle, hwparams, channels) < 0) {
		fprintf(stderr, "Error setting channels.\n");
		return(-1);
	}

	if (snd_pcm_hw_params(priv->chandle, hwparams) < 0) {
		fprintf(stderr, "Error setting HW params.\n");
		return(-1);
	}

	if (snd_pcm_prepare(priv->chandle) < 0) {
		fprintf(stderr, "Failed to prepare interface.\n");
		return(-1);
	}

	priv->loaded = 1;

	return 0;
}

int inp_alsa_cleanup (VisInputPlugin *plugin)
{
	alsaPrivate *priv = plugin->private;

	if (priv->loaded == 1)
		snd_pcm_close(priv->chandle);

	free (priv);
	plugin->private = NULL;
	return 0;
}

int inp_alsa_upload (VisInputPlugin *plugin, VisAudio *audio)
{
	short data[PCM_BUF_SIZE];
	alsaPrivate *priv = plugin->private;
	int rcnt;
	int i;


	rcnt = snd_pcm_readi(priv->chandle, data, PCM_BUF_SIZE/2);

	if (rcnt < 0) {
		if (rcnt == -EPIPE){
#if 0
			fprintf(stderr,"=========BUFFER OVERRUN\n");
#endif
			if (snd_pcm_prepare(priv->chandle) < 0) {
				fprintf(stderr, "Failed to prepare interface.\n");
				return(-1);
			}
		}

		if (priv->fakebufloaded == 1) {
			priv->clearcount++;

			if (priv->clearcount > 100)
				memset (priv->fakebuf, 0, PCM_BUF_SIZE * btmul);

			memcpy (data, priv->fakebuf, PCM_BUF_SIZE * btmul);
		} else {
			memset (data, 0, sizeof (data));
		}
	} else {
		priv->clearcount = 0;
	}
	
	priv->fakebufloaded = 1;

	memcpy (priv->fakebuf, data, PCM_BUF_SIZE * btmul);

	for (i = 0; i < PCM_BUF_SIZE && i < 1024; i += 2) {
		audio->plugpcm[0][i >> 1] = priv->fakebuf[i];
		audio->plugpcm[1][i >> 1] = priv->fakebuf[i + 1];
	}

	return 0;
}

