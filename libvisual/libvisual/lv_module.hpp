#ifndef _LV_MODULE_HPP
#define _LV_MODULE_HPP

#include <libvisual/lv_scoped_ptr.hpp>
#include <libvisual/lv_intrusive_ptr.hpp>
#include <string>

namespace LV {

  class Module;

  typedef IntrusivePtr<Module> SharedModule;

  class Module
  {
  public:

      static SharedModule load (std::string const& path)
      {
          return SharedModule (new Module (path));
      }

      ~Module ();

      void* get_symbol (std::string const& name);

  private:

      class Impl;

      ScopedPtr<Impl> m_impl;
      unsigned int    m_ref_count;

      explicit Module (std::string const& path);

      Module (Module const&);
      Module& operator= (Module const&);

      friend void intrusive_ptr_add_ref (Module* module);
      friend void intrusive_ptr_release (Module* module);
  };

  void intrusive_ptr_add_ref (Module* module)
  {
      module->m_ref_count++;
  }

  void intrusive_ptr_release (Module* module)
  {
      module->m_ref_count--;
      if (module->m_ref_count == 0) {
          delete module;
      }
  }

} // LV namespace

#endif // _LV_MODULE_HPP
