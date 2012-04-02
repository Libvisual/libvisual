#define _SVID_SOURCE

#include "config.h"
#include "lv_plugin_registry.h"

#include "lv_common.h"
#include "lv_actor.h"
#include "lv_input.h"
#include "lv_morph.h"
#include "lv_transform.h"
#include "lv_util.hpp"
#include "gettext.h"

#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <cstdio>
#include <cstdlib>

#include <dirent.h>

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#endif

namespace LV {

  namespace Internal {

    // Predicate closure that checks if a plugin entry is of a
    // particular type
    struct PluginHasType
        : public std::unary_function<bool, VisPluginRef const*>
    {
        std::string type;

        PluginHasType (std::string const& type_)
            : type (type_)
        {}

        bool operator() (VisPluginRef const* ref)
        {
            return (type == ref->info->type);
        }
    };

    void get_plugins_by_type (PluginList& output, PluginList const& input, std::string const& type)
    {
        output.clear ();

        copy_if (input.begin(),
                 input.end (),
                 std::back_insert_iterator<PluginList> (output),
                 PluginHasType (type));
    }

  } // Internal namespace

  using namespace Internal;

  struct PluginRegistry::Impl
  {
      std::vector<std::string> plugin_paths;

      PluginList plugins;
      PluginList actor_plugins;
      PluginList morph_plugins;
      PluginList input_plugins;
      PluginList transform_plugins;

      void fetch_plugin_list ();
      void add_plugins_from_dir (PluginList& list, std::string const& dir);
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

      m_impl->fetch_plugin_list ();
  }

  PluginRegistry::~PluginRegistry ()
  {
      // empty;
  }

  void PluginRegistry::add_path (std::string const& path)
  {
      visual_log (VISUAL_LOG_INFO, "Adding to plugin search path: %s", path.c_str());

      m_impl->plugin_paths.push_back (path);
  }

  PluginList const&
  PluginRegistry::get_plugins () const
  {
      return m_impl->plugins;
  }

  PluginList const&
  PluginRegistry::get_actor_plugins () const
  {
      return m_impl->actor_plugins;
  }

  PluginList const&
  PluginRegistry::get_input_plugins () const
  {
      return m_impl->input_plugins;
  }

  PluginList const&
  PluginRegistry::get_morph_plugins () const
  {
      return m_impl->morph_plugins;
  }

  PluginList const&
  PluginRegistry::get_transform_plugins () const
  {
      return m_impl->transform_plugins;
  }

  void PluginRegistry::get_plugins_by_type (PluginList& list, std::string const& type)
  {
      Internal::get_plugins_by_type (list, m_impl->plugins, type);
  }

  void PluginRegistry::Impl::fetch_plugin_list ()
  {
      typedef std::vector<std::string>::const_iterator PathIter;

      for (PathIter path = plugin_paths.begin (), path_end = plugin_paths.end ();
           path != path_end;
           ++path)
      {
          add_plugins_from_dir (plugins, *path);
      }

      Internal::get_plugins_by_type (actor_plugins, plugins, VISUAL_PLUGIN_TYPE_ACTOR);
      Internal::get_plugins_by_type (input_plugins, plugins, VISUAL_PLUGIN_TYPE_INPUT);
      Internal::get_plugins_by_type (morph_plugins, plugins, VISUAL_PLUGIN_TYPE_MORPH);
      Internal::get_plugins_by_type (transform_plugins, plugins, VISUAL_PLUGIN_TYPE_TRANSFORM);
  }

  void PluginRegistry::Impl::add_plugins_from_dir (PluginList& list, std::string const& dir)
  {
      int cnt = 0;

#if defined(VISUAL_OS_WIN32)
      TCHAR szDir[MAX_PATH+1];
      std::snprintf (szDir, MAX_PATH, "%s/*", dir);

      WIN32_FIND_DATA FileData;
      HANDLE hList = FindFirstFile (szDir, &FileData);

      if (hList == INVALID_HANDLE_VALUE) {
          FindClose (hList);
          return 0;
      }

      visual_mem_set (temp, 0, sizeof (temp));

      bool fFinished = false;

      while (!fFinished) {
          VisPluginRef **ref = NULL;

          if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
              snprintf (temp, 1023, "%s/%s", dir, FileData.cFileName);
              std::size_t len = strlen (temp);

              if (len > 5 && (strncmp (&temp[len - 4], ".dll", 4) == 0))
                  ref = visual_plugin_get_references (temp, &cnt);

              if (ref != NULL) {
                  for (int i = 0; i < cnt; i++)
                      visual_list_add (list, ref[i]);

                  /* This is the pointer pointer pointer, not a ref itself */
                  visual_mem_free (ref);
              }
          }

          if (!FindNextFile (hList, &FileData)) {
              if (GetLastError () == ERROR_NO_MORE_FILES) {
                  fFinished = TRUE;
              }
          }
      }

      FindClose (hList);
#else

      struct dirent **namelist;

      int n = scandir (dir.c_str (), &namelist, NULL, alphasort);
      if (n < 0)
          return;

      // First two entries are '.' and '..'
      visual_mem_free (namelist[0]);
      visual_mem_free (namelist[1]);

      for (int i = 2; i < n; i++) {
          std::string full_path = dir + "/" + namelist[i]->d_name;

          if (str_has_suffix (full_path, ".so")) {
              VisPluginRef** ref = visual_plugin_get_references (full_path.c_str (), &cnt);

              if (ref) {
                  for (int j = 0; j < cnt; j++)
                      list.push_back (ref[j]);

                  visual_mem_free (ref);
              }
          }

          visual_mem_free (namelist[i]);
      }

      visual_mem_free (namelist);

#endif
  }

} // LV namespace
