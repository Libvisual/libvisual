#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <libvisual/libvisual.h>

typedef struct {
	VisPalette pal;
} ScopePrivate;

int lv_scope_init (VisActorPlugin *plugin);
int lv_scope_cleanup (VisActorPlugin *plugin);
int lv_scope_requisition (VisActorPlugin *plugin, int *width, int *height);
int lv_scope_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int lv_scope_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *lv_scope_palette (VisActorPlugin *plugin);
int lv_scope_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *lv_scope;
	ScopePrivate *priv;

	plugin = visual_plugin_new ();
	lv_scope = visual_plugin_actor_new ();

	lv_scope->name = "lv_scope";
	lv_scope->info = visual_plugin_info_new (
			"libvisual scope",
			"Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The Libvisual scope plugin",
			"This is a test plugin that'll display a simple scope");

	lv_scope->init =	lv_scope_init;
	lv_scope->cleanup =	lv_scope_cleanup;
	lv_scope->requisition =	lv_scope_requisition;
	lv_scope->events =	lv_scope_events;
	lv_scope->palette =	lv_scope_palette;
	lv_scope->render =	lv_scope_render;

	lv_scope->depth = VISUAL_VIDEO_DEPTH_8BIT;

	priv = malloc (sizeof (ScopePrivate));
	memset (priv, 0, sizeof (ScopePrivate));

	lv_scope->priv = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = lv_scope;

	return plugin;
}

int lv_scope_init (VisActorPlugin *plugin)
{
	return 0;
}

int lv_scope_cleanup (VisActorPlugin *plugin)
{
	ScopePrivate *priv = plugin->priv;

	free (priv);

	return 0;
}

int lv_scope_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw, reqh;

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

int lv_scope_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	return 0;
}

int lv_scope_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_scope_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *lv_scope_palette (VisActorPlugin *plugin)
{
	ScopePrivate *priv = plugin->priv;
	int i;
	
	for (i = 0; i < 256; i++) {
		priv->pal.r[i] = i;
		priv->pal.g[i] = i;
		priv->pal.b[i] = i;
	}

	return &priv->pal;
}

int lv_scope_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	int adder = video->width > 512 ? (video->width - 512) / 2 : 0;
	int i, y;
	uint8_t *buf;

	if (video == NULL)
		return -1;

	y = video->height >> 1;
	
	buf = (uint8_t *) video->screenbuffer;

	memset (buf, 0, video->pitch * video->height);
	
	for (i = 0; i < video->width && i < 512; i++) {
		y = (video->height / 2) + (audio->pcm[2][i >> 1] >> 9);

		if (y < 0)
			y = 0;
		else if (y > video->height - 1)
			y = video->height - 1;
		
		buf[(y * video->pitch) + i + adder] = i;
	}

	return 0;
}

