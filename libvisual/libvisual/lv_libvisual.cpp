/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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

#include "config.h"
#include "version.h"

#include "lv_libvisual.h"
#include "lv_common.h"

#include "lv_alpha_blend.h"
#include "lv_fourier.h"
#include "lv_plugin_registry.h"
#include "lv_log.h"
#include "lv_param.h"
#include "lv_util.h"
#include "private/lv_time_system.hpp"

#include "gettext.h"

#include <string>

extern "C" {
  void visual_cpu_initialize (void);
  void visual_mem_initialize (void);
}

namespace LV
{

  namespace {

    RandomSeed random_seed ()
    {
        return RandomSeed (Time::now ().to_usecs ());
    }

  } // anonymous namespace

  class System::Impl
  {
  public:

      ParamList     params;
      RandomContext rng;

      Impl ();

      static ParamList initial_params ();
  };

  System::Impl::Impl ()
      : params {initial_params ()}
      , rng    {random_seed ()}
  {}

  ParamList System::Impl::initial_params ()
  {
      return {
           visual_param_new_integer ("songinfo-show",
                                     "Show song info",
                                     1,
                                     nullptr),
           visual_param_new_integer ("songinfo-timeout",
                                     "Songinfo timeout in seconds",
                                     5,
                                     nullptr),
           visual_param_new_bool    ("songinfo-in-plugins",
                                     "Show songinfo in plugins",
                                     true,
                                     nullptr),
           visual_param_new_integer ("songinfo-cover-width",
                                     "Song cover art width",
                                     128,
                                     visual_param_in_range_integer (32, 1000)),
           visual_param_new_integer ("songinfo-cover-height",
                                     "Song cover art height",
                                     128,
                                     visual_param_in_range_integer (32, 1000))
      };
  }

  void System::init (int& argc, char**& argv)
  {
      if (m_instance) {
          visual_log (VISUAL_LOG_WARNING, "Attempt to initialize LV a second time.");
          return;
      }

      m_instance.reset (new System {argc, argv});
  }

  std::string const& System::get_version () const
  {
      static std::string const version_str {VISUAL_VERSION " (" LV_REVISION ")"};
      return version_str;
  }

  int System::get_api_version () const
  {
      return VISUAL_API_VERSION;
  }

  ParamList& System::get_params () const
  {
      return m_impl->params;
  }

  RandomContext& System::get_rng () const
  {
      return m_impl->rng;
  }

  void System::set_rng_seed (VisRandomSeed seed)
  {
      m_impl->rng.set_seed (seed);
  }

  System::System (int& argc, char**& argv)
      : m_impl {std::make_unique<Impl> ()}
  {
      (void)argc;
      (void)argv;

      visual_log (VISUAL_LOG_INFO, "Starting Libvisual %s", get_version ().c_str ());

#if ENABLE_NLS
      bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

      // Initialize CPU caps
      visual_cpu_initialize ();

      // Initialize Mem system
      visual_mem_initialize ();

      // Initialize high-resolution timer system
      TimeSystem::start ();

      // Initialize the plugin registry
      PluginRegistry::init ();
  }

  System::~System ()
  {
      PluginRegistry::destroy ();
      TimeSystem::shutdown ();
  }

} // LV namespace
