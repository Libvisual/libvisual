/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
#include <gdk/gdk.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

typedef struct {
	GdkPixbuf	*source;
	VisVideo	*target;
	char		*filename;
	int		 width;
	int		 height;

	/* Config flags, set through the param interface */
	int		 set_scaled;
	int		 aspect;
	int		 center;
	int		 set_size;
	int		 set_width;
	int		 set_height;
	int		 x_offset;
	int		 y_offset;
	int		 interpolate;
} PixbufPrivate;

static int         act_gdkpixbuf_init        (VisPluginData *plugin);
static void        act_gdkpixbuf_cleanup     (VisPluginData *plugin);
static void        act_gdkpixbuf_requisition (VisPluginData *plugin, int *width, int *height);
static void        act_gdkpixbuf_resize      (VisPluginData *plugin, int width, int height);
static int         act_gdkpixbuf_events      (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_gdkpixbuf_palette     (VisPluginData *plugin);
static void        act_gdkpixbuf_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

static int  act_gdkpixbuf_load_file    (PixbufPrivate *priv, const char *filename);
static void act_gdkpixbuf_update_image (PixbufPrivate *priv);

static VisVideo *visvideo_from_gdkpixbuf        (GdkPixbuf *src);
static VisVideo *visvideo_from_scaled_gdkpixbuf (GdkPixbuf *src, int width, int height, GdkInterpType interp);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_gdkpixbuf_requisition,
		.palette     = act_gdkpixbuf_palette,
		.render      = act_gdkpixbuf_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_24BIT
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "gdkpixbuf",
		.name     = "GdkPixbuf image loader",
		.author   = "Dennis Smit <ds@nerds-incorporated.org>",
		.version  = "0.0.1",
		.about    = N_("GdkPixbuf image loader for libvisual"),
		.help     = N_("This plugin can be used to show images"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = act_gdkpixbuf_init,
		.cleanup  = act_gdkpixbuf_cleanup,
		.events   = act_gdkpixbuf_events,
		.plugin   = &actor
	};

	return &info;
}

static int act_gdkpixbuf_init (VisPluginData *plugin)
{
#if GLIB_VERSION_CUR_STABLE < GLIB_VERSION_2_36
    /* Initialize GObjects, needed for GdkPixbuf if Glib < 2.36 */
    g_type_init ();
#endif

#if ENABLE_NLS
    bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

    PixbufPrivate *priv = visual_mem_new0 (PixbufPrivate, 1);
    visual_plugin_set_private (plugin, priv);

    VisParamList *params = visual_plugin_get_params (plugin);;
    visual_param_list_add_many (params,
                                visual_param_new_string  ("filename", N_("Input filename"),
                                                          "",
                                                          NULL),
                                visual_param_new_bool    ("scaled", N_("Scaled"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_bool    ("aspect", N_("Aspect"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_bool    ("center", N_("Centered"),
                                                          TRUE,
                                                          NULL),
                                visual_param_new_bool    ("set_size", N_("Set size"),
                                                          FALSE,
                                                          NULL),
                                visual_param_new_integer ("width", N_("Width"),
                                                          0,
                                                          visual_param_in_range_integer (0, INT_MAX)),
                                visual_param_new_integer ("height", N_("Height"),
                                                          0,
                                                          visual_param_in_range_integer (0, INT_MAX)),
                                visual_param_new_integer ("x", N_("x"),
                                                          0,
                                                          visual_param_in_range_integer (0, INT_MAX)),
                                visual_param_new_integer ("y", N_("y"),
                                                          0,
                                                          visual_param_in_range_integer (0, INT_MAX)),
                                visual_param_new_integer ("interpolate", N_("Interpolation method"),
                                                          0,
                                                          NULL),
                                NULL);

    priv->target = NULL;

    return TRUE;
}

static void act_gdkpixbuf_cleanup (VisPluginData *plugin)
{
	PixbufPrivate *priv = visual_plugin_get_private (plugin);

	visual_mem_free (priv->filename);

	if (priv->source) {
		g_object_unref (priv->source);
	}

	if (priv->target) {
		visual_video_unref (priv->target);
	}

	visual_mem_free (priv);
}

static void act_gdkpixbuf_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw = *width;
	int reqh = *height;

	if (reqw < 1)
		reqw = 1;

	if (reqh < 1)
		reqh = 1;

	*width = reqw;
	*height = reqh;
}

void act_gdkpixbuf_resize (VisPluginData *plugin, int width, int height)
{
	PixbufPrivate *priv = visual_plugin_get_private (plugin);

	priv->width  = width;
	priv->height = height;

	if (priv->source) {
		act_gdkpixbuf_update_image (priv);
	}
}

static int act_gdkpixbuf_events (VisPluginData *plugin, VisEventQueue *events)
{
	PixbufPrivate *priv = visual_plugin_get_private (plugin);
	VisParam *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_gdkpixbuf_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_has_name (param, "filename")) {
					visual_log (VISUAL_LOG_DEBUG, "New file to be loaded: %s",
							visual_param_get_value_string (param));

					act_gdkpixbuf_load_file (priv, visual_param_get_value_string (param));

				} else if (visual_param_has_name (param, "scaled")) {
					priv->set_scaled = visual_param_get_value_bool (param);

					act_gdkpixbuf_update_image (priv);

				} else if (visual_param_has_name (param, "aspect")) {
					priv->aspect = visual_param_get_value_bool (param);

					act_gdkpixbuf_update_image (priv);

				} else if (visual_param_has_name (param, "center")) {
					priv->center = visual_param_get_value_bool (param);

				} else if (visual_param_has_name (param, "set size")) {
					priv->set_size = visual_param_get_value_bool (param);

					act_gdkpixbuf_update_image (priv);

				} else if (visual_param_has_name (param, "width")) {
					priv->set_width = visual_param_get_value_integer (param);

					act_gdkpixbuf_update_image (priv);

				} else if (visual_param_has_name (param, "height")) {
					priv->set_height = visual_param_get_value_integer (param);

					act_gdkpixbuf_update_image (priv);

				} else if (visual_param_has_name (param, "x")) {
					priv->x_offset = visual_param_get_value_integer (param);

				} else if (visual_param_has_name (param, "y")) {
					priv->y_offset = visual_param_get_value_integer (param);

				} else if (visual_param_has_name (param, "interpolate")) {
					priv->interpolate = visual_param_get_value_integer (param);

					act_gdkpixbuf_update_image (priv);
				}

			default: /* to avoid warnings */
				break;
		}
	}

	return TRUE;
}

