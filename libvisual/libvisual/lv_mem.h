#ifndef _LV_MEM_H
#define _LV_MEM_H

#include <libvisual/lvconfig.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined(__GNUC__) && !defined(VISUAL_OS_WIN32)
void *visual_mem_malloc0 (visual_size_t nbytes) __attribute_malloc__;
#else
void *visual_mem_malloc0 (visual_size_t nbytes);
#endif /* __GNUC__ */

int visual_mem_free (void *ptr);

/**
 * @ingroup VisMem
 * 
 * Convenient macro to request @a n_structs structures of type @a struct_type
 * initialized to 0.
 */
#define visual_mem_new0(struct_type, n_structs)           \
    ((struct_type *) visual_mem_malloc0 (((visual_size_t) sizeof (struct_type)) * ((visual_size_t) (n_structs))))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_MEM_H */
