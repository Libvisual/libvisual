/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_libvisual.c,v 1.39 2006/01/22 13:23:37 synap Exp $
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

#include <config.h>
#include "lv_libvisual.h"
#include "lv_common.h"

#include "lv_alpha_blend.h"
#include "lv_fourier.h"
#include "lv_plugin_registry.h"
#include "lv_log.h"
#include "lv_param.h"
#include "lv_thread.h"
#include "lv_cpu.h"
#include "lv_util.h"

#include "gettext.h"

extern "C" {

  // Set a progname from argv[0] when we're capable of doing so.
  char *__lv_progname = 0;
}

namespace LV
{

  namespace {

    int init_params (VisParamContainer *paramcontainer)
    {
        VisParamEntry *param;

        visual_return_val_if_fail (paramcontainer != NULL, -1);

        /* Initialize all the global parameters here */

        /* Song information parameters */
        /* Show songinfo */
        param = visual_param_entry_new ("songinfo show");
        visual_param_entry_set_integer (param, 1);
        visual_param_container_add (paramcontainer, param);

        /* Songinfo timeout, in seconds */
        param = visual_param_entry_new ("songinfo timeout");
        visual_param_entry_set_integer (param, 5);
        visual_param_container_add (paramcontainer, param);

        /*
         * Show songinfo in plugins, plugins that optionally show song
         * info should query this parameter
         */
        param = visual_param_entry_new ("songinfo in plugin");
        visual_param_entry_set_integer (param, 1);
        visual_param_container_add (paramcontainer, param);

        /* Cover art dimension */
        param = visual_param_entry_new ("songinfo cover size x");
        visual_param_entry_set_integer (param, 128);
        visual_param_container_add (paramcontainer, param);

        param = visual_param_entry_new ("songinfo cover size y");
        visual_param_entry_set_integer (param, 128);
        visual_param_container_add (paramcontainer, param);

        return 0;
    }

  } // anonymous namespace

  class System::Impl
  {
  public:

      VisParamContainer *params;

      Impl ()
          : params (0)
      {}
  };

  template <>
  LV_API System* Singleton<System>::m_instance = 0;

  void System::init (int& argc, char**& argv)
  {
      if (!m_instance)
          m_instance = new System (argc, argv);
  }

  std::string System::get_version () const
  {
      return VISUAL_VERSION;
  }

  int System::get_api_version () const
  {
      return VISUAL_API_VERSION;
  }

  VisParamContainer *System::get_params () const
  {
      return m_impl->params;
  }

  System::System (int& argc, char**& argv)
      : m_impl(new Impl)
  {
#if ENABLE_NLS
      bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
      bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

      __lv_progname = visual_strdup (argv[0]);

      /* Initialize CPU caps */
      visual_cpu_initialize ();

      /* Initialize Mem system */
      visual_mem_initialize ();

      /* Initialize CPU-accelerated graphics functions */
      visual_alpha_blend_initialize ();

      /* Initialize high-resolution timer system */
	  Time::init ();

      /* Initialize Thread system */
      visual_thread_initialize ();

      /* Initialize FFT system */
      Fourier::init ();

      /* Initialize the plugin registry */
      PluginRegistry::init ();

      m_impl->params = visual_param_container_new ();
      init_params (m_impl->params);
  }

  System::~System ()
  {
      PluginRegistry::deinit ();
	  Fourier::deinit();

      visual_object_unref (VISUAL_OBJECT (m_impl->params));
  }

} // LV namespace
