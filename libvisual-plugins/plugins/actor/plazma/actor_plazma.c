#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "plazma.h"
#include "actor_plazma.h"

int act_plazma_init (VisPluginData *plugin);
int act_plazma_cleanup (VisPluginData *plugin);
int act_plazma_requisition (VisPluginData *plugin, int *width, int *height);
int act_plazma_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_plazma_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_plazma_palette (VisPluginData *plugin);
int act_plazma_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_plazma_requisition,
		.palette = act_plazma_palette,
		.render = act_plazma_render,
		.depth = VISUAL_VIDEO_DEPTH_8BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "plazma",
		.name = "Plazma plugin",
		.author = "Original by: Pascal Brochart <p.brochart@libertysurf.fr>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.0.1",
		.about = "The plazma visual plugin",
		.help = "This is the libvisual port of the xmms Plazma plugin",

		.init = act_plazma_init,
		.cleanup = act_plazma_cleanup,
		.events = act_plazma_events,

		.plugin = (void *) &actor[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_plazma_init (VisPluginData *plugin)
{
	PlazmaPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);
	VisParamEntry *param;

	priv = visual_mem_new0 (PlazmaPrivate, 1);
	plugin->priv = priv;

	visual_palette_allocate_colors (&priv->colors, 256);
	
	priv->val_maxi =		127;
	priv->chcol0 =			36;
	priv->state =			1368;
	priv->old_state =		1368;
	
	/* Configs */
	priv->bass_sensibility =	0;
	priv->effect =			TRUE;
	priv->options =			FALSE;
	priv->lines =			TRUE;
	priv->spectrum =		TRUE;
	priv->use_3d =			TRUE;
	priv->rot_tourni =		0.4;

	/* Parameters */
	/* Bass sensitivity */
	param = visual_param_entry_new ("bass sensitivity");
	visual_param_entry_set_integer (param, priv->bass_sensibility);
	visual_param_container_add (paramcontainer, param);

	/* Plasma effect */
	param = visual_param_entry_new ("plasma effect");
	visual_param_entry_set_integer (param, priv->effect);
	visual_param_container_add (paramcontainer, param);

	/* 3D effect options */
	param = visual_param_entry_new ("3d effect option");
	visual_param_entry_set_integer (param, priv->options);
	visual_param_container_add (paramcontainer, param);

	/* Lines */
	param = visual_param_entry_new ("lines");
	visual_param_entry_set_integer (param, priv->lines);
	visual_param_container_add (paramcontainer, param);

	/* Spectrum */
	param = visual_param_entry_new ("spectrum");
	visual_param_entry_set_integer (param, priv->spectrum);
	visual_param_container_add (paramcontainer, param);

	/* Use 3D */
	param = visual_param_entry_new ("3d effect");
	visual_param_entry_set_integer (param, priv->use_3d);
	visual_param_container_add (paramcontainer, param);

	/* Rotation speed */
	param = visual_param_entry_new ("rotation speed");
	visual_param_entry_set_float (param, priv->rot_tourni);
	visual_param_container_add (paramcontainer, param);

	
	return 0;
}

int act_plazma_cleanup (VisPluginData *plugin)
{
	PlazmaPrivate *priv = plugin->priv;

	_plazma_cleanup (priv);

	visual_palette_free_colors (&priv->colors);

	visual_mem_free (priv);

	return 0;
}

int act_plazma_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 4)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

int act_plazma_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	PlazmaPrivate *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	priv->width = width;
	priv->height = height;

	priv->tablex = width * 2;
	priv->tabley = height * 2;

	_plazma_cleanup (priv);
	_plazma_init (priv);

	return 0;
}

int act_plazma_events (VisPluginData *plugin, VisEventQueue *events)
{
	PlazmaPrivate *priv = plugin->priv;
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_plazma_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				if (visual_param_entry_is (param, "bass sensitivity")) {
					priv->bass_sensibility = visual_param_entry_get_integer (param);
					
				} else if (visual_param_entry_is (param, "plasma effect")) {
					priv->effect = visual_param_entry_get_integer (param);
					_plazma_change_effect (priv);

				} else if (visual_param_entry_is (param, "3d effect option")) {
					priv->options = visual_param_entry_get_integer (param);
				
				} else if (visual_param_entry_is (param, "lines")) {
					priv->lines = visual_param_entry_get_integer (param);
				
				} else if (visual_param_entry_is (param, "spectrum")) {
					priv->spectrum = visual_param_entry_get_integer (param);
				
				} else if (visual_param_entry_is (param, "3d effect")) {
					priv->use_3d = visual_param_entry_get_integer (param);
				
				} else if (visual_param_entry_is (param, "rotation speed")) {
					priv->rot_tourni = visual_param_entry_get_float (param);
				
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_plazma_palette (VisPluginData *plugin)
{
	PlazmaPrivate *priv = plugin->priv;

	return &priv->colors;;
}

int act_plazma_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PlazmaPrivate *priv = plugin->priv;
	int i;
	
	/* Analyse spectrum data */
	priv->bass = 0;
	for (i = 0; i < 6; i++)
		priv->bass += audio->freq[2][i] >> 4;

	priv->old_state = priv->state;
	
	if (priv->bass_sensibility >= 0)
		priv->state += (priv->bass / 400) + 1 + (priv->bass_sensibility / 2);
	if (priv->bass_sensibility < 0)   {
		priv->state += (priv->bass / 400) + 1 - (abs (priv->bass_sensibility / 2));
		if (priv->state < (priv->old_state + 1))
			priv->state = priv->old_state + 1;
	}

	memcpy (priv->render_buffer, audio->freq, sizeof (priv->render_buffer));
	memcpy (priv->pcm_buffer, audio->pcm, sizeof (priv->pcm_buffer));

	priv->video = video;
	priv->pixel = video->pixels;

	_plazma_run (priv);
	
	return 0;
}

