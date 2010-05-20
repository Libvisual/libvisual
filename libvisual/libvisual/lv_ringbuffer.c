/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_ringbuffer.c,v 1.8 2006/01/22 13:23:37 synap Exp $
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
#include <gettext.h>

#include <lvconfig.h>
#include "lv_ringbuffer.h"
#include "lv_log.h"
#include "lv_mem.h"

static int ringbuffer_dtor (VisObject *object);
static int ringbuffer_entry_dtor (VisObject *object);

static int fixate_with_partial_data_request (VisRingBuffer *ringbuffer, VisBuffer *data, int offset, int nbytes,
		int *buffercorr);


static int ringbuffer_dtor (VisObject *object)
{
	VisRingBuffer *ringbuffer = VISUAL_RINGBUFFER (object);

	if (ringbuffer->entries != NULL)
		visual_object_unref (VISUAL_OBJECT (ringbuffer->entries));

	ringbuffer->entries = NULL;

	return VISUAL_OK;
}

static int ringbuffer_entry_dtor (VisObject *object)
{
	VisRingBufferEntry *entry = VISUAL_RINGBUFFER_ENTRY (object);

	if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

		if (entry->buffer != NULL)
			visual_object_unref (VISUAL_OBJECT (entry->buffer));

	} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

		if (entry->destroyfunc != NULL)
			entry->destroyfunc (entry);
	}

	entry->type = VISUAL_RINGBUFFER_ENTRY_TYPE_NONE;
	entry->datafunc = NULL;
	entry->destroyfunc = NULL;
	entry->sizefunc = NULL;
	entry->buffer = NULL;
	entry->functiondata = NULL;

	return VISUAL_OK;
}

/**
 * @defgroup VisRingBuffer VisRingBuffer
 * @{
 */

/**
 * Creates a new VisRingBuffer structure.
 * The VisRingBuffer system is a double linked ringbuffer implementation.
 *
 * @return A newly allocated VisRingBuffer.
 */
VisRingBuffer *visual_ringbuffer_new ()
{
	VisRingBuffer *ringbuffer;

	ringbuffer = visual_mem_new0 (VisRingBuffer, 1);

	visual_ringbuffer_init (ringbuffer);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (ringbuffer), TRUE);
	visual_object_ref (VISUAL_OBJECT (ringbuffer));

	return ringbuffer;
}

int visual_ringbuffer_init (VisRingBuffer *ringbuffer)
{
	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (ringbuffer));
	visual_object_set_dtor (VISUAL_OBJECT (ringbuffer), ringbuffer_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (ringbuffer), FALSE);

	/* Reset the VisRingBuffer structure */
	ringbuffer->entries = visual_list_new (visual_object_collection_destroyer);

	return VISUAL_OK;
}

/**
 * Adds a VisRingBufferEntry to the end of the ringbuffer.
 *
 * @param ringbuffer The VisRingBuffer to which the VisRingBufferEntry is added.
 * @param entry The VisRingBufferEntry that is added to the end of the ringbuffer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_RINGBUFFER_NULL or -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL
 *	on failure.
 */
int visual_ringbuffer_add_entry (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_log_return_val_if_fail (entry != NULL, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL);

	visual_list_add (ringbuffer->entries, entry);

	return VISUAL_OK;
}

/**
 * Adds a VisBuffer to the end of the ringbuffer.
 *
 * @param ringbuffer The VisRingBuffer to which the VisBuffer is added.
 * @param buffer The VisBuffer that is added to the VisRingBuffer.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_RINGBUFFER_NULL, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL
 *	on failure.
 */
int visual_ringbuffer_add_buffer (VisRingBuffer *ringbuffer, VisBuffer *buffer)
{
	VisRingBufferEntry *entry;

	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);

	entry = visual_ringbuffer_entry_new (buffer);

	return visual_ringbuffer_add_entry (ringbuffer, entry);
}

