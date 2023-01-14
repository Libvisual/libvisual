/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and
 *            4Front Technologies
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <math.h>

#ifdef USE_OPENGL_ES
#include <GLES/gl.h>
#include "common/GL/glu.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#define BARS	16

static int xranges[] = {0, 1, 2, 3, 5, 7, 10, 14, 20, 28, 40, 54, 74, 101, 137, 187, 255};

typedef struct {
	GLfloat y_angle;
	GLfloat y_initial_angle;
	GLfloat y_speed;  // degrees per second
	GLfloat x_angle;
	GLfloat x_initial_angle;
	GLfloat x_speed;  // degrees per second
	GLfloat z_angle;
	GLfloat z_initial_angle;
	GLfloat z_speed;  // degrees per second
	GLfloat heights[16][16];

	int transparent;

	VisTimer * started_at;
} GLtestPrivate;

static int         lv_gltest_init        (VisPluginData *plugin);
static void        lv_gltest_cleanup     (VisPluginData *plugin);
static void        lv_gltest_requisition (VisPluginData *plugin, int *width, int *height);
static void        lv_gltest_resize      (VisPluginData *plugin, int width, int height);
static int         lv_gltest_events      (VisPluginData *plugin, VisEventQueue *events);
static void        lv_gltest_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *lv_gltest_palette     (VisPluginData *plugin);

static void draw_bars (GLtestPrivate *priv);
static void draw_rectangle (GLtestPrivate *priv, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2);
static void draw_bar (GLtestPrivate *priv, GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue);
static void draw_bars (GLtestPrivate *priv);

/* Main plugin stuff */
const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = lv_gltest_requisition,
		.palette     = lv_gltest_palette,
		.render      = lv_gltest_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_GL
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "lv_gltest",
		.name     = "libvisual GL analyzer",
		.author   = N_("Original by: Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version  = "0.1",
		.about    = N_("Libvisual GL analyzer plugin"),
		.help     = N_("This plugin shows an openGL bar analyzer like the xmms one"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init    = lv_gltest_init,
		.cleanup = lv_gltest_cleanup,
		.events  = lv_gltest_events,
		.plugin  = &actor
	};

	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RED_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_GREEN_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_BLUE_SIZE, 5);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DEPTH_SIZE, 16);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER, 1);
	VISUAL_VIDEO_ATTR_OPTIONS_GL_ENTRY(actor.vidoptions, VISUAL_GL_ATTRIBUTE_RGBA, 1);

	return &info;
}

static int lv_gltest_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	GLtestPrivate *priv = visual_mem_new0 (GLtestPrivate, 1);
	visual_plugin_set_private (plugin, priv);

	VisParamList *params = visual_plugin_get_params (plugin);
	visual_param_list_add_many (params,
                                visual_param_new_bool ("transparent_bars",
                                                       N_("Transparent bars"),
                                                       TRUE,
                                                       NULL),
                                NULL);

	/* GL setting up the rest! */
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode (GL_PROJECTION);

	glLoadIdentity ();

#ifdef USE_OPENGL_ES
	glFrustumf (-1, 1, -1, 1, 1.5, 10);
#else
	glFrustum (-1, 1, -1, 1, 1.5, 10);
#endif

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	glBlendFunc (GL_SRC_ALPHA,GL_ONE);

	int x, y;

	for (x = 0; x < 16; x++) {
		for (y = 0; y < 16; y++) {
			priv->heights[y][x] = 0.0;
		}
	}

	priv->x_speed = 0.0f;
	priv->y_speed = 360.0f / 13.0f;
	priv->z_speed = 0.0f;
	priv->x_initial_angle = 20.0f;
	priv->y_initial_angle = 45.0f;
	priv->z_initial_angle = 0.0f;

	priv->started_at = visual_timer_new();
	visual_return_val_if_fail( priv->started_at != NULL, -1 );

	visual_timer_start(priv->started_at);

	return TRUE;
}

static void lv_gltest_cleanup (VisPluginData *plugin)
{
	GLtestPrivate *priv = visual_plugin_get_private (plugin);

	if (priv) {
	    visual_timer_free (priv->started_at);
	}

	visual_mem_free (priv);
}

static void lv_gltest_requisition (VisPluginData *plugin, int *width, int *height)
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
}


static void lv_gltest_resize (VisPluginData *plugin, int width, int height)
{
	GLfloat ratio;

	ratio = (GLfloat) width / (GLfloat) height;

	glViewport (0, 0, (GLsizei) width, (GLsizei) height);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();

	gluPerspective (45.0, ratio, 0.1, 100.0);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();
}

