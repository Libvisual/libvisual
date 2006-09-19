/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem_utils.h,v 1.1 2006-09-19 18:28:51 synap Exp $
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

#ifndef _LV_MEM_UTILS_H
#define _LV_MEM_UTILS_H

#include <libvisual/lvconfig.h>

#include <libvisual/lv_defines.h>

VISUAL_BEGIN_DECLS

typedef struct _VisMemChunk VisMemChunk;
typedef struct _VisMemPointerStack VisMemPointerStack;

struct _VisMemChunk {
	VisMemChunk	*next;

	void		*data;
};

struct _VisMemPointerStack {
	int		 stackptr;
	visual_size_t	 size;

	void		**stack;
};

/* prototypes */
VisMemChunk *visual_mem_chunk_new (VisMemChunk *parent, visual_size_t object_size, visual_size_t entries);

int visual_mem_chunk_chain (VisMemChunk *parent, VisMemChunk *child);

int visual_mem_chunk_free (VisMemChunk *mchunk, int free_data);
int visual_mem_chunk_free_chain (VisMemChunk *mchunk, int free_data);

int visual_mem_chunk_free_data (VisMemChunk *mchunk);


VisMemPointerStack *visual_mem_pointerstack_new (visual_size_t stack_size);
int visual_mem_pointerstack_free (VisMemPointerStack *mpstack);
int visual_mem_pointerstack_resize (VisMemPointerStack *mpstack, visual_size_t stack_size);

int visual_mem_pointerstack_push_all (VisMemPointerStack *mpstack, visual_size_t object_size, visual_size_t entries, void *ptr);
int visual_mem_pointerstack_push (VisMemPointerStack *mpstack, void *ptr);
void *visual_mem_pointerstack_peek (VisMemPointerStack *mpstack);
void *visual_mem_pointerstack_pop (VisMemPointerStack *mpstack);

visual_size_t visual_mem_pointerstack_size (VisMemPointerStack *mpstack);
int visual_mem_pointerstack_count (VisMemPointerStack *mpstack);


VISUAL_END_DECLS

#endif /* _LV_MEM_UTILS_H */
