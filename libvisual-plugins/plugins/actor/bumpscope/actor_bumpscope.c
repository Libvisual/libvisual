#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "actor_bumpscope.h"
#include "bump_scope.h"

int act_bumpscope_init (VisActorPlugin *plugin);
int act_bumpscope_cleanup (VisActorPlugin *plugin);
int act_bumpscope_requisition (VisActorPlugin *plugin, int *width, int *height);
int act_bumpscope_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int act_bumpscope_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *act_bumpscope_palette (VisActorPlugin *plugin);
int act_bumpscope_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *bumpscope;
	BumpscopePrivate *priv;

	plugin = visual_plugin_new ();
	bumpscope = visual_plugin_actor_new ();
	
	bumpscope->name = "bumpscope";
	bumpscope->info = visual_plugin_info_new (
			"Bumpscope plugin",
			"Original by: Zinx Verituse <zinx@xmms.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
			"0.0.1",
			"The bumpscope visual plugin",
			"This is the libvisual port of the xmms Bumpscope plugin");

	bumpscope->init =		act_bumpscope_init;
	bumpscope->cleanup =		act_bumpscope_cleanup;
	bumpscope->requisition =	act_bumpscope_requisition;
	bumpscope->events =		act_bumpscope_events;
	bumpscope->palette =		act_bumpscope_palette;
	bumpscope->render =		act_bumpscope_render;

	bumpscope->depth = VISUAL_VIDEO_DEPTH_8BIT;

	priv = malloc (sizeof (BumpscopePrivate));
	memset (priv, 0, sizeof (BumpscopePrivate));

	bumpscope->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = bumpscope;
	
	return plugin;
}

int act_bumpscope_init (VisActorPlugin *plugin)
{
	BumpscopePrivate *priv = plugin->priv;
	VisParamContainer *paramcontainer = &plugin->params;
	VisParamEntry *param;

	visual_palette_allocate_colors (&priv->pal, 256);

	priv->color.r =		122;
	priv->color.g =		204;
	priv->color.b =		255;
	priv->phongres =	256;
	priv->color_cycle =	TRUE;
	priv->moving_light =	TRUE;
	priv->diamond =		FALSE;

	/* Parameters */
	/* Color */
	param = visual_param_entry_new ("color");
	visual_param_entry_set_color_by_color (param, &priv->color);
	visual_param_container_add (paramcontainer, param);

	/* Light size */
	param = visual_param_entry_new ("light size");
	visual_param_entry_set_integer (param, priv->phongres);
	visual_param_container_add (paramcontainer, param);

	/* Color cycle */
	param = visual_param_entry_new ("color cycle");
	visual_param_entry_set_integer (param, priv->color_cycle);
	visual_param_container_add (paramcontainer, param);

	/* Moving light */
	param = visual_param_entry_new ("moving light");
	visual_param_entry_set_integer (param, priv->moving_light);
	visual_param_container_add (paramcontainer, param);

	/* Diamond or circle */
	param = visual_param_entry_new ("diamond");
	visual_param_entry_set_integer (param, priv->diamond);
	visual_param_container_add (paramcontainer, param);

	return 0;
}

int act_bumpscope_cleanup (VisActorPlugin *plugin)
{
	BumpscopePrivate *priv = plugin->priv;
	
	__bumpscope_cleanup (priv);

	visual_palette_free_colors (&priv->pal);

	free (priv);

	return 0;
}

int act_bumpscope_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	while (reqw % 4)
		reqw--;

	while (reqh % 2)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

int act_bumpscope_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	BumpscopePrivate *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	priv->video = video;

	priv->width = width;
	priv->height = height;

	__bumpscope_cleanup (priv);
	__bumpscope_init (priv);

	return 0;
}

int act_bumpscope_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	BumpscopePrivate *priv = plugin->priv;
	VisEvent ev;
	VisParamEntry *param;
	VisColor *tmp;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_bumpscope_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				if (ev.mousemotion.state == VISUAL_MOUSE_DOWN) {
					priv->light_x = ev.mousemotion.x;
					priv->light_y = ev.mousemotion.y;
				}

				break;
				
			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				if (visual_param_entry_is (param, "color")) {
					tmp = visual_param_entry_get_color (param);
					visual_color_copy (&priv->color, tmp);

					__bumpscope_generate_palette (priv, &priv->color);
				} else if (visual_param_entry_is (param, "light size")) {
					priv->phongres = visual_param_entry_get_integer (param);

					__bumpscope_generate_phongdat (priv);	
				} else if (visual_param_entry_is (param, "color cycle")) {
					priv->color_cycle = visual_param_entry_get_integer (param);
					
				} else if (visual_param_entry_is (param, "moving light")) {
					priv->moving_light = visual_param_entry_get_integer (param);
					
				} else if (visual_param_entry_is (param, "diamond")) {
					priv->diamond = visual_param_entry_get_integer (param);
					
					__bumpscope_generate_phongdat (priv);
				}

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_bumpscope_palette (VisActorPlugin *plugin)
{
	BumpscopePrivate *priv = plugin->priv;

	return &priv->pal;
}

int act_bumpscope_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	BumpscopePrivate *priv = plugin->priv;

	priv->video = video;

	__bumpscope_render_pcm (priv, audio->pcm);

	memcpy (video->screenbuffer, priv->rgb_buf2, video->size);

	return 0;
}

