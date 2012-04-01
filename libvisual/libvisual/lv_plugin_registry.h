#ifndef _LV_PLUGIN_REGISTRY_H
#define _LV_PLUGIN_REGISTRY_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>

#ifdef __cplusplus

#include <string>
#include <memory>

namespace LV {

  class PluginRegistry
  {
  public:

      void add_path (std::string const& path);

      static void init ()
      {
          m_instance = new PluginRegistry;
      }

      static void deinit ()
      {
          delete m_instance;
      }

      static PluginRegistry* instance () {
          return m_instance;
      }

  private:

      class Impl;

      static PluginRegistry* m_instance;

      std::auto_ptr<Impl> m_impl;

      PluginRegistry ();
      PluginRegistry (PluginRegistry const&);
      ~PluginRegistry ();
  };

} // LV namespace

#endif /* __cplusplus */

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
