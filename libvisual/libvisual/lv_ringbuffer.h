/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_ringbuffer.h,v 1.6 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_RINGBUFFER_H
#define _LV_RINGBUFFER_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_buffer.h>

#ifdef __cplusplus

#include <vector>

/**
 * @defgroup VisRingBuffer VisRingBuffer
 * @{
 */

#define VISUAL_RINGBUFFER(obj)        (VISUAL_CHECK_CAST ((obj), VisRingBuffer))
#define VISUAL_RINGBUFFER_ENTRY(obj)  (VISUAL_CHECK_CAST ((obj), VisRingBufferEntry))

/**
 * Enum defining the VisRingBufferEntryTypes.
 */
typedef enum {
    VISUAL_RINGBUFFER_ENTRY_TYPE_NONE     = 0,  /**< State less entry. */
    VISUAL_RINGBUFFER_ENTRY_TYPE_BUFFER   = 1,  /**< Normal byte buffer. */
    VISUAL_RINGBUFFER_ENTRY_TYPE_FUNCTION = 2   /**< Data retrieval using a callback. */
} VisRingBufferEntryType;

class VisRingBuffer;
class VisRingBufferEntry;

/**
 * A VisRingBuffer data provider function needs this signature. It can be used to provide
 * ringbuffer entry data on runtime through a callback.
 *
 * @arg ringbuffer The VisRingBuffer data structure.
 * @arg entry The VisRingBufferEntry to which the callback entry is connected.
 */
typedef VisBuffer *(*VisRingBufferDataFunc)(VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);

typedef void (*VisRingBufferDestroyFunc)(VisRingBufferEntry *entry);

typedef int (*VisRingBufferSizeFunc)(VisRingBuffer *ringbuffer, VisRingBufferEntry *entry);


/**
 * The VisRingBufferEntry data structure is an entry within the ringbuffer.
 */
class LV_API VisRingBufferEntry
{
public:

    VisRingBufferEntryType   type;
    VisRingBufferDataFunc    datafunc;
    VisRingBufferDestroyFunc destroyfunc;
    VisRingBufferSizeFunc    sizefunc;

    VisBuffer               *buffer;

    void                    *functiondata;

    explicit VisRingBufferEntry (VisBuffer *buffer);

    VisRingBufferEntry (VisRingBufferDataFunc datafunc,
                        VisRingBufferDestroyFunc destroyfunc,
                        VisRingBufferSizeFunc sizefunc,
                        void *functiondata);

    VisRingBufferEntry (VisRingBufferEntry const&) = delete;

    ~VisRingBufferEntry ();

    VisRingBufferEntry& operator= (VisRingBufferEntry const&) = delete;

    void *get_functiondata (VisRingBufferEntry *entry) const
    {
        return functiondata;
    }
};

/**
 * The VisRingBuffer data structure holding the ringbuffer.
 */
class LV_API VisRingBuffer {
public:

    typedef VisRingBufferEntry Entry;

    typedef std::vector<Entry*> EntryList;

    EntryList entries;

    /**
     * Creates a new VisRingBuffer structure. The VisRingBuffer system is
     * a double linked ringbuffer implementation.
     */
    VisRingBuffer ();

    VisRingBuffer (VisRingBuffer const&) = delete;

    ~VisRingBuffer ();

    VisRingBuffer& operator= (VisRingBuffer const&) = delete;

    /**
     * Adds a VisRingBufferEntry to the end of the ringbuffer.
     *
     * @param entry Entry to add
     */
    void add_entry (Entry* entry);

    /**
     * Adds a VisBuffer to the end of the ringbuffer.
     *
     * @param buffer The VisBuffer that is added to the VisRingBuffer.
     */
    void add_buffer (VisBuffer *buffer);

    /**
     * Adds a portion of data to the ringbuffer of nbytes byte size.
     *
     * @param data Pointer to the data that is added to the ringbuffer.
     * @param nbytes The size of the data that is added to the ringbuffer.
     */
    void add_buffer_by_data (void *data, int nbytes);

    void add_function (VisRingBufferDataFunc datafunc,
                       VisRingBufferDestroyFunc destroyfunc,
                       VisRingBufferSizeFunc sizefunc,
                       void *functiondata);

    int get_size ();

    /**
     * Gets a list of all ringbuffer fragments that are currently in the
     * ringbuffer.
     *
     * @return A VisList of VisRingBufferEntry items or NULL on failure.
     */
    EntryList const& get_entries () const
    {
        return entries;
    }

    int get_data (VisBuffer *data, int nbytes);
    int get_data_offset (VisBuffer *data, int offset, int nbytes);
    int get_data_from_end (VisBuffer *data, int nbytes);

    int get_data_without_wrap (VisBuffer *data, int nbytes);

    VisBuffer* get_data_new (int nbytes);
    VisBuffer* get_data_new_without_wrap (int nbytes);
};

/**
 * @}
 */

#endif

#endif /* _LV_RINGBUFFER_H */
