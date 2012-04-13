#include "config.h"
#include "lv_module.hpp"
#include "lv_common.h"
#include <dlfcn.h>

namespace LV {

  class Module::Impl
  {
  public:

      void* handle;
  };

  Module::Module (std::string const& path)
      : m_impl (new Impl)
      , m_ref_count (0)
  {
      m_impl->handle = dlopen (path.c_str(), RTLD_LAZY);

      if (!m_impl->handle) {
          std::string msg = "Failed to load shared object (" + path + "): " + dlerror();
          throw Error (msg);
      }
  }

  Module::~Module ()
  {
      dlclose (m_impl->handle);
  }

  void* Module::get_symbol (std::string const& name)
  {
      return dlsym (m_impl->handle, name.c_str ());
  }

} // LV namespace
