#ifndef _LV_MEM_H
#define _LV_MEM_H

#include <stdlib.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Allocates @a nbytes of memory initialized to 0.
 *
 * @return On success, a pointer to a new allocated memory initialized
 * to 0 of size @a nbytes, on error, program is aborted, so you never need
 * to check the return value.
 */
void *visual_mem_malloc0 (size_t nbytes);

/**
 * Convenient macro to request @a n_structs structures of type @a struct_type
 * initialized to 0.
 */
#define visual_mem_new0(struct_type, n_structs)           \
    ((struct_type *) visual_mem_malloc0 (((size_t) sizeof (struct_type)) * ((size_t) (n_structs))))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_MEM_H */
