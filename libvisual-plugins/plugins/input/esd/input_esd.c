#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <esd.h>

#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE	1024

typedef struct {
	int esdhandle;
	int fakebufloaded;
	short fakebuf[PCM_BUF_SIZE];
	int clearcount;
	
	int loaded;
} EsdPrivate;

int inp_esd_init (VisPluginData *plugin);
int inp_esd_cleanup (VisPluginData *plugin);
int inp_esd_upload (VisPluginData *plugin, VisAudio *audio);

static const int inp_esd_var_btmul = sizeof (short);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisInputPlugin input[] = {{
		.upload = inp_esd_upload
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "esd",
		.name = "esd",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = "The ESOUND capture plugin",
		.help = "Use this plugin to capture PCM data from the ESD daemon",

		.init = inp_esd_init,
		.cleanup = inp_esd_cleanup,

		.plugin = (void *) &input[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int inp_esd_init (VisPluginData *plugin)
{
	EsdPrivate *priv;

	visual_log_return_val_if_fail (plugin != NULL, -1);
	
	priv = visual_mem_new0 (EsdPrivate, 1);
	visual_log_return_val_if_fail (priv != NULL, -1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);	

	priv->esdhandle = esd_monitor_stream (ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_MONITOR, 44100, NULL, "lv_esd_plugin");

	if (priv->esdhandle <= 0)
		return -1;

	fcntl (priv->esdhandle, F_SETFL, O_NONBLOCK);

	priv->loaded = 1;

	return 0;
}

int inp_esd_cleanup (VisPluginData *plugin)
{
	EsdPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_log_return_val_if_fail( plugin != NULL, -1 );
	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_log_return_val_if_fail( priv != NULL, -1 );
	
	if (priv->loaded == 1)
		esd_close (priv->esdhandle);

	visual_mem_free (priv);

	return 0;
}

int inp_esd_upload (VisPluginData *plugin, VisAudio *audio)
{
	EsdPrivate *priv = NULL;
	short esddata[PCM_BUF_SIZE];
	int rcnt;	
	int i;

	visual_log_return_val_if_fail(audio != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_log_return_val_if_fail(priv != NULL, -1);

	rcnt = read (priv->esdhandle, esddata,
		     PCM_BUF_SIZE * inp_esd_var_btmul);
	
	if (rcnt < 0) {
		if (priv->fakebufloaded == 1) {
			priv->clearcount++;

			if (priv->clearcount > 100)
				memset (priv->fakebuf, 0, 
					PCM_BUF_SIZE * inp_esd_var_btmul);
			
			memcpy (esddata, priv->fakebuf, 
				PCM_BUF_SIZE * inp_esd_var_btmul);
		} else {
			memset (esddata, 0, sizeof (esddata));
		}	
	} else {
		priv->clearcount = 0;
	}
	
	priv->fakebufloaded = 1;

	memcpy (priv->fakebuf, esddata, PCM_BUF_SIZE * inp_esd_var_btmul);

	for (i = 0; i < PCM_BUF_SIZE && i < 1024; i += 2) {
		audio->plugpcm[0][i >> 1] = priv->fakebuf[i];
		audio->plugpcm[1][i >> 1] = priv->fakebuf[i + 1];
	}

	return 0;
}

