#include <string.h>

#include "lv_mem.h"
#include "lv_log.h"


/*void *visual_mem_malloc0 (size_t nbytes)*/
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

