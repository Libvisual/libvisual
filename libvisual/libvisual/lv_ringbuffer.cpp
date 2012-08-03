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

static int fixate_with_partial_data_request (VisRingBuffer *ringbuffer, VisBuffer *data, int offset, int nbytes,
                                             int& buffercorr);

VisRingBuffer::VisRingBuffer ()
{
}

VisRingBuffer::~VisRingBuffer ()
{
    for (auto& entry : entries) {
        delete entry;
    }
}

void VisRingBuffer::add_entry (VisRingBufferEntry* entry)
{
    visual_return_if_fail (entry != nullptr);

    entries.push_back (entry);
}

void VisRingBuffer::add_buffer (VisBuffer *buffer)
{
    auto entry = new Entry (buffer);
    add_entry (entry);
}

void VisRingBuffer::add_buffer_by_data (void *data, int nbytes)
{
    visual_return_if_fail (data != nullptr);

    auto buffer = visual_buffer_new_wrap_data (data, nbytes);
    add_buffer (buffer);
}

void VisRingBuffer::add_function (VisRingBufferDataFunc datafunc,
                                  VisRingBufferDestroyFunc destroyfunc,
                                  VisRingBufferSizeFunc sizefunc,
                                  void *functiondata)
{
    visual_return_if_fail (datafunc != nullptr);

    auto entry = new Entry (datafunc, destroyfunc, sizefunc, functiondata);
    add_entry (entry);
}

int VisRingBuffer::get_size ()
{
    int totalsize = 0;

    for (auto& entry : entries) {
        switch (entry->type) {
            case VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER: {
                totalsize += std::max (0, int (entry->buffer->get_size()));
                break;
            }
            case VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION: {
                if (entry->sizefunc) {
                    totalsize += entry->sizefunc (this, entry);
                } else {
                    auto tempbuf = entry->datafunc (this, entry);
                    totalsize += std::max (0, int (tempbuf->get_size ()));
                    tempbuf->unref ();
                }
                break;
            }
            default:;
        }
    }

    return totalsize;
}

int VisRingBuffer::get_data (VisBuffer *data, int nbytes)
{
    return get_data_offset (data, 0, nbytes);
}

int VisRingBuffer::get_data_offset (VisBuffer *data, int offset, int nbytes)
{
    visual_return_val_if_fail (data != nullptr, -VISUAL_ERROR_BUFFER_NULL);

    int startat = 0;
    int buffercorr = 0;

    /* Fixate possible partial buffer */
    if (offset > 0)
        startat = fixate_with_partial_data_request (this, data, offset, nbytes, buffercorr);

    int curposition = buffercorr;

    /* Buffer fixated with partial segment, request the other segments */
    while (curposition < nbytes) {
        /* return immediately if there are no elements in the list */
        if (entries.empty ())
            return VISUAL_OK;

        int lindex = 0;

        for (auto& entry : entries) {

            VisBuffer *tempbuf = nullptr;

            lindex++;

            /* Skip to the right offset buffer fragment */
            if (lindex <= startat)
                continue;

            switch (entry->type) {
                case VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER: {
                    tempbuf = entry->buffer;
                    break;
                }
                case VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION: {
                    if (!entry->datafunc) {
                        visual_log (VISUAL_LOG_ERROR,
                                    "No VisRingBufferDataFunc data provider function set on "
                                    "type VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION");

                        return -VISUAL_ERROR_IMPOSSIBLE;
                    }

                    tempbuf = entry->datafunc (this, entry);
                    break;
                }
                default:;
            }

            if (curposition + int (visual_buffer_get_size (tempbuf)) > nbytes) {
                auto buf = visual_buffer_new_wrap_data (tempbuf->get_data (), nbytes - curposition);
                data->put (buf, curposition);
                buf->unref ();

                if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION)
                    tempbuf->unref ();

                return VISUAL_OK;
            }

            data->put (tempbuf, curposition);

            curposition += tempbuf->get_size ();

            if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {
                tempbuf->unref ();
            }

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
    int curoffset = 0;
    int startat = 0;

    buffercorr = 0;

    for (auto& entry : ringbuffer->get_entries ()) {

        int bsize = 0;

        startat++;

        if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER) {

            if ((bsize = entry->buffer->get_size ()) > 0)
                curoffset += bsize;

            /* This buffer partially falls within the offset */
            if (curoffset > offset) {
                data->put (static_cast<uint8_t*> (entry->buffer->get_data ()) + entry->buffer->get_size () - (curoffset - offset),
                           curoffset - offset, 0);

                buffercorr = curoffset - offset;

                break;
            }
        } else if (entry->type == VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION) {

            if (entry->sizefunc != nullptr) {
                curoffset += entry->sizefunc (ringbuffer, entry);

                /* This buffer partially falls within the offset */
                if (curoffset > offset) {

                    auto tempbuf = entry->datafunc (ringbuffer, entry);

                    data->put (static_cast<uint8_t*> (tempbuf->get_data ()) + tempbuf->get_size () - (curoffset - offset),
                               curoffset - offset, 0);

                    tempbuf->unref ();

                    buffercorr = curoffset - offset;

                    break;
                }
            } else {
                auto tempbuf = entry->datafunc (ringbuffer, entry);

                if ((bsize = tempbuf->get_size ()) > 0)
                    curoffset += bsize;

                /* This buffer partially falls within the offset */
                if (curoffset > offset) {
                    data->put (static_cast<uint8_t*> (tempbuf->get_data ()) + tempbuf->get_size () - (curoffset - offset),
                               curoffset - offset, 0);

                    buffercorr = curoffset - offset;

                    break;
                }

                tempbuf->unref ();
            }
        }
    }

    return startat;
}

