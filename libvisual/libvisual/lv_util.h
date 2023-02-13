#ifndef _LV_UTIL_H
#define _LV_UTIL_H

#include <libvisual/lv_defines.h>

LV_BEGIN_DECLS

LV_API LV_NODISCARD char *visual_strdup (const char *s);

LV_API const char *visual_truncate_path (const char* filename, unsigned int parts);

LV_END_DECLS

#endif /* _LV_UTIL_H */
