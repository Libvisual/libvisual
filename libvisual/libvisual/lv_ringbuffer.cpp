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
#include "lv_util.hpp"

namespace LV {

  namespace {

    int fixate_with_partial_data_request (RingBuffer&      ringbuffer,
                                          BufferPtr const& data,
                                          int              offset,
                                          int              nbytes,
                                          int&             buffercorr)
    {
        int curoffset = 0;
        int startat = 0;

        buffercorr = 0;

        for (auto& entry : ringbuffer.get_entries ()) {

            int bsize = 0;

            startat++;

            if (entry->type == RingBufferEntry::TYPE_BUFFER) {

                if ((bsize = entry->buffer->get_size ()) > 0)
                    curoffset += bsize;

                /* This buffer partially falls within the offset */
                if (curoffset > offset) {
                    data->put (static_cast<uint8_t*> (entry->buffer->get_data ()) + entry->buffer->get_size () - (curoffset - offset),
                               curoffset - offset, 0);

                    buffercorr = curoffset - offset;

                    break;
                }
            } else if (entry->type == RingBufferEntry::TYPE_FUNCTION) {

                if (entry->size_func != nullptr) {
                    curoffset += entry->size_func (*entry, ringbuffer);

                    /* This buffer partially falls within the offset */
                    if (curoffset > offset) {

                        auto tempbuf = entry->data_func (*entry, ringbuffer);

                        data->put (static_cast<uint8_t*> (tempbuf->get_data ()) + tempbuf->get_size () - (curoffset - offset),
                                   curoffset - offset, 0);

                        buffercorr = curoffset - offset;

                        break;
                    }
                } else {
                    auto tempbuf = entry->data_func (*entry, ringbuffer);

                    if ((bsize = tempbuf->get_size ()) > 0)
                        curoffset += bsize;

                    /* This buffer partially falls within the offset */
                    if (curoffset > offset) {
                        data->put (static_cast<uint8_t*> (tempbuf->get_data ()) + tempbuf->get_size () - (curoffset - offset),
                                   curoffset - offset, 0);

                        buffercorr = curoffset - offset;

                        break;
                    }
                }
            }
        }

        return startat;
    }

  } // anonymous namespace

  RingBuffer::RingBuffer ()
  {
  }

  RingBuffer::~RingBuffer ()
  {
      // empty
  }

  void RingBuffer::add_entry (EntryPtr&& entry)
  {
      entries.push_back (std::move (entry));
  }

  void RingBuffer::add_buffer (BufferPtr const& buffer)
  {
      add_entry (make_unique<Entry> (buffer));
  }

  void RingBuffer::add_buffer_by_data (void* data, int nbytes)
  {
      visual_return_if_fail (data != nullptr);

      auto buffer = Buffer::create (data, nbytes, false);
      add_buffer (buffer);
  }

  void RingBuffer::add_function (Entry::DataFunc    data_func,
                                 Entry::DestroyFunc destroy_func,
                                 Entry::SizeFunc    size_func,
                                 void*              func_data)
  {
      visual_return_if_fail (data_func != nullptr);

      add_entry (make_unique<Entry> (data_func, destroy_func, size_func, func_data));
  }

  int RingBuffer::get_size ()
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
                      totalsize += entry->size_func (*entry, *this);
                  } else {
                      auto tempbuf = entry->data_func (*entry, *this);
                      totalsize += std::max (0, int (tempbuf->get_size ()));
                  }
                  break;
              }
              default:;
          }
      }

      return totalsize;
  }

  int RingBuffer::get_data (BufferPtr const& data, int nbytes)
  {
      return get_data_offset (data, 0, nbytes);
  }

  int RingBuffer::get_data_offset (BufferPtr const& data, int offset, int nbytes)
  {
      visual_return_val_if_fail (data, -VISUAL_ERROR_BUFFER_NULL);

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
              return VISUAL_OK;

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
                                      "No LV::RingBufferDataFunc data provider function set on "
                                      "type LV::RingBufferEntry::TYPE_FUNCTION");

                          return -VISUAL_ERROR_IMPOSSIBLE;
                      }

                      tempbuf = entry->data_func (*entry, *this);
                      break;
                  }
                  default:;
              }

              if (curposition + int (tempbuf->get_size ()) > nbytes) {
                  auto buf = Buffer::create (tempbuf->get_data (), nbytes - curposition, false);
                  data->put (buf, curposition);

                  return VISUAL_OK;
              }

              data->put (tempbuf, curposition);

              curposition += tempbuf->get_size ();

              /* Filled without room for partial buffer addition */
              if (curposition == nbytes)
                  return VISUAL_OK;
          }

          startat = 0;
      }

      return VISUAL_OK;
  }

  int RingBuffer::get_data_from_end (BufferPtr const& data, int nbytes)
  {
      int totalsize = get_size ();
      int offset = totalsize - nbytes;

      if ((nbytes / totalsize) > 0)
          offset = totalsize - (nbytes % totalsize);

      return get_data_offset (data, offset, nbytes);
  }

  int RingBuffer::get_data_without_wrap (BufferPtr const& data, int nbytes)
  {
      int ringsize = get_size ();
      int amount = std::min (ringsize, nbytes);

      return get_data_offset (data, 0, amount);
  }

  BufferPtr RingBuffer::get_data_new (int nbytes)
  {
      auto buffer = Buffer::create (nbytes);
      get_data_offset (buffer, 0, nbytes);

      return buffer;
  }

  BufferPtr RingBuffer::get_data_new_without_wrap (int nbytes)
  {
      int ringsize = get_size ();
      int amount = std::min (ringsize, nbytes);

      auto buffer = Buffer::create (amount);
      get_data_without_wrap (buffer, amount);

      return buffer;
  }

  RingBufferEntry::RingBufferEntry (BufferPtr const& buffer_)
      : type          (TYPE_BUFFER)
      , buffer        (buffer_)
      , data_func     (nullptr)
      , destroy_func  (nullptr)
      , size_func     (nullptr)
      , func_data     (nullptr)
  {
      // empty
  }

  RingBufferEntry::RingBufferEntry (DataFunc    data_func_,
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

  RingBufferEntry::~RingBufferEntry ()
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
