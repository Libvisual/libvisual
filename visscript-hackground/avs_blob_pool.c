#include <stdio.h>
#include <stdlib.h>

#include "avs.h"


/*	Pop a single element from the blob pool.
 *	This function doesn't actually free the memory of th element.
 *	But adds to the Available Element Pool instead.
 *	After popping the passed element it should NOT be used anymore.
 *
 *	@param bp Blob Pool Manager.
 *	@param elem Element to pop.
 *
 *	@note The element given by @a elem SHOULD NOT be used anymore after popping.
 *
 *	@return On success VISUAL_OK, VISUAL_ERROR_GENERAL on failure.
 */
int avs_blob_pool_pop(AvsBlobPool *bp, void *elem)
{
	AvsBlobPoolElement *bpe = (AvsBlobPoolElement *)(elem - sizeof(AvsBlobPoolElement));

	/* Add element to the Available Element Pool */
	bpe->next = bp->pool;
	bp->pool = bpe;
	return VISUAL_OK;
}

/* 
 * 	Push a single element onto the blob pool.
 *
 * 	@param bp Blob Pool.
 *
 * 	@returns Pointer to newly allocated memory blob on success,
 * 	NULL when the maximum number of allowable elements has been reached.
 */
void * avs_blob_pool_push(AvsBlobPool *bp)
{
	AvsBlobPoolElement *elem;

	/* First try to recycle used blob elements.
	 * By lookingo into the Available Element Pool */
	if (bp->pool) {
		elem = bp->pool;
		bp->pool = elem->next;
	} else {
		/* No used blob elements available to recycle. 
		 * Allocate a new blob element */
		elem = visual_mem_malloc0(bp->elem_size);
	}

	if (!elem)
		return NULL;

	memset(elem->data, 0, bp->elem_size - sizeof(AvsBlobPoolElement));
	return elem->data;
}

static int pool_dtor(VisObject * object)
{
	AvsBlobPool *bp = AVS_BLOB_POOL(object);
	visual_object_unref(VISUAL_OBJECT(bp->blob));
	return VISUAL_OK;
}

/**
 *	Create a new blob pool.
 *	Uses avs blob manager for memory management.
 *
 * 	@param elem_size Size of precisely one element.
 *
 * 	@see avs_blob_manager_new
 * 	@returns A newly allocated blob pool manager on success, NULL on failure.
 */
AvsBlobPool *avs_blob_pool_new(int elem_size)
{
	AvsBlobPool *bp;

	bp = visual_mem_new0(AvsBlobPool, 1);
	visual_object_initialize(VISUAL_OBJECT(bp), TRUE, pool_dtor);

	/* Set element size */
	bp->elem_size = elem_size + sizeof(AvsBlobPoolElement);
	
	/* Allocate blob manager */
	bp->blob = avs_blob_manager_new();

	/* Initialize AEP */
	bp->pool = NULL;

	return bp;
}