/**
 * Adds a portion of data to the ringbuffer of nbytes byte size.
 *
 * @param ringbuffer Pointer to the ringbuffer to which the data is added.
 * @param data Pointer to the data that is added to the ringbuffer.
 * @param nbytes The size of the data that is added to the ringbuffer.
 *
 * @return VISUAL_OK on succes or -VISUAL_ERROR_RINGBUFFER_NULL, -VISUAL_ERROR_NULL,
 *	-VISUAL_ERROR_RINGBUFFER_ENTRY_NULL on failure.
 */
int visual_ringbuffer_add_buffer_by_data (VisRingBuffer *ringbuffer, void *data, int nbytes)
{
	VisBuffer *buffer;

	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_log_return_val_if_fail (data != NULL, -VISUAL_ERROR_NULL);

	buffer = visual_buffer_new_with_buffer (data, nbytes, NULL);

	return visual_ringbuffer_add_buffer (ringbuffer, buffer);
}

int visual_ringbuffer_add_function (VisRingBuffer *ringbuffer,
		VisRingBufferDataFunc datafunc,
		VisRingBufferDestroyFunc destroyfunc,
		VisRingBufferSizeFunc sizefunc,
		void *functiondata)
{
	VisRingBufferEntry *entry;

	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_log_return_val_if_fail (datafunc != NULL, -VISUAL_ERROR_RINGBUFFER_DATAFUNC_NULL);

	entry = visual_ringbuffer_entry_new_function (datafunc, destroyfunc, sizefunc, functiondata);

	return visual_ringbuffer_add_entry (ringbuffer, entry);
}

int visual_ringbuffer_get_size (VisRingBuffer *ringbuffer)
{
	VisListEntry *le = NULL;
	VisRingBufferEntry *entry;
	int totalsize = 0;

	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);

	while ((entry = visual_list_next (ringbuffer->entries, &le)) != NULL) {
		int bsize = 0;

		if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

			if ((bsize = visual_buffer_get_size (entry->buffer)) > 0)
					totalsize += bsize;

		} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

			if (entry->sizefunc != NULL) {
				totalsize += entry->sizefunc (ringbuffer, entry);
			} else {
				VisBuffer *tempbuf = entry->datafunc (ringbuffer, entry);

				if ((bsize = visual_buffer_get_size (tempbuf)) > 0)
						totalsize += bsize;

				visual_object_unref (VISUAL_OBJECT (tempbuf));
			}
		}
	}

	return totalsize;
}

/**
 * Gets a list of all ringbuffer fragments that are currently in the ringbuffer.
 *
 * @param ringbuffer Pointer to the VisRingBuffer of which the fragments are requested.
 *
 * @return A VisList of VisRingBufferEntry items or NULL on failure.
 */
VisList *visual_ringbuffer_get_list (VisRingBuffer *ringbuffer)
{
	visual_log_return_val_if_fail (ringbuffer != NULL, NULL);

	return ringbuffer->entries;
}

int visual_ringbuffer_get_data (VisRingBuffer *ringbuffer, VisBuffer *data, int nbytes)
{
	return visual_ringbuffer_get_data_offset (ringbuffer, data, 0, nbytes);
}