static VisPalette *act_gdkpixbuf_palette (VisPluginData *plugin)
{
	return NULL;
}

static void act_gdkpixbuf_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PixbufPrivate *priv = visual_plugin_get_private (plugin);

	if (priv->target) {
		int xoff, yoff;

		if (priv->center) {
			xoff = (visual_video_get_width	(video) - visual_video_get_width  (priv->target)) / 2;
			yoff = (visual_video_get_height (video) - visual_video_get_height (priv->target)) / 2;
		} else {
			xoff = priv->x_offset;
			yoff = priv->y_offset;
		}

		visual_video_blit (video, priv->target, xoff, yoff, FALSE);
	}
}

static int act_gdkpixbuf_load_file (PixbufPrivate *priv, const char *filename)
{
	visual_log (VISUAL_LOG_INFO, "Loading image from '%s'", filename);

	if (priv->source) {
		g_object_unref (priv->source);
		priv->source = NULL;
	}

	if (priv->target) {
		visual_video_unref (priv->target);
		priv->target = NULL;
	}

	visual_mem_free (priv->filename);
	priv->filename = visual_strdup (filename);

	priv->source = gdk_pixbuf_new_from_file (filename, NULL);
	if (!priv->source) {
		visual_log (VISUAL_LOG_ERROR, "Failed to load image from file '%s'", filename);
		return FALSE;
	}

	act_gdkpixbuf_update_image (priv);

	return TRUE;
}

static void act_gdkpixbuf_update_image (PixbufPrivate *priv)
{
	if (priv->target) {
		visual_video_unref (priv->target);
		priv->target = NULL;
	}

	if (!priv->source) {
		return;
	}

	if (priv->set_scaled) {
		GdkInterpType interp;

		switch (priv->interpolate) {
			case 0:
				interp = GDK_INTERP_NEAREST;
				break;
			case 1:
				interp = GDK_INTERP_TILES;
				break;
			case 2:
				interp = GDK_INTERP_BILINEAR;
				break;
			case 3:
				interp = GDK_INTERP_HYPER;
				break;
			default:
				interp = GDK_INTERP_NEAREST;
				break;
		}

		int width  = 0;
		int height = 0;

		if (priv->set_size) {
			width  = priv->set_width;
			height = priv->set_height;
		} else if (priv->aspect) {
			int rw = gdk_pixbuf_get_width (priv->source);
			int rh = gdk_pixbuf_get_height (priv->source);

			if ((priv->width - rw) > (priv->height - rh)) {
				height = priv->height;
				width  = rw * ((float) height / rh);
			} else {
				width  = priv->width;
				height = rh * ((float) width / rw);
			}
		} else {
			width  = priv->width;
			height = priv->height;
		}

		if (width == 0 || height == 0) {
			return;
		}

		priv->target = visvideo_from_scaled_gdkpixbuf (priv->source, width, height, interp);
	} else {
		priv->target = visvideo_from_gdkpixbuf (priv->source);
	}
}

static VisVideo *visvideo_from_scaled_gdkpixbuf (GdkPixbuf *src, int width, int height, GdkInterpType interp)
{
	GdkPixbuf *scaled = gdk_pixbuf_scale_simple (src, width, height, interp);
	VisVideo  *result = visvideo_from_gdkpixbuf (scaled);
	g_object_unref (scaled);

	return result;
}

static VisVideo *visvideo_from_gdkpixbuf (GdkPixbuf *src)
{
	int width  = gdk_pixbuf_get_width (src);
	int height = gdk_pixbuf_get_height (src);

	VisVideoDepth depth = visual_video_depth_from_bpp (gdk_pixbuf_get_n_channels (src) * 8);

	/* Wrap GdkPixbuf's pixel buffer in VisVideo */
	VisVideo *bgr = visual_video_new_wrap_buffer (gdk_pixbuf_get_pixels (src),
	                                              FALSE,
	                                              width,
	                                              height,
	                                              depth,
	                                              gdk_pixbuf_get_rowstride (src));

	/* Flip RGB byte order */
	VisVideo *target = visual_video_new_with_buffer (width, height, depth);
	visual_video_flip_pixel_bytes (target, bgr);

	visual_video_unref (bgr);

	return target;
}
