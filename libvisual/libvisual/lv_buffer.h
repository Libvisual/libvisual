/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_buffer.h,v 1.9 2006/01/22 13:23:37 synap Exp $
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

#include <libvisual/lv_object.h>

/**
 * @defgroup VisBuffer VisBuffer
 * @{
 */

#define VISUAL_BUFFER(obj)				(VISUAL_CHECK_CAST ((obj), VisBuffer))

typedef struct _VisBuffer VisBuffer;

/**
 * A VisBuffer destroyer function needs this signature. It can be used to destroy
 * the content of a VisBuffer on a unref.
 *
 * @arg buffer The VisBuffer data structure.
 */
typedef void (*VisBufferDestroyerFunc)(VisBuffer *buffer);

/**
 * The VisBuffer data structure holding a buffer and information regarding the buffer.
 */
struct _VisBuffer {
	VisObject		 object;	/**< The VisObject data. */

	int			 allocated;	/**< Is the buffer allocated or not. */

	VisBufferDestroyerFunc	 destroyer;	/**< The destroyer function for the encapsulated data. */

	void			*data;		/**< Pointer to the data. */
	visual_size_t		 datasize;	/**< The size of the data in N bytes. */
};

LV_BEGIN_DECLS

/**
 * Creates a new VisBuffer. A VisBuffer encapsulates a memory buffer and it's size, it's also a VisObject so it can be
 * referenced.
 *
 * @return A newly allocated VisBuffer.
 */
LV_API VisBuffer *visual_buffer_new (void);

/**
 * Initializes a VisBuffer, it's safe to use visual_buffer_set_data_pair on a non heap allocated VisBuffer,
 * however do not unref it when you're using this method. The reason to do this can be performance because
 * the VisObject initialization is skipped. Be very careful with this and only do it when you know what
 * you're doing.
 *
 * @see visual_buffer_new
 *
 * @param buffer Pointer to the VisBuffer that is to be initialized.
 * @param data The which the VisBuffer encapsulates.
 * @param datasize The size of the data (in bytes).
 * @param destroyer The destroyer that is to be used on the data when the buffer is destroyed or the refcount
 *	reaches 0.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_init (VisBuffer *buffer, void *data, visual_size_t datasize, VisBufferDestroyerFunc destroyer);

/**
 * Creates a new VisBuffer with data, datasize and destroyer set.
 *
 * @param data The which the VisBuffer encapsulates.
 * @param datasize The size of the data (in bytes).
 * @param destroyer The destroyer that is to be used on the data when the buffer is destroyed or the refcount
 *	reaches 0.
 *
 * @return A newly allocated VisBuffer.
 */
LV_API VisBuffer *visual_buffer_new_with_buffer (void *data, visual_size_t datasize, VisBufferDestroyerFunc destroyer);

/**
 * Creates a new VisBuffer, allocates data, sets the destroyer.
 *
 * @param datasize The size of the data requested (in bytes).
 * @param destroyer The destroyer that is to be used on the data when the buffer is destroyed or the refcount
 *	reaches 0.
 *
 * @return A newly allocated VisBuffer.
 */
LV_API VisBuffer *visual_buffer_new_allocate (visual_size_t datasize, VisBufferDestroyerFunc destroyer);

/**
 * Initializes a VisBuffer and allocates the data, it's safe to use visual_buffer_set_data_pair on a
 * non heap allocated VisBuffer, however do not unref it when you're using this method.
 * The reason to do this can be performance because the VisObject initialization is skipped.
 * Be very careful with this and only do it when you know what you're doing.
 *
 * @see visual_buffer_new_allocate
 *
 * @param buffer Pointer to the VisBuffer that is to be initialized.
 * @param datasize The size of the data (in bytes).
 * @param destroyer The destroyer that is to be used on the data when the buffer is destroyed or the refcount
 *	reaches 0.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_init_allocate (VisBuffer *buffer, visual_size_t datasize, VisBufferDestroyerFunc destroyer);

/**
 * Destroys the content of a VisBuffer using the VisBufferDestroyerFunc that is set to the VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer of which the content is destroyed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_destroy_content (VisBuffer *buffer);

/**
 * Sets the destroyer function for the content of a VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer to which the destroyer function is set.
 * @param destroyer The destroyer function.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_set_destroyer (VisBuffer *buffer, VisBufferDestroyerFunc destroyer);

/**
 * Retrieves the destroyer function that is set to a VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer of which the destroyer function is requested.
 *
 * @return Pointer to the destroyer function, or NULL on failure.
 */
LV_API VisBufferDestroyerFunc visual_buffer_get_destroyer (VisBuffer *buffer);

/**
 * Sets the data pair (data and it's size) to a VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer to which the data pair is set.
 * @param data Pointer to the data.
 * @param datasize Size in bytes of the data.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_set_data_pair (VisBuffer *buffer, void *data, visual_size_t datasize);

/**
 * Sets the size of the data to a VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer to which the data size is set.
 * @param datasize Size in bytes of the data.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_set_size (VisBuffer *buffer, visual_size_t datasize);

/**
 * Sets the data to a VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer to which the data is set.
 * @param data Pointer to the data.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_set_data (VisBuffer *buffer, void *data);

/**
 * Allocates the data for a VisBuffer, the amount of bytes allocated is defined by the data size
 * that is set to the VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer for which the data is allocated.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_allocate_data (VisBuffer *buffer);

/**
 * Gets pointer to the data that is encapsulated by the VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer of which the data is requested.
 *
 * @return Pointer to the data on success, NULL on failure.
 */
