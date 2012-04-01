#include "config.h"
#include "lv_common.h"
#include "lv_actor.h"
#include "lv_input.h"
#include "lv_morph.h"
#include "lv_transform.h"
#include "gettext.h"

#include "lv_plugin_registry.h"

#include <vector>
#include <cstdio>
#include <cstdlib>

extern "C" {
  VisList *__lv_plugins = NULL;

  /* Contains all the actor plugins after initialize. */
  VisList *__lv_plugins_actor = NULL;

  /* Contains all the input plugins after initialize. */
  VisList *__lv_plugins_input = NULL;

  /* Contains all the morph plugins after initialize. */
  VisList *__lv_plugins_morph = NULL;

  /* Contains all the transform plugins after initialize. */
  VisList *__lv_plugins_transform = NULL;
}

namespace LV {

  PluginRegistry* PluginRegistry::m_instance = 0;

  struct PluginRegistry::Impl
  {
    std::vector<std::string> plugin_paths;
  };

  PluginRegistry::PluginRegistry ()
    : m_impl (new Impl)
  {
      visual_log (VISUAL_LOG_DEBUG, "Initializing plugin registry");

      // Add the standard plugin paths
      add_path (VISUAL_PLUGIN_PATH "/actor");
      add_path (VISUAL_PLUGIN_PATH "/input");
      add_path (VISUAL_PLUGIN_PATH "/morph");
      add_path (VISUAL_PLUGIN_PATH "/transform");

#if !defined(VISUAL_OS_WIN32) || defined(VISUAL_WITH_CYGWIN)
    // Add homedirectory plugin paths
      char const* home_env = std::getenv ("HOME");

      if (home_env) {
          std::string home_dir (home_env);

          add_path (home_dir + "/.libvisual/actor");
          add_path (home_dir + "/.libvisual/input");
          add_path (home_dir + "/.libvisual/morph");
          add_path (home_dir + "/.libvisual/transform");
      }
#endif

      // FIXME: Hack to work this work with visual_plugin_get_list() for now
      typedef std::vector<std::string>::const_iterator PathIter;

      std::vector<char const*> c_plugin_paths;
      for (PathIter path = m_impl->plugin_paths.begin(), path_end = m_impl->plugin_paths.end();
           path != path_end;
           ++path) {
        c_plugin_paths.push_back (path->c_str ());
      }
      c_plugin_paths.push_back (NULL);

      __lv_plugins = visual_plugin_get_list (&c_plugin_paths[0], true);
      if (!__lv_plugins) {
          throw Error(VISUAL_ERROR_LIBVISUAL_NO_REGISTRY, "Failed to load plugn list");
      }

      __lv_plugins_actor = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_ACTOR);
      __lv_plugins_input = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_INPUT);
      __lv_plugins_morph = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_MORPH);
      __lv_plugins_transform = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_TRANSFORM);
  }

  void PluginRegistry::add_path (std::string const& path)
  {
      visual_log (VISUAL_LOG_INFO, "Adding to plugin search path: %s", path.c_str());

      m_impl->plugin_paths.push_back (path);
  }

  PluginRegistry::~PluginRegistry ()
  {
      int ret;

      ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins));
      if (ret < 0)
          visual_log (VISUAL_LOG_WARNING, _("Plugins references list: destroy failed: %s"), visual_error_to_string (ret));

      ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_actor));
      if (ret < 0)
          visual_log (VISUAL_LOG_WARNING, _("Actor plugins list: destroy failed: %s"), visual_error_to_string (ret));

      ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_input));
      if (ret < 0)
          visual_log (VISUAL_LOG_WARNING, _("Input plugins list: destroy failed: %s"), visual_error_to_string (ret));

      ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_morph));
      if (ret < 0)
          visual_log (VISUAL_LOG_WARNING, _("Morph plugins list: destroy failed: %s"), visual_error_to_string (ret));

      ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_transform));
      if (ret < 0)
          visual_log (VISUAL_LOG_WARNING, _("Transform plugins list: destroy failed: %s"), visual_error_to_string (ret));
  }

} // LV namespace

