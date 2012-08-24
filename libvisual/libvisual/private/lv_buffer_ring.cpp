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

#include "config.h"
#include "private/lv_buffer_ring.hpp"
#include "lv_common.h"
#include "lv_util.hpp"

namespace LV {

  namespace {

    int fixate_with_partial_data_request (BufferRing&      ringbuffer,
                                          BufferPtr const& data,
                                          int              offset,
                                          int              nbytes,
                                          int&             buffercorr)
    {
        int curoffset = 0;
        int startat = 0;

        buffercorr = 0;

        for (auto& entry : ringbuffer.get_entries ()) {

            startat++;

            BufferPtr buffer;
            int size = 0;

            if (entry->type == BufferRingEntry::TYPE_BUFFER) {
                buffer = entry->buffer;
                size   = buffer->get_size ();
            } else if (entry->type == BufferRingEntry::TYPE_FUNCTION) {
                if (entry->size_func) {
                    size = entry->size_func (*entry);
                } else {
                    buffer = entry->data_func (*entry);
                    size   = buffer->get_size ();
                }
            }

            curoffset += std::max (0, size);

            if (curoffset > offset) {
                // This buffer partially falls within the offset

                if (!buffer) {
                    buffer = entry->data_func (*entry);
                }

                buffercorr = curoffset - offset;

                data->put (static_cast<uint8_t*> (buffer->get_data (buffer->get_size () - buffercorr)), buffercorr, 0);
                break;
            }
        }

        return startat;
    }

  } // anonymous namespace

  BufferRing::BufferRing ()
  {
  }

  BufferRing::~BufferRing ()
  {
      // empty
  }

  void BufferRing::add_entry (EntryPtr&& entry)
  {
      entries.push_back (std::move (entry));
  }

  void BufferRing::add_buffer (BufferPtr const& buffer)
  {
      add_entry (make_unique<Entry> (buffer));
  }

  void BufferRing::add_buffer_by_data (void* data, int nbytes)
  {
      visual_return_if_fail (data != nullptr);

      auto buffer = Buffer::wrap (data, nbytes, false);
      add_buffer (buffer);
  }

  void BufferRing::add_function (Entry::DataFunc    data_func,
                                 Entry::DestroyFunc destroy_func,
                                 Entry::SizeFunc    size_func,
                                 void*              func_data)
  {
      visual_return_if_fail (data_func != nullptr);

      add_entry (make_unique<Entry> (data_func, destroy_func, size_func, func_data));
  }

  int BufferRing::get_size ()
  {
      int totalsize = 0;

      for (auto& entry : entries) {
          switch (entry->type) {
              case Entry::TYPE_BUFFER: {
                  totalsize += std::max (0, int (entry->buffer->get_size()));
                  break;
              }
              case Entry::TYPE_FUNCTION: {
                  if (entry->size_func) {
                      totalsize += entry->size_func (*entry);
                  } else {
                      auto buffer = entry->data_func (*entry);
                      totalsize += std::max (0, int (buffer->get_size ()));
                  }
                  break;
              }
              default:;
          }
      }

      return totalsize;
  }

  bool BufferRing::get_data (BufferPtr const& data, int nbytes)
  {
      return get_data_offset (data, 0, nbytes);
  }

  bool BufferRing::get_data_offset (BufferPtr const& data, int offset, int nbytes)
  {
      visual_return_val_if_fail (data, false);

      int startat = 0;
      int buffercorr = 0;

      /* Fixate possible partial buffer */
      if (offset > 0)
          startat = fixate_with_partial_data_request (*this, data, offset, nbytes, buffercorr);

      int curposition = buffercorr;

      /* Buffer fixated with partial segment, request the other segments */
      while (curposition < nbytes) {
          /* return immediately if there are no elements in the list */
          if (entries.empty ())
              return true;

          int lindex = 0;

          for (auto& entry : entries) {

              BufferPtr tempbuf;

              lindex++;

              /* Skip to the right offset buffer fragment */
              if (lindex <= startat)
                  continue;

              switch (entry->type) {
                  case Entry::TYPE_BUFFER: {
                      tempbuf = entry->buffer;
                      break;
                  }
                  case Entry::TYPE_FUNCTION: {
                      if (!entry->data_func) {
                          visual_log (VISUAL_LOG_ERROR,
                                      "No LV::BufferRingDataFunc data provider function set on "
                                      "type LV::BufferRingEntry::TYPE_FUNCTION");

                          return -VISUAL_ERROR_IMPOSSIBLE;
                      }

                      tempbuf = entry->data_func (*entry);
                      break;
                  }
                  default:;
              }

              if (curposition + int (tempbuf->get_size ()) > nbytes) {
                  auto buf = Buffer::wrap (tempbuf->get_data (), nbytes - curposition, false);
                  data->put (buf, curposition);

                  return true;
              }

              data->put (tempbuf, curposition);

              curposition += tempbuf->get_size ();

              /* Filled without room for partial buffer addition */
              if (curposition == nbytes)
                  return true;
          }

          startat = 0;
      }

      return true;
  }

  bool BufferRing::get_data_from_end (BufferPtr const& data, int nbytes)
  {
      int totalsize = get_size ();
      int offset = totalsize - nbytes;

      if ((nbytes / totalsize) > 0)
          offset = totalsize - (nbytes % totalsize);

      return get_data_offset (data, offset, nbytes);
  }

  bool BufferRing::get_data_without_wrap (BufferPtr const& data, int nbytes)
  {
      int ringsize = get_size ();
      int amount = std::min (ringsize, nbytes);

      return get_data_offset (data, 0, amount);
  }

  BufferPtr BufferRing::get_data_new (int nbytes)
  {
      auto buffer = Buffer::create (nbytes);
      get_data_offset (buffer, 0, nbytes);

      return buffer;
  }

  BufferPtr BufferRing::get_data_new_without_wrap (int nbytes)
  {
      int ringsize = get_size ();
      int amount = std::min (ringsize, nbytes);

      auto buffer = Buffer::create (amount);
      get_data_without_wrap (buffer, amount);

      return buffer;
  }

  BufferRingEntry::BufferRingEntry (BufferPtr const& buffer_)
      : type          (TYPE_BUFFER)
      , buffer        (buffer_)
      , data_func     (nullptr)
      , destroy_func  (nullptr)
      , size_func     (nullptr)
      , func_data     (nullptr)
  {
      // empty
  }

  BufferRingEntry::BufferRingEntry (DataFunc    data_func_,
                                    DestroyFunc destroy_func_,
                                    SizeFunc    size_func_,
                                    void*       func_data_)
      : type         (TYPE_FUNCTION)
      , buffer       ()
      , data_func    (data_func_)
      , destroy_func (destroy_func_)
      , size_func    (size_func_)
      , func_data    (func_data_)
  {}

  BufferRingEntry::~BufferRingEntry ()
  {
      switch (type) {
          case TYPE_BUFFER:
              break;

          case TYPE_FUNCTION:
              if (destroy_func)
                  destroy_func (*this);
              break;

          default:;
      }
  }

} // LV namespace
