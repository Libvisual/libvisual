#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include <goom/goom.h>

#include <libvisual/libvisual.h>

typedef struct {
	PluginInfo	*goominfo; /* The goom internal private struct */

	char		*prevname;
	int		 name_changed;
} GoomPrivate;

int lv_goom_init (VisActorPlugin *plugin);
int lv_goom_cleanup (VisActorPlugin *plugin);
int lv_goom_requisition (VisActorPlugin *plugin, int *width, int *height);
int lv_goom_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int lv_goom_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *lv_goom_palette (VisActorPlugin *plugin);
int lv_goom_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *goom;
	GoomPrivate *priv;

	plugin = visual_plugin_new ();
	goom = visual_plugin_actor_new ();

	goom->name = "goom";
	goom->info = visual_plugin_info_new (
			"libvisual goom2 plugin",
			"Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The Libvisual goom2 plugin",
			"This plugin adds support for the supercool goom2 plugin that is simply awesome");

	goom->init =		lv_goom_init;
	goom->cleanup =		lv_goom_cleanup;
	goom->requisition =	lv_goom_requisition;
	goom->events =		lv_goom_events;
	goom->palette =		lv_goom_palette;
	goom->render =		lv_goom_render;

	goom->depth = VISUAL_VIDEO_DEPTH_32BIT;

	priv = malloc (sizeof (GoomPrivate));
	memset (priv, 0, sizeof (GoomPrivate));

	goom->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = goom;

	return plugin;
}

int lv_goom_init (VisActorPlugin *plugin)
{
	GoomPrivate *priv = plugin->private;

	priv->goominfo = goom_init (128, 128);
	
	return 0;
}

int lv_goom_cleanup (VisActorPlugin *plugin)
{
	GoomPrivate *priv = plugin->private;

	if (priv->goominfo != NULL)
		goom_close (priv->goominfo);

	if (priv->prevname != NULL)
		free (priv->prevname);

	free (priv);

	return 0;
}

int lv_goom_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	/* We don't change the value, we can handle anything */	

	return 0;
}

int lv_goom_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	GoomPrivate *priv = plugin->private;

	visual_video_set_dimension (video, width, height);

	goom_set_resolution (priv->goominfo, width, height);

	return 0;
}

int lv_goom_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_NEWSONG:
				printf ("NEW SONG: %s\n",
						ev.newsong.songinfo->songname);
				break;

			case VISUAL_EVENT_RESIZE:
				lv_goom_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);

				printf ("NEW RESIZE: %d %d %p\n",
						ev.resize.width,
						ev.resize.height,
						ev.resize.video);
				break;

			case VISUAL_EVENT_MOUSEMOTION:
				printf ("NEW MOUSE MOTION: %d %d\n",
						ev.mousemotion.x,
						ev.mousemotion.y);
				break;

			case VISUAL_EVENT_MOUSEBUTTONDOWN:
			case VISUAL_EVENT_MOUSEBUTTONUP:
				printf ("NEW MOUSE BUTTON: %d %d | %d %d\n",
						ev.mousebutton.state,
						ev.mousebutton.button,
						ev.mousebutton.x,
						ev.mousebutton.y);
				break;
		
			case VISUAL_EVENT_KEYDOWN:
				printf ("NEW KEYBOARD: %d %d\n",
						ev.keyboard.keysym.sym,
						ev.keyboard.keysym.mod);
				break;
		}
	}

	return 0;
}

VisPalette *lv_goom_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int lv_goom_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	GoomPrivate *priv = plugin->private;
	VisSongInfo *songinfo = plugin->songinfo;
	short pcmdata[2][512];
	uint32_t *buf;
	int i;

	for (i = 0; i < 512; i++) {
		pcmdata[0][i] = audio->pcm[0][i];
		pcmdata[1][i] = audio->pcm[1][i];
	}

	/* FIXME add a prop to disable songinfo display, make that a global prop */
	/* FIXME goom should support setting a pointer, so we don't need that final memcpy */
	if (songinfo != NULL && visual_songinfo_age (songinfo) <= 1) {
		if (songinfo->type == VISUAL_SONGINFO_TYPE_SIMPLE) {
			buf = goom_update (priv->goominfo, pcmdata, 0, 0, songinfo->songname, NULL);
		} else if (songinfo->type == VISUAL_SONGINFO_TYPE_ADVANCED) {
			buf = goom_update (priv->goominfo, pcmdata, 0, 0, songinfo->song, NULL);
		} else
			buf = goom_update (priv->goominfo, pcmdata, 0, 0, NULL, NULL);
	}
	else
		buf = goom_update (priv->goominfo, pcmdata, 0, 0, NULL, NULL);

	memcpy (video->screenbuffer, buf, video->width * video->height * video->bpp);

	return 0;
}

