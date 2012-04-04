#include "config.h"
#include "lv_module.hpp"
#include "lv_common.h"
#include <windows.h>

namespace LV {

  class Module::Impl
  {
  public:

      HMODULE handle;
  };

  Module::Module (std::string const& path)
      : m_impl (new Impl)
  {
      m_impl->handle = LoadLibrary (ref->file);

      if (!m_impl->handle) {
          std::string msg = "Failed to load shared object (" + path + "): Win32 error code #" + GetLastError ();
          throw Error (msg);
      }
  }

  Module::~Module ()
  {
      FreeLibrary (m_impl->handle);
  }

  void* Module::get_symbol (std::string const& name)
  {
      return GetProcAddress (m_impl->handle, name.c_str ());
  }

} // LV namespace
