#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "lvconfig.h"
#include "lv_plugin.h"
#include "lv_libvisual.h"
#include "lv_log.h"
#include "lv_param.h"

#define VERSION "0.1.6"

/** Set when libvisual is initialized. */
int __lv_initialized = FALSE;
/** Set a progname from argv[0] when we're capable of doing so. */
char *__lv_progname = NULL;

/** Contains the completely plugin registry after initialize. */
VisList *__lv_plugins = NULL;
/** Contains all the actor plugins after initialize. */
VisList *__lv_plugins_actor = NULL;
/** Contains all the input plugins after initialize. */
VisList *__lv_plugins_input = NULL;
/** Contains all the morph plugins after initialize. */
VisList *__lv_plugins_morph = NULL;

/** The global params container */
VisParamContainer *__lv_paramcontainer = NULL;

/** Contains the number of plugin registry paths. */
int __lv_plugpath_cnt = 0;
/** Char ** list of all the plugin paths. */
char **__lv_plugpaths = NULL;


static int init_params (VisParamContainer *paramcontainer);

static int init_params (VisParamContainer *paramcontainer)
{
	VisParamEntry *param;

	visual_log_return_val_if_fail (paramcontainer != NULL, -1);

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

	return 0;
}

/**
 * @defgroup Libvisual Libvisual
 * @{
 */

/**
 * Gives the libvisual version.
 *
 * @return A const char containing the libvisual version.
 */
const char *visual_get_version ()
{
	return VERSION;
}

/**
 * Returns a pointer to the libvisual global VisParamContainer.
 *
 * @return A pointer to the libvisual global VisParamContainer.
 */
VisParamContainer *visual_get_params ()
{
	return __lv_paramcontainer;
}

/**
 * Adds extra plugin registry paths.
 *
 * @param pathadd A string containing a path where plugins are located.
 *
 * @return 0 on succes -1 on error.
 */
int visual_init_path_add (char *pathadd)
{
	__lv_plugpath_cnt++;
	__lv_plugpaths = realloc (__lv_plugpaths, sizeof (char *) * __lv_plugpath_cnt);
	__lv_plugpaths[__lv_plugpath_cnt - 1] = pathadd;

	return 0;
}

/**
 * Initialize libvisual. Sets up a plugin registry, register the program name and such.
 *
 * @param argc Pointer to an int containing the number of arguments within argv or NULL.
 * @param argv Pointer to a list of strings that make up the argument vector or NULL.
 *
 * @return 0 on succes -1 on error.
 */
int visual_init (int *argc, char ***argv)
{
	if (__lv_initialized == TRUE) {
		visual_log (VISUAL_LOG_ERROR, "Over initialized");
                return -1;
        }
	
	if (argc == NULL || argv == NULL) {
		if (argc != NULL || argv != NULL)
			visual_log (VISUAL_LOG_WARNING, "Something wrong with the argc, argv pointers, one is NULL and one is not\n");
		
		__lv_progname = strdup ("no progname");
		
		if (__lv_progname == NULL)
                        visual_log (VISUAL_LOG_WARNING, "Could not set program name");

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
                        visual_log (VISUAL_LOG_WARNING, "Could not set program name");
        }

	visual_init_path_add (PLUGPATH"/actor");
	visual_init_path_add (PLUGPATH"/input");
	visual_init_path_add (PLUGPATH"/morph");
	/* NULL terminated */
	visual_init_path_add (NULL);

	__lv_plugins = visual_plugin_get_list (__lv_plugpaths);
	__lv_plugins_actor = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_ACTOR);
	__lv_plugins_input = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_INPUT);
	__lv_plugins_morph = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_MORPH);

	__lv_paramcontainer = visual_param_container_new ();
	init_params (__lv_paramcontainer);

	__lv_initialized = TRUE;

	return 0;
}

/**
 * Quits libvisual, destroys all the plugin registries.
 *
 * @return 0 on succes -1 on error.
 */
int visual_quit ()
{
	int ret;

	if (__lv_initialized == FALSE) {
                visual_log (VISUAL_LOG_WARNING, "Never initialized");
		return -1;
	}

        if (__lv_progname != NULL)
                visual_mem_free (__lv_progname);

	ret = visual_plugin_ref_list_destroy (__lv_plugins);
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, "Plugins references list: destroy failed");

	ret = visual_list_destroy (__lv_plugins_actor, NULL);
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, "Actor plugins list: destroy failed");

	ret = visual_list_destroy (__lv_plugins_input, NULL);
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, "Input plugins list: destroy failed");

	ret = visual_list_destroy (__lv_plugins_morph, NULL);
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, "Morph plugins list: destroy failed");

	ret = visual_param_container_destroy (__lv_paramcontainer);
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, "Global param container: destroy failed");

	__lv_initialized = FALSE;
	return 0;
}

/**
 * @}
 */

