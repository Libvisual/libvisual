#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include <time.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <libvisual/libvisual.h>

/* FIXME Make params for these when the core supports this */
#define NUM_STARS	512
#define SPEED		715

typedef struct {
	int draw_mode;
	
	int texture[2];

	int maxlines;
	float texsize;
	float xrot;
	float yrot;
	float zrot;
	float total;
	float frame;

	float gdata[256];

	struct timeval tv_past;
} MadspinPrivate;

int lv_madspin_init (VisActorPlugin *plugin);
int lv_madspin_cleanup (VisActorPlugin *plugin);
int lv_madspin_requisition (VisActorPlugin *plugin, int *width, int *height);
int lv_madspin_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int lv_madspin_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *lv_madspin_palette (VisActorPlugin *plugin);
int lv_madspin_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

static int madspin_load_textures (MadspinPrivate *priv);
static int madspin_sound (MadspinPrivate *priv, VisAudio *audio);
static int madspin_draw (MadspinPrivate *priv, VisVideo *video);

/* Main plugin stuff */
LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *lv_madspin;
	MadspinPrivate *priv;

	plugin = visual_plugin_new ();
	lv_madspin = visual_plugin_actor_new ();

	lv_madspin->name	= "madspin";
	lv_madspin->info = visual_plugin_info_new (
			"libvisual madspin port",
			"Original by: Andrew Birck <birck@uiuc.edu>, Port by: Dennis Smit <ds@nerds-incorporated.org>",
			"0.1",
			"The Libvisual madspin plugin",
			"This plugin shows a nifty visual effect using openGL");

	lv_madspin->init =		lv_madspin_init;
	lv_madspin->cleanup =		lv_madspin_cleanup;
	lv_madspin->requisition =	lv_madspin_requisition;
	lv_madspin->events =		lv_madspin_events;
	lv_madspin->palette =		lv_madspin_palette;
	lv_madspin->render =		lv_madspin_render;
	
	lv_madspin->depth = VISUAL_VIDEO_DEPTH_GL;

	priv = malloc (sizeof (MadspinPrivate));
	memset (priv, 0, sizeof (MadspinPrivate));

	lv_madspin->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = lv_madspin;

	return plugin;
}

int lv_madspin_init (VisActorPlugin *plugin)
{
	MadspinPrivate *priv = plugin->private;

	priv->maxlines = 1;
	priv->texsize = 0.25f;
	priv->xrot = 0.0f;
	priv->yrot = 0.0f;
	priv->zrot = 0.0f;
	priv->total = 0;
	priv->frame = 0;

	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	glOrtho (-4.0, 4.0, -4.0, 4.0, -18.0, 18.0);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glDisable (GL_DEPTH_TEST);
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel (GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE);
	glEnable (GL_BLEND);
	glEnable (GL_TEXTURE_2D);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	madspin_load_textures (plugin->private);

	return 0;
}

int lv_madspin_cleanup (VisActorPlugin *plugin)
{
	MadspinPrivate *priv = plugin->private;

	free (priv);

	return 0;
}

int lv_madspin_requisition (VisActorPlugin *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

	if (reqw < 1)
		reqw = 1;

	if (reqh < 1)
		reqh = 1;

	*width = reqw;
	*height = reqh;

	return 0;
}

int lv_madspin_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	GLfloat ratio;
	
	visual_video_set_dimension (video, width, height);

	glViewport (0, 0, width, height);

	return 0;
}

int lv_madspin_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_madspin_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
		}
	}

	return 0;
}

VisPalette *lv_madspin_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int lv_madspin_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	madspin_sound (plugin->private, audio);
	madspin_draw (plugin->private, video);

	return 0;
}

static int madspin_load_textures (MadspinPrivate *priv)
{
	char file1[512] = STAR_DIR;
	char file2[512] = STAR_DIR;
	VisVideo *textureimage;

	glGenTextures (2, &priv->texture[0]);

	textureimage = visual_bitmap_load_new_video (strcat (file1, "star1.bmp"));
	glBindTexture (GL_TEXTURE_2D, priv->texture[0]);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, 3, textureimage->width, textureimage->height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, textureimage->screenbuffer);

	visual_video_free_with_buffer (textureimage);

	textureimage = visual_bitmap_load_new_video (strcat (file2, "star2.bmp"));
	glBindTexture (GL_TEXTURE_2D, priv->texture[1]);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D (GL_TEXTURE_2D, 0, 3, textureimage->width, textureimage->height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, textureimage->screenbuffer);

	visual_video_free_with_buffer (textureimage);

	return 0;
}

static int madspin_sound (MadspinPrivate *priv, VisAudio *audio)
{
	int i;

	/* Make our data from the freq data */
	for (i = 0; i < 256; i++) {
		priv->gdata[i] = (float) audio->freq[0][i] / 2000.0;

		if (priv->gdata[i] > 1.0)
			priv->gdata[i] = 1.0;
		if (i > 80)
			priv->gdata[i] = priv->gdata[i] * (2 + (i /256));
	}

	return 0;
}

