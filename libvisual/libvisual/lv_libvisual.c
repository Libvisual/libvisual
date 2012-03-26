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
#include "gettext.h"


/* Set when libvisual is initialized. */
int __lv_initialized = FALSE;

/* Set a progname from argv[0] when we're capable of doing so. */
char *__lv_progname = NULL;

/* The global params container */
VisParamContainer *__lv_paramcontainer = NULL;

static int init_params (VisParamContainer *paramcontainer);

static int init_params (VisParamContainer *paramcontainer)
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

const char *visual_get_version ()
{
	return VISUAL_VERSION;
}

int visual_get_api_version ()
{
	return VISUAL_API_VERSION;
}

VisParamContainer *visual_get_params ()
{
	return __lv_paramcontainer;
}

int visual_init (int *argc, char ***argv)
{
#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

	if (__lv_initialized == TRUE) {
		visual_log (VISUAL_LOG_ERROR, _("Over initialized"));
                return -VISUAL_ERROR_LIBVISUAL_ALREADY_INITIALIZED;
        }

	if (argc == NULL || argv == NULL) {
		if (argc == NULL && argv == NULL) {
			__lv_progname = strdup (_("no progname"));


			if (__lv_progname == NULL)
				visual_log (VISUAL_LOG_WARNING, _("Could not set program name"));
		} else
			visual_log (VISUAL_LOG_ERROR, _("Initialization failed, bad argv, argc"));

	} else {
                /*
                 * We must copy the argument, to let the client
                 * call this method from any context.
                 */
#ifdef __USE_GNU
                __lv_progname = strndup (*argv[0], 1024);
#else
                __lv_progname = strdup (*argv[0]);
#endif
                if (__lv_progname == NULL)
                        visual_log (VISUAL_LOG_WARNING, _("Could not set program name"));
        }

	/* Initialize CPU caps */
	visual_cpu_initialize ();

	/* Initialize Mem system */
	visual_mem_initialize ();

	/* Initialize CPU-accelerated graphics functions */
	visual_alpha_blend_initialize ();

	/* Initialize Thread system */
	visual_thread_initialize ();

	/* Initialize FFT system */
	visual_fourier_initialize ();

	/* Initialize the plugin registry */
	visual_plugin_registry_initialize ();

	__lv_paramcontainer = visual_param_container_new ();
	init_params (__lv_paramcontainer);

	__lv_initialized = TRUE;

	return VISUAL_OK;
}

int visual_is_initialized ()
{
	return __lv_initialized;
}

int visual_quit ()
{
	int ret;

	if (__lv_initialized == FALSE) {
                visual_log (VISUAL_LOG_WARNING, _("Never initialized"));

		return -VISUAL_ERROR_LIBVISUAL_NOT_INITIALIZED;
	}

	if (visual_fourier_is_initialized () == TRUE)
		visual_fourier_deinitialize ();

	visual_plugin_registry_deinitialize ();

	ret = visual_object_unref (VISUAL_OBJECT (__lv_paramcontainer));
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, _("Global param container: destroy failed: %s"), visual_error_to_string (ret));

	__lv_initialized = FALSE;
	return VISUAL_OK;
}
