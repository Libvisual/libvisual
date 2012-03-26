/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_event.c,v 1.27 2006/01/23 21:06:24 synap Exp $
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

#include "config.h"
#include "lv_event.h"
#include "lv_common.h"
#include "gettext.h"

static int eventqueue_dtor (VisObject *object);

static int event_list_destroy (void *data);

static int eventqueue_dtor (VisObject *object)
{
	VisEventQueue *eventqueue = VISUAL_EVENTQUEUE (object);

	visual_collection_destroy (VISUAL_COLLECTION (&eventqueue->events));

	return VISUAL_OK;
}

static int event_list_destroy (void *data)
{
	VisEvent *event = VISUAL_EVENT (data);

	if (event == NULL)
		return 0;

	visual_object_unref (VISUAL_OBJECT (event));

	return 0;
}


VisEvent *visual_event_new ()
{
	VisEvent *event;

	event = visual_mem_new0 (VisEvent, 1);

	visual_event_init (event);

	/* Do the VisObject initialization*/
	visual_object_set_allocated (VISUAL_OBJECT (event), TRUE);
	visual_object_ref (VISUAL_OBJECT (event));

	return event;
}

int visual_event_init (VisEvent *event)
{
	visual_return_val_if_fail (event != NULL, -VISUAL_ERROR_EVENT_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (event));
	visual_object_set_dtor (VISUAL_OBJECT (event), NULL);
	visual_object_set_allocated (VISUAL_OBJECT (event), FALSE);

	/* Set the VisEvent data */
	visual_object_clean (VISUAL_OBJECT (event), VisEvent);

	return VISUAL_OK;
}

int visual_event_copy (VisEvent *dest, VisEvent *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_EVENT_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_EVENT_NULL);

	/* FIXME: This is far from safe, since it won't do any refcounting jobs */
	visual_object_copy_data (dest, src, VisEvent);

	return VISUAL_OK;
}

VisEventQueue *visual_event_queue_new ()
{
	VisEventQueue *eventqueue;

	eventqueue = visual_mem_new0 (VisEventQueue, 1);

	visual_event_queue_init (eventqueue);

	/* Do the VisObject initialization*/
	visual_object_set_allocated (VISUAL_OBJECT (eventqueue), TRUE);
	visual_object_ref (VISUAL_OBJECT (eventqueue));

	return eventqueue;
}

int visual_event_queue_init (VisEventQueue *eventqueue)
{
	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (eventqueue));
	visual_object_set_dtor (VISUAL_OBJECT (eventqueue), eventqueue_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (eventqueue), FALSE);

	/* Set the VisEventQueue data */
	visual_object_clean (VISUAL_OBJECT (eventqueue), VisEventQueue);

	eventqueue->mousestate = VISUAL_MOUSE_UP;

	visual_collection_set_destroyer (VISUAL_COLLECTION (&eventqueue->events), event_list_destroy);

	visual_event_init (&eventqueue->lastresize);

	return VISUAL_OK;
}

int visual_event_queue_poll (VisEventQueue *eventqueue, VisEvent *event)
{
	VisEvent *lev;
	int more_events;

	more_events = visual_event_queue_poll_by_reference (eventqueue, &lev);

	if (more_events != FALSE) {
		visual_event_copy (event, lev);

		/* FIXME when we start to ref count attributes in events, we need to unref here */
		visual_object_unref (VISUAL_OBJECT (lev));
	}

	return more_events;
}

int visual_event_queue_poll_by_reference (VisEventQueue *eventqueue, VisEvent **event)
{
	VisEvent *lev;
	VisListEntry *listentry = NULL;;

	visual_return_val_if_fail (eventqueue != NULL, FALSE);
	visual_return_val_if_fail (event != NULL, FALSE);

	/* FIXME solve this better */
	if (eventqueue->resizenew == TRUE) {
		eventqueue->resizenew = FALSE;

		*event = visual_event_new ();
		visual_event_copy (*event, &eventqueue->lastresize);

		return TRUE;
	}

	if (eventqueue->eventcount <= 0)
		return FALSE;

	lev = visual_list_next (&eventqueue->events, &listentry);

	*event = lev;

	visual_list_delete (&eventqueue->events, &listentry);

	eventqueue->eventcount--;

	return TRUE;
}

