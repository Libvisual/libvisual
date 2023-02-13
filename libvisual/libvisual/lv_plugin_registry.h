#ifndef _LV_PLUGIN_REGISTRY_H
#define _LV_PLUGIN_REGISTRY_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>

#ifdef __cplusplus

#include <libvisual/lv_singleton.hpp>
#include <libvisual/lv_plugin.h>
#include <string>
#include <string_view>
#include <memory>

namespace LV {

  typedef ::VisPluginType PluginType;

  class LV_API PluginRegistry;

  //! Manages the registry of plugins
  //!
  //! @note This is a singleton class. Its only instance must
  //!       be accessed via the instance() method.
  //!
  class LV_API PluginRegistry final
      : public Singleton<PluginRegistry>
  {
  public:

      PluginRegistry (PluginRegistry const&) = delete;

      /** Destructor */
      ~PluginRegistry () override;

      /**
       * Adds an extra plugin search path.
       *
       * @param path Path to plugin directory
       */
      void add_path (std::string const& path);

      PluginRef const* find_plugin (PluginType type, std::string_view name) const;

      /**
       * Checks if a plugin is available.
       *
       * @param type Type of plugin
       * @param name Name of plugin
       *
       * @return Returns true if plugin is available, false otherwise
       */
      bool has_plugin (PluginType type, std::string_view name) const;

      /**
       * Returns the list of all available plugins.
       *
       * @return List of plugins
       */
      PluginList const& get_plugins () const;

      /**
       * Returns the list of all available plugins of a given type.
       *
       * @param type Type of plugin
       *
       * @return List of plugins of the given type
       */
      PluginList const& get_plugins_by_type (PluginType type) const;

      /**
       * Returns information on a plugin
       *
       * @param type Type of plugin
       * @param name Name of plugin
       *
       * @return Plugin information
       */
      VisPluginInfo const* get_plugin_info (PluginType type, std::string_view name) const;

  private:

      friend class System;

      class Impl;

      const std::unique_ptr<Impl> m_impl;

      PluginRegistry ();

      static void init ();
  };

} // LV namespace

#endif /* __cplusplus */

LV_BEGIN_DECLS

LV_API void visual_plugin_registry_add_path (const char *path);
LV_API int  visual_plugin_registry_has_plugin (VisPluginType type, const char *name);

LV_END_DECLS

#endif /*_LV_PLUGIN_REGISTRY_H */
