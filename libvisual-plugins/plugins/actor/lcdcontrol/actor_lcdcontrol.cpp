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


extern "C" const VisPluginInfo *get_plugin_info ();

using namespace LCD;

namespace {

int lcdcontrol_init (VisPluginData *plugin);
int lcdcontrol_cleanup (VisPluginData *plugin);
int lcdcontrol_requisition (VisPluginData *plugin, int *width, int *height);
int lcdcontrol_dimension (VisPluginData *plugin, VisVideo *video, int width, int height);
int lcdcontrol_events (VisPluginData *plugin, VisEventQueue *events);
VisPalette *lcdcontrol_palette (VisPluginData *plugin);
int lcdcontrol_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio);

} // End namespace


VISUAL_PLUGIN_API_VERSION_VALIDATOR

extern "C" const VisPluginInfo *get_plugin_info ()
{
    static VisActorPlugin actor;
    actor.requisition = lcdcontrol_requisition;
    actor.palette = lcdcontrol_palette;
    actor.render = lcdcontrol_render;
    actor.vidoptions.depth = VISUAL_VIDEO_DEPTH_32BIT;

    static VisPluginInfo info;
    info.type = VISUAL_PLUGIN_TYPE_ACTOR;
    info.plugname = "lcdcontrol";
    info.name = "LCDControl";
    info.author = "Scott Sibley <sisibley@gmail.com>";
    info.version = "0.1";
    info.about = "LibVisual LCD Simulation";
    info.help = "This plugin simulates an LCD and is based on LCD4Linux.";
    info.init = lcdcontrol_init;
    info.cleanup = lcdcontrol_cleanup;
    info.events = lcdcontrol_events;
    info.plugin = VISUAL_OBJECT(&actor);

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
	LCDPrivate *priv;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (LCDPrivate, 1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

    priv->pal = visual_palette_new(256);

/*
	visual_buffer_init_allocate (&priv->pcm, sizeof (float) * PCM_SIZE, visual_buffer_destroyer_free);
*/

    //priv->thread = visual_thread_create(my_thread_func, priv, TRUE);

    priv->control = new LCDControl((void *)priv, plugin->eventqueue);

    priv->control->Start();

    /* initialize threading */
/*
    if(pthread_attr_init(&priv->attr) != 0)
        return -1;
*/

    /* start reader thread */
/*
    if(pthread_create(&priv->id, &priv->attr, my_thread_func, priv->control) != 0)
    {
        visual_log(VISUAL_LOG_CRITICAL, "pthread_create() failed");
        return -1;
    }
*/
    plugin->eventqueue = visual_event_queue_new();


	return 0;
}


int lcdcontrol_cleanup (VisPluginData *plugin)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));

	//visual_object_unref (VISUAL_OBJECT (&priv->pcm));

	visual_mem_free (priv);

    //priv->control->Stop();

    /* deinitialize pthreads */

    /* join thread */
    //pthread_join(priv->id, 0);

    //pthread_attr_destroy(&priv->attr);

    //visual_thread_free(priv->thread);

    delete priv->control;

	return 0;
}

int lcdcontrol_requisition (VisPluginData *plugin, int *width, int *height)
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

	return 0;
}

int lcdcontrol_dimension (VisPluginData *plugin, VisVideo *video, int width, int height)
{
	//LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_video_set_dimension (video, width, height);

    visual_video_set_pitch(video, width * sizeof(int));

	return 0;
}

int lcdcontrol_events (VisPluginData *plugin, VisEventQueue *events)
{
	//LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));
	VisEvent ev;

	while (visual_event_queue_poll (events, &ev)) {
		switch (ev.type) {
			case VISUAL_EVENT_RESIZE:
            {
/*
				lcdcontrol_dimension (plugin, ev.event.resize.video,
						ev.event.resize.width, ev.event.resize.height);
*/
				break;
            }
/*
            case VISUAL_EVENT_GENERIC:
            {
                priv->control->Lock();
                LCDEvent *lcd_event = (LCDEvent *)ev.event.generic.data_ptr;
                lcd_event->mFunc(lcd_event->mData);
                priv->control->Unlock();
                
                break;
            }
*/
			default: /* to avoid warnings */
            {
				break;
            }
		}
	}

	return 0;
}


VisPalette *lcdcontrol_palette (VisPluginData *plugin)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));
	int i;

	for (i = 0; i < 256; i++) {
		priv->pal->colors[i].r = i;
		priv->pal->colors[i].g = i;
		priv->pal->colors[i].b = i;
	}

	return priv->pal;
}

int lcdcontrol_render (VisPluginData *plugin, VisVideo *video, VisAudio *audio)
{
	LCDPrivate *priv = (LCDPrivate *)visual_object_get_private (VISUAL_OBJECT (plugin));

/*
	visual_audio_get_sample_mixed (audio, &priv->pcm, TRUE, 2,
			VISUAL_AUDIO_CHANNEL_LEFT,
			VISUAL_AUDIO_CHANNEL_RIGHT,
			1.0,
			1.0);
*/
/*
	float *pcmbuf = (float *)visual_buffer_get_data (&priv->pcm);

	uint8_t *buf = (uint8_t *) visual_video_get_pixels (video);
*/

    priv->control->Lock();
    priv->control->Tick();
    visual_video_scale(video, priv->control->GetVideo(), VISUAL_VIDEO_SCALE_BILINEAR);
    priv->control->Unlock();

	return 0;
}

} // end anonymous namespace
