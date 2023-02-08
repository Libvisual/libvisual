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

#ifndef _LV_BUFFER_H
#define _LV_BUFFER_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

/**
 * @defgroup VisBuffer VisBuffer
 * @{
 */

#ifdef __cplusplus

#include <libvisual/lv_intrusive_ptr.hpp>
#include <memory>
#include <cstdlib>

namespace LV {

  class Buffer;

  typedef IntrusivePtr<Buffer> BufferPtr;
  typedef IntrusivePtr<Buffer const> BufferConstPtr;

  //! Reference-counted memory block
  class LV_API Buffer
  {
  public:

      Buffer (Buffer const&) = delete;

      Buffer& operator= (Buffer const&) = delete;

      /**
       * Constructs a new empty Buffer.
       *
       * @note This is *obsolete* and reserved for internal use.
       * @todo Remove this.
       */
      static BufferPtr create ();

      /**
       * Constructs a new Buffer with an externally allocated block.
       *
       * @note Use this to wrap and optionally manage memory blocks allocated by C functions.
       *
       * @param data pointer to memory block
       * @param size size of memory block in bytes
       * @param own  indicates whether to take ownership
       */
      static BufferPtr wrap (void *data, std::size_t size, bool own = true);

      /**
       * Constructs a new Buffer of a given size.
       *
       * @param size size in bytes
       */
      static BufferPtr create (std::size_t size);

      ~Buffer ();

      /**
       * Destroys the buffer content.
       */
      void destroy_content ();

      /**
       * Sets the data pair (data and its size).
       *
       * @param data Pointer to memory block
       * @param size Size of memory block
       *
       * @todo remove this.
       */
      void set (void* data, std::size_t size);

      /**
       * Sets the size of the data.
       *
       * @param size Size in bytes of the data.
       *
       * @note This is *obsolete* and reserved for internal use.
       * @todo Remove this.
       */
      void set_size (std::size_t size);

      /**
       * Sets the pointer to a memory block to manage
       *
       * @param data Pointer to the data.
       *
       * @note This is *obsolete* and reserved for internal use.
       * @todo Remove this.
       */
      void set_data (void* data);

      /**
       * Allocates the data for a Buffer.
       *
       * @param size Size of memory block
       *
       * @note This is *obsolete* and reserved for internal use.
       * @todo Remove this.
       */
      void allocate (std::size_t size);

      /**
       * Return the pointer to the managed memory block.
       *
       * @return Pointer to memory block
       */
      void* get_data () const;

      /**
       * Returns the pointer to the managed memory block, starting at a given offset.
       *
       * @param offset offset in bytes
       *
       * @return Pointer to memory block
       */
      void* get_data (std::size_t offset) const;

      /**
       * Gets the size in bytes of a Buffer.
       *
       * @return size in bytes
       */
      std::size_t get_size () const;

      /**
       * Returns true if memory block is allocated and managed by the Buffer
       *
       * @return true if memory block is allocated and managed, false otherwise
       */
      bool is_allocated () const;

      /**
       * Clones the content of another Buffer
       *
       * @param src source Buffer to clone data from
       */
      void copy (BufferConstPtr const& src);

      /**
       * Copies all data to a given Buffer.
       *
       * @param dest Buffer to copy to
       */
      void copy_to (BufferPtr const& dest) const;

      /**
       * Copies all data to a given memory location.
       *
       * @param dest pointer to memory location
       * @param size number of bytes to copy
       */
      void copy_to (void *dest, std::size_t size) const;

      /**
       * Copies data from another Buffer.
       *
       * @param src    source Buffer
       * @param offset offset to copy first byte to
       */
      void put (BufferConstPtr const& src, std::size_t offset);

      /**
       * Copies data from a memory location.
       *
       * @param data   pointer to memory location
       * @param size   number of bytes to copy
       * @param offset offset to copy first byte to
       */
      void put (void const* data, std::size_t size, std::size_t offset);

      /**
       * Fills the buffer with a byte value.
       *
       * @param value fill value
       */
      void fill (uint8_t value);

      /**
       * Fills the buffer with a data pattern drawn from a memory location.
       *
       * @param data pointer to memory block to source pattern from
       * @param size number of bytes to fill
       */
      void fill_with_pattern (void const* data, std::size_t size);

  private:

      friend void intrusive_ptr_add_ref (Buffer const* buffer);
      friend void intrusive_ptr_release (Buffer const* buffer);

      class Impl;
      const std::unique_ptr<Impl> m_impl;

      mutable unsigned int m_ref_count;

      Buffer ();
  };

  inline void intrusive_ptr_add_ref (Buffer const* buffer)
  {
      buffer->m_ref_count++;
  }

  inline void intrusive_ptr_release (Buffer const* buffer)
  {
      if (--buffer->m_ref_count == 0) {
          delete buffer;
      }
  }

} // LV namespace

#endif /* __cplusplus */

#ifdef __cplusplus
typedef LV::Buffer VisBuffer;
#else
typedef struct _VisBuffer VisBuffer;
struct _VisBuffer;
#endif

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisBuffer *visual_buffer_new_wrap_data (void *data, visual_size_t size, int own);
LV_NODISCARD LV_API VisBuffer *visual_buffer_new_allocate  (visual_size_t size);
LV_NODISCARD LV_API VisBuffer *visual_buffer_clone (VisBuffer *source);

LV_API void *visual_buffer_get_data (VisBuffer *buffer);
LV_API void *visual_buffer_get_data_offset (VisBuffer *buffer, visual_size_t offset);

LV_API visual_size_t visual_buffer_get_size (VisBuffer *buffer);

LV_API int  visual_buffer_is_allocated    (VisBuffer *buffer);

LV_API void visual_buffer_copy_to (VisBuffer *src, VisBuffer *dest);
LV_API void visual_buffer_copy_to_data (VisBuffer *src, void *dest, visual_size_t size);

LV_API void visual_buffer_put      (VisBuffer *dest, VisBuffer *src, visual_size_t offset);
LV_API void visual_buffer_put_data (VisBuffer *dest, const void *data, visual_size_t size, visual_size_t offset);

LV_API void visual_buffer_fill (VisBuffer *buffer, uint8_t value);
LV_API void visual_buffer_fill_with_pattern (VisBuffer *buffer, const void *data, visual_size_t size);

LV_API void visual_buffer_ref   (VisBuffer *buffer);
LV_API void visual_buffer_unref (VisBuffer *buffer);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_BUFFER_H */