static int madspin_draw (MadspinPrivate *priv, VisVideo *video)
{
	double csab, csapb;
	double snab, snapb;
	double sa, ca, s5;
	double s1r, s1g, s1b, s1a;
	double s2r, s2g, s2b, s2a;
	float point;
	int line;
	int trail = 40;
	float b;
	float aa, a;
	double x, y, z;
	int i;
	int ampl = 200;
	float elapsed_time;

	struct timeval tv_now;

	gettimeofday (&priv->tv_past, NULL);

	for (i = 1; i < 50; i++)
		priv->total += priv->gdata[i];

	if (priv->total > 2.5)
		priv->total = 2.5;

	priv->total /= 2.5f;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc (GL_SRC_ALPHA,GL_ONE);
	glClearColor (0.0f, 0.0f, 0.0f, 0.0f);

	for (line = priv->maxlines; line > 0; line--) {
		for (point = 0; point <= NUM_STARS; point++) {
			b = 1.5f + point / 33.33333f;
			aa = trail - line * 1.0f;
			a = (aa + priv->frame) / 33.33333f;

			csab = cos (a / b);
			csapb = cos (a + b);
			snab = sin (a / b);
			snapb = sin (a + b);

			sa = sin (a * .1f);
			ca = cos (a * .1f);
			s5 = sin (a * .05f);

			x = sa * snapb * ampl * csab + ca * (csapb * ampl * b * csab + s5 * (ampl * (csab + 2 *snapb)));
			y = sa * csapb * ampl * csab + ca * (csapb * ampl * b * snab + s5 * (ampl * (csab + 2 *csapb)));
			z = ca * csapb * ampl * csab + ca * (snapb * ampl * b * snab + s5 * (ampl * (snab + 2 *snapb)));

			x /= 255.0f;
			y /= 255.0f;
			z /= 255.0f;

			glPushMatrix ();

			glTranslatef ((float) x, (float) y, (float) z);
			glBindTexture (GL_TEXTURE_2D, priv->texture[0]);

			s1r = ((point * 1.0f) / NUM_STARS);
			s1g = (NUM_STARS - point) / (NUM_STARS * 1.0f);
			s1b = ((point * 1.0f) / NUM_STARS) * 0.5f;
			s1a = ((priv->gdata[(int) (point / NUM_STARS * 220)] + (priv->total / 200.0f)) / 4.0f);

			s2r = sin (priv->frame / 400.0f);
			s2g = cos (priv->frame / 200.0f);
			s2b = cos (priv->frame / 300.0f);
			s2a = (priv->gdata[(int) (point / NUM_STARS * 220)] / 2.0f );

			if (s1a > 0.008f) {
				glBegin (GL_TRIANGLE_STRIP);
				glColor4f ((float) s1r, (float) s1g, (float) s1b, (float) s1a);
				priv->texsize = (((priv->gdata[(int) (point / NUM_STARS * 220)]))
						/ (2048.01f - (point * 4.0f))) *
						(((point - NUM_STARS) / (-NUM_STARS)) * 18.0f) + 0.15f;

				/* Top Right */
				glTexCoord2d (1, 1);
				glVertex3f (priv->texsize, priv->texsize, (float) z);
				/* Top Left */
				glTexCoord2d (0, 1);
				glVertex3f (-priv->texsize, priv->texsize, (float) z);
				/* Bottom Right */
				glTexCoord2d (1, 0);
				glVertex3f (priv->texsize, -priv->texsize, (float) z);
				/* Bottom Left */
				glTexCoord2d (0, 0);
				glVertex3f (-priv->texsize, -priv->texsize, (float) z);

				glEnd ();
			}

			glBindTexture (GL_TEXTURE_2D, priv->texture[1]);
			glRotatef (priv->frame + point, 0.0f, 0.0f, 1.0f);

			if (s2a > 0.005f) {
				glBegin (GL_TRIANGLE_STRIP);
				glColor4f ((float) s2r, (float) s2g, (float) s2b, (float) s2a);
				priv->texsize = (((priv->gdata[(int) (point / NUM_STARS * 220)]))
						/ (2048.01f - (point * 4.0f))) *
						(((point - NUM_STARS) / (-NUM_STARS)) * 18.0f) + 0.35f;

				priv->texsize *= ((rand() % 100) / 100.0f) * 2.0f;

				/* Top Right */
				glTexCoord2d (1, 1);
				glVertex3f (priv->texsize, priv->texsize, (float) z);
				/* Top Left */
				glTexCoord2d (0, 1);
				glVertex3f (-priv->texsize, priv->texsize, (float) z);
				/* Bottom Right */
				glTexCoord2d (1, 0);
				glVertex3f (priv->texsize, -priv->texsize, (float) z);
				/* Bottom Left */
				glTexCoord2d (0, 0);
				glVertex3f (-priv->texsize, -priv->texsize, (float) z);
				
				glEnd ();
			}

			/* Move back to main position */
			glPopMatrix ();
		}
	}

	glLoadIdentity ();

	gettimeofday (&tv_now, NULL);

	elapsed_time = ((float) tv_now.tv_usec - (float) priv->tv_past.tv_usec) / 1000000;
	if (elapsed_time < 0)
		elapsed_time = 0;

	priv->frame += elapsed_time * SPEED;

	return 0;
}

