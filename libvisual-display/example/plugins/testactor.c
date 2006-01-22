/* Libvisual-display testactor - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005, 2006 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: testactor.c,v 1.10 2006-01-22 13:29:14 synap Exp $ testactor.c,v 1.8 2005/02/13 17:36:15 vitalyvb Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include <GL/gl.h>
#include <GL/glu.h>

#include <libvisual/libvisual.h>
#include "lvdisplay/lv_display.h"

#define NUM_BANDS	16

typedef struct {
	float rot;
	Lvd *v;
	LvdDContext *myctx1;
	int tex;
	int w, h;
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

		.plugname = "testactor",
		.name = "libvisual two DNA helix animations",
		.author = "Vitaly V. Bursov. Based on the work of Dennis Smit <ds@nerds-incorporated.org>, after some begging by Ronald Bultje",
		.version = "0.1",
		.about = "The Libvisual DNA helix animation plugin",
		.help = "This plugin shows an two openGL DNAs twisting unfolding and folding on the music.",

		.init = lv_dna_init,
		.cleanup = lv_dna_cleanup,
		.events = lv_dna_events,

		.plugin = (void *) &actor[0]
	}};

	*count = sizeof (info) / sizeof (*info);
	
	return info;
}

int lv_dna_init (VisPluginData *plugin)
{
	int params[20];
	int i;
	DNAPrivate *priv;
	Lvd *v;
	LvdDContext *ctxorig;
	void *envobj;

	envobj = visual_plugin_environ_get (plugin, VISUAL_PLUGIN_ENVIRON_TYPE_LVD);

	if (envobj == NULL){
		return -1;
	}

	priv = visual_mem_new0 (DNAPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();

	glFrustum (-1, 1, -1, 1, 1.5, 10);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);   

	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glClearDepth (1.0);

	glGenTextures(1, &priv->tex);

	v = ((LvdPluginEnvironData*) envobj)->lvd;
	fprintf(stderr,"got lvd: %p\n",v);
	priv->v=v;

	i=0;
	params[i++] = LVD_SET_WIDTH;  params[i++] = 64;
	params[i++] = LVD_SET_HEIGHT; params[i++] = 64;
	params[i++] = LVD_SET_RENDERTARGET; params[i++] = LVD_SET_OFFSCREEN;
	params[i++] = LVD_SET_DONE;

	priv->myctx1 = lvdisplay_context_create_special(priv->v, params);
	fprintf(stderr,"creatd context: %p\n",priv->myctx1);

	priv->w = 64;
	priv->h = 64;

	fprintf(stderr,"new ctx setup...\n");

	lvdisplay_context_push_activate(v, priv->myctx1);

	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();

	glFrustum (1, -1, -1, 1, 1.5, 10);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glDepthFunc (GL_LEQUAL);
	glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glClearColor (0.5, 0.0, 0.5, 0.0);
	glClearDepth (1.0);


	lvdisplay_context_pop(v);
	return 0;
}

int lv_dna_cleanup (VisPluginData *plugin)
{
	DNAPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	glDeleteTextures(1, &priv->tex);

	lvdisplay_context_delete(priv->v, priv->myctx1);

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

static void do_render(DNAPrivate *priv, int f)
{
	float res;
	float sinr = 0;
	float height = -1.0;
	int i;

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity ();

	glTranslatef (0.0, 0.0, -3.0);
	if (f)
		glRotatef (priv->rot, 0.0, 1.0, 0.0);
	else
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
}

int lv_dna_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	DNAPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	Lvd *v = priv->v;
	LvdDContext *ctxorig;

	v=priv->v;

	do_render(priv, 0);

	lvdisplay_context_push_activate(v, priv->myctx1);

		do_render(priv, 1);

		// copy screen to texture
		glBindTexture(GL_TEXTURE_2D, priv->tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
				0, 0, priv->w, priv->h, 0);

		glBindTexture(GL_TEXTURE_2D, 0);

	lvdisplay_context_pop(v);

	glBindTexture(GL_TEXTURE_2D, priv->tex);

	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0.0f,  0.0f);
	glVertex2f(  0.0f,  0.0f);
	glTexCoord2f(1.0f,  0.0f);
	glVertex2f(  1.0f,  0.0f);
	glTexCoord2f(0.0f,  1.0f);
	glVertex2f(  0.0f,  1.0f);
	glTexCoord2f(1.0f,  1.0f);
	glVertex2f(  1.0f,  1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	return 0;
}

