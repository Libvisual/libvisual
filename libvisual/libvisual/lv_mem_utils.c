/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_mem_utils.c,v 1.1 2006-09-19 18:28:51 synap Exp $
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

#include <string.h>
#include <stdlib.h>

#include "lv_mem_utils.h"
#include "lv_common.h"
#include "lv_log.h"
#include "lv_error.h"

/**
 * @defgroup VisMem VisMem
 * @{
 */

VisMemChunk *visual_mem_chunk_new (VisMemChunk *parent, visual_size_t object_size, visual_size_t entries)
{
	VisMemChunk *mchunk;

	mchunk = visual_mem_new0 (VisMemChunk, 1);

	visual_mem_chunk_chain (parent, mchunk);

	mchunk->data = visual_mem_malloc (object_size * entries);

	return mchunk;
}

int visual_mem_chunk_chain (VisMemChunk *parent, VisMemChunk *child)
{
	if (parent != NULL)
		parent->next = child;

	return VISUAL_OK;
}

int visual_mem_chunk_free (VisMemChunk *mchunk, int free_data)
{
	visual_log_return_val_if_fail (mchunk != NULL, -VISUAL_ERROR_MEM_CHUNK_NULL);

	if (free_data > 0)
		visual_mem_chunk_free_data (mchunk);

	visual_mem_free (mchunk);

	return VISUAL_OK;
}

int visual_mem_chunk_free_chain (VisMemChunk *mchunk, int free_data)
{
	VisMemChunk *next = mchunk;

	while (next != NULL) {
		visual_mem_chunk_free (next, free_data);

		next = mchunk->next;
	}

	return VISUAL_OK;
}

int visual_mem_chunk_free_data (VisMemChunk *mchunk)
{
	visual_log_return_val_if_fail (mchunk != NULL, -VISUAL_ERROR_MEM_CHUNK_NULL);

	if (mchunk->data != NULL) {
		visual_mem_free (mchunk->data);
	}

	return VISUAL_OK;
}

VisMemPointerStack *visual_mem_pointerstack_new (visual_size_t stack_size)
{
	VisMemPointerStack *mpstack;

	mpstack = visual_mem_new0 (VisMemPointerStack, 1);

	visual_mem_pointerstack_resize (mpstack, stack_size);

	return mpstack;
}

int visual_mem_pointerstack_free (VisMemPointerStack *mpstack)
{
	visual_log_return_val_if_fail (mpstack != NULL, -VISUAL_ERROR_MEM_POINTERSTACK_NULL);

	if (mpstack->stack != NULL)
		visual_mem_free (mpstack->stack);

	visual_mem_free (mpstack);

	return VISUAL_OK;
}

int visual_mem_pointerstack_resize (VisMemPointerStack *mpstack, visual_size_t stack_size)
{
	visual_log_return_val_if_fail (mpstack != NULL, -VISUAL_ERROR_MEM_POINTERSTACK_NULL);

	mpstack->stack = visual_mem_realloc (mpstack->stack, stack_size * sizeof (void *));
	mpstack->size = stack_size;


	return VISUAL_OK;
}

int visual_mem_pointerstack_push_all (VisMemPointerStack *mpstack, visual_size_t object_size, visual_size_t entries, void *ptr)
{
	int i;

	visual_log_return_val_if_fail (mpstack != NULL, -VISUAL_ERROR_MEM_POINTERSTACK_NULL);

	for (i = 0; i < entries ; i++) {
		if (visual_mem_pointerstack_push (mpstack, ((char *) ptr) + (object_size * i)) == -VISUAL_ERROR_MEM_POINTERSTACK_FULL) {
			return -VISUAL_ERROR_MEM_POINTERSTACK_FULL;
		}
	}

	return VISUAL_OK;
}

int visual_mem_pointerstack_push (VisMemPointerStack *mpstack, void *ptr)
{
	visual_log_return_val_if_fail (mpstack != NULL, -VISUAL_ERROR_MEM_POINTERSTACK_NULL);
	visual_log_return_val_if_fail (mpstack->stack != NULL, -VISUAL_ERROR_NULL);

	if (mpstack->stackptr < mpstack->size) {
		mpstack->stack[mpstack->stackptr] = ptr;
		mpstack->stackptr++;
	} else {
		return -VISUAL_ERROR_MEM_POINTERSTACK_FULL;
	}

	return VISUAL_OK;
}

void *visual_mem_pointerstack_peek (VisMemPointerStack *mpstack)
{
	visual_log_return_val_if_fail (mpstack != NULL, NULL);
	visual_log_return_val_if_fail (mpstack->stack != NULL, NULL);

	if (mpstack->stackptr <= 0)
		return NULL;

	return mpstack->stack[mpstack->stackptr - 1];
}

void *visual_mem_pointerstack_pop (VisMemPointerStack *mpstack)
{
	visual_log_return_val_if_fail (mpstack != NULL, NULL);
	visual_log_return_val_if_fail (mpstack->stack != NULL, NULL);

	if (mpstack->stackptr <= 0)
		return NULL;

	mpstack->stackptr--;

	return mpstack->stack[mpstack->stackptr];
}

/**
 * Gives the number of maximum elements on the pointer stack.
 *
 * @param mpstack Pointer to the VisMemPointerStack of which the number of maximum elements on the
 *	stack is requested.
 *
 * @return The number of maximum elements on the stack, -VISUAL_ERROR_MEM_POINTERSTACK_NULL on failure.
 */
visual_size_t visual_mem_pointerstack_size (VisMemPointerStack *mpstack)
{
	visual_log_return_val_if_fail (mpstack != NULL, -VISUAL_ERROR_MEM_POINTERSTACK_NULL);

	return mpstack->size;
}

/**
 * Gives the number of elements on the pointer stack.
 *
 * @param mpstack Pointer to the VisMemPointerStack of which the number of elements on the
 *	stack is requested.
 *
 * @return The number of elements on the stack, -VISUAL_ERROR_MEM_POINTERSTACK_NULL on failure.
 */
int visual_mem_pointerstack_count (VisMemPointerStack *mpstack)
{
	visual_log_return_val_if_fail (mpstack != NULL, -VISUAL_ERROR_MEM_POINTERSTACK_NULL);

	return mpstack->stackptr;
}

/**
 * @}
 */

