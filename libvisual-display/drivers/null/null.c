/* Libvisual-display NULL Backend, Frontend -
 *	The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *
 * $Id: null.c,v 1.6 2005-12-20 18:34:32 synap Exp $ null.c,v 1.5 2005/02/12 18:17:27 vitalyvb Exp $
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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lv_display.h"

#include <GL/gl.h>
#include <GL/glx.h>

static int plugin_init (VisPluginData *plugin);
static int plugin_cleanup (VisPluginData *plugin);



static int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
	int *params, int params_count);
static LvdDContext *context_create(VisPluginData *plugin, VisVideo *video);
static void context_delete(VisPluginData *plugin, LvdDContext*);
static void context_activate(VisPluginData *plugin, LvdDContext*);
static void draw(VisPluginData *plugin);
static int handle_events(VisPluginData *plugin, VisEventQueue *eventqueue);

static int create(VisPluginData*, int **params, int *params_count, VisVideo *video);
static void *get_compat_data(VisPluginData *plugin);
static int get_events(VisPluginData *plugin, VisEventQueue *eventqueue);

const VisPluginInfo *get_plugin_info (int *count)
{
	static const LvdBackendDescription classdes[] = {{
		.compat_type = 123,

		.setup = (void*)setup,

		.context_create = context_create,
		.context_delete = context_delete,
		.context_activate = context_activate,

		.draw = draw,
	}};
	static const LvdFrontendDescription descr2[] = {{
		.compat_count = 1,
		.compat_type = 123,

		.create = create,
		.get_compat_data = get_compat_data,
	}};

	static const VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_BACKEND,

		.plugname = "null",
		.name = "null",
		.author = "me",
		.version = "0.0",
		.about = "asd",
		.help = "call 911!",

		.init = plugin_init,
		.cleanup = plugin_cleanup,
		.events = handle_events,

		.plugin = (void*)&classdes[0],

	}, {
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_DISPLAY_FRONTEND,

		.plugname = "null",
		.name = "null",
		.author = "me",
		.version = "0.0",
		.about = "asd",
		.help = "call 911!",

		.init = plugin_init,
		.cleanup = plugin_cleanup,
		.events = get_events,

		.plugin = (void*)&descr2[0],
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}


int plugin_init (VisPluginData *plugin)
{
	return 0;
}

int plugin_cleanup (VisPluginData *plugin)
{
	return 0;
}

/*=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=**=*/

int setup(VisPluginData *plugin, LvdCompatDataX11 *data,
		int *params, int params_count)
{
	return 0;
}


LvdDContext *context_create(VisPluginData *plugin, VisVideo *video)
{
	return (void*)1;
}

void context_delete(VisPluginData *plugin, LvdDContext *ctx)
{

}

void context_activate(VisPluginData *plugin, LvdDContext *ctx)
{

}

void draw(VisPluginData *plugin)
{

}

void *get_compat_data(VisPluginData *plugin)
{
    return NULL;
}

static VisVideo *video;

int handle_events(VisPluginData *plugin, VisEventQueue *eventqueue)
{
	return 0;
}

int get_events(VisPluginData *plugin, VisEventQueue *eventqueue)
{
	static int evil = 0;
	int w = 400;
	int h = 300;

	evil++;

	if (evil == 200){
		visual_video_set_dimension(video, w, h);
		visual_video_free_buffer(video);
		visual_video_allocate_buffer(video);

		visual_event_queue_add_resize(eventqueue, video, w, h);
		fprintf(stderr, "resizzing....\n");
	}

	if (evil == 400){
		visual_event_queue_add_quit(eventqueue, 0);
	}

	return 0;
}

int create(VisPluginData *plugin,
		int **params, int *params_count, VisVideo *vid)
{
//	visual_video_set_depth(video, VISUAL_VIDEO_DEPTH_8BIT);
	video = vid;
	visual_video_set_dimension(video, 320, 240);
	return 0;
}
