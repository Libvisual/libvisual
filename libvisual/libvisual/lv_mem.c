#include <string.h>

#include "lv_mem.h"
#include "lv_log.h"


void *visual_mem_malloc0 (size_t nbytes)
{
	void *buf = malloc (nbytes);

	if (buf == NULL) {
		visual_log (VISUAL_LOG_ERROR, "Cannot get more memory");
		return NULL;
	}
	
	memset (buf, 0, nbytes);

	return buf;
}

