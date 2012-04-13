#define _SVID_SOURCE

#include "config.h"
#include "lv_plugin_registry.h"

#include "lv_common.h"
#include "lv_actor.h"
#include "lv_input.h"
#include "lv_morph.h"
#include "lv_transform.h"
#include "lv_util.hpp"
#include "lv_libvisual.h"
#include "gettext.h"

#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>

#include <dirent.h>
#include <dlfcn.h>

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

namespace LV {

  namespace {

    typedef std::map<PluginType, PluginList> PluginListMap;

    struct PluginHasType
        : public std::unary_function<bool, PluginRef const*>
    {
        PluginType type;

        PluginHasType (PluginType const& type_)
            : type (type_)
        {}

        bool operator() (PluginRef const* ref)
        {
            return (type == ref->info->type);
        }
    };

    struct PluginHasName
        : public std::unary_function<bool, PluginRef const*>
    {
        std::string name;

        PluginHasName (std::string const& name_)
            : name (name_)
        {}

        bool operator() (PluginRef const* ref)
        {
            return (name == ref->info->plugname);
        }
    };
  }

  class PluginRegistry::Impl
  {
  public:

      std::vector<std::string> plugin_paths;

      PluginListMap plugin_list_map;

      Impl ();

      ~Impl ();

      void get_plugins_from_dir (PluginList& list, std::string const& dir);
  };

  PluginRef *load_plugin_ref (std::string const& plugin_path)
  {
      // NOTE: This does not check if a plugin has already been loaded

#if defined(VISUAL_OS_WIN32)
      HMODULE handle = LoadLibrary (plugin_path.c_str ());
#else
      void* handle = dlopen (plugin_path.c_str (), RTLD_LAZY);
#endif

      if (!handle) {
#if defined(VISUAL_OS_WIN32)
          visual_log (VISUAL_LOG_ERROR, "Cannot load plugin: win32 error code: %ld", GetLastError());
#else
          visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin: %s"), dlerror ());
#endif

          return NULL;
      }
      
#if defined(VISUAL_OS_WIN32)
      int* plugin_version = reinterpret_cast<int*> (GetProcAddress (handle, VISUAL_PLUGIN_VERSION_TAG));
#else
      int* plugin_version = static_cast<int*> (dlsym (handle, VISUAL_PLUGIN_VERSION_TAG));
#endif

      if (!plugin_version || *plugin_version != VISUAL_PLUGIN_API_VERSION) {
          visual_log (VISUAL_LOG_ERROR, _("Plugin %s is not compatible with version %s of libvisual"),
                      plugin_path.c_str (), visual_get_version ());

#if defined(VISUAL_OS_WIN32)
          FreeLibrary (handle);
#else
          dlclose (handle);
#endif

          return NULL;
      }

      VisPluginGetInfoFunc get_plugin_info;
    
#if defined(VISUAL_OS_WIN32)
      get_plugin_info = reinterpret_cast<VisPluginGetInfoFunc> (GetProcAddress (handle, "get_plugin_info"));
#else
      get_plugin_info = reinterpret_cast<VisPluginGetInfoFunc> (dlsym (handle, "get_plugin_info"));
#endif

      if (!get_plugin_info) {
#if defined(VISUAL_OS_WIN32)
          visual_log (VISUAL_LOG_ERROR, "Cannot initialize plugin: win32 error code: %ld", GetLastError ());

          FreeLibrary (handle);
#else
          visual_log (VISUAL_LOG_ERROR, _("Cannot initialize plugin: %s"), dlerror ());
        
          dlclose (handle);
#endif
          return NULL;
      }

      VisPluginInfo const* plugin_info = get_plugin_info ();

      if (!plugin_info) {
          visual_log (VISUAL_LOG_ERROR, _("Cannot get plugin info"));

#if defined(VISUAL_OS_WIN32)
          FreeLibrary (handle);
#else
          dlclose (handle);
#endif

          return NULL;
      }

      PluginRef* ref = new PluginRef;

      ref->info   = plugin_info;
      ref->file   = plugin_path;
      ref->handle = handle;

      return ref;
  }

