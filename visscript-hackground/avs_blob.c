#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avs.h"

/**
 * 	Destroy blob manager and all its items 
 */
static int blob_manager_dtor(VisObject * object)
{
	AvsBlobManager *bm = AVS_BLOB_MANAGER(object);
	AvsBlob *blob, *next;

	/* Free all associated blobs */
	for (blob=bm->pool; blob != NULL; blob = next) {
		next = blob->next;
		visual_mem_free(blob);
	}

	return VISUAL_OK;
}

/**
 * 	Free a previously allocated blob
 *
 * 	@param bm Blob Manager.
 * 	@param data Data pointer of blob to deallocate.
 * 		    The pointer that has been returned by avs_blob_new().
 *
 * 	@see avs_blob_new
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_blob_destroy(AvsBlobManager *bm, void *data)
{
	AvsBlob *blob = (AvsBlob *) (data - sizeof(AvsBlob));

	/* Relink blob pool, remove 'blob' out of the chain */
	if (blob->prev)
		blob->prev->next = blob->next;
	else 
		bm->pool = blob->next;

	/* Free blob */
	visual_mem_free(blob);
	return VISUAL_OK;
}


/**
 * 	Allocate a new blob
 *
 * 	@param bm Blob Manager.
 * 	@param nbytes Number of bytes to allocate for blob.
 *
 * 	@return Pointer to newly created blob on success, NULL on failure.
 */
void * avs_blob_new(AvsBlobManager *bm, visual_size_t nbytes)
{
	AvsBlob *blob;

	/* Allocate memory */
	nbytes += sizeof(AvsBlob);
	blob = visual_mem_malloc0(nbytes);

	if (blob == NULL)
		return NULL;

	/* Add blob to the Blob Manager's pool */
	if (bm->pool) {
		blob->next = bm->pool;
		blob->prev = NULL;
		bm->pool->prev = blob;
		bm->pool = blob;
	} else {
		blob->next = NULL;
		blob->prev = NULL;
		bm->pool = blob;
	}

	return blob->data;
}

/**
 * 	Initialize a static AvsBlobManager
 *
 * 	@param bm Blob Manager to initialize.
 *
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_blob_manager_init(AvsBlobManager *bm)
{
	/* Initialize object */
	visual_object_initialize(VISUAL_OBJECT(bm), FALSE, blob_manager_dtor);

	/* Initialize blob pool */
	bm->pool = NULL;
	
	return VISUAL_OK;
}

/**
 *	Create a new blob manager.
 *
 *	@return A pointer to a newly created AvsBlobManager on success,
 *		NULL on failure.
 */
AvsBlobManager * avs_blob_manager_new(void)
{
	AvsBlobManager *bm;

	/* Allocate memory */
	bm = visual_mem_new0(AvsBlobManager, 1);
	visual_object_initialize(VISUAL_OBJECT(bm), TRUE, blob_manager_dtor);

	/* Initialize blob pool */
	bm->pool = NULL;

	return bm;
}


