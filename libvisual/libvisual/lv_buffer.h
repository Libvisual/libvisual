/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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

#include <libvisual/lv_scoped_ptr.hpp>
#include <libvisual/lv_intrusive_ptr.hpp>
#include <cstdlib>

namespace LV {

  class Buffer;

  typedef IntrusivePtr<Buffer> BufferPtr;
  typedef IntrusivePtr<Buffer const> BufferConstPtr;

  class LV_API Buffer
  {
  public:

      /**
       * Constructs a new empty Buffer.
       */
      static BufferPtr create ();

      /**
       * Constructs a new Buffer with externally allocated content.
       *
       * @param data The which the Buffer encapsulates
       * @param size The size of the data (in bytes)
       * @param own  Indicates whether to take ownership
       */
      static BufferPtr create (void *data, std::size_t size, bool own = true);

      /**
       * Constructs a new Buffer
       *
       * @param size size of the buffer
       */
      static BufferPtr create (std::size_t size);

      ~Buffer ();

      /**
       * Destroys the buffer content.
       */
      void destroy_content ();

      /**
       * Sets the data pair (data and its size)
       *
       * @param data Pointer to the data.
       * @param size Size in bytes of the data.
       */
      void set (void* data, std::size_t size);

      /**
       * Sets the size of the data.
       *
       * @param size Size in bytes of the data.
       */
      void set_size (std::size_t size);

      /**
       * Sets the data to a Buffer.
       *
       * @param data Pointer to the data.
       */
      void set_data (void* data);

      /**
       * Allocates the data for a Buffer, the amount of bytes allocated is defined by the data size
       * that is set to the Buffer.
       */
      void allocate (std::size_t size);

      /**
       * Gets pointer to the data that is encapsulated by the VisBuffer.
       */
      void* get_data () const;

      /**
       * Gets pointer to the data that is encapsulated by the VisBuffer using an
       * offset.
       *
       * @param offset offset in bytes.
       *
       * @return Pointer to the data on success, NULL on failure.
       */
      void* get_data (std::size_t offset) const;

      /**
       * Gets the size in bytes of a Buffer.
       *
       * @return size in bytes
       */
      std::size_t get_size () const;

      /**
       * Returns if the content is allocated and managed by the Buffer
       *
       * @return true if content is allocated and managed, false otherwise
       */
      bool is_allocated () const;

      /**
       * Clones the content of another Buffer
       *
       * @param src source Buffer to clone data from
       */
      void copy (BufferConstPtr const& src);

      /**
       * Copies all the data contained by the buffer into dest.
       */
      void copy_data_to (void *dest);

      /**
       * Copies data from another Buffer starting at a given offset.
       *
       * @param src    source Buffer
       * @param offset write offset
       */
      void put (BufferConstPtr const& src, std::size_t offset);

      /**
       * Copies a block of data.
       *
       * @param data   pointer to data
       * @param size   size of data
       * @param offset write offset
       */
      void put (void const* data, std::size_t size, std::size_t offset);

      /**
       * Fills the buffer with a byte value.
       *
       * @param value fill value
       */
      void fill (uint8_t value);

      /**
       * Fills the buffer with a pattern of data.
       *
       * @param data pointer to memory block containing the pattern
       * @param size size of the memory block
       */
      void fill_with_pattern (void const* data, std::size_t size);

      void ref () const;
      void unref () const;

  private:

      class Impl;

      ScopedPtr<Impl>      m_impl;
      mutable unsigned int m_ref_count;

      Buffer ();
      Buffer (Buffer const&);
      Buffer& operator= (Buffer const&);
  };

  inline void intrusive_ptr_add_ref (Buffer* buffer)
  {
      buffer->ref ();
  }

  inline void intrusive_ptr_release (Buffer* buffer)
  {
      buffer->unref ();
  }

  inline void intrusive_ptr_add_ref (Buffer const* buffer)
  {
      buffer->ref ();
  }

  inline void intrusive_ptr_release (Buffer const* buffer)
  {
      buffer->unref ();
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

LV_API VisBuffer *visual_buffer_new (void);
LV_API VisBuffer *visual_buffer_new_with_data (void *data, visual_size_t size);
LV_API VisBuffer *visual_buffer_new_wrap_data (void *data, visual_size_t size);
LV_API VisBuffer *visual_buffer_new_allocate  (visual_size_t size);
LV_API VisBuffer *visual_buffer_clone (VisBuffer *source);

LV_API void  visual_buffer_set_data_pair (VisBuffer *buffer, void *data, visual_size_t size);
LV_API void  visual_buffer_set_data (VisBuffer *buffer, void *data);
LV_API void *visual_buffer_get_data (VisBuffer *buffer);
LV_API void *visual_buffer_get_data_offset (VisBuffer *buffer, visual_size_t offset);

LV_API void          visual_buffer_set_size (VisBuffer *buffer, visual_size_t size);
LV_API visual_size_t visual_buffer_get_size (VisBuffer *buffer);

LV_API int  visual_buffer_is_allocated    (VisBuffer *buffer);
LV_API void visual_buffer_allocate        (VisBuffer *buffer, visual_size_t size);
LV_API void visual_buffer_destroy_content (VisBuffer *buffer);

LV_API void visual_buffer_copy_data_to (VisBuffer *src, void *dest);

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
