/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_buffer.c,v 1.9 2006/01/22 13:23:37 synap Exp $
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

#include <config.h>
#include "lv_buffer.h"
#include "lv_common.h"

static int buffer_dtor (VisObject *object);

static int buffer_dtor (VisObject *object)
{
	VisBuffer *buffer = VISUAL_BUFFER (object);

	if (buffer->destroyer != NULL)
		buffer->destroyer (buffer);

	buffer->data = NULL;

	return VISUAL_OK;
}

VisBuffer *visual_buffer_new ()
{
	VisBuffer *buffer;

	buffer = visual_mem_new0 (VisBuffer, 1);

	visual_buffer_init (buffer, NULL, 0, NULL);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (buffer), TRUE);
	visual_object_ref (VISUAL_OBJECT (buffer));

	return buffer;
}

int visual_buffer_init (VisBuffer *buffer, void *data, visual_size_t datasize, VisBufferDestroyerFunc destroyer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (buffer));
	visual_object_set_dtor (VISUAL_OBJECT (buffer), buffer_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (buffer), FALSE);

	/* Set the VisBuffer data */
	visual_buffer_set_data_pair (buffer, data, datasize);
	visual_buffer_set_destroyer (buffer, destroyer);

	buffer->allocated = FALSE;

	return VISUAL_OK;
}

VisBuffer *visual_buffer_new_with_buffer (void *data, visual_size_t datasize, VisBufferDestroyerFunc destroyer)
{
	VisBuffer *buffer = visual_buffer_new ();

	visual_buffer_set_data_pair (buffer, data, datasize);
	visual_buffer_set_destroyer (buffer, destroyer);

	return buffer;
}

VisBuffer *visual_buffer_new_allocate (visual_size_t datasize, VisBufferDestroyerFunc destroyer)
{
	VisBuffer *buffer = visual_buffer_new ();

	visual_buffer_set_size (buffer, datasize);
	visual_buffer_set_destroyer (buffer, destroyer);
	visual_buffer_allocate_data (buffer);

	return buffer;
}

int visual_buffer_init_allocate (VisBuffer *buffer, visual_size_t datasize, VisBufferDestroyerFunc destroyer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_buffer_init (buffer, NULL, 0, NULL);

	visual_buffer_set_size (buffer, datasize);
	visual_buffer_set_destroyer (buffer, destroyer);
	visual_buffer_allocate_data (buffer);

	return VISUAL_OK;
}

int visual_buffer_destroy_content (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (buffer->destroyer != NULL)
		buffer->destroyer (buffer);

	visual_buffer_set_data_pair (buffer, NULL, 0);

	return VISUAL_OK;
}

int visual_buffer_set_destroyer (VisBuffer *buffer, VisBufferDestroyerFunc destroyer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	buffer->destroyer = destroyer;

	return VISUAL_OK;
}

VisBufferDestroyerFunc visual_buffer_get_destroyer (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, NULL);

	return buffer->destroyer;
}

int visual_buffer_set_data_pair (VisBuffer *buffer, void *data, visual_size_t datasize)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_buffer_set_data (buffer, data);
	visual_buffer_set_size (buffer, datasize);

	return VISUAL_OK;
}

int visual_buffer_set_size (VisBuffer *buffer, visual_size_t datasize)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	buffer->datasize = datasize;

	return VISUAL_OK;
}

int visual_buffer_set_data (VisBuffer *buffer, void *data)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	buffer->data = data;

	buffer->allocated = FALSE;

	return VISUAL_OK;
}

int visual_buffer_allocate_data (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (buffer->datasize > 0)
		buffer->data = visual_mem_malloc0 (buffer->datasize);

	buffer->allocated = TRUE;

	return VISUAL_OK;
}

void *visual_buffer_get_data (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, NULL);

	return buffer->data;
}

void *visual_buffer_get_data_offset (VisBuffer *buffer, int byteoffset)
{
	visual_return_val_if_fail (buffer != NULL, NULL);

	return (unsigned char *) (buffer->data) + byteoffset;
}