int visual_ringbuffer_get_data_offset (VisRingBuffer *ringbuffer, VisBuffer *data, int offset, int nbytes)
{
	VisListEntry *le = NULL;
	VisRingBufferEntry *entry;
	int curposition = 0;
	int curoffset = 0;
	int positioncorr = 0;
	int startat = 0;
	int buffercorr = 0;

	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_log_return_val_if_fail (data != NULL, -VISUAL_ERROR_BUFFER_NULL);

	/* Fixate possible partial buffer */
	if (offset > 0)
		startat = fixate_with_partial_data_request (ringbuffer, data, offset, nbytes, &buffercorr);

	curposition = buffercorr;

	/* Buffer fixated with partial segment, request the other segments */
	while (curposition < nbytes) {
		int lindex = 0;
		le = NULL;

		while ((entry = visual_list_next (ringbuffer->entries, &le)) != NULL) {
			VisBuffer *tempbuf;

			lindex++;

			/* Skip to the right offset buffer fragment */
			if (lindex <= startat)
				continue;

			if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

				tempbuf = entry->buffer;

			} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

				/* Will bail out through visual_error_raise(), this is fatal, let's not try to
				 * recover, it's a very obvious bug in the software */
				if (entry->datafunc == NULL) {
					visual_log (VISUAL_LOG_ERROR,
							_("No VisRingBufferDataFunc data provider function set on "
								"type VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION"));

					return -VISUAL_ERROR_IMPOSSIBLE;
				}

				tempbuf = entry->datafunc (ringbuffer, entry);
			}

			if (curposition + visual_buffer_get_size (tempbuf) > nbytes) {
				VisBuffer buf;

				visual_buffer_init (&buf, visual_buffer_get_data (tempbuf),
						nbytes - curposition, NULL);

				visual_buffer_put (data, &buf, curposition);

				if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION)
					visual_object_unref (VISUAL_OBJECT (tempbuf));

				return VISUAL_OK;
			}

			visual_buffer_put (data, tempbuf, curposition);

			curposition += visual_buffer_get_size (tempbuf);

			if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION)
				visual_object_unref (VISUAL_OBJECT (tempbuf));

			/* Filled without room for partial buffer addition */
			if (curposition == nbytes)
				return VISUAL_OK;
		}

		startat = 0;
	}

	return VISUAL_OK;
}

static int fixate_with_partial_data_request (VisRingBuffer *ringbuffer, VisBuffer *data, int offset, int nbytes,
		int *buffercorr)
{
	VisListEntry *le = NULL;
	VisRingBufferEntry *entry;
	int curposition = 0;
	int curoffset = 0;
	int startat = 0;

	*buffercorr = 0;

	while ((entry = visual_list_next (ringbuffer->entries, &le)) != NULL) {
		int bsize = 0;

		startat++;

		if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

			if ((bsize = visual_buffer_get_size (entry->buffer)) > 0)
				curoffset += bsize;

			/* This buffer partially falls within the offset */
			if (curoffset > offset) {
				visual_buffer_put_data (data,
						(visual_buffer_get_data (entry->buffer) +
						 visual_buffer_get_size (entry->buffer)) -
						(curoffset - offset), curoffset - offset, 0);

				*buffercorr = curoffset - offset;

				break;
			}
		} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

			if (entry->sizefunc != NULL) {
				curoffset += entry->sizefunc (ringbuffer, entry);

				/* This buffer partially falls within the offset */
				if (curoffset > offset) {

					VisBuffer *tempbuf = entry->datafunc (ringbuffer, entry);

					visual_buffer_put_data (data,
							(visual_buffer_get_data (tempbuf) +
							visual_buffer_get_size (tempbuf)) -
							(curoffset - offset), curoffset - offset, 0);

					visual_object_unref (VISUAL_OBJECT (tempbuf));

					*buffercorr = curoffset - offset;

					break;
				}
			} else {
				VisBuffer *tempbuf = entry->datafunc (ringbuffer, entry);

				if ((bsize = visual_buffer_get_size (tempbuf)) > 0)
					curoffset += bsize;

				/* This buffer partially falls within the offset */
				if (curoffset > offset) {
					visual_buffer_put_data (data,
							(visual_buffer_get_data (tempbuf) +
							visual_buffer_get_size (tempbuf)) -
							(curoffset - offset), curoffset - offset, 0);

					*buffercorr = curoffset - offset;

					break;
				}

				visual_object_unref (VISUAL_OBJECT (tempbuf));
			}
		}
	}

	return startat;
}

int visual_ringbuffer_get_data_from_end (VisRingBuffer *ringbuffer, VisBuffer *data, int nbytes)
{
	int totalsize = visual_ringbuffer_get_size (ringbuffer);
	int offset = totalsize - nbytes;

	if ((nbytes / totalsize) > 0)
		offset = totalsize - (nbytes % totalsize);

	return visual_ringbuffer_get_data_offset (ringbuffer, data, offset, nbytes);
}

