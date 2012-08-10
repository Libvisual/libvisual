/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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

#ifndef _LV_BUFFER_RING_HPP
#define _LV_BUFFER_RING_HPP

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_buffer.h>

#include <vector>
#include <memory>
#include <functional>

namespace LV {

  class BufferRing;
  class BufferRingEntry;

  typedef std::unique_ptr<BufferRingEntry> BufferRingEntryPtr;
  typedef std::unique_ptr<BufferRing>      BufferRingPtr;

  class BufferRingEntry
  {
  public:

      /** Enum defining the BufferRingEntryTypes. */
      enum Type {
          TYPE_NONE     = 0,  /**< State less entry. */
          TYPE_BUFFER   = 1,  /**< Normal byte buffer. */
          TYPE_FUNCTION = 2   /**< Data retrieval using a callback. */
      };

      typedef std::function<BufferPtr (BufferRingEntry&)> DataFunc;
      typedef std::function<int (BufferRingEntry&)>       SizeFunc;
      typedef std::function<void (BufferRingEntry&)>      DestroyFunc;

      Type        type;

      // Used only when type is Buffer
      BufferPtr   buffer;

      // Used only when type is function
      DataFunc    data_func;
      DestroyFunc destroy_func;
      SizeFunc    size_func;
      void*       func_data;

      explicit BufferRingEntry (BufferPtr const& buffer);

      BufferRingEntry (DataFunc    data_func,
                       DestroyFunc destroy_func,
                       SizeFunc    size_func,
                       void*       func_data);

      ~BufferRingEntry ();

      // Non-copyable
      BufferRingEntry (BufferRingEntry const&) = delete;
      BufferRingEntry& operator= (BufferRingEntry const&) = delete;
  };

  class BufferRing
  {
  public:

      typedef BufferRingEntry    Entry;
      typedef BufferRingEntryPtr EntryPtr;

      typedef std::vector<EntryPtr> EntryList;

      EntryList entries;

      BufferRing ();

      ~BufferRing ();

      // Non-copyable
      BufferRing (BufferRing const&) = delete;
      BufferRing& operator= (BufferRing const&) = delete;

      void add_entry (EntryPtr&& entry);

      void add_buffer (BufferPtr const& buffer);

      void add_buffer_by_data (void *data, int nbytes);

      void add_function (Entry::DataFunc    data_func,
                         Entry::DestroyFunc destroy_func,
                         Entry::SizeFunc    size_func,
                         void*              func_data);

      int get_size ();

      EntryList const& get_entries () const
      {
          return entries;
      }

      bool get_data (BufferPtr const& data, int nbytes);
      bool get_data_offset (BufferPtr const& data, int offset, int nbytes);
      bool get_data_from_end (BufferPtr const& data, int nbytes);

      bool get_data_without_wrap (BufferPtr const& data, int nbytes);

      BufferPtr get_data_new (int nbytes);
      BufferPtr get_data_new_without_wrap (int nbytes);
  };

} // LV namespace

#endif // _LV_BUFFER_RING_HPP
