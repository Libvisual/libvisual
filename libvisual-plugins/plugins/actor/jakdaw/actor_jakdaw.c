#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "feedback.h"
#include "plotter.h"

#include <libvisual/libvisual.h>

int act_jakdaw_init (VisActorPlugin *plugin);
int act_jakdaw_cleanup (VisActorPlugin *plugin);
int act_jakdaw_requisition (VisActorPlugin *plugin, int *width, int *height);
int act_jakdaw_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int act_jakdaw_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *act_jakdaw_palette (VisActorPlugin *plugin);
int act_jakdaw_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *jakdaw;
	JakdawPrivate *priv;

	plugin = visual_plugin_new ();
	jakdaw = visual_plugin_actor_new ();
	
	jakdaw->name = "jakdaw";
	jakdaw->info = visual_plugin_info_new (
			"Jakdaw plugin",
			"Original by: Christopher Wilson <Jakdaw@usa.net>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
			"0.0.1",
			"The jakdaw visual plugin",
			"This is the libvisual port of the xmms Jakdaw plugin");

	jakdaw->init =		act_jakdaw_init;
	jakdaw->cleanup =	act_jakdaw_cleanup;
	jakdaw->requisition =	act_jakdaw_requisition;
	jakdaw->events =	act_jakdaw_events;
	jakdaw->palette =	act_jakdaw_palette;
	jakdaw->render =	act_jakdaw_render;

	jakdaw->depth = VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (JakdawPrivate));
	memset (priv, 0, sizeof (JakdawPrivate));

	jakdaw->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = jakdaw;
	
	return plugin;
}

int act_jakdaw_init (VisActorPlugin *plugin)
{
	JakdawPrivate *priv = plugin->priv;
	VisParamContainer *paramcontainer = &plugin->params;
	VisParamEntry *param;

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

int act_jakdaw_cleanup (VisActorPlugin *plugin)
{
	JakdawPrivate *priv = plugin->priv;
	
	free (priv);

	return 0;
}

int act_jakdaw_requisition (VisActorPlugin *plugin, int *width, int *height)
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

int act_jakdaw_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	JakdawPrivate *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	priv->xres = width;
	priv->yres = height;

	_jakdaw_feedback_reset (priv, width, height);
	_jakdaw_plotter_reset (priv, width, height);
	
	return 0;
}

int act_jakdaw_events (VisActorPlugin *plugin, VisEventQueue *events)
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

VisPalette *act_jakdaw_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int act_jakdaw_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	JakdawPrivate *priv = plugin->priv;
	uint32_t *vscr = video->screenbuffer;

	_jakdaw_feedback_render (priv, vscr);
	_jakdaw_plotter_draw (priv, audio->pcm, audio->freq, vscr);
	
	return 0;
}