int visual_ringbuffer_get_data_without_wrap (VisRingBuffer *ringbuffer, VisBuffer *data, int nbytes)
{
	int ringsize;
	int amount = nbytes;

	visual_log_return_val_if_fail (ringbuffer != NULL, -VISUAL_ERROR_RINGBUFFER_NULL);

	if ((ringsize = visual_ringbuffer_get_size (ringbuffer)) < nbytes)
		amount = ringsize;

	return visual_ringbuffer_get_data_offset (ringbuffer, data, 0, amount);
}

VisBuffer *visual_ringbuffer_get_data_new (VisRingBuffer *ringbuffer, int nbytes)
{
	VisBuffer *buffer;

	visual_log_return_val_if_fail (ringbuffer != NULL, NULL);

	buffer = visual_buffer_new_allocate (nbytes, NULL);

	visual_ringbuffer_get_data_offset (ringbuffer, buffer, 0, nbytes);

	return buffer;
}

VisBuffer *visual_ringbuffer_get_data_new_without_wrap (VisRingBuffer *ringbuffer, int nbytes)
{
	VisBuffer *buffer;
	int ringsize;
	int amount = nbytes;

	visual_log_return_val_if_fail (ringbuffer != NULL, NULL);

	if ((ringsize = visual_ringbuffer_get_size (ringbuffer)) < nbytes)
		amount = ringsize;

	buffer = visual_buffer_new_allocate (amount, NULL);

	visual_ringbuffer_get_data_without_wrap (ringbuffer, buffer, amount);

	return buffer;
}

VisRingBufferEntry *visual_ringbuffer_entry_new (VisBuffer *buffer)
{
	VisRingBufferEntry *entry;

	entry = visual_mem_new0 (VisRingBufferEntry, 1);

	visual_ringbuffer_entry_init (entry, buffer);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (entry), TRUE);
	visual_object_ref (VISUAL_OBJECT (entry));

	return entry;
}

int visual_ringbuffer_entry_init (VisRingBufferEntry *entry, VisBuffer *buffer)
{
	visual_log_return_val_if_fail (entry != NULL, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (entry));
	visual_object_set_dtor (VISUAL_OBJECT (entry), ringbuffer_entry_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (entry), FALSE);

	/* Reset the VisRingBufferEntry data */
	entry->type = VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER;
	entry->datafunc = NULL;
	entry->destroyfunc = NULL;
	entry->sizefunc = NULL;
	entry->buffer = buffer;
	entry->functiondata = NULL;

	return VISUAL_OK;
}

VisRingBufferEntry *visual_ringbuffer_entry_new_function (
		VisRingBufferDataFunc datafunc,
		VisRingBufferDestroyFunc destroyfunc,
		VisRingBufferSizeFunc sizefunc,
		void *functiondata)
{
	VisRingBufferEntry *entry;

	entry = visual_mem_new0 (VisRingBufferEntry, 1);

	visual_ringbuffer_entry_init_function (entry, datafunc, destroyfunc, sizefunc, functiondata);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (entry), TRUE);
	visual_object_ref (VISUAL_OBJECT (entry));

	return entry;
}

int visual_ringbuffer_entry_init_function (VisRingBufferEntry *entry,
		VisRingBufferDataFunc datafunc,
		VisRingBufferDestroyFunc destroyfunc,
		VisRingBufferSizeFunc sizefunc,
		void *functiondata)
{
	visual_log_return_val_if_fail (entry != NULL, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (entry));
	visual_object_set_dtor (VISUAL_OBJECT (entry), ringbuffer_entry_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (entry), FALSE);

	/* Reset the VisRingBufferEntry data */
	entry->type = VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION;
	entry->datafunc = datafunc;
	entry->destroyfunc = destroyfunc;
	entry->sizefunc = sizefunc;
	entry->buffer = NULL;
	entry->functiondata = functiondata;

	return VISUAL_OK;
}

void *visual_ringbuffer_entry_get_functiondata (VisRingBufferEntry *entry)
{
	visual_log_return_val_if_fail (entry != NULL, NULL);

	return entry->functiondata;
}

/**
 * @}
 */

