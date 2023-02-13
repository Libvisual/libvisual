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
#include <libvisual/lv_random.h>

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

  class LV_API System final
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

      ~System () override;

      /**
       * Returns the Libvisual version.
       *
       * @return version string
       */
      std::string const& get_version () const;

      /**
       * Returns the Libvisual API verison.
	   *
       * @return API version
       */
      int get_api_version () const;

      /**
       * Returns the system-wide parameter list
       *
       * @return Parameter list
       */
      ParamList& get_params () const;

      /**
       * Returns the system-wide random number generator.
       */
      RandomContext& get_rng () const;

      /**
       * Sets the seed for the system-wide random number generator.
       *
       * @param seed seed
       */
      void set_rng_seed (RandomSeed seed);

  private:

      class Impl;

      const std::unique_ptr<Impl> m_impl;

      System (int& argc, char**& argv);
  };

  // FIXME: Move this into lv_random.h
  /**
   * Drop-in replacement for std::rand() using LV's system-wide random
   * number generator.
   *
   * @return a random number
   */
  inline uint32_t rand ()
  {
      return System::instance()->get_rng ().get_int ();
  }

} // LV namespace

#endif // __cplusplus


LV_BEGIN_DECLS

LV_API void visual_init (int *argc, char ***argv);

LV_API int  visual_is_initialized (void);

LV_API void visual_quit (void);

LV_API const char *visual_get_version (void);

LV_API int visual_get_api_version (void);

LV_API VisParamList *visual_get_params (void);

LV_API void visual_set_rng_seed (VisRandomSeed seed);

LV_API VisRandomContext *visual_get_rng (void);

// FIXME: Move this into lv_random.h
static inline uint32_t visual_rand (void)
{
    return visual_random_context_int (visual_get_rng ());
}

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_LIBVISUAL_H */
