/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2000, 2001 Remi Arquier <arquier@crans.org>
 *
 * Authors: Remi Arquier <arquier@crans.org>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "gettext.h"

#include "def.h"
#include "struct.h"
#include "distorsion.h"
#include "analyser.h"
#include "analyser_struct.h"
#include "renderer.h"
#include "pal.h"
#include "draw_low_level.h"
#include "jess.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

static int         act_jess_init        (VisPluginData *plugin);
static void        act_jess_cleanup     (VisPluginData *plugin);
static void        act_jess_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_jess_resize      (VisPluginData *plugin, int width, int height);
static int         act_jess_events      (VisPluginData *plugin, VisEventQueue *events);
static void        act_jess_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
static VisPalette *act_jess_palette     (VisPluginData *plugin);

static void jess_init (JessPrivate *priv);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_jess_requisition,
		.palette     = act_jess_palette,
		.render      = act_jess_render,
		.vidoptions.depth =
			VISUAL_VIDEO_DEPTH_8BIT |
			VISUAL_VIDEO_DEPTH_32BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "jess",
		.name     = "jess plugin",
		.author   = N_("Original by: Remi Arquier <arquier@crans.org>, Port by: Dennis Smit <ds@nerds-incorporated.org>"),
		.version  = "0.1",
		.about    = N_("Jess visual plugin"),
		.help     = N_("This is the libvisual plugin for the jess visual"),
		.license  = VISUAL_PLUGIN_LICENSE_GPL,

		.init    = act_jess_init,
		.cleanup = act_jess_cleanup,
		.events  = act_jess_events,
		.plugin  = &actor
	};

	return &info;
}

static int act_jess_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	JessPrivate *priv = visual_mem_new0 (JessPrivate, 1);

	visual_plugin_set_private (plugin, priv);

	priv->rcontext = visual_plugin_get_random_context (plugin);

	priv->conteur.burn_mode = 4;
	priv->conteur.draw_mode = 4;
	priv->conteur.blur_mode = 3;
	priv->video = 8;
	priv->conteur.term_display = OUI;
	priv->resx = 320;
	priv->resy = 200;

	priv->conteur.courbe = 0;
	priv->conteur.angle = 0;
	priv->conteur.angle2 = 0;
	priv->conteur.k1 = 0;
	priv->conteur.k2 = 0;
	priv->conteur.k3 = 10000;

	priv->conteur.fps = 40;

	priv->conteur.mix_reprise = 1000;
	priv->conteur.last_flash = 1000;
	priv->conteur.burn_mode = 2;
	priv->conteur.draw_mode = 5;
	priv->conteur.v_angle2 = 1;
	priv->conteur.general = 0;
	priv->conteur.blur_mode = 3;
	priv->conteur.freeze = 0;
	priv->conteur.freeze_mode = 0;

	priv->lys.E_moyen = 0;
	priv->lys.dEdt_moyen = 0;

	priv->jess_pal = visual_palette_new (256);

	priv->pcm_data1 = visual_buffer_new_wrap_data (priv->pcm_data[0], 512 * sizeof (float), FALSE);
	priv->pcm_data2 = visual_buffer_new_wrap_data (priv->pcm_data[1], 512 * sizeof (float), FALSE);

	start_ticks (priv);

	return TRUE;
}

static void act_jess_cleanup (VisPluginData *plugin)
{
	JessPrivate *priv = visual_plugin_get_private (plugin);

	if (priv->big_ball != NULL)
		visual_mem_free (priv->big_ball);

    int i;

	for (i = 0; i < BIG_BALL_SIZE; i++)
	{
		if (priv->big_ball_scale[i] != NULL)
			visual_mem_free (priv->big_ball_scale[i]);
	}

	if (priv->table1 != NULL)
		visual_mem_free (priv->table1);

	if (priv->table2 != NULL)
		visual_mem_free (priv->table2);

	if (priv->table3 != NULL)
		visual_mem_free (priv->table3);

	if (priv->table4 != NULL)
		visual_mem_free (priv->table4);

	if (priv->buffer != NULL)
		visual_mem_free (priv->buffer);

	visual_buffer_unref (priv->pcm_data1);
	visual_buffer_unref (priv->pcm_data2);

	visual_palette_free (priv->jess_pal);

	visual_mem_free (priv);
}

