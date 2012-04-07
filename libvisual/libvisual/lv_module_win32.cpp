#include "config.h"
#include "lv_module.hpp"
#include "lv_common.h"

#ifdef VISUAL_OS_WIN32

#include <sstream>
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
      m_impl->handle = LoadLibrary (path.c_str ());

      if (!m_impl->handle) {
          std::ostringstream msg;
          msg << "Failed to load shared object (" << path << "): Win32 error code #" << GetLastError ();
          throw Error (msg.str ());
      }
  }

  Module::~Module ()
  {
      FreeLibrary (m_impl->handle);
  }

  void* Module::get_symbol (std::string const& name)
  {
      return reinterpret_cast<void*> (GetProcAddress (m_impl->handle, name.c_str ()));
  }

} // LV namespace

#endif /* VISUAL_OS_WIN32 */
