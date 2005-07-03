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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include <lvconfig.h>
#include "lv_ringbuffer.h"
#include "lv_log.h"
#include "lv_mem.h"

static int ringbuffer_dtor (VisObject *object);

static int ringbuffer_dtor (VisObject *object)
{

	return VISUAL_OK;
}

/**
 * @defgroup VisRingBuffer VisRingBuffer
 * @{
 */

/**
 * Creates a new VisRingBuffer structure.
 * The VisRingBuffer system is a double linked ringbuffer implementation.
 *
 * @return A newly allocated VisRingBuffer.
 */
VisRingBuffer *visual_ringbuffer_new ()
{
	VisRingBuffer *ringbuffer;

	ringbuffer = visual_mem_new0 (VisRingBuffer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (ringbuffer), TRUE, ringbuffer_dtor);

//	ringbuffer->destroyer = destroyer;

	return ringbuffer;
}

/**
 * @}
 */

