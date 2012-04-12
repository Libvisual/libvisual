/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_gdkpixbuf.c,v 1.19 2006/01/27 20:19:15 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>

#include <gdk/gdk.h>
#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (void);

typedef struct {
	GdkPixbuf	*pixbuf;
	GdkPixbuf	*scaled;
	VisVideo	 target;
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

static int load_new_file (PixbufPrivate *priv, const char *filename);
static int update_scaled_pixbuf (PixbufPrivate *priv);
static int update_into_visvideo (PixbufPrivate *priv, GdkPixbuf *src);

static int act_gdkpixbuf_init (VisPluginData *plugin);
static int act_gdkpixbuf_cleanup (VisPluginData *plugin);
static int act_gdkpixbuf_requisition (VisPluginData *plugin, int *width, int *height);
static int act_gdkpixbuf_resize (VisPluginData *plugin, int width, int height);
static int act_gdkpixbuf_events (VisPluginData *plugin, VisEventQueue *events);
static VisPalette *act_gdkpixbuf_palette (VisPluginData *plugin);
static int act_gdkpixbuf_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

const VisPluginInfo *get_plugin_info (void)
{
	static VisActorPlugin actor = {
		.requisition = act_gdkpixbuf_requisition,
		.palette = act_gdkpixbuf_palette,
		.render = act_gdkpixbuf_render,
		.vidoptions.depth = VISUAL_VIDEO_DEPTH_24BIT
	};

	static VisPluginInfo info = {
		.type = VISUAL_PLUGIN_TYPE_ACTOR,

		.plugname = "gdkpixbuf",
		.name = "GdkPixbuf image loader",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.0.1",
		.about = N_("GdkPixbuf image loader for libvisual"),
		.help = N_("This plugin can be used to show images"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = act_gdkpixbuf_init,
		.cleanup = act_gdkpixbuf_cleanup,
		.events = act_gdkpixbuf_events,

		.plugin = VISUAL_OBJECT (&actor)
	};

	return &info;
}

static int act_gdkpixbuf_init (VisPluginData *plugin)
{
	PixbufPrivate *priv;
	VisParamContainer *paramcontainer = visual_plugin_get_params (plugin);;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY_STRING	("filename",	""),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("scaled",	TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("aspect",	FALSE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("center",	TRUE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("set size",	FALSE),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("width",	0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("height",	0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("x",		0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("y",		0),
		VISUAL_PARAM_LIST_ENTRY_INTEGER	("interpolate",	0),
		VISUAL_PARAM_LIST_END
	};

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (PixbufPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	/* Initialize g_type, needed for GdkPixbuf */
	g_type_init ();

	visual_param_container_add_many (paramcontainer, params);

	return 0;
}

static int act_gdkpixbuf_cleanup (VisPluginData *plugin)
{
	PixbufPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if (priv->filename != NULL)
		free (priv->filename);

	if (priv->pixbuf != NULL)
		g_object_unref (priv->pixbuf);

	if (priv->scaled != NULL)
		g_object_unref (priv->scaled);

	if (visual_video_get_pixels (&priv->target) != NULL)
		visual_video_free_buffer (&priv->target);

	visual_mem_free (priv);

	return 0;
}

static int act_gdkpixbuf_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw = *width;
	int reqh = *height;

	if (reqw < 1)
		reqw = 1;

	if (reqh < 1)
		reqh = 1;

	*width = reqw;
	*height = reqh;

	return 0;
}

int act_gdkpixbuf_resize (VisPluginData *plugin, int width, int height)
{
	PixbufPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	priv->width = width;
	priv->height = height;

	if (priv->pixbuf != NULL)
		update_scaled_pixbuf (priv);
	else {
		/* If there is no image reset the VisVideo pixels, just to be sure */
		if (visual_video_get_pixels (&priv->target) != NULL)
			visual_video_free_buffer (&priv->target);

		visual_video_set_buffer (&priv->target, NULL);
	}
	return 0;
}

static int act_gdkpixbuf_events (VisPluginData *plugin, VisEventQueue *events)
{
	PixbufPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	VisParamEntry *param;
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
				act_gdkpixbuf_resize (plugin, ev.event.resize.width, ev.event.resize.height);
				break;

			case VISUAL_EVENT_PARAM:
				param = ev.event.param.param;

				if (visual_param_entry_is (param, "filename")) {
					visual_log (VISUAL_LOG_DEBUG, "New file to be loaded: %s",
							visual_param_entry_get_string (param));

					load_new_file (priv, visual_param_entry_get_string (param));

				} else if (visual_param_entry_is (param, "scaled")) {
					priv->set_scaled = visual_param_entry_get_integer (param);

					update_scaled_pixbuf (priv);

				} else if (visual_param_entry_is (param, "aspect")) {
					priv->aspect = visual_param_entry_get_integer (param);

					update_scaled_pixbuf (priv);

				} else if (visual_param_entry_is (param, "center")) {
					priv->center = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "set size")) {
					priv->set_size = visual_param_entry_get_integer (param);

					update_scaled_pixbuf (priv);

				} else if (visual_param_entry_is (param, "width")) {
					priv->set_width = visual_param_entry_get_integer (param);

					update_scaled_pixbuf (priv);

				} else if (visual_param_entry_is (param, "height")) {
					priv->set_height = visual_param_entry_get_integer (param);

					update_scaled_pixbuf (priv);

				} else if (visual_param_entry_is (param, "x")) {
					priv->x_offset = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "y")) {
					priv->y_offset = visual_param_entry_get_integer (param);

				} else if (visual_param_entry_is (param, "interpolate")) {
					priv->interpolate = visual_param_entry_get_integer (param);

					update_scaled_pixbuf (priv);

				}

			default: /* to avoid warnings */
				break;
		}
	}

	return 0;
}

static VisPalette *act_gdkpixbuf_palette (VisPluginData *plugin)
{
	return NULL;
}

static int act_gdkpixbuf_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	PixbufPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	if (visual_video_get_pixels (&priv->target) != NULL) {
		if (priv->center == TRUE) {
			int xoff, yoff;

			xoff = (video->width - priv->target.width) / 2;
			yoff = (video->height - priv->target.height) / 2;

			visual_video_blit_overlay (video, &priv->target, xoff, yoff, FALSE);

		} else {
			visual_video_blit_overlay (video, &priv->target, priv->x_offset, priv->y_offset, FALSE);
		}
	}

	return 0;
}

static int load_new_file (PixbufPrivate *priv, const char *filename)
{
	if (priv->pixbuf != NULL)
		g_object_unref (priv->pixbuf);

	if (priv->scaled != NULL)
		g_object_unref (priv->scaled);

	if (priv->filename != NULL)
		free (priv->filename);

	priv->filename = visual_strdup (filename);

	priv->pixbuf = gdk_pixbuf_new_from_file (filename, NULL);
	visual_return_val_if_fail (priv->pixbuf != NULL, -1);

	return update_scaled_pixbuf (priv);
}

static int update_scaled_pixbuf (PixbufPrivate *priv)
{
	GdkInterpType inter;

	if (priv->scaled != NULL)
		g_object_unref (priv->scaled);

	switch (priv->interpolate) {
		case 0:
			inter = GDK_INTERP_NEAREST;
			break;

		case 1:
			inter = GDK_INTERP_TILES;
			break;

		case 2:
			inter = GDK_INTERP_BILINEAR;
			break;

		case 3:
			inter = GDK_INTERP_HYPER;
			break;

		default:
			inter = GDK_INTERP_NEAREST;
			break;
	}

	if (priv->set_scaled == TRUE) {
		if (priv->set_size == TRUE) {
			/* We want to allow this, but gdk_pixbuf does spit warnings, so we catch this */
			if (priv->set_width == 0 || priv->set_height == 0) {
				visual_video_set_buffer (&priv->target, NULL);

				return 0;
			}

			priv->scaled = gdk_pixbuf_scale_simple (priv->pixbuf, priv->set_width, priv->set_height, inter);

		} else if (priv->aspect == TRUE) {
			int as_w, as_h;
			int rw, rh;

			rw = gdk_pixbuf_get_width (priv->pixbuf);
			rh = gdk_pixbuf_get_height (priv->pixbuf);

			/* Determine which dimension we need to create aspect */
			if ((priv->width - rw) > (priv->height - rh)) {
				as_h = priv->height;
				as_w = rw * ((float) as_h / rh);
			} else {
				as_w = priv->width;
				as_h = rh * ((float) as_w / rw);
			}

			priv->scaled = gdk_pixbuf_scale_simple (priv->pixbuf, as_w, as_h, inter);

		} else {
			priv->scaled = gdk_pixbuf_scale_simple (priv->pixbuf, priv->width, priv->height, inter);
		}

		visual_return_val_if_fail (priv->scaled != NULL, -1);

		update_into_visvideo (priv, priv->scaled);

	} else {
		visual_return_val_if_fail (priv->pixbuf != NULL, -1);

		update_into_visvideo (priv, priv->pixbuf);

	}

	return 0;
}

static int update_into_visvideo (PixbufPrivate *priv, GdkPixbuf *src)
{
	VisVideo *target;
	VisVideo bgr;

	target = &priv->target;

	/* Create a VisVideo from the pixbuf */
	visual_video_set_depth (&bgr,
			visual_video_depth_enum_from_value (gdk_pixbuf_get_n_channels (src) * 8));
	visual_video_set_dimension (&bgr, gdk_pixbuf_get_width (src), gdk_pixbuf_get_height (src));
	visual_video_set_pitch (&bgr, gdk_pixbuf_get_rowstride (src));
	visual_video_set_buffer (&bgr, gdk_pixbuf_get_pixels (src));

	if (visual_video_get_pixels (target) != NULL)
		visual_video_free_buffer (target);

	visual_video_copy_attrs (target, &bgr);
	visual_video_allocate_buffer (target);

	/* Gdk uses a different color order than we do */
	visual_video_flip_pixel_bytes (target, &bgr);

	return 0;
}

