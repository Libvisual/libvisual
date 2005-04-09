#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <libvisual/libvisual.h>

#include "actor_xpyre.h"
#include "draw.h"

int act_xpyre_init (VisActorPlugin *plugin);
int act_xpyre_cleanup (VisActorPlugin *plugin);
int act_xpyre_requisition (VisActorPlugin *plugin, int *width, int *height);
int act_xpyre_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int act_xpyre_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *act_xpyre_palette (VisActorPlugin *plugin);
int act_xpyre_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *xpyre;
	XpyrePrivate *priv;

	plugin = visual_plugin_new ();
	xpyre = visual_plugin_actor_new ();
	
	xpyre->name = "xpyre";
	xpyre->info = visual_plugin_info_new (
			"Xpyre plugin",
			"Original by: Andy Goth <unununium@openverse.com>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
			"0.0.1",
			"The xpyre visual plugin",
			"This is the libvisual port of the xmms Xpyre plugin");

	xpyre->init =		act_xpyre_init;
	xpyre->cleanup =	act_xpyre_cleanup;
	xpyre->requisition =	act_xpyre_requisition;
	xpyre->events =		act_xpyre_events;
	xpyre->palette =	act_xpyre_palette;
	xpyre->render =		act_xpyre_render;

	xpyre->depth = VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (XpyrePrivate));
	visual_mem_set (priv, 0, sizeof (XpyrePrivate));

	xpyre->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = xpyre;
	
	return plugin;
}

int act_xpyre_init (VisActorPlugin *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	return 0;
}

int act_xpyre_cleanup (VisActorPlugin *plugin)
{
	XpyrePrivate *priv = plugin->priv;
	
	free (priv);

	return 0;
}

int act_xpyre_requisition (VisActorPlugin *plugin, int *width, int *height)
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

int act_xpyre_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	XpyrePrivate *priv = plugin->priv;
	
	visual_video_set_dimension (video, width, height);

	return 0;
}

int act_xpyre_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	XpyrePrivate *priv = plugin->priv;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_xpyre_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_xpyre_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int act_xpyre_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	XpyrePrivate *priv = plugin->priv;
	uint32_t *vscr = video->screenbuffer;
	
	return 0;
}

