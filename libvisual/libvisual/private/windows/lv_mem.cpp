#include "config.h"
#include "lv_mem.h"
#include <malloc.h>

void* visual_mem_malloc_aligned (visual_size_t size, visual_size_t alignment)
{
#ifndef VISUAL_WITH_MINGW
    return _aligned_malloc (size, alignment);
#else
    return __mingw_aligned_malloc (size, alignment);
#endif
}

void visual_mem_free_aligned (void* block)
{
#ifndef VISUAL_WITH_MINGW
    return _aligned_free (block);
#else
    return __mingw_aligned_free (block);
#endif
}
