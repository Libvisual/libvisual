#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include <time.h>

#include "nebulus.h"

float framerate = 50;
GLfloat scale, heights[16][16];

int loudness = 0, too_long;
int beat = 0, beat_compteur = 0;
static int beathistory[BEAT_MAX];
static int beatbase;

short pcm_data[512];
nebulus general = { 32, 32, 0, 4, 4, 100, 100, 0, FALSE, FALSE,
	                    TRUE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };
nebulus *point_general = &general;

effect my_effect[EFFECT_NUMBER] = {{ "Knot", 100 }, { "Spectrum", 100 },
	{ "Face blur", 100 }, { "Glthreads", 100 },
	{ "Tunnel", 100 }, { "Tentacles", 100 },
	{ "Twist", 100 }, { "Child", 100 },
	{ "Energy", 100 }};
effect my_effect_old[EFFECT_NUMBER];

GLint maxtexsize;

typedef struct {
	struct timeval tv_past;
} NebulusPrivate;

int lv_nebulus_init (VisActorPlugin *plugin);
int lv_nebulus_cleanup (VisActorPlugin *plugin);
int lv_nebulus_requisition (VisActorPlugin *plugin, int *width, int *height);
int lv_nebulus_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int lv_nebulus_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *lv_nebulus_palette (VisActorPlugin *plugin);
int lv_nebulus_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

static int nebulus_random_effect ();
static int nebulus_detect_beat (int loudness);
static int nebulus_sound (NebulusPrivate *priv, VisAudio *audio);
static int nebulus_draw (NebulusPrivate *priv, VisVideo *video);

/* Main plugin stuff */
LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *lv_nebulus;
	NebulusPrivate *priv;

	plugin = visual_plugin_new ();
	lv_nebulus = visual_plugin_actor_new ();

	lv_nebulus->name	= "nebulus";
	lv_nebulus->info = visual_plugin_info_new (
			"libvisual nebulus port",
			"Original by: Pascal Brochart <pbrochart@tuxfamily.org> and many others, Port and maintaince by: Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The Libvisual nebulus plugin",
			"This plugin shows multiple visual effect using openGL");

	lv_nebulus->init =		lv_nebulus_init;
	lv_nebulus->cleanup =		lv_nebulus_cleanup;
	lv_nebulus->requisition =	lv_nebulus_requisition;
	lv_nebulus->events =		lv_nebulus_events;
	lv_nebulus->palette =		lv_nebulus_palette;
	lv_nebulus->render =		lv_nebulus_render;
	
	lv_nebulus->depth = VISUAL_VIDEO_DEPTH_GL;

	priv = malloc (sizeof (NebulusPrivate));
	memset (priv, 0, sizeof (NebulusPrivate));

	lv_nebulus->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = lv_nebulus;

	return plugin;
}

int lv_nebulus_init (VisActorPlugin *plugin)
{
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxtexsize);
	if (maxtexsize < 256) {
		visual_log (VISUAL_LOG_CRITICAL, "Nebulus: max texture size is lower than 256\n");
		return -1;
	}

	if (tunnel_first)
		precalculate_tunnel();

	init_gl();

	return 0;
}

int lv_nebulus_cleanup (VisActorPlugin *plugin)
{
	NebulusPrivate *priv = plugin->private;

	if (!face_first)
		glDeleteLists(facedl, 1);
	if (!tentacles_first)
		glDeleteLists(cubedl, 1);
	if (!child_first)
		glDeleteLists(childdl, 1);

	delete_gl_texture(knotbg);
	delete_gl_texture(tunnel);
	delete_gl_texture(tentacle);
	delete_gl_texture(twist);
	delete_gl_texture(twistbg);
	delete_gl_texture(texchild);
	delete_gl_texture(childbg);
	delete_gl_texture(energy);

	visual_video_free_buffer (&child_image);
	visual_video_free_buffer (&energy_image);
	visual_video_free_buffer (&tentacle_image);
	visual_video_free_buffer (&tunnel_image);
	visual_video_free_buffer (&twist_image);
	visual_video_free_buffer (&background_image);

	free (priv);

	return 0;
}

int lv_nebulus_requisition (VisActorPlugin *plugin, int *width, int *height)
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

int lv_nebulus_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	visual_video_set_dimension (video, width, height);

	glViewport (0, 0, width, height);

	point_general->WIDTH = width;
	point_general->HEIGHT = height;
	
	return 0;
}

int lv_nebulus_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_nebulus_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default:
				break;
		}
	}

	return 0;
}

VisPalette *lv_nebulus_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int lv_nebulus_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	nebulus_sound (plugin->private, audio);
	nebulus_draw (plugin->private, video);

	return 0;
}

