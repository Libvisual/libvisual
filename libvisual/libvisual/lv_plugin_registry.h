#ifndef _LV_PLUGIN_REGISTRY_H
#define _LV_PLUGIN_REGISTRY_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>

#ifdef __cplusplus

#include <libvisual/lv_scoped_ptr.hpp>
#include <libvisual/lv_singleton.hpp>
#include <libvisual/lv_plugin.h>
#include <string>

namespace LV {

  typedef ::VisPluginType PluginType;

  class LV_API PluginRegistry
      : public Singleton<PluginRegistry>
  {
  public:

      static void init ();

      /**
       * Adds an extra plugin search path.
       *
       * @param path Path to plugin directory
       */
      void add_path (std::string const& path);

      ~PluginRegistry ();

      PluginRef const* find_plugin (PluginType type, std::string const& name) const;

      bool has_plugin (PluginType type, std::string const& name) const;

      PluginList const& get_plugins () const;
      PluginList const& get_plugins_by_type (PluginType type) const;

      VisPluginInfo const* get_plugin_info (PluginType type, std::string const& name) const;

  private:

      class Impl;

      ScopedPtr<Impl> m_impl;

      PluginRegistry ();
      PluginRegistry (PluginRegistry const&);
  };

} // LV namespace

#endif /* __cplusplus */

LV_BEGIN_DECLS

LV_API int visual_plugin_registry_initialize (void);
LV_API int visual_plugin_registry_deinitialize (void);

LV_API int visual_plugin_registry_add_path (const char *path);
LV_API int visual_plugin_registry_has_plugin (VisPluginType type, const char *name);

LV_END_DECLS

#endif /*_LV_PLUGIN_REGISTRY_H */
