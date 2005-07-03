/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#include <libvisual/lv_common.h>
#include <libvisual/lv_list.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_RINGBUFFER(obj)				(VISUAL_CHECK_CAST ((obj), VisRingBuffer))
#define VISUAL_RINGBUFFER_ENTRY(obj)			(VISUAL_CHECK_CAST ((obj), VisRingBufferEntry))

typedef struct _VisRingBufferEntry VisRingBufferEntry;
typedef struct _VisRingBuffer VisRingBuffer;

/**
 * The VisRingBufferEntry data structure is an entry within the ringbuffer.
 */
struct _VisRingBufferEntry {
//	type; -> naieve byte buffer, function based ringbuffer.
//	getdatafunc hiero. met eigen context object, hang aan de context de sample.
};

/**
 * The VisRingBuffer data structure holding the ringbuffer.
 */
struct _VisRingBuffer {
	VisObject		 object;	/**< The VisObject data. */

	VisList			 entries;	/**< The ring buffer entries list. */
};

/* prototypes */
VisRingBuffer *visual_ringbuffer_new (void);
int visual_ringbuffer_init (VisRingBuffer *ringbuffer);

int visual_ringbuffer_get_data (VisRingBuffer *ringbuffer, void *data, int nbytes);
int visual_ringbuffer_get_data_without_wrap (VisRingBuffer *ringbuffer, void *data, int nbytes);

void *visual_ringbuffer_get_data_new (VisRingBuffer *ringbuffer, int *nbytes);
void *visual_ringbuffer_get_data_new_without_wrap (VisRingBuffer *ringbuffer, int *nbytes);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_RINGBUFFER_H */
