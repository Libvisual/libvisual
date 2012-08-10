/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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
