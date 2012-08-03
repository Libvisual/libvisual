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

#include "config.h"
#include "lv_ringbuffer.h"
#include "lv_common.h"

static void ringbuffer_dtor (VisObject *object);
static void ringbuffer_entry_dtor (VisObject *object);

static int fixate_with_partial_data_request (VisRingBuffer *ringbuffer, VisBuffer *data, int offset, int nbytes,
                                             int& buffercorr);

void ringbuffer_dtor (VisObject *object)
{
	auto ringbuffer = VISUAL_RINGBUFFER (object);

	if (ringbuffer->entries)
		visual_object_unref (VISUAL_OBJECT (ringbuffer->entries));
}

void ringbuffer_entry_dtor (VisObject *object)
{
	auto entry = VISUAL_RINGBUFFER_ENTRY (object);

	switch (entry->type) {
		case VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER:
			if (entry->buffer)
				visual_buffer_unref (entry->buffer);
			break;

		case VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION:
			if (entry->destroyfunc)
				entry->destroyfunc (entry);
			break;

		default:;
	}
}

VisRingBuffer *visual_ringbuffer_new ()
{
	auto ringbuffer = visual_mem_new0 (VisRingBuffer, 1);
	visual_ringbuffer_init (ringbuffer);

	return ringbuffer;
}

int visual_ringbuffer_init (VisRingBuffer *ringbuffer)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (ringbuffer), ringbuffer_dtor);

	/* Reset the VisRingBuffer structure */
	ringbuffer->entries = visual_list_new (visual_object_collection_destroyer);

	return VISUAL_OK;
}

int visual_ringbuffer_add_entry (VisRingBuffer *ringbuffer, VisRingBufferEntry *entry)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_return_val_if_fail (entry != nullptr, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL);

	visual_list_add (ringbuffer->entries, entry);

	return VISUAL_OK;
}

int visual_ringbuffer_add_buffer (VisRingBuffer *ringbuffer, VisBuffer *buffer)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);

	auto entry = visual_ringbuffer_entry_new (buffer);

	return visual_ringbuffer_add_entry (ringbuffer, entry);
}

int visual_ringbuffer_add_buffer_by_data (VisRingBuffer *ringbuffer, void *data, int nbytes)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_return_val_if_fail (data != nullptr, -VISUAL_ERROR_NULL);

	auto buffer = visual_buffer_new_wrap_data (data, nbytes);

	return visual_ringbuffer_add_buffer (ringbuffer, buffer);
}

int visual_ringbuffer_add_function (VisRingBuffer *ringbuffer,
                                    VisRingBufferDataFunc datafunc,
                                    VisRingBufferDestroyFunc destroyfunc,
                                    VisRingBufferSizeFunc sizefunc,
                                    void *functiondata)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_return_val_if_fail (datafunc != nullptr, -VISUAL_ERROR_RINGBUFFER_DATAFUNC_NULL);

	auto entry = visual_ringbuffer_entry_new_function (datafunc, destroyfunc, sizefunc, functiondata);

	return visual_ringbuffer_add_entry (ringbuffer, entry);
}

int visual_ringbuffer_get_size (VisRingBuffer *ringbuffer)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);

	VisListEntry *le = nullptr;
	VisRingBufferEntry *entry;
	int totalsize = 0;

	while ((entry = static_cast<VisRingBufferEntry*> (visual_list_next (ringbuffer->entries, &le))) != nullptr) {
		int bsize = 0;

		if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

			if ((bsize = visual_buffer_get_size (entry->buffer)) > 0)
				totalsize += bsize;

		} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

			if (entry->sizefunc != nullptr) {
				totalsize += entry->sizefunc (ringbuffer, entry);
			} else {
				VisBuffer *tempbuf = entry->datafunc (ringbuffer, entry);

				if ((bsize = visual_buffer_get_size (tempbuf)) > 0)
						totalsize += bsize;

				visual_buffer_unref (tempbuf);
			}
		}
	}

	return totalsize;
}

VisList *visual_ringbuffer_get_list (VisRingBuffer *ringbuffer)
{
	visual_return_val_if_fail (ringbuffer != nullptr, nullptr);

	return ringbuffer->entries;
}

