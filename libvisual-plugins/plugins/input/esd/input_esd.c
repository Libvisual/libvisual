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

int inp_esd_init (VisInputPlugin *plugin);
int inp_esd_cleanup (VisInputPlugin *plugin);
int inp_esd_upload (VisInputPlugin *plugin, VisAudio *audio);

static const int inp_esd_var_btmul = sizeof (short);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisInputPlugin *esd_input;
	EsdPrivate *priv;
	
	plugin = visual_plugin_new ();
	visual_log_return_val_if_fail(plugin != NULL, NULL);
	esd_input = visual_plugin_input_new ();
	visual_log_return_val_if_fail(esd_input != NULL, NULL);
	
	esd_input->name = "esd";
	esd_input->info = visual_plugin_info_new ("esd", "Dennis Smit <ds@nerds-incorporated.org>", "0.1",
			"The ESOUND capture plugin", "Use this plugin to capture PCM data from the ESD daemon");

	esd_input->init =	inp_esd_init;
	esd_input->cleanup =	inp_esd_cleanup;
	esd_input->upload =	inp_esd_upload;

	priv = malloc (sizeof (EsdPrivate));
	visual_log_return_val_if_fail(priv != NULL, NULL);
	memset (priv, 0, sizeof (EsdPrivate));
	
	esd_input->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_INPUT;
	plugin->plugin.inputplugin = esd_input;

	return plugin;
}

int inp_esd_init (VisInputPlugin *plugin)
{
	EsdPrivate *priv = NULL;

	visual_log_return_val_if_fail( plugin != NULL, -1 );
	priv = plugin->priv;
	visual_log_return_val_if_fail( priv != NULL, -1 );
	
	priv->esdhandle = esd_monitor_stream (ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_MONITOR, 44100, NULL, "lv_esd_plugin");

	if (priv->esdhandle <= 0)
		return -1;

	fcntl (priv->esdhandle, F_SETFL, O_NONBLOCK);

	priv->loaded = 1;

	return 0;
}

int inp_esd_cleanup (VisInputPlugin *plugin)
{
	EsdPrivate *priv = plugin->priv;

	visual_log_return_val_if_fail( plugin != NULL, -1 );
	priv = plugin->priv;
	visual_log_return_val_if_fail( priv != NULL, -1 );
	
	if (priv->loaded == 1)
		esd_close (priv->esdhandle);

	free (priv);

	return 0;
}

int inp_esd_upload (VisInputPlugin *plugin, VisAudio *audio)
{
	EsdPrivate *priv = NULL;
	short esddata[PCM_BUF_SIZE];
	int rcnt;	
	int i;

	visual_log_return_val_if_fail(audio != NULL, -1);
	visual_log_return_val_if_fail(plugin != NULL, -1);
	priv = plugin->priv;
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

