/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_libvisual.h,v 1.11 2006/01/22 13:23:37 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_LIBVISUAL_H
#define _LV_LIBVISUAL_H

#include <libvisual/lv_param.h>

/**
 * @defgroup Libvisual Libvisual
 * @{
 */

/**
 * Indicates at which version the API is.
 */
#define VISUAL_API_VERSION  4000

#ifdef __cplusplus

#include <libvisual/lv_scoped_ptr.hpp>
#include <libvisual/lv_singleton.hpp>
#include <string>

namespace LV {

  class System
      : public Singleton<System>
  {
  public:

      /**
       * Initializes libvisual
       *
       * @param argc Number of arguments
       * @param argv Argument strings
       */
      static void init (int& argc, char**& argv)
      {
          if (!m_instance) {
              m_instance = new System (argc, argv);
          }
      }

      ~System ();

      /**
       * Returns the libvisual version.
       *
       * @return version string
       */
      std::string get_version () const;

      /**
       * Returns the libvisual API verison.
	   *
       * @return API version
       */
      int get_api_version () const;

      /**
       * Returns a pointer to the libvisual global VisParamContainer.
       *
       * @return A pointer to the libvisual global VisParamContainer.
       */
      VisParamContainer* get_params () const;

  private:

      class Impl;

      ScopedPtr<Impl> m_impl;

      System (int& argc, char**& argv);
      System (System const&);
  };

} // LV namespace

#endif // __cplusplus


VISUAL_BEGIN_DECLS

int visual_init (int *argc, char ***argv);

int visual_is_initialized (void);

int visual_quit (void);

const char *visual_get_version (void);

int visual_get_api_version (void);

VisParamContainer *visual_get_params (void);


VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_LIBVISUAL_H */
