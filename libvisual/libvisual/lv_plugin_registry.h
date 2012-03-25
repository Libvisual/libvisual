#ifndef _LV_PLUGIN_REGISTRY_H
#define _LV_PLUGIN_REGISTRY_H

#include <libvisual/lv_defines.h>

VISUAL_BEGIN_DECLS

/**
 * Adds extra plugin registry paths.
 *
 * @param pathadd A string containing a path where plugins are located.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_LIBVISUAL_NO_PATHS on failure.
 */
int visual_init_path_add (const char *path);

int visual_plugin_registry_initialize (void);
int visual_plugin_registry_deinitialize (void);

VISUAL_END_DECLS

#endif /*_LV_PLUGIN_REGISTRY_H */
