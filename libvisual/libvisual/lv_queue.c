/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_queue.c,v 1.1 2006-09-20 19:26:07 synap Exp $
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
#include <string.h>
#include <fcntl.h>

#include <lvconfig.h>
#include "lv_queue.h"
#include "lv_log.h"
#include "lv_mem.h"


/**
 * @defgroup VisQueue VisQueue
 * @{
 */

/**
 * Creates a new FIFO VisQueue structure. The VisQueue system is a first in first out structure
 * which is backed by VisList.
 *
 * @return A newly allocated VisQueue.
 */
VisQueue *visual_queue_new (VisCollectionDestroyerFunc destroyer)
{
	VisQueue *queue;

	queue = visual_mem_new0 (VisQueue, 1);

	visual_queue_init (queue, destroyer);

	/* do the visobject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (queue), TRUE);
	visual_object_ref (VISUAL_OBJECT (queue));

	return queue;
}

/**
 * Initializes a FIFO VisQueue.
 *
 * @see visual_queue_new
 *
 * @param queue Pointer to the VisQueue that is initialized.
 * @param destroyer The collection destroyer that is used to destroy the individual members.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_QUEUE_NULL on failure.
 */
int visual_queue_init (VisQueue *queue, VisCollectionDestroyerFunc destroyer)
{
	visual_log_return_val_if_fail (queue != NULL, -VISUAL_ERROR_QUEUE_NULL);

	/* Do the VisObject initialization */
	visual_list_init (VISUAL_LIST (queue), destroyer);

	return VISUAL_OK;
}

/**
 * Peeks the first element. This will not remove the element from the queue.
 *
 * @param queue Pointer to the VisQueue on which is peeked.
 *
 * @return The next value on the queue or NULL.
 */
void *visual_queue_peek (VisQueue *queue)
{
	VisList *list;

	visual_log_return_val_if_fail (queue != NULL, NULL);

	list = VISUAL_LIST (queue);

	if (list->head == NULL)
		return NULL;

	return list->head->data;
}

/**
 * Pops an element from the queue.
 *
 * @param queue Pointer to the VisQueue of which an element is popped.
 *
 * @return The next value on the queue or NULL.
 */
void *visual_queue_pop (VisQueue *queue)
{
	VisList *list;
	void *data = NULL;

	visual_log_return_val_if_fail (queue != NULL, NULL);

	list = VISUAL_LIST (queue);

	if (list->head != NULL) {
		VisListEntry *le = list->head;

		data = list->head->data;

		visual_list_delete (list, &le);
	}

	return data;
}

/**
 * Pushes a new element on the queue.
 *
 * @param queue Pointer to the VisQueue on which an element is pushed.
 * @param data The element that is being pushed on the VisQueue.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_QUEUE_NULL on failure.
 */
int visual_queue_push (VisQueue *queue, void *data)
{
	VisList *list;

	visual_log_return_val_if_fail (queue != NULL, -VISUAL_ERROR_QUEUE_NULL);

	list = VISUAL_LIST (queue);

	return visual_list_add (list, data);
}

/**
 * Pushes a new element at the front of the queue so it will become the first element
 * that is popped.
 *
 * @see visual_queue_push
 * 
 * @param queue Pointer to the VisQueue on which an element is pushed in front.
 * @param data The element that is being pushed on the VisQueue.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_QUEUE_NULL on failure.
 */
int visual_queue_push_front (VisQueue *queue, void *data)
{
	VisList *list;

	visual_log_return_val_if_fail (queue != NULL, -VISUAL_ERROR_QUEUE_NULL);

	list = VISUAL_LIST (queue);

	return visual_list_add_at_begin (list, data);
}

/**
 * @}
 */

