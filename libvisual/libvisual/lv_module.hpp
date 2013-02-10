#ifndef _LV_MODULE_HPP
#define _LV_MODULE_HPP

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_intrusive_ptr.hpp>
#include <memory>
#include <string>

namespace LV {

  class Module;

  typedef IntrusivePtr<Module> ModulePtr;

  class LV_API Module
  {
  public:

      static ModulePtr load (std::string const& path);

      Module (Module const&) = delete;

      Module& operator= (Module const&) = delete;

      ~Module ();

      void* get_symbol (std::string const& name);

      static std::string const& path_suffix ();

  private:

      class Impl;
      const std::unique_ptr<Impl> m_impl;

      unsigned int m_ref_count;

      explicit Module (std::string const& path);

      friend void intrusive_ptr_add_ref (Module* module);
      friend void intrusive_ptr_release (Module* module);
  };

  inline void intrusive_ptr_add_ref (Module* module)
  {
      module->m_ref_count++;
  }

  inline void intrusive_ptr_release (Module* module)
  {
      module->m_ref_count--;
      if (module->m_ref_count == 0) {
          delete module;
      }
  }

} // LV namespace

#endif // _LV_MODULE_HPP
