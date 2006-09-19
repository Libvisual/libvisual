/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_event.c,v 1.28 2006-09-19 18:28:51 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gettext.h>

#include "lvconfig.h"
#include "lv_param.h"
#include "lv_event.h"
#include "lv_log.h"

static int eventqueue_dtor (VisObject *object);
static int event_dtor (VisObject *object);

static int event_ref_subtype_pointers (VisEvent *event);
static int event_unref_subtype_pointers (VisEvent *event);


static int eventqueue_dtor (VisObject *object)
{
	VisEventQueue *eventqueue = VISUAL_EVENTQUEUE (object);

	visual_collection_destroy (VISUAL_COLLECTION (&eventqueue->events));

	return TRUE;
}

static int event_dtor (VisObject *object)
{
	VisEvent *event = VISUAL_EVENT (object);

	event_unref_subtype_pointers (event);

	visual_object_clean (VISUAL_OBJECT (event), VisEvent);

	return TRUE;
}


static int event_ref_subtype_pointers (VisEvent *event)
{
	switch (event->type) {
		case VISUAL_EVENT_RESIZE:
			if (event->event.resize.video != NULL)
				visual_object_ref (VISUAL_OBJECT (event->event.resize.video));
			break;

		case VISUAL_EVENT_NEWSONG:
			if (event->event.newsong.songinfo != NULL)
				visual_object_ref (VISUAL_OBJECT (event->event.newsong.songinfo));
			break;

		case VISUAL_EVENT_PARAM:
			if (event->event.param.param != NULL)
				visual_object_ref (VISUAL_OBJECT (event->event.param.param));
			break;

		default:
			break;
	}

	return VISUAL_OK;
}

static int event_unref_subtype_pointers (VisEvent *event)
{
	switch (event->type) {
		case VISUAL_EVENT_RESIZE:
			if (event->event.resize.video != NULL)
				visual_object_unref (VISUAL_OBJECT (event->event.resize.video));
			break;

		case VISUAL_EVENT_NEWSONG:
			if (event->event.newsong.songinfo != NULL)
				visual_object_unref (VISUAL_OBJECT (event->event.newsong.songinfo));
			break;

		case VISUAL_EVENT_PARAM:
			if (event->event.param.param != NULL)
				visual_object_unref (VISUAL_OBJECT (event->event.param.param));
			break;

		default:
			break;
	}

	return VISUAL_OK;
}

/**
 * @defgroup VisEvent VisEvent
 * @{
 */

/**
 * Creates a new VisEvent structure.
 *
 * @see visual_actor_get_eventqueue
 *
 * @return A newly allocated VisEvent
 */
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
	visual_log_return_val_if_fail (event != NULL, -VISUAL_ERROR_EVENT_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (event));
	visual_object_set_dtor (VISUAL_OBJECT (event), event_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (event), FALSE);

	/* Set the VisEvent data */
	visual_object_clean (VISUAL_OBJECT (event), VisEvent);

	return VISUAL_OK;
}

int visual_event_copy (VisEvent *dest, VisEvent *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_EVENT_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_EVENT_NULL);

	visual_object_copy_data (dest, src, VisEvent);

	event_ref_subtype_pointers (dest);

	return VISUAL_OK;
}

/**
 * Creates a new VisEventQueue data structure.
 *
 * @return Newly allocated VisEventQueue.
 */
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
	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (eventqueue));
	visual_object_set_dtor (VISUAL_OBJECT (eventqueue), eventqueue_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (eventqueue), FALSE);

	/* Set the VisEventQueue data */
	visual_object_clean (VISUAL_OBJECT (eventqueue), VisEventQueue);

	visual_queue_init (&eventqueue->events, visual_object_collection_destroyer);

	eventqueue->mousestate = VISUAL_MOUSE_UP;

	visual_event_init (&eventqueue->lastresize);

	return VISUAL_OK;
}

/**
 * Polls for new events. Looks at the event queue for new events and deletes
 * them from the queue while loading them into the event argument.
 *
 * @param eventqueue Pointer to a VisEventQueue from which new events should be taken.
 * @param event Pointer to a VisEvent in which the new events should be loaded.
 *
 * @return TRUE when events are handled and FALSE when the queue is out of events.
 */
