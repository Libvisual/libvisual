/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_buffer.h,v 1.10 2006-09-19 18:28:51 synap Exp $
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

#include <libvisual/lv_common.h>

VISUAL_BEGIN_DECLS

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

	int			offset;		/**< Offset in data buffer in N bytes. */
};

/* prototypes */
VisBuffer *visual_buffer_new (void);
int visual_buffer_init (VisBuffer *buffer, void *data, visual_size_t datasize, VisBufferDestroyerFunc destroyer);
VisBuffer *visual_buffer_new_with_buffer (void *data, visual_size_t datasize, VisBufferDestroyerFunc destroyer);
VisBuffer *visual_buffer_new_allocate (visual_size_t datasize, VisBufferDestroyerFunc destroyer);
int visual_buffer_init_allocate (VisBuffer *buffer, visual_size_t datasize, VisBufferDestroyerFunc destroyer);
int visual_buffer_destroy_content (VisBuffer *buffer);

int visual_buffer_set_destroyer (VisBuffer *buffer, VisBufferDestroyerFunc destroyer);
VisBufferDestroyerFunc visual_buffer_get_destroyer (VisBuffer *buffer);

int visual_buffer_set_data_pair (VisBuffer *buffer, void *data, visual_size_t datasize);
int visual_buffer_set_size (VisBuffer *buffer, visual_size_t datasize);
int visual_buffer_set_data (VisBuffer *buffer, void *data);
int visual_buffer_set_offset_sequential (VisBuffer *buffer, int offset);
int visual_buffer_reset_offset_sequential (VisBuffer *buffer);
int visual_buffer_allocate_data (VisBuffer *buffer);

void *visual_buffer_get_data (VisBuffer *buffer);
void *visual_buffer_get_data_offset (VisBuffer *buffer, int byteoffset);
void *visual_buffer_get_data_offset_sequential (VisBuffer *buffer);
int visual_buffer_get_size (VisBuffer *buffer);
int visual_buffer_get_offset_sequential (VisBuffer *buffer);
int visual_buffer_get_available_sequential (VisBuffer *buffer);


int visual_buffer_get_allocated (VisBuffer *buffer);

int visual_buffer_clone (VisBuffer *dest, VisBuffer *src);
int visual_buffer_clone_with_attributes (VisBuffer *dest, VisBuffer *src);
VisBuffer *visual_buffer_clone_new (VisBuffer *src);
VisBuffer *visual_buffer_clone_new_with_attributes (VisBuffer *src);

int visual_buffer_copy_data_to (VisBuffer *src, void *dest);
int visual_buffer_copy_data_to_length (VisBuffer *src, void *dest, visual_size_t size);
int visual_buffer_copy_data_to_offset_length (VisBuffer *src, void *dest, int byteoffset, visual_size_t size);

int visual_buffer_put (VisBuffer *dest, VisBuffer *src, int byteoffset);
int visual_buffer_put_length (VisBuffer *dest, VisBuffer *src, int byteoffset, visual_size_t size);
int visual_buffer_put_data (VisBuffer *dest, void *data, visual_size_t size, int byteoffset);
int visual_buffer_put_atomic (VisBuffer *dest, VisBuffer *src, int byteoffset);
int visual_buffer_put_data_atomic (VisBuffer *dest, void *data, visual_size_t size, int byteoffset);
int visual_buffer_put_sequential (VisBuffer *dest, VisBuffer *src);
int visual_buffer_put_data_sequential (VisBuffer *dest, void *data, visual_size_t size);
int visual_buffer_put_sequential_atomic (VisBuffer *dest, VisBuffer *src);
int visual_buffer_put_data_sequential_atomic (VisBuffer *dest, void *data, visual_size_t size);

int visual_buffer_append (VisBuffer *dest, VisBuffer *src);
int visual_buffer_append_data (VisBuffer *dest, void *data, visual_size_t size);

int visual_buffer_fill (VisBuffer *buffer, char value);
int visual_buffer_fill_with_pattern (VisBuffer *buffer, void *data, visual_size_t size);

void visual_buffer_destroyer_free (VisBuffer *buffer);

VISUAL_END_DECLS

#endif /* _LV_BUFFER_H */