int visual_ringbuffer_get_data (VisRingBuffer *ringbuffer, VisBuffer *data, int nbytes)
{
	return visual_ringbuffer_get_data_offset (ringbuffer, data, 0, nbytes);
}

int visual_ringbuffer_get_data_offset (VisRingBuffer *ringbuffer, VisBuffer *data, int offset, int nbytes)
{
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);
	visual_return_val_if_fail (data != nullptr, -VISUAL_ERROR_BUFFER_NULL);

	int startat = 0;
	int buffercorr = 0;

	/* Fixate possible partial buffer */
	if (offset > 0)
		startat = fixate_with_partial_data_request (ringbuffer, data, offset, nbytes, buffercorr);

	int curposition = buffercorr;

	/* Buffer fixated with partial segment, request the other segments */
	while (curposition < nbytes) {
		/* return immediately if there are no elements in the list */
		if(visual_list_count(ringbuffer->entries) == 0)
			return VISUAL_OK;

        VisRingBufferEntry *entry;
        VisListEntry *le = nullptr;
		int lindex = 0;

		while ((entry = static_cast<VisRingBufferEntry*> (visual_list_next (ringbuffer->entries, &le))) != nullptr) {
			VisBuffer *tempbuf = nullptr;

			lindex++;

			/* Skip to the right offset buffer fragment */
			if (lindex <= startat)
				continue;

			if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

				tempbuf = entry->buffer;

			} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

				/* Will bail out through visual_error_raise(), this is fatal, let's not try to
				 * recover, it's a very obvious bug in the software */
				if (entry->datafunc == nullptr) {
					visual_log (VISUAL_LOG_ERROR,
							"No VisRingBufferDataFunc data provider function set on "
							"type VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION");

					return -VISUAL_ERROR_IMPOSSIBLE;
				}

				tempbuf = entry->datafunc (ringbuffer, entry);
			}

			if (curposition + int (visual_buffer_get_size (tempbuf)) > nbytes) {
				VisBuffer *buf;

				buf = visual_buffer_new_wrap_data (visual_buffer_get_data (tempbuf), nbytes - curposition);
				visual_buffer_put (data, buf, curposition);
				visual_buffer_unref (buf);

				if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION)
					visual_buffer_unref (tempbuf);

				return VISUAL_OK;
			}

			visual_buffer_put (data, tempbuf, curposition);

			curposition += visual_buffer_get_size (tempbuf);

			if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION)
				visual_buffer_unref (tempbuf);

			/* Filled without room for partial buffer addition */
			if (curposition == nbytes)
				return VISUAL_OK;
		}

		startat = 0;
	}

	return VISUAL_OK;
}

