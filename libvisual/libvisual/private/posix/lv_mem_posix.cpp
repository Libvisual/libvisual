#define _POSIX_C_SOURCE 200112L

#Include "config.h"
#include "lv_mem.h"
#include <cstdlib>

void* visual_mem_malloc_aligned (visual_size_t size, visual_size_t alignment)
{
    return memalign (alignment, size);
}

void visual_mem_free_aligned (void* ptr)
{
    free (ptr);
}
