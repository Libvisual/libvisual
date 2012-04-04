#ifndef _LV_MODULE_HPP
#define _LV_MODULE_HPP

#include <libvisual/lv_scoped_ptr.hpp>
#include <string>

namespace LV {

  class Module
  {
  public:

      explicit Module (std::string const& path);

      ~Module ();

      void* get_symbol (std::string const& name);

  private:

      class Impl;

      ScopedPtr<Impl> m_impl;

      Module (Module const&);
      Module& operator= (Module const&);
  };

} // VL namespace

#endif // _LV_MODULE_HPP
