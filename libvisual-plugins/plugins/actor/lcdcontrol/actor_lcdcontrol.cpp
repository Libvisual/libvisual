/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: actor_lcdcontrol.c,v 1.21 2006/01/27 20:19:17 synap Exp $
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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <gettext.h>
#include <math.h>

#include <libvisual/libvisual.h>
#include "LCDEvent.h"
#include "LCDControl.h"

#include "actor_lcdcontrol.h"

VISUAL_PLUGIN_API_VERSION_VALIDATOR

namespace {

  int         lcdcontrol_init        (VisPluginData *plugin);
  void        lcdcontrol_cleanup     (VisPluginData *plugin);
  void        lcdcontrol_requisition (VisPluginData *plugin, int *width, int *height);
  void        lcdcontrol_resize      (VisPluginData *plugin, int width, int height);
  int         lcdcontrol_events      (VisPluginData *plugin, VisEventQueue *events);
  void        lcdcontrol_render      (VisPluginData *plugin, VisVideo *video, VisAudio *audio);
  VisPalette *lcdcontrol_palette     (VisPluginData *plugin);

} // End namespace


using namespace LCD;

const VisPluginInfo *get_plugin_info ()
{
    static VisActorPlugin actor;
    actor.requisition = lcdcontrol_requisition;
    actor.palette     = lcdcontrol_palette;
    actor.render      = lcdcontrol_render;
    actor.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT;

    static VisPluginInfo info;
    info.type     = VISUAL_PLUGIN_TYPE_ACTOR;
    info.plugname = "lcdcontrol";
    info.name     = "LCDControl";
    info.author   = "Scott Sibley <sisibley@gmail.com>";
    info.version  = "0.1";
    info.about    = "LibVisual LCD Simulation";
    info.help     = "This plugin simulates an LCD and is based on LCD4Linux.";
    info.init     = lcdcontrol_init;
    info.cleanup  = lcdcontrol_cleanup;
    info.events   = lcdcontrol_events;
    info.plugin   = &actor;

	return &info;
}

namespace {

/*
void *my_thread_func(void *data)
{
    LCDControl *control = (LCDControl *)data;
    control->Start();
    return NULL;
}
*/

int lcdcontrol_init (VisPluginData *plugin)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	auto priv = visual_mem_new0 (LCDPrivate, 1);
	visual_plugin_set_private (plugin, priv);

    priv->pal = visual_palette_new(256);

    priv->control = new LCDControl((void *)priv, visual_plugin_get_event_queue (plugin));
    priv->control->Start();

	return 0;
}


void lcdcontrol_cleanup (VisPluginData *plugin)
{
	auto priv = static_cast<LCDPrivate*> (visual_plugin_get_private (plugin));

    delete priv->control;
	visual_mem_free (priv);
}

void lcdcontrol_requisition (VisPluginData *plugin, int *width, int *height)
{
	int reqw, reqh;

	reqw = *width;
	reqh = *height;

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

void lcdcontrol_resize (VisPluginData *plugin, int width, int height)
{
    // FIXME: Implement this
}

int lcdcontrol_events (VisPluginData *plugin, VisEventQueue *events)
{
    VisEvent ev;

    while (visual_event_queue_poll (events, &ev)) {
        switch (ev.type) {
            case VISUAL_EVENT_RESIZE:
            {
                lcdcontrol_resize (plugin, ev.event.resize.width, ev.event.resize.height);
                break;
            }
            default: /* to avoid warnings */
            {
                break;
            }
        }
    }

    return true;
}


VisPalette *lcdcontrol_palette (VisPluginData *plugin)
{
	auto priv = static_cast<LCDPrivate*> (visual_plugin_get_private (plugin));

	int i;

	for (i = 0; i < 256; i++) {
		priv->pal->colors[i].r = i;
		priv->pal->colors[i].g = i;
		priv->pal->colors[i].b = i;
	}

	return priv->pal;
}

void lcdcontrol_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	auto priv = static_cast<LCDPrivate*> (visual_plugin_get_private (plugin));

    priv->control->Lock();
    priv->control->Tick();

    VisVideo *vid = priv->control->GetVideo();
    if(vid) {
        visual_video_scale(video, vid, VISUAL_VIDEO_SCALE_BILINEAR);
    }

    priv->control->Unlock();
}

} // end anonymous namespace
