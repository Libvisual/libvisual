#include <string.h>

#include "lv_mem.h"
#include "lv_log.h"

/**
 * @defgroup VisMem VisMem
 * @{
 */

/**
 * Allocates @a nbytes of memory initialized to 0.
 *
 * @param nbytes N bytes of mem requested to be allocated.
 * 
 * @return On success, a pointer to a new allocated memory initialized
 * to 0 of size @a nbytes, on error, program is aborted, so you never need
 * to check the return value.
 */
void *visual_mem_malloc0 (visual_size_t nbytes)
{
	void *buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, "Cannot get %" VISUAL_SIZE_T_FORMAT " bytes of memory", nbytes);
		return NULL;
	}
	
	memset (buf, 0, nbytes);

	return buf;
}

/**
 * Frees allocated memory.
 *
 * @param ptr Frees memory to which ptr points to.
 *
 * @return 0 on succes -1 on error.
 */
int visual_mem_free (void *ptr)
{
	visual_log_return_val_if_fail (ptr != NULL, -1);

	free (ptr);

	return 0;
}
	

/**
 * @}
 */