  void delete_plugin_ref (PluginRef* ref)
  {
#if defined(VISUAL_OS_WIN32)
      FreeLibrary (ref->handle);
#else
      dlclose (ref->handle);
#endif
      delete ref;
  }

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
  }

  PluginRegistry::~PluginRegistry ()
  {
      // empty;
  }

  void PluginRegistry::add_path (std::string const& path)
  {
      visual_log (VISUAL_LOG_INFO, "Adding to plugin search path: %s", path.c_str());

      m_impl->plugin_paths.push_back (path);

      PluginList plugins;
      m_impl->get_plugins_from_dir (plugins, path);

      for (PluginList::iterator plugin = plugins.begin (), plugin_end = plugins.end ();
           plugin != plugin_end;
           ++plugin)
      {
          PluginList& list = m_impl->plugin_list_map[(*plugin)->info->type];
          list.push_back (*plugin);
      }
  }

  PluginRef* PluginRegistry::find_plugin (PluginType type, std::string const& name) const
  {
      PluginList const& list = get_plugins_by_type (type);

      PluginList::const_iterator iter =
          std::find_if (list.begin (),
                        list.end (),
                        PluginHasName (name));

      return iter != list.end () ? *iter : 0;
  }

  bool PluginRegistry::has_plugin (PluginType type, std::string const& name) const
  {
      return find_plugin (type, name) != 0;
  }

  PluginList const& PluginRegistry::get_plugins_by_type (PluginType type) const
  {
      static PluginList empty;

      PluginListMap::const_iterator match = m_impl->plugin_list_map.find (type);
      if (match == m_impl->plugin_list_map.end ())
          return empty;

      return match->second;
  }

  VisPluginInfo const* PluginRegistry::get_plugin_info (PluginType type, std::string const& name) const
  {
      PluginRef* ref = find_plugin (type, name);

      return ref ? ref->info : 0;
  }

  PluginRegistry::Impl::Impl ()
  {
      // empty
  }

  PluginRegistry::Impl::~Impl ()
  {
      for (PluginListMap::const_iterator list = plugin_list_map.begin (), list_end = plugin_list_map.end ();
           list != list_end;
           ++list) {
          std::for_each (list->second.begin (), list->second.end (), delete_plugin_ref);
      }
  }

  void PluginRegistry::Impl::get_plugins_from_dir (PluginList& list, std::string const& dir)
  {
      list.clear ();

#if defined(VISUAL_OS_WIN32)
      std::string pattern = dir + "/*";

      WIN32_FIND_DATA file_data;
      HANDLE hList = FindFirstFile (pattern.c_str (), &file_data);

      if (hList == INVALID_HANDLE_VALUE) {
          FindClose (hList);
          return;
      }

      bool finished = false;

      while (!finished) {
          if (!(file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
              std::string full_path = dir + "/" + file_data.cFileName;

              if (str_has_suffix (full_path, ".dll")) {
                  PluginRef* ref = load_plugin_ref (full_path);

                  if (ref) {
                      list.push_back (ref);
                  }
              }
          }

          if (!FindNextFile (hList, &file_data)) {
              if (GetLastError () == ERROR_NO_MORE_FILES) {
                  finished = true;
              }
          }
      }

      FindClose (hList);
#else
      // NOTE: This typecast is needed for glibc versions that define
      // alphasort() as taking const void * arguments

      typedef int (*ScandirCompareFunc) (const struct dirent **, const struct dirent **);

      struct dirent **namelist;

      int n = scandir (dir.c_str (), &namelist, NULL, ScandirCompareFunc (alphasort));
      if (n < 0)
          return;

      // First two entries are '.' and '..'
      visual_mem_free (namelist[0]);
      visual_mem_free (namelist[1]);

      for (int i = 2; i < n; i++) {
          std::string full_path = dir + "/" + namelist[i]->d_name;

          if (str_has_suffix (full_path, ".so")) {
              PluginRef* ref = load_plugin_ref (full_path);

              if (ref) {
                  list.push_back (ref);
              }
          }

          visual_mem_free (namelist[i]);
      }

      visual_mem_free (namelist);

#endif
  }

} // LV namespace
