#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <libvisual/libvisual.h>

#define NUM_BANDS	16

typedef struct {
	float rot;
} DNAPrivate;

int lv_dna_init (VisActorPlugin *plugin);
int lv_dna_cleanup (VisActorPlugin *plugin);
int lv_dna_requisition (VisActorPlugin *plugin, int *width, int *height);
int lv_dna_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height);
int lv_dna_events (VisActorPlugin *plugin, VisEventQueue *events);
VisPalette *lv_dna_palette (VisActorPlugin *plugin);
int lv_dna_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio);

/*static void draw_bars (DNAPrivate *priv);
static void draw_rectangle (DNAPrivate *priv, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2);
static void draw_bar (DNAPrivate *priv, GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue);*/

/* Main plugin stuff */
LVPlugin *get_plugin_info (VisPluginRef *ref)
{
	LVPlugin *plugin;
	VisActorPlugin *lv_dna;
	DNAPrivate *priv;

	plugin = visual_plugin_new ();
	lv_dna = visual_plugin_actor_new ();

	lv_dna->name	= "lv_dna";
	lv_dna->info = visual_plugin_info_new ("libvisual DNA helix animation",
			"Written by: Dennis Smit <ds@nerds-incorporated.org>, after some begging by Ronald Bultje",
			"0.1",
			"The Libvisual DNA helix animation plugin",
			"This plugin shows an openGL DNA twisting unfolding and folding on the music.");

	lv_dna->init =			lv_dna_init;
	lv_dna->cleanup =		lv_dna_cleanup;
	lv_dna->requisition =		lv_dna_requisition;
	lv_dna->events =		lv_dna_events;
	lv_dna->palette =		lv_dna_palette;
	lv_dna->render =		lv_dna_render;
	
	lv_dna->depth = VISUAL_VIDEO_DEPTH_GL;

	priv = malloc (sizeof (DNAPrivate));
	memset (priv, 0, sizeof (DNAPrivate));

	lv_dna->private = priv;

	plugin->type = VISUAL_PLUGIN_TYPE_ACTOR;
	plugin->plugin.actorplugin = lv_dna;

	return plugin;
}

int lv_dna_init (VisActorPlugin *plugin)
{
	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();

	glFrustum (-1, 1, -1, 1, 1.5, 10);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glDepthFunc (GL_LEQUAL);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClearDepth (1.0);

	return 0;
}

int lv_dna_cleanup (VisActorPlugin *plugin)
{
	DNAPrivate *priv = plugin->private;

	free (priv);

	return 0;
}

int lv_dna_requisition (VisActorPlugin *plugin, int *width, int *height)
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

int lv_dna_dimension (VisActorPlugin *plugin, VisVideo *video, int width, int height)
{
	GLfloat ratio;
	
	visual_video_set_dimension (video, width, height);

	ratio = (GLfloat) width / (GLfloat) height;

	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (45.0, ratio, 0.1, 100.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	return 0;
}

int lv_dna_events (VisActorPlugin *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_dna_dimension (plugin, ev.resize.video,
						ev.resize.width, ev.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

VisPalette *lv_dna_palette (VisActorPlugin *plugin)
{
	return NULL;
}

int lv_dna_render (VisActorPlugin *plugin, VisVideo *video, VisAudio *audio)
{
	DNAPrivate *priv = plugin->private;
	float res;
	float sinr = 0;
	float height = -1.0;
	int i;
	
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	glTranslatef (0.0, 0.0, -3.0);
	glRotatef (priv->rot, 1.0, 0.0, 0.0);

	priv->rot += 0.1;

	if (priv->rot > 360)
		priv->rot = 0;

	glColor3f (1.0, 1.0, 1.0);
	
	for (i = 0; i < 10; i++) {

		res = sin (sinr);
		sinr += 0.4;

		glBegin (GL_QUADS);
		glVertex3f (-0.5, height, +res);
		glVertex3f (0.5, height, -res);
		glVertex3f (0.5, height + 0.1, -res);
		glVertex3f (-0.5, height + 0.1, +res);
		glEnd ();

		height += 0.2;
	}

	return 0;
}