int visual_event_queue_poll (VisEventQueue *eventqueue, VisEvent *event)
{
	VisEvent *lev;
	int more_events;

	more_events = visual_event_queue_poll_by_reference (eventqueue, &lev);

	if (more_events != FALSE) {
		visual_event_copy (event, lev);

		visual_object_unref (VISUAL_OBJECT (lev));
	}

	return more_events;
}

int visual_event_queue_poll_by_reference (VisEventQueue *eventqueue, VisEvent **event)
{
	VisEvent *lev;

	visual_log_return_val_if_fail (eventqueue != NULL, FALSE);
	visual_log_return_val_if_fail (event != NULL, FALSE);

	/* FIXME solve this better */
	if (eventqueue->resizenew == TRUE) {
		eventqueue->resizenew = FALSE;

		*event = visual_event_new ();
		visual_event_copy (*event, &eventqueue->lastresize);

		return TRUE;
	}

	if (visual_collection_size (VISUAL_COLLECTION (&eventqueue->events)) <= 0)
		return FALSE;

	*event = visual_queue_pop (&eventqueue->events);

	return TRUE;
}


/**
 * Adds an event to the event queue. Add new VisEvents into the VisEventQueue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param event Pointer to a VisEvent that needs to be added to the queue.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL or -VISUAL_ERROR_EVENT_NULL
 *	on failure.
 */
int visual_event_queue_add (VisEventQueue *eventqueue, VisEvent *event)
{
	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);
	visual_log_return_val_if_fail (event != NULL, -VISUAL_ERROR_EVENT_NULL);

	event_ref_subtype_pointers (event);

	visual_queue_push (&eventqueue->events, event);

	return VISUAL_OK;
}

/**
 * Adds a new keyboard event to the event queue. By giving keyboard state information
 * a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param keysym A keysym from the VisKey enumerate to set the key to which the event relates.
 * @param keymod Key modifier information from the VisKeyMod enumerate.
 * @param state Contains information about whatever the key is down or up.
 *
 * return VISUAL_OK on succes -VISUAL_ERROR_EVENT_QUEUE_NULL, -VISUAL_ERROR_EVENT_NULL or error values
 *	returned by visual_event_queue_add on failure.
 */
int visual_event_queue_add_keyboard (VisEventQueue *eventqueue, VisKey keysym, int keymod, VisKeyState state)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	if (event == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
				_("Cannot create a new VisEvent structure"));

		return -VISUAL_ERROR_EVENT_NULL;
	}

	if (state == VISUAL_KEYB_DOWN)
		event->type = VISUAL_EVENT_KEYDOWN;
	else
		event->type = VISUAL_EVENT_KEYUP;

	event->event.keyboard.keysym.sym = keysym;
	event->event.keyboard.keysym.mod = keymod;

	return visual_event_queue_add (eventqueue, event);
}

/**
 * Adds a new mouse movement event to the event queue. By giving absolute X and Y coordinates
 * for the mouse a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param x Absolute X value for the mouse location.
 * @param y Absolute Y value for the mouse location.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL or error values returned by
 *	visual_event_queue_add () on failure.
 */
int visual_event_queue_add_mousemotion (VisEventQueue *eventqueue, int x, int y)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

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

/**
 * Adds a new mouse button event to the event queue. By giving a mouse button index number and
 * a mouse button key state a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param button Index that indicates to which mouse button the state relates.
 * @param state Contains information about whatever the button is down or up
 * @param x Absolute X value for the mouse location.
 * @param y Absolute Y value for the mouse location.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL or error values returned by
 *	visual_event_queue_add () on failure.
 */
int visual_event_queue_add_mousebutton (VisEventQueue *eventqueue, int button, VisMouseState state, int x, int y)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

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

