#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "main.h"
#include "renderer.h"
#include "display.h"

int act_infinite_init (VisActorPlugin *plugin);
int act_infinite_cleanup (VisActorPlugin *plugin);
int act_infinite_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int act_infinite_requisition (VisActorPlugin *plugin, int *width, int *height);
int act_infinite_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *act_infinite_palette (VisActorPlugin *plugin);
int act_infinite_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *infinite;
	InfinitePrivate *priv;

	plugin = visual_plugin_new ();
	infinite = visual_plugin_actor_new ();

	infinite->name = "infinite";
	infinite->info = visual_plugin_info_new (
			"infinite plugin", 
			"Original by: Julien Carme <julien.carme@acm.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The infinite visual plugin",
			"This is the libvisual plugin for the infinite visual");

	infinite->init =	act_infinite_init;
	infinite->cleanup =	act_infinite_cleanup;
	infinite->requisition = act_infinite_requisition;
	infinite->events =	act_infinite_events;
	infinite->palette =	act_infinite_palette;
	infinite->render =	act_infinite_render;

	infinite->depth = VISUAL_VIDEO_DEPTH_8BIT;

	priv = malloc (sizeof (InfinitePrivate));
	memset (priv, 0, sizeof (InfinitePrivate));

	infinite->priv = priv;	 

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = infinite;
	
	return plugin;
}

int act_infinite_init (VisActorPlugin *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);

	InfinitePrivate *priv = plugin->priv;

	priv->plugwidth = 32;
	priv->plugheight = 32;

	_inf_init_renderer (priv);

	return 0;
}

int act_infinite_cleanup (VisActorPlugin *plugin)
{
	InfinitePrivate *priv;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	priv = plugin->priv;

	_inf_close_renderer (priv);

	free (priv);

	return 0;
}

int act_infinite_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw, reqh;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	reqw = *width;
	reqh = *height;

	while (reqw % 2 || (reqw / 2) % 2)
		reqw--;

	while (reqh % 2 || (reqh / 2) % 2)
		reqh--;

	if (reqw < 32)
		reqw = 32;

	if (reqh < 32)
		reqh = 32;

	*width = reqw;
	*height = reqh;

	return 0;
}

int act_infinite_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	InfinitePrivate *priv;

	visual_log_return_val_if_fail (plugin != NULL, -1);
	visual_log_return_val_if_fail (video != NULL, -1);

	priv = plugin->priv;
	priv->plugwidth = width;
	priv->plugheight = height;

	visual_video_set_dimension (video, width, height);

	_inf_close_renderer (priv);

	if (video->depth != VISUAL_VIDEO_DEPTH_8BIT)
		return -1;

	_inf_init_renderer (priv);

	return 0;
}

int act_infinite_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_infinite_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *act_infinite_palette (VisActorPlugin *plugin)
{
	InfinitePrivate *priv;
	
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	priv = plugin->priv;

	return &priv->pal;
}

int act_infinite_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	InfinitePrivate *priv;
	int i;

	visual_log_return_val_if_fail (plugin != NULL, -1);
	visual_log_return_val_if_fail (video != NULL, -1);
	visual_log_return_val_if_fail (audio != NULL, -1);

	priv = plugin->priv;

	for (i = 0; i < 512; i++) {
		priv->pcm_data[0][i] = audio->pcm[0][i];
		priv->pcm_data[1][i] = audio->pcm[1][i];
	}

	_inf_renderer (priv);
	_inf_display (priv, (uint8_t *) video->screenbuffer);

	return 0;
}

