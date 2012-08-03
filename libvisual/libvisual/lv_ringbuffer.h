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
 * @defgroup RingBuffer RingBuffer
 * @{
 */

namespace LV {

  class RingBuffer;
  class RingBufferEntry;

  /**
   * The RingBufferEntry data structure is an entry within the
   * ringbuffer.
   */
  class LV_API RingBufferEntry
  {
  public:

      /** Enum defining the RingBufferEntryTypes. */
      enum Type {
          TYPE_NONE     = 0,  /**< State less entry. */
          TYPE_BUFFER   = 1,  /**< Normal byte buffer. */
          TYPE_FUNCTION = 2   /**< Data retrieval using a callback. */
      };

      typedef Buffer* (*DataFunc)    (RingBufferEntry* entry, RingBuffer* ringbuffer);
      typedef int     (*SizeFunc)    (RingBufferEntry* entry, RingBuffer* ringbuffer);
      typedef void    (*DestroyFunc) (RingBufferEntry* entry);

      Type        type;

      // Used only when type is Buffer
      Buffer*     buffer;

      // Used only when type is function
      DataFunc    data_func;
      DestroyFunc destroy_func;
      SizeFunc    size_func;
      void*       func_data;

      explicit RingBufferEntry (Buffer *buffer);

      RingBufferEntry (DataFunc    data_func,
                       DestroyFunc destroy_func,
                       SizeFunc    size_func,
                       void*       func_data);

      RingBufferEntry (RingBufferEntry const&) = delete;

      ~RingBufferEntry ();

      RingBufferEntry& operator= (RingBufferEntry const&) = delete;
  };

  /**
   * The RingBuffer data structure holding the ringbuffer.
   */
  class LV_API RingBuffer
  {
  public:

      typedef RingBufferEntry Entry;

      typedef std::vector<Entry*> EntryList;

      EntryList entries;

      /**
       * Creates a new RingBuffer structure. The RingBuffer system is
       * a double linked ringbuffer implementation.
       */
      RingBuffer ();

      RingBuffer (RingBuffer const&) = delete;

      ~RingBuffer ();

      RingBuffer& operator= (RingBuffer const&) = delete;

      /**
       * Adds a RingBufferEntry to the end of the ringbuffer.
       *
       * @param entry Entry to add
       */
      void add_entry (Entry* entry);

      /**
       * Adds a Buffer to the end of the ringbuffer.
       *
       * @param buffer The Buffer that is added to the RingBuffer.
       */
      void add_buffer (Buffer *buffer);

      /**
       * Adds a portion of data to the ringbuffer of nbytes byte size.
       *
       * @param data Pointer to the data that is added to the ringbuffer.
       * @param nbytes The size of the data that is added to the ringbuffer.
       */
      void add_buffer_by_data (void *data, int nbytes);

      void add_function (Entry::DataFunc    data_func,
                         Entry::DestroyFunc destroy_func,
                         Entry::SizeFunc    size_func,
                         void*              func_data);

      int get_size ();

      /**
       * Gets a list of all ringbuffer fragments that are currently in
       * the ringbuffer.
       *
       * @return A list of LV::RingBufferEntry items
       */
      EntryList const& get_entries () const
      {
          return entries;
      }

      int get_data (Buffer *data, int nbytes);
      int get_data_offset (Buffer *data, int offset, int nbytes);
      int get_data_from_end (Buffer *data, int nbytes);

      int get_data_without_wrap (Buffer *data, int nbytes);

      Buffer* get_data_new (int nbytes);
      Buffer* get_data_new_without_wrap (int nbytes);
  };

} // LV namespace

/**
 * @}
 */

#endif

#endif /* _LV_RINGBUFFER_H */
