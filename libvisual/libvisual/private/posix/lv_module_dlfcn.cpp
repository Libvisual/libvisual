#include "config.h"
#include "lv_module.hpp"
#include "lv_common.h"
#include <dlfcn.h>

namespace LV {

  class Module::Impl
  {
  public:

      void*       handle;
      std::string path;
  };

  Module::Module (std::string const& path)
      : m_impl (new Impl)
      , m_ref_count (1)
  {
      visual_log (VISUAL_LOG_DEBUG, "Loading shared object: %s", path.c_str ());

      m_impl->handle = dlopen (path.c_str(), RTLD_LAZY);
      m_impl->path = path;

      if (!m_impl->handle) {
          std::string msg = "Failed to load shared object (" + path + "): " + dlerror();
          throw Error (msg);
      }
  }

  Module::~Module ()
  {
      visual_log (VISUAL_LOG_DEBUG, "Unloading shared object: %s", m_impl->path.c_str ());

      dlclose (m_impl->handle);
  }

  void* Module::get_symbol (std::string const& name)
  {
      return dlsym (m_impl->handle, name.c_str ());
  }

} // LV namespace
