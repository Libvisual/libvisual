/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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

#ifdef __cplusplus

#include <libvisual/lv_singleton.hpp>
#include <memory>
#include <string>

//! Libvisual namespace
namespace LV {

  class LV_API System
      : public Singleton<System>
  {
  public:

      /**
       * Initializes Libvisual.
       *
       * @param argc Number of arguments
       * @param argv Argument strings
       */
      static void init (int& argc, char**& argv);

      System (System const&) = delete;

      ~System ();

      /**
       * Returns the Libvisual version.
       *
       * @return version string
       */
      std::string get_version () const;

      /**
       * Returns the Libvisual API verison.
	   *
       * @return API version
       */
      int get_api_version () const;

      /**
       * Returns a pointer to Libvisual system parameters.
       *
       * @return A pointer to Libvisual system parameters.
       */
      VisParamList* get_params () const;

  private:

      class Impl;

      const std::unique_ptr<Impl> m_impl;

      System (int& argc, char**& argv);
  };

} // LV namespace

#endif // __cplusplus


LV_BEGIN_DECLS

LV_API void visual_init (int *argc, char ***argv);

LV_API int  visual_is_initialized (void);

LV_API void visual_quit (void);

LV_API const char *visual_get_version (void);

LV_API int visual_get_api_version (void);

LV_API VisParamList *visual_get_params (void);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_LIBVISUAL_H */
