#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <config.h>

#include "lv_plugin.h"
#include "lv_libvisual.h"

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

/** Contains the number of plugin registry paths. */
int __lv_plugpath_cnt = 0;
/** Char ** list of all the plugin paths. */
char **__lv_plugpaths = NULL;

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
int visual_init (int *argc, char **argv[])
{
	if (__lv_initialized == TRUE)
		/* FIXME use lv_log */
		printf ("Over initialized\n");
	
	if (argc == NULL || argv == NULL) {
		if (argc != NULL || argv != NULL)
			printf ("OI, your argc,argv is borked\n");
		/*  FIXME, print a warning here, one
		 *  is NULL and one isn't */
		__lv_progname = "no progname";
	} else
		__lv_progname = *argv[0];

	visual_init_path_add (PLUGPATH"/actor");
	visual_init_path_add (PLUGPATH"/input");
	visual_init_path_add (PLUGPATH"/morph");
	visual_init_path_add (NULL);

	__lv_plugins = _lv_plugin_get_list (__lv_plugpaths);
	__lv_plugins_actor = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_ACTOR);
	__lv_plugins_input = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_INPUT);
	__lv_plugins_morph = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_MORPH);

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
	if (__lv_initialized == FALSE) {
		/* FIXME print a warning here */
		return -1;
	}

	visual_plugin_ref_list_destroy (__lv_plugins);
	visual_list_destroy (__lv_plugins_actor, NULL);
	visual_list_destroy (__lv_plugins_input, NULL);
	visual_list_destroy (__lv_plugins_morph, NULL);

	__lv_initialized = FALSE;
	return 0;
}

/**
 * @}
 */

