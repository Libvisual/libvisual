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
#include "lv_buffer.h"
#include "lv_common.h"

namespace LV {

  class Buffer::Impl
  {
  public:

      void*       data;
      std::size_t size;
      bool        is_owner;

      Impl ()
          : data (0)
          , size (0)
          , is_owner (false)
      {}

      ~Impl ()
      {
          free ();
      }

      void wrap (void* data_, std::size_t size_, bool own)
      {
          if (is_owner) {
              visual_mem_free (data);
          }

          data = data_;
          size = size_;
          is_owner = own;
      }

      void allocate (std::size_t size_)
      {
          if (is_owner) {
              visual_mem_free (data);
          }

          data = visual_mem_malloc0 (size_);
          size = size_;
          is_owner = true;
      }

      void free ()
      {
          if (is_owner) {
              visual_mem_free (data);
          }

          data = 0;
          size = 0;
          is_owner = false;
      }
  };

  Buffer::Buffer ()
      : m_impl (new Impl)
      , m_ref_count (1)
  {
  }

  BufferPtr Buffer::create ()
  {
      return BufferPtr (new Buffer, false);
  }

  BufferPtr Buffer::wrap (void* data, std::size_t size, bool own)
  {
      BufferPtr self (new Buffer, false);

      self->m_impl->wrap (data, size, own);

      return self;
  }

  BufferPtr Buffer::create (std::size_t size)
  {
      BufferPtr self (new Buffer, false);

      self->m_impl->allocate (size);

      return self;
  }

  Buffer::~Buffer ()
  {
      // empty
  }

  void Buffer::destroy_content ()
  {
      m_impl->free ();
  }

  void Buffer::set (void* data, std::size_t size)
  {
      m_impl->wrap (data, size, false);
  }

  void Buffer::set_size (std::size_t size)
  {
      m_impl->size = size;
  }

  void Buffer::set_data (void *data)
  {
      m_impl->wrap (data, m_impl->size, false);
  }

  void Buffer::allocate (std::size_t size)
  {
      m_impl->allocate (size);
  }

  void* Buffer::get_data () const
  {
      return m_impl->data;
  }

  void* Buffer::get_data (std::size_t offset) const
  {
      return static_cast<uint8_t*> (m_impl->data) + offset;
  }

  std::size_t Buffer::get_size () const
  {
      return m_impl->size;
  }

  bool Buffer::is_allocated () const
  {
      return m_impl->is_owner;
  }

  void Buffer::copy (BufferConstPtr const& src)
  {
      m_impl->allocate (src->m_impl->size);
      visual_mem_copy (m_impl->data, src->m_impl->data, src->m_impl->size);
  }

  void Buffer::copy_to (void* dest, std::size_t size) const
  {
      visual_return_if_fail (dest != nullptr);

      visual_mem_copy (dest, m_impl->data, std::min (size, m_impl->size));
  }

  void Buffer::copy_to (BufferPtr const& dest) const
  {
      copy_to (dest->get_data (), dest->get_size ());
  }

  void Buffer::put (BufferConstPtr const& src, std::size_t offset)
  {
      put (src->m_impl->data, src->m_impl->size, offset);
  }

  void Buffer::put (void const* data, std::size_t size, std::size_t offset)
  {
      visual_return_if_fail (data != nullptr);
      visual_return_if_fail (offset < m_impl->size);

      size = std::min (m_impl->size - offset, size);

      visual_mem_copy (static_cast<uint8_t*> (m_impl->data) + offset, data, size);
  }

  void Buffer::fill (uint8_t value)
  {
      visual_mem_set (m_impl->data, value, m_impl->size);
  }

  void Buffer::fill_with_pattern (void const* data, std::size_t size)
  {
      visual_return_if_fail (data != nullptr);

      for (std::size_t offset = 0; offset < m_impl->size; offset += size)
          put (data, size, offset);
  }

} // LV namespace
