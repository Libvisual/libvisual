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

int lv_dna_init (VisPluginData *plugin);
int lv_dna_cleanup (VisPluginData *plugin);
int lv_dna_requisition (VisPluginData *plugin, int *width, int *height);
int lv_dna_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_dna_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_dna_palette (VisPluginData *plugin);
int lv_dna_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

/*static void draw_bars (DNAPrivate *priv);
static void draw_rectangle (DNAPrivate *priv, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2);
static void draw_bar (DNAPrivate *priv, GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue);*/

/* Main plugin stuff */
const VisPluginInfo *get_plugin_info (int *count)
{
	static const VisActorPlugin actor[] = {{
		.requisition = lv_dna_requisition,
		.palette = lv_dna_palette,
		.render = lv_dna_render,
		.depth = VISUAL_VIDEO_DEPTH_GL
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_dna",
		.name = "libvisual DNA helix animation",
		.author = "Written by: Dennis Smit <ds@nerds-incorporated.org>, after some begging by Ronald Bultje",
		.version = "0.1",
		.about = "The Libvisual DNA helix animation plugin",
		.help = "This plugin shows an openGL DNA twisting unfolding and folding on the music.",

		.init = lv_dna_init,
		.cleanup = lv_dna_cleanup,
		.events = lv_dna_events,

		.plugin = VISUAL_OBJECT (&actor[0])
	}};

	*count = sizeof (info) / sizeof (*info);
	
	return info;
}

int lv_dna_init (VisPluginData *plugin)
{
	DNAPrivate *priv;

	priv = visual_mem_new0 (DNAPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

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

int lv_dna_cleanup (VisPluginData *plugin)
{
	DNAPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_mem_free (priv);

	return 0;
}

int lv_dna_requisition (VisPluginData *plugin, int *width, int *height)
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

int lv_dna_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
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

int lv_dna_events (VisPluginData *plugin, VisEventQueue *events)
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

VisPalette *lv_dna_palette (VisPluginData *plugin)
{
	return NULL;
}

int lv_dna_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DNAPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
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

