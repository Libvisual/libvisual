#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

#include "actor_bumpscope.h"

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
	bumpscope->cleanup =	act_bumpscope_cleanup;
	bumpscope->requisition =	act_bumpscope_requisition;
	bumpscope->events =	act_bumpscope_events;
	bumpscope->palette =	act_bumpscope_palette;
	bumpscope->render =	act_bumpscope_render;

	bumpscope->depth = VISUAL_VIDEO_DEPTH_32BIT;

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

	return 0;
}

int act_bumpscope_cleanup (VisActorPlugin *plugin)
{
	BumpscopePrivate *priv = plugin->priv;
	
	free (priv);

	return 0;
}

int act_bumpscope_requisition (VisActorPlugin *plugin, int *width, int *height)
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

int act_bumpscope_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	BumpscopePrivate *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	return 0;
}

int act_bumpscope_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	BumpscopePrivate *priv = plugin->priv;
	VisEvent ev;
	VisParamEntry *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_bumpscope_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.param.param;

				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_bumpscope_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int act_bumpscope_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	BumpscopePrivate *priv = plugin->priv;
	
	return 0;
}