static int lv_gltest_events (VisPluginData *plugin, VisEventQueue *events)
{
	GLtestPrivate *priv = visual_plugin_get_private (plugin);
	VisEvent ev;
	VisParam *param;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				lv_gltest_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_has_name (param, "transparent_bars")) {
					priv->transparent = visual_param_get_value_bool (param);

					if (priv->transparent == FALSE)
						glDisable (GL_BLEND);
					else
						glEnable (GL_BLEND);
				}

			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *lv_gltest_palette (VisPluginData *plugin)
{
	return NULL;
}

static void lv_gltest_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	GLtestPrivate *priv = visual_plugin_get_private (plugin);

	int i,c;
	int y;
	float ff;

	VisBuffer *pcm_buffer = visual_buffer_new_allocate (512 * sizeof (float));
	visual_audio_get_sample_mixed_simple (audio, pcm_buffer, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT);

	VisBuffer *freq_buffer = visual_buffer_new_allocate (256 * sizeof (float));
	visual_audio_get_spectrum_for_sample (freq_buffer, pcm_buffer, TRUE);

	visual_buffer_unref (pcm_buffer);

	float *freq = (float *) visual_buffer_get_data (freq_buffer);

	for (y = BARS - 1; y > 0; y--)
	{
		for(i = 0; i < BARS; i++)
		{
			priv->heights[y][i] = priv->heights[y - 1][i];
		}
	}

	for (i = 0; i < BARS; i++)
	{
		ff = 0;
		for (c = xranges[i]; c < xranges[i + 1]; c++)
		{
			if (freq[c] > ff)
				ff = freq[c];
		}

		priv->heights[0][i] = ff * 10;
	}

	const float seconds_elapsed = visual_timer_elapsed_msecs(priv->started_at) / 1000.0f;

	priv->x_angle = fmodf(priv->x_initial_angle + priv->x_speed * seconds_elapsed, 360.0);
	priv->y_angle = fmodf(priv->y_initial_angle + priv->y_speed * seconds_elapsed, 360.0);
	priv->z_angle = fmodf(priv->z_initial_angle + priv->z_speed * seconds_elapsed, 360.0);

	draw_bars (priv);

	visual_buffer_unref (freq_buffer);
}

/* Drawing stuff */
static void draw_rectangle (GLtestPrivate *priv, GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
    glEnableClientState(GL_VERTEX_ARRAY);
    if (y1 == y2) {

        const GLfloat vertices[] = {
            x1, y1, z1,
            x2, y1, z1,
            x2, y2, z2,

            x2, y2, z2,
            x1, y2, z2,
            x1, y1, z1,

        };
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    } else {
        const GLfloat vertices[] = {
            x1, y1, z1,
            x2, y1, z2,
            x2, y2, z2,

            x2, y2, z2,
            x1, y2, z1,
            x1, y1, z1,
        };
        glVertexPointer(3, GL_FLOAT, 0, vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glDisableClientState(GL_VERTEX_ARRAY);
}

static void draw_bar (GLtestPrivate *priv, GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue)
{
	GLfloat width = 0.1;

    glColor4f(red, green, blue, 1.0f);

	draw_rectangle (priv, x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
	draw_rectangle (priv, x_offset, 0, z_offset, x_offset + width, 0, z_offset + 0.1);

    glColor4f(0.5 * red, 0.5 * green, 0.5 * blue, 1.0f);
	draw_rectangle (priv, x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);
	draw_rectangle (priv, x_offset, 0.0, z_offset, x_offset + width, height, z_offset );

    glColor4f(0.25 * red, 0.25 * green, 0.25 * blue, 1.0f);
	draw_rectangle (priv, x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);
	draw_rectangle (priv, x_offset + width, 0.0, z_offset , x_offset + width, height, z_offset + 0.1);
}

static void draw_bars (GLtestPrivate *priv)
{
	int x,y;
	GLfloat x_offset, z_offset, r_base, b_base;

	glClearColor (0,0,0,0);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix ();
	glTranslatef (0.0,-0.5,-5.0);
	glRotatef (priv->x_angle,1.0,0.0,0.0);
	glRotatef (priv->y_angle,0.0,1.0,0.0);
	glRotatef (priv->z_angle,0.0,0.0,1.0);

	for (y = 0; y < 16; y++)
	{
		z_offset = -1.6 + ((15 - y) * 0.2);

		b_base = y * (1.0 / 15);
		r_base = 1.0 - b_base;

		for(x = 0; x < 16; x++)
		{
			x_offset = -1.6 + (x * 0.2);
			draw_bar (priv, x_offset, z_offset, priv->heights[y][x] * 0.2, r_base - (x * (r_base / 15.0)), x * (1.0 / 15), b_base);
		}
	}

	glPopMatrix ();
}
