#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "feedback.h"
#include "plotter.h"

#include <libvisual/libvisual.h>

int act_jakdaw_init (VisPluginData *plugin);
int act_jakdaw_cleanup (VisPluginData *plugin);
int act_jakdaw_requisition (VisPluginData *plugin, int *width, int *height);
int act_jakdaw_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int act_jakdaw_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *act_jakdaw_palette (VisPluginData *plugin);
int act_jakdaw_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = act_jakdaw_requisition,
		.palette = act_jakdaw_palette,
		.render = act_jakdaw_render,
		.depth = VISUAL_VIDEO_DEPTH_32BIT
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "jakdaw",
		.name = "Jakdaw plugin",
		.author = "Original by: Christopher Wilson <Jakdaw@usa.net>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.0.1",
		.about = "The jakdaw visual plugin",
		.help = "This is the libvisual port of the xmms Jakdaw plugin",

		.init = act_jakdaw_init,
		.cleanup = act_jakdaw_cleanup,
		.events = act_jakdaw_events,

		.plugin = (void *) &actor[0]
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int act_jakdaw_init (VisPluginData *plugin)
{
	JakdawPrivate *priv;
	VisParamContainer *paramcontainer = &plugin->params;
	VisParamEntry *param;

	priv = visual_mem_new0 (JakdawPrivate, 1);
	plugin->priv = priv;

	priv->decay_rate = 1;
	
	priv->zoom_mode = FEEDBACK_ZOOMRIPPLE;
	priv->zoom_ripplesize = 32;
	priv->zoom_ripplefact = 0.1;
	priv->zoom_zoomfact = 0.9;

	priv->plotter_amplitude = 100;
	priv->plotter_colortype = PLOTTER_COLOUR_MUSICTRIG;
	priv->plotter_scopecolor = 0xff00ff;
	priv->plotter_scopetype = PLOTTER_SCOPE_LINES;

	/* Parameters */
	/* Zoom mode */
	param = visual_param_entry_new ("zoom mode");
	visual_param_entry_set_integer (param, priv->zoom_mode);
	visual_param_container_add (paramcontainer, param);

	/* Plotter color trigger */
	param = visual_param_entry_new ("plotter trigger");
	visual_param_entry_set_integer (param, priv->plotter_colortype);
	visual_param_container_add (paramcontainer, param);

	/* Zoom mode */
	param = visual_param_entry_new ("plotter type");
	visual_param_entry_set_integer (param, priv->plotter_scopetype);
	visual_param_container_add (paramcontainer, param);

	return 0;
}

int act_jakdaw_cleanup (VisPluginData *plugin)
{
	JakdawPrivate *priv = plugin->priv;
	
	visual_mem_free (priv);

	return 0;
}

int act_jakdaw_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

int act_jakdaw_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	JakdawPrivate *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	priv->xres = width;
	priv->yres = height;

	_jakdaw_feedback_reset (priv, width, height);
	_jakdaw_plotter_reset (priv, width, height);
	
	return 0;
}

int act_jakdaw_events (VisPluginData *plugin, VisEventQueue *events)
{
	JakdawPrivate *priv = plugin->priv;
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_jakdaw_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				visual_log (VISUAL_LOG_DEBUG, "Param changed: %s\n", param->name);

				if (visual_param_entry_is (param, "zoom mode")) {
					visual_log (VISUAL_LOG_DEBUG, "New value for the zoom mode param: %d\n",
							param->data.integer);

					priv->zoom_mode = visual_param_entry_get_integer (param);

					_jakdaw_feedback_reset (priv, priv->xres, priv->yres);
				}
				else if (visual_param_entry_is (param, "plotter trigger")) {
					visual_log (VISUAL_LOG_DEBUG, "New value for the plotter trigger param: %d\n",
							param->data.integer);

					priv->plotter_colortype = visual_param_entry_get_integer (param);

					_jakdaw_plotter_reset (priv, priv->xres, priv->yres);
				}
				else if (visual_param_entry_is (param, "plotter type")) {
					visual_log (VISUAL_LOG_DEBUG, "New value for the plotter type param: %d\n",
							param->data.integer);

					priv->plotter_scopetype = visual_param_entry_get_integer (param);

					_jakdaw_feedback_reset (priv, priv->xres, priv->yres);
				}
				
				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_jakdaw_palette (VisPluginData *plugin)
{
	return NULL;
}

int act_jakdaw_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	JakdawPrivate *priv = plugin->priv;
	uint32_t *vscr = video->screenbuffer;

	_jakdaw_feedback_render (priv, vscr);
	_jakdaw_plotter_draw (priv, audio->pcm, audio->freq, vscr);
	
	return 0;
}