LV_API void *visual_buffer_get_data (VisBuffer *buffer);

/**
 * Gets pointer to the data that is encapsulated by the VisBuffer using an
 * offset.
 *
 * @param buffer Pointer to the VisBuffer of which the data is requested.
 * @param byteoffset The offset in N bytes.
 *
 * @return Pointer to the data on success, NULL on failure.
 */
LV_API void *visual_buffer_get_data_offset (VisBuffer *buffer, int byteoffset);

/**
 * Gets the size in bytes of a VisBuffer.
 *
 * @param buffer Pointer to the VisBuffer of which the size is requested.
 *
 * @return Size in bytes on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API visual_size_t visual_buffer_get_size (VisBuffer *buffer);

/**
 * Return if the internal buffer is flagged as allocated or not.
 *
 * @param buffer Pointer to the VisBuffer of which the allocated flag is requested.
 *
 * @return TRUE or FALSE depending on the allocated flag on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_get_allocated (VisBuffer *buffer);

/**
 * Clones a VisBuffer, this will copy the data.
 *
 * @param dest Pointer to the destination VisBuffer.
 * @param src Pointer to the source VisBuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_clone (VisBuffer *dest, VisBuffer *src);

/**
 * Clones a VisBuffer in the form of a newly allocated structure, this will copy the data.
 *
 * @param src Pointer to the source VisBuffer.
 *
 * @return Newly allocated cloned VisBuffer on success, NULL on failure.
 */
LV_API VisBuffer *visual_buffer_clone_new (VisBuffer *src);

/**
 * Copies all the data contained by the VisBuffer into dest.
 *
 * @param src Pointer to the VisBuffer which contains the source data.
 * @param dest Pointer to the buffer in which all the data is copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_copy_data_to (VisBuffer *src, void *dest);

/**
 * Copies data into the VisBuffer from another VisBuffer starting at byteoffset. The copy is
 * NOT atomic.
 *
 * @param dest Pointer to the destination VisBuffer.
 * @param src Pointer to the source VisBuffer.
 * @param byteoffset The offset in the destination VisBuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_put (VisBuffer *dest, VisBuffer *src, int byteoffset);

/**
 * Copies data into the VisBuffer using a data pointer and it's size. The copy is NOT atomic.
 *
 * @see visual_buffer_put
 *
 * @param dest Pointer to the destination VisBuffer.
 * @param data Pointer to the data.
 * @param size The size of the data.
 * @param byteoffset The offset in the destination VisBuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_put_data (VisBuffer *dest, void *data, visual_size_t size, int byteoffset);

/**
 * Copies data into the VisBuffer from another VisBuffer starting at byteoffset.
 * The copy is atomic, everything or nothing is copied.
 *
 * @param dest Pointer to the destination VisBuffer.
 * @param src Pointer to the source VisBuffer.
 * @param byteoffset The offset in the destination VisBuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_put_atomic (VisBuffer *dest, VisBuffer *src, int byteoffset);

/**
 * Copies data into the VisBuffer using a data pointer and it's size. The copy is atomic,
 * everything or nothing is copied.
 *
 * @see visual_buffer_put_atomic
 *
 * @param dest Pointer to the destination VisBuffer.
 * @param data Pointer to the data.
 * @param size The size of the data.
 * @param byteoffset The offset in the destination VisBuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_put_data_atomic (VisBuffer *dest, void *data, visual_size_t size, int byteoffset);

/**
 * Appends a VisBuffer to a VisBuffer, this will also reallocate the buffer so it's big enough.
 *
 * @param dest Pointer to the VisBuffer to which the other VisBuffer is appended.
 * @param src Pointer to the VisBuffer that is appended.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_append (VisBuffer *dest, VisBuffer *src);

/**
 * Appends data to a VisBuffer, this will also reallocate the buffer so it's big enough.
 *
 * @param dest Pointer to the VisBufer to which the data is appended.
 * @param data Pointer to the data that is appended.
 * @param size The size in bytes of the data that is appended to the VisBuffer
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL or -VISUAL_ERROR_NULL on failure.
 */
LV_API int visual_buffer_append_data (VisBuffer *dest, void *data, visual_size_t size);

/**
 * Fills the buffer with a value (byte wise).
 *
 * @param buffer Pointer to the VisBuffer which is to be filled with one byte value.
 * @param value The value that is used to fill the buffer with.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_BUFFER_NULL on failure.
 */
LV_API int visual_buffer_fill (VisBuffer *buffer, char value);

/**
 * Fills the buffer with a pattern of data. This can be used to fill a buffer with a structure, floats
 * whatever. It's assumed that the buffer is rightly allocated, when this is not the case it can happen
 * that only a part of the pattern is copied.
 *
 * @param buffer Pointer to the VisBuffer which is to be filled with a pattern.
 * @param data The data pattern.
 * @param size The size of the pattern
 */
LV_API int visual_buffer_fill_with_pattern (VisBuffer *buffer, void *data, visual_size_t size);

/**
 * A standard destroyer that can be set on VisBuffers. This destroyer simply frees the memory
 * that is allocated.
 *
 * @param buffer Pointer to the VisBuffer that owns the memory that needs to be freed.
 */
LV_API void visual_buffer_destroyer_free (VisBuffer *buffer);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_BUFFER_H */
