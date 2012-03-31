#include "config.h"
#include "lv_plugin_registry.h"
#include "lv_common.h"
#include "lv_actor.h"
#include "lv_input.h"
#include "lv_morph.h"
#include "lv_transform.h"
#include "lv_util.h"
#include "gettext.h"

#include <stdio.h>
#include <stdlib.h>

/* Contains the completely plugin registry after initialize. */
VisList *__lv_plugins = NULL;

/* Contains all the actor plugins after initialize. */
VisList *__lv_plugins_actor = NULL;

/* Contains all the input plugins after initialize. */
VisList *__lv_plugins_input = NULL;

/* Contains all the morph plugins after initialize. */
VisList *__lv_plugins_morph = NULL;

/* Contains all the transform plugins after initialize. */
VisList *__lv_plugins_transform = NULL;

/* Contains the number of plugin registry paths. */
int __lv_plugpath_cnt = 0;
/* char ** list of all the plugin paths. */
char **__lv_plugpaths = NULL;

int visual_init_path_add (const char *pathadd)
{
	visual_log (VISUAL_LOG_INFO, "Adding to plugin search path: %s", pathadd);

	__lv_plugpath_cnt++;
	__lv_plugpaths = realloc (__lv_plugpaths, sizeof (char *) * __lv_plugpath_cnt);

	visual_return_val_if_fail (__lv_plugpaths != NULL, -VISUAL_ERROR_LIBVISUAL_NO_PATHS);

	if (pathadd == NULL)
		__lv_plugpaths[__lv_plugpath_cnt - 1] = NULL;
	else
		__lv_plugpaths[__lv_plugpath_cnt - 1] = visual_strdup (pathadd);

	return VISUAL_OK;
}

static int free_plugpaths (void)
{
	int i;

	if (__lv_plugpaths == NULL)
		return VISUAL_OK;

	for (i = 0; i < __lv_plugpath_cnt - 1; i++)
		visual_mem_free (__lv_plugpaths[i]);

	free (__lv_plugpaths);

	return VISUAL_OK;
}

int visual_plugin_registry_initialize (void)
{
	int ret;

#if !defined(VISUAL_OS_WIN32) || defined(VISUAL_WITH_CYGWIN)
	char *homedir = NULL;
	char temppluginpath[FILENAME_MAX+1];
#endif

	visual_log (VISUAL_LOG_DEBUG, "Initializing plugin registry");

	/* Add the standard plugin paths */
	ret = visual_init_path_add (VISUAL_PLUGIN_PATH "/actor");
	visual_return_val_if_fail (ret == VISUAL_OK, ret);

	ret = visual_init_path_add (VISUAL_PLUGIN_PATH "/input");
	visual_return_val_if_fail (ret == VISUAL_OK, ret);

	ret = visual_init_path_add (VISUAL_PLUGIN_PATH "/morph");
	visual_return_val_if_fail (ret == VISUAL_OK, ret);

	ret = visual_init_path_add (VISUAL_PLUGIN_PATH "/transform");
	visual_return_val_if_fail (ret == VISUAL_OK, ret);

#if !defined(VISUAL_OS_WIN32) || defined(VISUAL_WITH_CYGWIN)
	/* Add homedirectory plugin paths */
	homedir = getenv ("HOME");

	if (homedir != NULL) {
		temppluginpath[sizeof (temppluginpath) - 1] = 0;

		snprintf (temppluginpath, sizeof (temppluginpath) - 1, "%s/.libvisual/actor", homedir);
		ret = visual_init_path_add (temppluginpath);
		visual_return_val_if_fail (ret == VISUAL_OK, ret);

		snprintf (temppluginpath, sizeof (temppluginpath) - 1, "%s/.libvisual/input", homedir);
		ret = visual_init_path_add (temppluginpath);
		visual_return_val_if_fail (ret == VISUAL_OK, ret);

		snprintf (temppluginpath, sizeof (temppluginpath) - 1, "%s/.libvisual/morph", homedir);
		ret = visual_init_path_add (temppluginpath);
		visual_return_val_if_fail (ret == VISUAL_OK, ret);

		snprintf (temppluginpath, sizeof (temppluginpath) - 1, "%s/.libvisual/transform", homedir);
		ret = visual_init_path_add (temppluginpath);
		visual_return_val_if_fail (ret == VISUAL_OK, ret);
	}
#endif

	/* And null terminate the list */
	ret = visual_init_path_add (NULL);
	visual_return_val_if_fail (ret == VISUAL_OK, ret);

	__lv_plugins = visual_plugin_get_list ((const char**)__lv_plugpaths, TRUE);
	visual_return_val_if_fail (__lv_plugins != NULL, -VISUAL_ERROR_LIBVISUAL_NO_REGISTRY);

	__lv_plugins_actor = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_ACTOR);
	__lv_plugins_input = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_INPUT);
	__lv_plugins_morph = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_MORPH);
	__lv_plugins_transform = visual_plugin_registry_filter (__lv_plugins, VISUAL_PLUGIN_TYPE_TRANSFORM);

	/* Free the strdupped plugpaths */
	free_plugpaths ();

	return VISUAL_OK;
}

int visual_plugin_registry_deinitialize (void)
{
	int ret;

	ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins));
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, _("Plugins references list: destroy failed: %s"), visual_error_to_string (ret));

	ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_actor));
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, _("Actor plugins list: destroy failed: %s"), visual_error_to_string (ret));

	ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_input));
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, _("Input plugins list: destroy failed: %s"), visual_error_to_string (ret));

	ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_morph));
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, _("Morph plugins list: destroy failed: %s"), visual_error_to_string (ret));

	ret = visual_object_unref (VISUAL_OBJECT (__lv_plugins_transform));
	if (ret < 0)
		visual_log (VISUAL_LOG_WARNING, _("Transform plugins list: destroy failed: %s"), visual_error_to_string (ret));

	return  VISUAL_OK;
}