visual_size_t visual_buffer_get_size (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	return buffer->datasize;
}

int visual_buffer_get_allocated (VisBuffer *buffer)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	return buffer->allocated;
}

int visual_buffer_clone (VisBuffer *dest, VisBuffer *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_buffer_set_size (dest, visual_buffer_get_size (src));
	visual_buffer_allocate_data (dest);

	if (dest->data != NULL) {
		visual_mem_copy (dest->data,
				visual_buffer_get_data (src),
				visual_buffer_get_size (src));
	}

	visual_buffer_set_destroyer (dest, visual_buffer_get_destroyer (src));

	return VISUAL_OK;
}

VisBuffer *visual_buffer_clone_new (VisBuffer *src)
{
	VisBuffer *dest;

	visual_return_val_if_fail (src != NULL, NULL);

	dest = visual_buffer_new ();

	visual_buffer_clone (dest, src);

	return dest;
}

int visual_buffer_copy_data_to (VisBuffer *src, void *dest)
{
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_NULL);

	visual_mem_copy (dest, src->data, src->datasize);

	return VISUAL_OK;
}

int visual_buffer_put (VisBuffer *dest, VisBuffer *src, int byteoffset)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);

	return visual_buffer_put_data (dest, src->data, src->datasize, byteoffset);
}

int visual_buffer_put_data (VisBuffer *dest, void *data, visual_size_t size, int byteoffset)
{
	int amount;

	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (data != NULL, -VISUAL_ERROR_NULL);
	visual_return_val_if_fail (byteoffset < visual_buffer_get_size (dest), -VISUAL_ERROR_BUFFER_OUT_OF_BOUNDS);

	amount = size;

	if (byteoffset + size > dest->datasize)
		amount = dest->datasize - byteoffset;

	visual_mem_copy ((uint8_t *) dest->data + byteoffset, data, amount);

	return VISUAL_OK;
}

int visual_buffer_put_atomic (VisBuffer *dest, VisBuffer *src, int byteoffset)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);

	return visual_buffer_put_data_atomic (dest, src->data, src->datasize, byteoffset);
}

int visual_buffer_put_data_atomic (VisBuffer *dest, void *data, visual_size_t size, int byteoffset)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);

	if (byteoffset + size > dest->datasize)
		return -VISUAL_ERROR_BUFFER_OUT_OF_BOUNDS;

	return visual_buffer_put_data (dest, data, size, byteoffset);
}

int visual_buffer_append (VisBuffer *dest, VisBuffer *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_BUFFER_NULL);

	dest->data = visual_mem_realloc (dest->data, dest->datasize + src->datasize);

	return visual_buffer_put (dest, src, dest->datasize);
}

int visual_buffer_append_data (VisBuffer *dest, void *data, visual_size_t size)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (data != NULL, -VISUAL_ERROR_NULL);

	dest->data = visual_mem_realloc (dest->data, dest->datasize + size);

	return visual_buffer_put_data (dest, data, size, dest->datasize);
}

int visual_buffer_fill (VisBuffer *buffer, char value)
{
	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);

	visual_mem_set (buffer->data, value, buffer->datasize);

	return VISUAL_OK;
}

int visual_buffer_fill_with_pattern (VisBuffer *buffer, void *data, visual_size_t size)
{
	visual_size_t offset = 0;

	visual_return_val_if_fail (buffer != NULL, -VISUAL_ERROR_BUFFER_NULL);
	visual_return_val_if_fail (data != NULL, -VISUAL_ERROR_NULL);

	while (offset < buffer->datasize) {
		visual_buffer_put_data (buffer, data, size, offset);

		offset += size;
	}

	return VISUAL_OK;
}

void visual_buffer_destroyer_free (VisBuffer *buffer)
{
	if (buffer->data != NULL)
		visual_mem_free (buffer->data);

	buffer->data = NULL;
}