int fixate_with_partial_data_request (VisRingBuffer *ringbuffer,
                                      VisBuffer *data,
                                      int offset,
                                      int nbytes,
                                      int& buffercorr)
{
	VisListEntry *le = nullptr;
	VisRingBufferEntry *entry;
	int curoffset = 0;
	int startat = 0;

	buffercorr = 0;

	while ((entry = static_cast<VisRingBufferEntry*> (visual_list_next (ringbuffer->entries, &le))) != nullptr) {
		int bsize = 0;

		startat++;

		if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

			if ((bsize = visual_buffer_get_size (entry->buffer)) > 0)
				curoffset += bsize;

			/* This buffer partially falls within the offset */
			if (curoffset > offset) {
				visual_buffer_put_data (data,
						((uint8_t *) visual_buffer_get_data (entry->buffer) +
						visual_buffer_get_size (entry->buffer)) -
						(curoffset - offset), curoffset - offset, 0);

				buffercorr = curoffset - offset;

				break;
			}
		} else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

			if (entry->sizefunc != nullptr) {
				curoffset += entry->sizefunc (ringbuffer, entry);

				/* This buffer partially falls within the offset */
				if (curoffset > offset) {

					VisBuffer *tempbuf = entry->datafunc (ringbuffer, entry);

					visual_buffer_put_data (data,
							((uint8_t *) visual_buffer_get_data (tempbuf) +
							visual_buffer_get_size (tempbuf)) -
							(curoffset - offset), curoffset - offset, 0);

					visual_buffer_unref (tempbuf);

					buffercorr = curoffset - offset;

					break;
				}
			} else {
				VisBuffer *tempbuf = entry->datafunc (ringbuffer, entry);

				if ((bsize = visual_buffer_get_size (tempbuf)) > 0)
					curoffset += bsize;

				/* This buffer partially falls within the offset */
				if (curoffset > offset) {
					visual_buffer_put_data (data,
							((uint8_t *) visual_buffer_get_data (tempbuf) +
							visual_buffer_get_size (tempbuf)) -
							(curoffset - offset), curoffset - offset, 0);

					buffercorr = curoffset - offset;

					break;
				}

				visual_buffer_unref (tempbuf);
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
	visual_return_val_if_fail (ringbuffer != nullptr, -VISUAL_ERROR_RINGBUFFER_NULL);

	int ringsize = visual_ringbuffer_get_size (ringbuffer);
	int amount = std::min (ringsize, nbytes);

	return visual_ringbuffer_get_data_offset (ringbuffer, data, 0, amount);
}

VisBuffer *visual_ringbuffer_get_data_new (VisRingBuffer *ringbuffer, int nbytes)
{
	visual_return_val_if_fail (ringbuffer != nullptr, nullptr);

	auto buffer = visual_buffer_new_allocate (nbytes);

	visual_ringbuffer_get_data_offset (ringbuffer, buffer, 0, nbytes);

	return buffer;
}

VisBuffer *visual_ringbuffer_get_data_new_without_wrap (VisRingBuffer *ringbuffer, int nbytes)
{
	visual_return_val_if_fail (ringbuffer != nullptr, nullptr);

	int ringsize = visual_ringbuffer_get_size (ringbuffer);
	int amount = std::min (ringsize, nbytes);

	auto buffer = visual_buffer_new_allocate (amount);

	visual_ringbuffer_get_data_without_wrap (ringbuffer, buffer, amount);

	return buffer;
}

VisRingBufferEntry *visual_ringbuffer_entry_new (VisBuffer *buffer)
{
	auto entry = visual_mem_new0 (VisRingBufferEntry, 1);
	visual_ringbuffer_entry_init (entry, buffer);

	return entry;
}

int visual_ringbuffer_entry_init (VisRingBufferEntry *entry, VisBuffer *buffer)
{
	visual_return_val_if_fail (entry != nullptr, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (entry), ringbuffer_entry_dtor);

	/* Reset the VisRingBufferEntry data */
	entry->type = VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER;
	entry->datafunc = nullptr;
	entry->destroyfunc = nullptr;
	entry->sizefunc = nullptr;
	entry->functiondata = nullptr;

	entry->buffer = buffer;
	visual_buffer_ref (buffer);

	return VISUAL_OK;
}

VisRingBufferEntry *visual_ringbuffer_entry_new_function (VisRingBufferDataFunc datafunc,
                                                          VisRingBufferDestroyFunc destroyfunc,
                                                          VisRingBufferSizeFunc sizefunc,
                                                          void *functiondata)
{
    auto entry = visual_mem_new0 (VisRingBufferEntry, 1);
	visual_ringbuffer_entry_init_function (entry, datafunc, destroyfunc, sizefunc, functiondata);

	return entry;
}

int visual_ringbuffer_entry_init_function (VisRingBufferEntry *entry,
                                           VisRingBufferDataFunc datafunc,
                                           VisRingBufferDestroyFunc destroyfunc,
                                           VisRingBufferSizeFunc sizefunc,
                                           void *functiondata)
{
	visual_return_val_if_fail (entry != nullptr, -VISUAL_ERROR_RINGBUFFER_ENTRY_NULL);

	/* Do the VisObject initialization */
	visual_object_init (VISUAL_OBJECT (entry), ringbuffer_entry_dtor);

	/* Reset the VisRingBufferEntry data */
	entry->type = VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION;
	entry->datafunc = datafunc;
	entry->destroyfunc = destroyfunc;
	entry->sizefunc = sizefunc;
	entry->buffer = nullptr;
	entry->functiondata = functiondata;

	return VISUAL_OK;
}

void *visual_ringbuffer_entry_get_functiondata (VisRingBufferEntry *entry)
{
	visual_return_val_if_fail (entry != nullptr, nullptr);

	return entry->functiondata;
}