static void act_jess_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw = *width;
	int reqh = *height;

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
}

static void act_jess_resize (VisPluginData *plugin, int width, int height)
{
	JessPrivate *priv = visual_plugin_get_private (plugin);

	priv->resx = width;
	priv->resy = height;

	if (priv->table1 != NULL)
		visual_mem_free (priv->table1);

	if (priv->table2 != NULL)
		visual_mem_free (priv->table2);

	if (priv->table3 != NULL)
		visual_mem_free (priv->table3);

	if (priv->table4 != NULL)
		visual_mem_free (priv->table4);

	if (priv->buffer != NULL)
		visual_mem_free (priv->buffer);

	ball_init (priv);
	jess_init (priv);
}

static int act_jess_events (VisPluginData *plugin, VisEventQueue *events)
{
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_jess_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;
			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *act_jess_palette (VisPluginData *plugin)
{
	JessPrivate *priv = visual_plugin_get_private (plugin);

	return priv->jess_pal;
}

static void act_jess_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	JessPrivate *priv = visual_plugin_get_private (plugin);

	VisBuffer* fbuf[2];
	float freq[2][256];
	short freqdata[2][256];
	int i, depth;

	visual_audio_get_sample (audio, priv->pcm_data1, VISUAL_AUDIO_CHANNEL_LEFT);
	visual_audio_get_sample (audio, priv->pcm_data2, VISUAL_AUDIO_CHANNEL_RIGHT);

	fbuf[0] = visual_buffer_new_wrap_data (freq[0], sizeof (freq[0]), FALSE);
	fbuf[1] = visual_buffer_new_wrap_data (freq[1], sizeof (freq[1]), FALSE);

	visual_audio_get_spectrum_for_sample (fbuf[0], priv->pcm_data1, FALSE);
	visual_audio_get_spectrum_for_sample (fbuf[1], priv->pcm_data2, FALSE);

	visual_buffer_unref (fbuf[0]);
	visual_buffer_unref (fbuf[1]);

	for (i = 0;i < 256; i++) {
		freqdata[0][i] = freq[0][i] * 32768;
		freqdata[1][i] = freq[1][i] * 32768;
	}

	priv->lys.conteur[ZERO]++;
	priv->lys.conteur[ONE]++;

	priv->lys.dt = time_last(priv, TWO, OUI);

	spectre_moyen(priv, freqdata);
	C_E_moyen(priv, freqdata);
	C_dEdt_moyen(priv);
	C_dEdt(priv);

	priv->pitch = visual_video_get_pitch (video);

    depth = priv->video;
	priv->video = visual_video_depth_bpp (visual_video_get_depth (video));
    if(depth != priv->video)
    {
        free(priv->buffer);
        if(priv->video == 8)
            priv->buffer = (uint8_t *)malloc(priv->resx * priv->resy);
        else
            priv->buffer = (uint8_t *)malloc(priv->resx * priv->resy * 4);
    }

	priv->pixel = ((uint8_t *) visual_video_get_pixels (video));

	renderer (priv);
}

static void jess_init (JessPrivate *priv)
{
	priv->xres2 = priv->resx / 2;
	priv->yres2 = priv->resy / 2;

	priv->conteur.fullscreen = 0;
	priv->conteur.blur_mode = 1;

	priv->table1 = (uint32_t *) visual_mem_malloc0 (priv->resx * priv->resy * sizeof (int));
	priv->table2 = (uint32_t *) visual_mem_malloc0 (priv->resx * priv->resy * sizeof (int));
	priv->table3 = (uint32_t *) visual_mem_malloc0 (priv->resx * priv->resy * sizeof (int));
	priv->table4 = (uint32_t *) visual_mem_malloc0 (priv->resx * priv->resy * sizeof (int));

	if (priv->video == 8)
		priv->buffer = (uint8_t *) visual_mem_malloc0 (priv->resx * priv->resy);
	else
		priv->buffer = (uint8_t *) visual_mem_malloc0 (priv->resx * priv->resy * 4);

	create_tables(priv);
}