int VisRingBuffer::get_data_from_end (VisBuffer *data, int nbytes)
{
    int totalsize = get_size ();
    int offset = totalsize - nbytes;

    if ((nbytes / totalsize) > 0)
        offset = totalsize - (nbytes % totalsize);

    return get_data_offset (data, offset, nbytes);
}

int VisRingBuffer::get_data_without_wrap (VisBuffer *data, int nbytes)
{
    int ringsize = get_size ();
    int amount = std::min (ringsize, nbytes);

    return get_data_offset (data, 0, amount);
}

VisBuffer *VisRingBuffer::get_data_new (int nbytes)
{
    auto buffer = visual_buffer_new_allocate (nbytes);
    get_data_offset (buffer, 0, nbytes);

    return buffer;
}

VisBuffer *VisRingBuffer::get_data_new_without_wrap (int nbytes)
{
    int ringsize = get_size ();
    int amount = std::min (ringsize, nbytes);

    auto buffer = visual_buffer_new_allocate (amount);
    get_data_without_wrap (buffer, amount);

    return buffer;
}

VisRingBufferEntry::VisRingBufferEntry (VisBuffer *buffer_)
    : type         (VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER)
    , datafunc     (nullptr)
    , destroyfunc  (nullptr)
    , sizefunc     (nullptr)
    , buffer       (buffer_)
    , functiondata (nullptr)
{
    visual_buffer_ref (buffer);
}

VisRingBufferEntry::VisRingBufferEntry (VisRingBufferDataFunc datafunc_,
                                        VisRingBufferDestroyFunc destroyfunc_,
                                        VisRingBufferSizeFunc sizefunc_,
                                        void *functiondata_)
    : type         (VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION)
    , datafunc     (datafunc_)
    , destroyfunc  (destroyfunc_)
    , sizefunc     (sizefunc_)
    , buffer       (nullptr)
    , functiondata (functiondata_)
{}

VisRingBufferEntry::~VisRingBufferEntry ()
{
    switch (type) {
        case VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER:
            if (buffer)
                buffer->unref ();
            break;

        case VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION:
            if (destroyfunc)
                destroyfunc (this);
            break;

        default:;
    }
}