/**
 * Adds a new touch event to the event queue. By giving all the touch information a
 * VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param x Absolute X value for the touch location.
 * @param y Absolute Y value for the touch location.
 * @param pressure Pressure ranging from 0.0 to 1.0 of the touch.
 * @param diameter Diameter of the touch.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL or error values returned by
 *	visual_event_queue_add () on failure.
 */
int visual_event_queue_add_touch (VisEventQueue *eventqueue, float x, float y, float pressure, float diameter)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_TOUCH;

	event->event.touch.x = x;
	event->event.touch.y = y;
	event->event.touch.pressure = pressure;
	event->event.touch.diameter = diameter;

	return visual_event_queue_add (eventqueue, event);
}

/**
 * Adds a new dimension change event to the event queue. By giving a pointer to
 * the VisVideo containing all the surface information and new width and height
 * a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param video Pointer to the VisVideo containing all the display information,
 *	also used for negotiation so values can change within the VisVideo
 *	structure.
 * @param width The width for the new surface.
 * @param height The height for the new surface.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL on failure.
 */
int visual_event_queue_add_resize (VisEventQueue *eventqueue, VisVideo *video, int width, int height)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = &eventqueue->lastresize;

	event->type = VISUAL_EVENT_RESIZE;

	event->event.resize.video = video;
	event->event.resize.width = width;
	event->event.resize.height = height;

	eventqueue->resizenew = TRUE;

	event_ref_subtype_pointers (event);

	return VISUAL_OK;
}

/**
 * Adds a new song change event to the event queue. By giving a pointer to the
 * new VisSongInfo structure a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param songinfo Pointer to the VisSongInfo containing all the new song information.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL, -VISUAL_ERROR_SONGINFO_NULL
 *	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_newsong (VisEventQueue *eventqueue, VisSongInfo *songinfo)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);
	visual_log_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	event = visual_event_new ();

	event->type = VISUAL_EVENT_NEWSONG;

	event->event.newsong.songinfo = songinfo;

	return visual_event_queue_add (eventqueue, event);
}

/**
 * Adds a new parameter change event to the event queue. By giving the pointer to the
 * VisParamEntry structure a new VisEvent will be created and added to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param param Pointer to the VisParamEntry containing the parameter that has been changed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL, -VISUAL_ERROR_PARAM_NULL
 *	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_param (VisEventQueue *eventqueue, void *param)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);
	visual_log_return_val_if_fail (param != NULL, -VISUAL_ERROR_PARAM_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_PARAM;

	event->event.param.param = param;

	return visual_event_queue_add (eventqueue, event);
}

/**
 * Adds a new quit event to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param pass_zero_please Might be used in the future, but for now just pass.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL
 *	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_quit (VisEventQueue *eventqueue, int pass_zero_please)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_QUIT;

	return visual_event_queue_add (eventqueue, event);
}

/**
 * Adds a new visibility event to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param is_visible TRUE when visible, FALSE when not visible.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL
 *	or error values returned by visual_event_queue_add () on failure.
 */
int visual_event_queue_add_visibility (VisEventQueue *eventqueue, int is_visible)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_VISIBILITY;

	event->event.visibility.is_visible = is_visible;

	return visual_event_queue_add (eventqueue, event);
}

/**
 * Adds a new generic event to the event queue.
 *
 * @param eventqueue Pointer to the VisEventQueue to which new events are added.
 * @param eid ID of the custom event..
 * @param param_int Integer value for the custom event.
 * @param param_ptr Pointer to data for the custom event..
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_EVENT_QUEUE_NULL
 *	or error values returned by visual_event_queue_add () on failure.
 */

int visual_event_queue_add_generic (VisEventQueue *eventqueue, int eid, int param_int, void *param_ptr)
{
	VisEvent *event;

	visual_log_return_val_if_fail (eventqueue != NULL, -VISUAL_ERROR_EVENT_QUEUE_NULL);

	event = visual_event_new ();
	event->type = VISUAL_EVENT_GENERIC;

	event->event.generic.event_id = eid;
	event->event.generic.data_int = param_int;
	event->event.generic.data_ptr = param_ptr;

	return visual_event_queue_add (eventqueue, event);
}


/**
 * @}
 */

