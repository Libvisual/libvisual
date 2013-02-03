/***************************************************************************
 *   Copyright (C) 2005 by yodor                                           *
 *   yodor@developer.bg                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define NUM_BANDS 16
#define SCOPE_DEPTH 16

typedef struct {
	float cam_y;
	float rot_x;
	float step_z;
	float fdist_z;
	int nw; //=640;
	int nh; //=480;
	float heights[SCOPE_DEPTH][NUM_BANDS];
	float scale; //1.0
	GLuint CYLINDER;
	int dx;
	int catch;
	int dy;
} NastyfftPrivate;

static int         lv_nastyfft_init        (VisPluginData *plugin);
static void        lv_nastyfft_cleanup     (VisPluginData *plugin);
static void        lv_nastyfft_requisition (VisPluginData *plugin, int *width, int *height);
static void        lv_nastyfft_resize      (VisPluginData *plugin, int width, int height);
static int         lv_nastyfft_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_nastyfft_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_nastyfft_palette     (VisPluginData *plugin);

static int nastyfft_sound (NastyfftPrivate *priv, VisAudio *audio);
static int nastyfft_draw  (NastyfftPrivate *priv, VisVideo *video);

static void init_gl    (NastyfftPrivate *priv);
static void draw_scene (NastyfftPrivate *priv);
static void make_all   (NastyfftPrivate *priv);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_nastyfft_requisition,
		.palette     = lv_nastyfft_palette,
		.render      = lv_nastyfft_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_GL
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "nastyfft",
		.name     = N_("Libvisual NastyFFT plugin"),
		.author   = "yodor <yodor@developer.bg>",
		.version  = "0.5",
		.about    = N_("The Libvisual NastyFFT plugin"),
		.help     = N_("This plugin shows nasty fft visualization effect using OpenGL"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = lv_nastyfft_init,
		.cleanup  = lv_nastyfft_cleanup,
		.events   = lv_nastyfft_events,
		.plugin   = &actor
	};

	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RGBA, 1);

	return &info;
}

static int lv_nastyfft_init (VisPluginData *plugin)
{
	NastyfftPrivate *priv = visual_mem_new0 (NastyfftPrivate, 1);

	visual_plugin_set_private (plugin, priv);

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	priv->cam_y=-1.0f;
	priv->rot_x=5.0f;
	priv->step_z=1.0f;
	priv->fdist_z=-63.0f;
	priv->scale=1.0f;
	priv->dx=0;
	priv->dy=0;
	priv->catch=0;
	priv->CYLINDER=1;

	int x=0;
	int z=0;
	for (z=SCOPE_DEPTH-1;z>=0;z--)
	{
		for (x=0;x<NUM_BANDS;x++)
		{
			priv->heights[z][x]=0.0f;
		}
	}

	return TRUE;
}

static void lv_nastyfft_cleanup (VisPluginData *plugin)
{
	NastyfftPrivate *priv = visual_plugin_get_private (plugin);

	visual_mem_free (priv);
}

static void lv_nastyfft_requisition (VisPluginData *plugin, int *width, int *height)
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
}

static VisPalette *lv_nastyfft_palette (VisPluginData *plugin)
{
	return NULL;
}

static void lv_nastyfft_resize (VisPluginData *plugin, int width, int height)
{
	NastyfftPrivate *priv = visual_plugin_get_private (plugin);

	priv->nw = width;
	priv->nh = height;

	init_gl(priv);
}

static int lv_nastyfft_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	NastyfftPrivate *priv = visual_plugin_get_private (plugin);

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_nastyfft_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;
			case VISUAL_EVENT_MOUSEBUTTONUP:
				priv->catch=0;
				break;
			case VISUAL_EVENT_MOUSEBUTTONDOWN:

				switch (ev.event.mousebutton.button)
				{
					case 2:
						priv->catch=-2;
						break;
					case 1:
						priv->catch=-1;
						break;
					case 4:
						priv->step_z+=(GLfloat)0.1;
						break;
					case 5:
						priv->step_z-=(GLfloat)0.1;
						break;
				}
				if (priv->step_z<0)
					priv->step_z=(GLfloat)0;
				break;

			case VISUAL_EVENT_MOUSEMOTION :

				if (priv->catch==-1)
				{
				priv->dx+=ev.event.mousemotion.yrel;
				priv->dy+=ev.event.mousemotion.xrel;
				if (priv->dx>360)priv->dx=0;
				if (priv->dy>360)priv->dy=0;
				}
				else if (priv->catch==-2)
				{
					priv->fdist_z+=(GLfloat)ev.event.mousemotion.yrel;
				}
				break;
			default:
				break;
		}
	}

	return TRUE;
}

static void lv_nastyfft_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	NastyfftPrivate *priv = visual_plugin_get_private (plugin);

	nastyfft_sound (priv, audio);
	nastyfft_draw (priv, video);
}

static int nastyfft_sound (NastyfftPrivate *priv, VisAudio *audio)
{
	int i, c, y;
	GLfloat val;
	int xscale[] = { 0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255 };
	VisBuffer *buffer;
	VisBuffer *pcmb;

	float freq[256];
	unsigned short rfreq[256];
	float pcm[256];

	buffer = visual_buffer_new_wrap_data (freq, sizeof (freq), FALSE);
	pcmb   = visual_buffer_new_wrap_data (pcm, sizeof (pcm), FALSE);

	visual_audio_get_sample_mixed_simple (audio, pcmb, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	visual_audio_get_spectrum_for_sample (buffer, pcmb, TRUE);

    visual_buffer_unref (buffer);
    visual_buffer_unref (pcmb);

	for (i = 0; i < 256; i++)
		rfreq[i] = freq[i] * 320000.0;

	for(y = SCOPE_DEPTH-1; y > 0; y--) {
		for(i = 0; i < NUM_BANDS; i++)
			priv->heights[y][i] = priv->heights[y - 1][i];
	}

	for(i = 0; i < NUM_BANDS; i++) {
		for(c = xscale[i], y = 0; c < xscale[i + 1]; c++) {
			if(rfreq[c] > y)
				y = rfreq[c];
		}
		y >>= 7;
		if(y > 0)
			val = (log(y) * priv->scale);
		else
			val = 0;
		priv->heights[0][i] = val;
	}

	return 0;
}

static int nastyfft_draw (NastyfftPrivate *priv, VisVideo *video)
{
	draw_scene (priv);

	return 0;
}

static void init_gl(NastyfftPrivate *priv)
{
	const double defFOVY = 30.0; // field-of-view in degrees in up direction
	double aspect = priv->nw/priv->nh;
	// field-of-view
	double fov = (priv->nw < priv->nh) ? defFOVY : defFOVY/aspect;
	float nearDist = 0.1f;
	float farDist = 500.0f;

	glViewport(0, 0, priv->nw, priv->nh);

	make_all(priv);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fov, aspect, nearDist, farDist);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//GLfloat ambientMaterial[] = { 1.0, 1.0, 1.0, 1.0 };
	//GLfloat difuseMaterial[] = { .5, .5, .5, 1.0 };
	GLfloat mat_specular[] = { 0.2, 0.2, 0.2, 1.0 };

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable (GL_BLEND);

	//glShadeModel (GL_SMOOTH);
	//glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//glMaterialfv(GL_FRONT, GL_DIFFUSE, ambientMaterial);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);

	//glMaterialf(GL_FRONT, GL_SHININESS, 0.5);

	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glEnable(GL_CULL_FACE);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glEnable(GL_COLOR_MATERIAL);
	GLfloat light_position1[] = { 0, 10, 0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position1);
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

static void make_all(NastyfftPrivate *priv)
{
	//cylinder
	GLUquadric *obj = gluNewQuadric();
	priv->CYLINDER = glGenLists(1);
	glNewList(priv->CYLINDER, GL_COMPILE);

	glRotatef(-90.0f,1.0f,0.0f,0.0f);
	glRotatef(-90.0f,0.0f,0.0f,1.0f);
	gluCylinder(obj, 0.5f, 0.5f, 0.1f , 6,6);
	glRotatef(180,1,0,0);
	gluDisk(obj, 0, 0.5f ,6,6);
	glRotatef(-180,1,0,0);
	glTranslatef(0,0,0.1f);
	gluDisk(obj, 0, 0.5f ,6,6);

	glEndList();
	gluDeleteQuadric(obj);
}

static void draw_scene(NastyfftPrivate *priv)
{
	glClearColor( 0.13, 0.17, 0.32, 0.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glLoadIdentity();

	glTranslatef(-(NUM_BANDS/2.0f)+0.5f,priv->cam_y,priv->fdist_z);

	glRotatef(priv->rot_x+priv->dx,1,0,0);
	glRotatef(priv->dy,0,1,0);

	double alpha=1.0;
	double s = 0.0;
	double xx=0.0;

	int a,z;

	for (z=SCOPE_DEPTH-1;z>=0;z--)
	{

		xx=0.0;
		alpha=1-sqrt((double)z/(double)SCOPE_DEPTH);


		for (a=0;a<NUM_BANDS;a++)
		{
			s=priv->heights[z][a];

			if (s>10.0f)s=10.0f;
			if (s<0.1f)s=0.1f;

			double d = (double)a/(double)NUM_BANDS;

			glPushMatrix();
			glColor4d(d,0,1-d,alpha);

			glScaled(1,s*10,1);
			glCallList(priv->CYLINDER);

			glPopMatrix();

			glTranslated(1,0,0);
			xx+=1.0;
		}
		glTranslated(-xx,0,0);
		glTranslated(0,0,priv->step_z);
	}
}