int visual_event_queue_add (VisEventQueue *eventqueue, VisEvent *event)
{
	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);
	visual_return_val_if_fail (event != NULL, -VISUAL_ERROR_EVENT_NULL);

	/* We've got way too much on the queue, not adding events, the important
	 * event.resize event got data in the event queue structure that makes sure it gets
	 * looked at */
	if (eventqueue->eventcount > VISUAL_EVENT_MAXEVENTS) {
		visual_object_unref (VISUAL_OBJECT (event));

		return -1;
	}

	visual_list_add (&eventqueue->events, event);

	eventqueue->eventcount++;

	return VISUAL_OK;
}

int visual_event_queue_add_keyboard (VisEventQueue *eventqueue, VisKey keysym, int keymod, VisKeyState state)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	if (event == NULL) {
		visual_log (VISUAL_LOG_ERROR,
				_("Cannot create a new VisEvent structure"));

		return -VISUAL_ERROR_EVENT_NULL;
	}

	/* FIXME name to VISUAL_KEYB_DOWN and KEYB_UP */
	if (state == VISUAL_KEY_DOWN)
		event->type = VISUAL_EVENT_KEYDOWN;
	else
		event->type = VISUAL_EVENT_KEYUP;

	event->event.keyboard.keysym.sym = keysym;
	event->event.keyboard.keysym.mod = keymod;

	return visual_event_queue_add (eventqueue, event);
}

int visual_event_queue_add_mousemotion (VisEventQueue *eventqueue, int x, int y)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();

	event->type = VISUAL_EVENT_MOUSEMOTION;

	event->event.mousemotion.state = eventqueue->mousestate;
	event->event.mousemotion.x = x;
	event->event.mousemotion.y = y;

	event->event.mousemotion.xrel = x - eventqueue->mousex;
	event->event.mousemotion.yrel = y - eventqueue->mousey;

	eventqueue->mousex = x;
	eventqueue->mousey = y;

	return visual_event_queue_add (eventqueue, event);;
}

int visual_event_queue_add_mousebutton (VisEventQueue *eventqueue, int button, VisMouseState state, int x, int y)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();

	if (state == VISUAL_MOUSE_DOWN)
		event->type = VISUAL_EVENT_MOUSEBUTTONDOWN;
	else
		event->type = VISUAL_EVENT_MOUSEBUTTONUP;

	event->event.mousebutton.button = button;
	event->event.mousebutton.state = state;

	event->event.mousebutton.x = x;
	event->event.mousebutton.y = y;

	eventqueue->mousestate = state;

	return visual_event_queue_add (eventqueue, event);
}

int visual_event_queue_add_resize (VisEventQueue *eventqueue, VisVideo *video, int width, int height)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = &eventqueue->lastresize;

	event->type = VISUAL_EVENT_RESIZE;

	/* FIXME ref counting */
	event->event.resize.video = video;
	event->event.resize.width = width;
	event->event.resize.height = height;

	eventqueue->resizenew = TRUE;

	return VISUAL_OK;
}

int visual_event_queue_add_newsong (VisEventQueue *eventqueue, VisSongInfo *songinfo)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	event = visual_event_new ();

	event->type = VISUAL_EVENT_NEWSONG;

	/* FIXME refcounting */
	event->event.newsong.songinfo = songinfo;

	return visual_event_queue_add (eventqueue, event);
}

int visual_event_queue_add_param (VisEventQueue *eventqueue, void *param)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);
	visual_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_PARAM;

	/* FIXME ref count the param */
	event->event.param.param = param;

	return visual_event_queue_add (eventqueue, event);
}

int visual_event_queue_add_quit (VisEventQueue *eventqueue, int pass_zero_please)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_QUIT;

	return visual_event_queue_add (eventqueue, event);
}

int visual_event_queue_add_visibility (VisEventQueue *eventqueue, int is_visible)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_VISIBILITY;

	event->event.visibility.is_visible = is_visible;

	return visual_event_queue_add (eventqueue, event);
}

int visual_event_queue_add_generic (VisEventQueue *eventqueue, int eid, int param_int, void *param_ptr)
{
	VisEvent *event;

	visual_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_GENERIC;

	event->event.generic.event_id = eid;
	event->event.generic.data_int = param_int;
	event->event.generic.data_ptr = param_ptr;

	return visual_event_queue_add (eventqueue, event);
}

