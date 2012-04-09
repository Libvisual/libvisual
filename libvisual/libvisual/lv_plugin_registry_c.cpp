#include "config.h"
#include "lv_plugin_registry.h"
#include "lv_error.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

  int visual_plugin_registry_initialize (void)
  {
      LV::PluginRegistry::init();

      return VISUAL_OK;
  }

  int visual_plugin_registry_deinitialize (void)
  {
      LV::PluginRegistry::deinit ();

      return VISUAL_OK;
  }

  int visual_plugin_registry_add_path (const char *path)
  {
      LV::PluginRegistry::instance()->add_path (path);

      return VISUAL_OK;
  }

  int visual_plugin_registry_has_plugin (VisPluginType type, const char *name)
  {
      return LV::PluginRegistry::instance()->has_plugin (type, name);
  }

} // C extern