static int nebulus_calc_fps (NebulusPrivate *priv)
{
	// FIXME make
	framerate = 25;
	return 40;
}

static int nebulus_random_effect ()
{
	int i;
	int rnd_effect = 0, new_effect = 0, effect = 0;

	for (i = 0; i < EFFECT_NUMBER; i++)
		rnd_effect += my_effect[i].value * 100;     /* 100 pour + de precision */
	if (!rnd_effect)
		return point_general->effect;
	new_effect = rand() % rnd_effect;
	rnd_effect = 0;
	for (i = 0; i < EFFECT_NUMBER; i++) {
		rnd_effect += my_effect[i].value * 100;     /* 100 pour + de precision */
		if (new_effect <= rnd_effect) {
			effect = i;
			if (!effect)
				effect = EFFECT_NUMBER;
			return effect;
		}
	}
	return point_general->effect;
}

static int nebulus_detect_beat (int loudness)
{
	static int    aged;
	static int    lowest;
	static int elapsed;
	static int isquiet;
	static int prevbeat;
	int i, j;
	int total;
	int sensitivity;
	int detected_beat;

	aged = (aged * 7 + loudness) >> 3;
	elapsed++;

	if (aged < 2000 || elapsed > BEAT_MAX) {
		elapsed = 0;
		lowest = aged;
		memset(beathistory, 0, sizeof beathistory);
	}
	else if (aged < lowest)
		lowest = aged;

	j = (beatbase + elapsed) % BEAT_MAX;
	beathistory[j] = loudness - aged;
	detected_beat = FALSE;
	if (elapsed > 15 && aged > 2000 && loudness * 4 > aged * 5) {
		for (i = BEAT_MAX / elapsed, total = 0;
				--i > 0;
				j = (j + BEAT_MAX - elapsed) % BEAT_MAX)
		{
			total += beathistory[j];
		}
		total = total * elapsed / BEAT_MAX;
		sensitivity = 6;
		i = 3 - abs(elapsed - prevbeat)/2;
		if (i > 0)
			sensitivity += i;
		if (total * sensitivity > aged) {
			prevbeat = elapsed;
			beatbase = (beatbase + elapsed) % BEAT_MAX;
			lowest = aged;
			elapsed = 0;
			detected_beat = TRUE;
		}
	}
	if (aged < (isquiet ? 1500 : 500))
		isquiet = TRUE;
	else
		isquiet = FALSE;

	return detected_beat;
}

static int nebulus_sound (NebulusPrivate *priv, VisAudio *audio)
{
	int i, c, y, tmp;
	GLfloat val, energy = 0;
	int xscale[] = { 0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255 };

	/* Copy over the pcmdata into the internal pcmdata */
	memcpy(pcm_data, audio->pcm, sizeof(pcm_data));

	for(y = 15; y > 0; y--) {
		for(i = 0; i < 16; i++)
			heights[y][i] = heights[y - 1][i];
	}
	for(i = 0; i < NUM_BANDS; i++) {
		for(c = xscale[i], y = 0; c < xscale[i + 1]; c++) {
			if(audio->freq[0][c] > y)
				y = audio->freq[0][c];
		}
		loudness += (y / (xscale[i + 1] - xscale[i] + 1)) *
			(abs (i - NUM_BANDS / 2) + NUM_BANDS / 2) * (4 + i);
		y >>= 7;
		if(y > 0)
			val = (log(y) * scale);
		else
			val = 0;
		heights[0][i] = val;
	}
	loudness /= (NUM_BANDS * 4);
	beat = nebulus_detect_beat (loudness);
	too_long++;
	if (!point_general->freeze) {
		if (too_long > 1000) {
			too_long = 0;
			point_general->effect = nebulus_random_effect();
			point_general->changement = TRUE;
		}
		if (beat) {
			if (beat_compteur > point_general->beat - 1) {
				point_general->effect = nebulus_random_effect();
				point_general->changement = TRUE;
				beat_compteur = 0;
				too_long = 0;
			}
			beat_compteur += beat;
		}
	}
	for (i = 0; i < 256; i++) {
		tmp = audio->freq[0][i] >> 4;
		energy += tmp * tmp;
	}
	energy =  energy / 65536 / 256 * 256;
	energy += 6.0f;
	energy *= 1.2f;
	if (energy > 10)
		energy = 10;
	point_general->energy = energy;

	return 0;
}

static int nebulus_draw (NebulusPrivate *priv, VisVideo *video)
{
	draw_scene ();
	glFinish ();

	nebulus_calc_fps (priv);

	return 0;
}

