/* Libvisual-display testactor - The display library for libvisual.
 * 
 * Copyright (C) 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: testactor2.c,v 1.1 2005-02-13 17:36:15 vitalyvb Exp $
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
	int first;
} DNAPrivate;

int lv_dna_init (VisPluginData *plugin);
int lv_dna_cleanup (VisPluginData *plugin);
int lv_dna_requisition (VisPluginData *plugin, int *width, int *height);
int lv_dna_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lv_dna_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lv_dna_palette (VisPluginData *plugin);
int lv_dna_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);


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

		.plugname = "testactor2",
		.name = "Blurred GL oscilloscope.",
		.author = "Author: Vitaly V. Bursov. Based on the work of Dennis Smit <ds@nerds-incorporated.org>, after some begging by Ronald Bultje",
		.version = "0.1",
		.about = "Blurred GL oscilloscope.",
		.help = "This plugin shows an GL blurred oscilloscope",

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
	float alight[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
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


		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glViewport( 0, 0, 256, 256 );

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0, 1.0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);


	glGenTextures(1, &priv->tex);


	/****************/
	v = ((LvdPluginEnvironData*) envobj)->lvd;
	//fprintf(stderr,"got lvd: %p\n",v);
	priv->v=v;

	i=0;
	params[i++] = LVD_SET_WIDTH;  params[i++] = 256;
	params[i++] = LVD_SET_HEIGHT; params[i++] = 256;
	params[i++] = LVD_SET_RENDERTARGET; params[i++] = LVD_SET_OFFSCREEN;
	params[i++] = LVD_SET_DONE;

	priv->myctx1 = lvdisplay_context_create_special(priv->v, params);
	//fprintf(stderr,"creatd context: %p\n",priv->myctx1);

	priv->w = 256;
	priv->h = 256;

	lvdisplay_context_push_activate(v, priv->myctx1);


		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_POINT_SMOOTH);

		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glViewport( 0, 0, priv->w, priv->h );

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0, 1.0);

		glEnable(GL_LIGHTING);
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, alight);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	lvdisplay_context_pop(v);

	priv->first = 1;

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

	glViewport (0, 0, (GLsizei) width, (GLsizei) height);

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

static void do_render(DNAPrivate *priv, VisAudio *audio)
{
	float matosc[] = {0.1, 0.7, 0.8, 1.0};
	int i;
	float p = 0.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, matosc);
	glBegin(GL_LINE_STRIP);

	glColor3f(0.9,0.8,0.0);
	for (i=0; i<512; i+=1, p += 1.0/512.0){
		glVertex2f( p, 0.5+audio->pcm[2][i]/32768.0);
	}

	glEnd();
}

int lv_dna_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	float decaycolor[] = {0.0, 0.0, 0.0, 252.0/256.0};
	DNAPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	Lvd *v = priv->v;
	LvdDContext *ctxorig;

	v=priv->v;
	lvdisplay_context_push_activate(v, priv->myctx1);
		glClear(GL_COLOR_BUFFER_BIT);

		/* draw previous frame */
		if (!priv->first){
			const float t = 0.005;
			glBindTexture(GL_TEXTURE_2D, priv->tex);
			glEnable(GL_TEXTURE_2D);
			glBegin(GL_TRIANGLE_STRIP);
			glTexCoord2f(0.0f+t,  0.0f+t);	glVertex2f(  0.0f,  0.0f);
			glTexCoord2f(1.0f-t,  0.0f+t);	glVertex2f(  1.0f,  0.0f);
			glTexCoord2f(0.0f+t,  1.0f-t);	glVertex2f(  0.0f,  1.0f);
			glTexCoord2f(1.0f-t,  1.0f-t);	glVertex2f(  1.0f,  1.0f);
			glEnd();
			glDisable(GL_TEXTURE_2D);


			/* decay */
			glEnable(GL_BLEND);

			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, decaycolor);
			glBegin(GL_TRIANGLE_STRIP);
			glVertex2f(  0.0f,  0.0f);
			glVertex2f(  1.0f,  0.0f);
			glVertex2f(  0.0f,  1.0f);
			glVertex2f(  1.0f,  1.0f);
			glEnd();

			glDisable(GL_BLEND);
		} else
			priv->first = 0;



		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_DST_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		do_render(priv, audio);
		glDisable(GL_BLEND);

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
	glTexCoord2f(0.0f,  0.0f);	glVertex2f(  0.0f,  0.0f);
	glTexCoord2f(1.0f,  0.0f);	glVertex2f(  1.0f,  0.0f);
	glTexCoord2f(0.0f,  1.0f);	glVertex2f(  0.0f,  1.0f);
	glTexCoord2f(1.0f,  1.0f);	glVertex2f(  1.0f,  1.0f);
	glEnd();
	glDisable(GL_TEXTURE_2D);

	return 0;
}

