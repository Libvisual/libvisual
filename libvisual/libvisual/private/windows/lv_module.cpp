#include "config.h"
#include "lv_module.hpp"
#include "lv_common.h"

#include <sstream>
#include <windows.h>

namespace LV {

  class Module::Impl
  {
  public:

      HMODULE     handle;
      std::string path;
  };

  Module::Module (std::string const& path)
      : m_impl (new Impl)
      , m_ref_count (1)
  {
      visual_log (VISUAL_LOG_DEBUG, "Loading DLL: %s", path.c_str ());

      m_impl->handle = LoadLibrary (path.c_str ());
      m_impl->path = path;

      if (!m_impl->handle) {
          std::ostringstream msg;
          msg << "Failed to load shared object (" << path << "): Win32 error code #" << GetLastError ();
          throw Error (msg.str ());
      }
  }

  Module::~Module ()
  {
      visual_log (VISUAL_LOG_DEBUG, "Unloading DLL: %s", m_impl->path.c_str ());

      FreeLibrary (m_impl->handle);
  }

  void* Module::get_symbol (std::string const& name)
  {
      return reinterpret_cast<void*> (GetProcAddress (m_impl->handle, name.c_str ()));
  }

} // LV namespace
