/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_plugin.c,v 1.82 2006-09-19 18:28:51 synap Exp $
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <gettext.h>

#include "lvconfig.h"

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#include <dirent.h>

#include "lv_libvisual.h"
#include "lv_plugin.h"
#include "lv_log.h"
#include "lv_mem.h"

/* Internal variables */
static VisHashmap *__lv_plugins = NULL;

static int plugin_info_dtor (VisObject *object);
static int plugin_ref_dtor (VisObject *object);
static int plugin_environ_dtor (VisObject *object);
static int plugin_dtor (VisObject *object);

static int plugin_add_dir_to_map (VisHashmap *map, const char *dir);
static char *get_delim_node (const char *str, char delim, int index);

static int plugin_info_dtor (VisObject *object)
{
	VisPluginInfo *pluginfo = VISUAL_PLUGININFO (object);

	if (pluginfo->plugname != NULL)
		visual_mem_free (pluginfo->plugname);

	if (pluginfo->type != NULL)
		visual_mem_free (pluginfo->type);

	if (pluginfo->name != NULL)
		visual_mem_free (pluginfo->name);

	if (pluginfo->author != NULL)
		visual_mem_free (pluginfo->author);

	if (pluginfo->version != NULL)
		visual_mem_free (pluginfo->version);

	if (pluginfo->about != NULL)
		visual_mem_free (pluginfo->about);

	if (pluginfo->help != NULL)
		visual_mem_free (pluginfo->help);

	pluginfo->plugname = NULL;
	pluginfo->type = NULL;
	pluginfo->name = NULL;
	pluginfo->author = NULL;
	pluginfo->version = NULL;
	pluginfo->about = NULL;
	pluginfo->help = NULL;

	return TRUE;
}

static int plugin_ref_dtor (VisObject *object)
{
	VisPluginRef *ref = VISUAL_PLUGINREF (object);

	if (ref->file != NULL)
		visual_mem_free (ref->file);

	if (ref->usecount > 0)
		visual_log (VISUAL_LOG_CRITICAL, _("A plugin reference with %d instances has been destroyed."), ref->usecount);

	if (ref->info != NULL)
		visual_object_unref (VISUAL_OBJECT (ref->info));

	ref->file = NULL;
	ref->info = NULL;

	return TRUE;
}

static int plugin_environ_dtor (VisObject *object)
{
	VisPluginEnviron *enve = VISUAL_PLUGINENVIRON (object);

	if (enve->environment != NULL)
		visual_object_unref (enve->environment);

	enve->environment = NULL;

	return TRUE;
}

static int plugin_dtor (VisObject *object)
{
	VisPluginData *plugin = VISUAL_PLUGINDATA (object);

	if (plugin->ref != NULL)
		visual_object_unref (VISUAL_OBJECT (plugin->ref));

	if (plugin->params != NULL)
		visual_object_unref (VISUAL_OBJECT (plugin->params));

	visual_collection_destroy (VISUAL_COLLECTION (&plugin->environment));

	plugin->ref = NULL;
	plugin->params = NULL;

	return TRUE;
}

static char *get_delim_node (const char *str, char delim, int index)
{
	char *buf;
	const char *start;
	const char *end = str;
	int i = 0;

	do {
		start = end;

		end = strchr (start + 1, delim);

		if (i == index) {
			/* Last section doesn't contain a delim */
			if (end == NULL)
				end = str + strlen (str);

			/* Cut off the delim that is in front */
			if (i > 0)
				start++;

			break;
		}

		i++;

	} while (end != NULL);

	if (end == NULL)
		return NULL;

	buf = visual_mem_malloc0 ((end - start) + 1);
	strncpy (buf, start, end - start);

	return buf;
}

/**
 * @defgroup VisPlugin VisPlugin
 * @{
 */

/**
 * Creates a new VisPluginInfo structure.
 *
 * @return A newly allocated VisPluginInfo
 */
VisPluginInfo *visual_plugin_info_new ()
{
	VisPluginInfo *pluginfo;

	pluginfo = visual_mem_new0 (VisPluginInfo, 1);

	visual_plugin_info_init (pluginfo);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (pluginfo), TRUE);
	visual_object_ref (VISUAL_OBJECT (pluginfo));

	return pluginfo;
}

int visual_plugin_info_init (VisPluginInfo *pluginfo)
{
	visual_log_return_val_if_fail (pluginfo != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (pluginfo));
	visual_object_set_dtor (VISUAL_OBJECT (pluginfo), plugin_info_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (pluginfo), FALSE);

	/* Set the VisPluginInfo data */
	visual_object_clean (VISUAL_OBJECT (pluginfo), VisPluginInfo);

	return VISUAL_OK;
}

/**
 * Copies data from one VisPluginInfo to another, this does not copy everything
 * but only things that are needed in the local copy for the plugin registry.
 *
 * @param dest Pointer to the destination VisPluginInfo in which some data is copied.
 * @param src Pointer to the source VisPluginInfo from which some data is copied.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_INFO_NULL on failure.
 */
int visual_plugin_info_copy (VisPluginInfo *dest, VisPluginInfo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);

    if(dest->plugname != NULL)
        free(dest->plugname);

    if(dest->type != NULL)
        visual_mem_free(dest->type);

    if(dest->name != NULL)
        visual_mem_free(dest->name);

    if(dest->author != NULL)
        visual_mem_free(dest->author);

    if(dest->version != NULL)
        visual_mem_free(dest->version);

    if(dest->about != NULL)
        visual_mem_free(dest->about);

    if(dest->help != NULL)
        visual_mem_free(dest->help);

	visual_mem_copy (dest, src, sizeof (VisPluginInfo));

	dest->plugname = strdup (src->plugname);
	dest->type = strdup (src->type);
	dest->name = strdup (src->name);
	dest->author = strdup (src->author);
	dest->version = strdup (src->version);
	dest->about = strdup (src->about);
	dest->help = strdup (src->help);
	return VISUAL_OK;
}

/**
 * Pumps the queued events into the plugin it's event handler if it has one.
 *
 * @param plugin Pointer to a VisPluginData of which the events need to be pumped into
 *	the handler.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER on failure.
 */
int visual_plugin_events_pump (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	if (plugin->info->events != NULL) {
		plugin->info->events (plugin, &plugin->eventqueue);

		return VISUAL_OK;
	}

	return -VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER;
}

/**
 * Gives the event queue from a VisPluginData. This queue needs to be used
 * when you want to send events to the plugin.
 *
 * @see visual_plugin_events_pump
 *
 * @param plugin Pointer to the VisPluginData from which we want the queue.
 *
 * @return A pointer to the requested VisEventQueue or NULL on failure.
 */
VisEventQueue *visual_plugin_get_eventqueue (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->eventqueue;
}

/**
 * Sets a VisUIWidget as top user interface widget for the plugin. When a VisUI
 * tree is requested by a client, to render a configuration userinterface, this
 * VisUIWidget is used as top widget.
 *
 * @param plugin Pointer to the VisPluginData to which we set the VisUIWidget as top widget.
 * @param widget Pointer to the VisUIWidget that we use as top widget for the user interface.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_NULL on failure.
 */
int visual_plugin_set_userinterface (VisPluginData *plugin, VisUIWidget *widget)
{
	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	plugin->userinterface = widget;

	return VISUAL_OK;
}

/**
 * Retrieves the VisUI top widget for the plugin.
 *
 * @param plugin Pointer to the VisPluginData of which we request the VisUIWidget that serves as top widget.
 *
 * @return Pointer to the VisUIWidget that serves as top widget, possibly NULL.
 */
VisUIWidget *visual_plugin_get_userinterface (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return plugin->userinterface;
}

/**
 * Gives the VisPluginInfo related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisPluginInfo is requested.
 *
 * @return The VisPluginInfo within the VisPluginData, or NULL on failure.
 */
VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return plugin->info;
}

/**
 * Gives the VisParamContainer related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisParamContainer is requested.
 *
 * @return The VisParamContainer within the VisPluginData, or NULL on failure.
 */
VisParamContainer *visual_plugin_get_params (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return plugin->params;
}

/**
 * Gives the VisRandomContext related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisRandomContext is requested.
 *
 * @return The VisRandomContext within the VisPluginDAta, or NULL on failure.
 */
VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->random;
}

/**
 * Retrieves the plugin specific part of a plugin.
 *
 * @param plugin The pointer to the VisPluginData from which we want the plugin specific part.
 *
 * @return Void * pointing to the plugin specific part which can be cast.
 */
void *visual_plugin_get_specific (VisPluginData *plugin)
{
	VisPluginInfo *pluginfo;

	visual_log_return_val_if_fail (plugin != NULL, NULL);

	pluginfo = visual_plugin_get_info (plugin);
	visual_log_return_val_if_fail (pluginfo != NULL, NULL);

	return pluginfo->plugin;
}

/**
 * Creates a new VisPluginRef structure.
 *
 * The VisPluginRef contains data for the plugin loader.
 *
 * @return Newly allocated VisPluginRef.
 */
VisPluginRef *visual_plugin_ref_new ()
{
	VisPluginRef *ref;

	ref = visual_mem_new0 (VisPluginRef, 1);

	visual_plugin_ref_init (ref);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (ref), TRUE);
	visual_object_ref (VISUAL_OBJECT (ref));

	return ref;
}

int visual_plugin_ref_init (VisPluginRef *ref)
{
	visual_log_return_val_if_fail (ref != NULL, -VISUAL_ERROR_PLUGIN_REF_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (ref));
	visual_object_set_dtor (VISUAL_OBJECT (ref), plugin_ref_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (ref), FALSE);

	/* Set the VisPluginRef data */
	visual_object_clean (VISUAL_OBJECT (ref), VisPluginRef);

	return VISUAL_OK;
}

/**
 * Creates a new VisPluginData structure.
 *
 * @return A newly allocated VisPluginData.
 */
VisPluginData *visual_plugin_new ()
{
	VisPluginData *plugin;

	plugin = visual_mem_new0 (VisPluginData, 1);

	visual_plugin_init (plugin);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (plugin), TRUE);
	visual_object_ref (VISUAL_OBJECT (plugin));

	return plugin;
}

int visual_plugin_init (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (plugin));
	visual_object_set_dtor (VISUAL_OBJECT (plugin), plugin_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (plugin), FALSE);

	/* Set the VisPluginRef data */
	visual_object_clean (VISUAL_OBJECT (plugin), VisPluginData);

	visual_event_queue_init (&plugin->eventqueue);
	plugin->params = visual_param_container_new ();

	visual_hashmap_init (&plugin->environment, visual_object_collection_destroyer);

	return VISUAL_OK;
}



/**
 * Gives a VisHashmap that contains references to all the plugins in the registry.
 *
 * @see VisPluginRef
 * @see visual_plugin_set_registry
 * 
 * @return VisHashmap of references to all the libvisual plugins.
 */
VisHashmap *visual_plugin_get_registry ()
{
	return __lv_plugins;
}

/**
 * Sets the VisHashmap that is used as the main plugin repository.
 *
 * @see visual_plugin_get_registry
 *
 * @return VISUAL_OK on succes.
 */
int visual_plugin_set_registry (VisHashmap *map)
{
	if (__lv_plugins != NULL)
		visual_object_unref (VISUAL_OBJECT (__lv_plugins));

	__lv_plugins = map;

	return VISUAL_OK;
}

/**
 * Gives a newly allocated VisHashmap with references for one plugin type.
 *
 * @see VisPluginRef
 *
 * @param plugmap Pointer to the VisHashmap that contains the plugin registry.
 * @param domain The plugin type that is filtered for.
 *
 * @return Newly allocated VisHashmap that is a filtered version of the plugin registry.
 */
VisHashmap *visual_plugin_registry_filter (VisHashmap *plugmap, const char *domain)
{
	VisCollectionIterator iter;
	VisHashmap *map;
	int ret;

	visual_log_return_val_if_fail (plugmap != NULL, NULL);

	map = visual_hashmap_new (visual_object_collection_destroyer);

	if (map == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot create a new map"));

		return NULL;
	}

	visual_collection_get_iterator (&iter, VISUAL_COLLECTION (plugmap));

	while (visual_collection_iterator_has_more (&iter)) {
		VisPluginRef *ref;
		int ret;

		VisHashmapChainEntry *mentry = visual_collection_iterator_get_data (&iter);

		ref = mentry->data;

		if ((ret = visual_plugin_type_member_of (ref->info->type, domain)) == TRUE) {
				visual_object_ref (VISUAL_OBJECT (ref));

				visual_hashmap_put_string (map, ref->info->plugname, ref);
		} else if (ret != FALSE) {
			visual_log (VISUAL_LOG_WARNING, visual_error_to_string (ret));
		}

		visual_collection_iterator_next (&iter);
	}

	visual_object_unref (VISUAL_OBJECT (&iter));

	return map;
}

static int plugin_add_dir_to_map (VisHashmap *map, const char *dir)
{
	VisPluginRef **refs;
	char temp[FILENAME_MAX];
	int i, j, n;
	size_t len;
	int cnt = 0;


#if defined(VISUAL_OS_WIN32)
	BOOL fFinished;
	HANDLE hList;
	TCHAR szDir[MAX_PATH+1];
	TCHAR szSubDir[MAX_PATH+1];
	WIN32_FIND_DATA FileData;

	snprintf (szDir, MAX_PATH, "%s\\*", dir);

	hList = FindFirstFile (szDir, &FileData);
	if (hList == INVALID_HANDLE_VALUE) {
		FindClose (hList);

		return 0;
	}

	visual_mem_set (temp, 0, sizeof (temp));

	fFinished = FALSE;

	while (!fFinished) {
		refs = NULL;

		if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

			snprintf (temp, 1023, "%s\\%s", dir, FileData.cFileName);

			len = strlen (temp);
			if (len > 5 && (strncmp (&temp[len - 5], ".dll", 5) == 0))
				refs = visual_plugin_get_references (temp, &cnt);

			if (refs != NULL) {
				for (j = 0; j < cnt; j++)
					visual_hashmap_put_string (map, refs[j]->info->plugname, refs[j]);

				/* This is the pointer pointer pointer, not a ref itself */
				visual_mem_free (refs);
			}

			if (!FindNextFile (hList, &FileData)) {
				if (GetLastError () == ERROR_NO_MORE_FILES) {
					fFinished = TRUE;
				}
			}
		}
	}

	FindClose (hList);
#else
	struct dirent **namelist;

	n = scandir (dir, &namelist, NULL, alphasort);

	if (n < 0)
		return -1;

	/* Free the . and .. entries */
	visual_mem_free (namelist[0]);
	visual_mem_free (namelist[1]);

	visual_mem_set (temp, 0, sizeof (temp));

	for (i = 2; i < n; i++) {
		refs = NULL;

		snprintf (temp, 1023, "%s/%s", dir, namelist[i]->d_name);

		len = strlen (temp);
		if (len > 3 && (strncmp (&temp[len - 3], ".so", 3) == 0))
			refs = visual_plugin_get_references (temp, &cnt);

		if (refs != NULL) {
			for (j = 0; j < cnt; j++)
				visual_hashmap_put_string (map, refs[j]->info->plugname, refs[j]);

			/* This is the pointer pointer pointer, not a ref itself */
			visual_mem_free (refs);
		}

		visual_mem_free (namelist[i]);
	}

	visual_mem_free (namelist);
#endif

	return 0;
}

/**
 * Private function to unload a plugin. After calling this function the
 * given argument is no longer usable.
 *
 * @param plugin Pointer to the VisPluginData that needs to be unloaded.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_NULL, -VISUAL_ERROR_PLUGIN_HANDLE_NULL or
 *	-VISUAL_ERROR_PLUGIN_REF_NULL on failure.
 */
int visual_plugin_unload (VisPluginData *plugin)
{
	VisPluginRef *ref;

	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	ref = plugin->ref;

	/* Not loaded */
	if (plugin->handle == NULL) {
		visual_object_unref (VISUAL_OBJECT (plugin));

		visual_log (VISUAL_LOG_CRITICAL, _("Tried unloading a plugin that never has been loaded."));

		return -VISUAL_ERROR_PLUGIN_HANDLE_NULL;
	}

	if (plugin->realized == TRUE)
		plugin->info->cleanup (plugin);

	if (plugin->info->plugin != NULL)
		visual_object_unref (VISUAL_OBJECT (plugin->info->plugin));

	if (plugin->info != NULL)
		visual_object_unref (VISUAL_OBJECT (plugin->info));

#if defined(VISUAL_OS_WIN32)
	FreeLibrary (plugin->handle);
#else
	dlclose (plugin->handle);
#endif

	plugin->info = NULL;

	if (ref != NULL) {
		if (ref->usecount > 0)
			ref->usecount--;
	}

	visual_param_container_set_eventqueue (plugin->params, NULL);

	visual_object_unref (VISUAL_OBJECT (plugin));

	return VISUAL_OK;
}

/**
 * Private function to load a plugin.
 *
 * @param ref Pointer to the VisPluginRef containing information about
 *	the plugin that needs to be loaded.
 *
 * @return A newly created and loaded VisPluginData.
 */
VisPluginData *visual_plugin_load (VisPluginRef *ref)
{
	VisPluginData *plugin;
	VisTime time_;
	VisPluginInfo *pluginfo;
	VisPluginGetInfoFunc get_plugin_info;
#if defined(VISUAL_OS_WIN32)
	HMODULE handle;
#else /* !VISUAL_OS_WIN32 */
	void *handle;
#endif
	int cnt;

	visual_log_return_val_if_fail (ref != NULL, NULL);
	visual_log_return_val_if_fail (ref->info != NULL, NULL);

	/* Check if this plugin is reentrant */
	if (ref->usecount > 0 && (ref->info->flags & VISUAL_PLUGIN_FLAG_NOT_REENTRANT)) {
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot load plugin %s, the plugin is already loaded and is not reentrant."),
				ref->info->plugname);

		return NULL;
	}

#if defined(VISUAL_OS_WIN32)
	handle = LoadLibrary (ref->file);
#else
	handle = dlopen (ref->file, RTLD_LAZY);
#endif

	if (handle == NULL) {
#if defined(VISUAL_OS_WIN32)
		visual_log (VISUAL_LOG_CRITICAL, "Cannot load plugin: win32 error code: %d", GetLastError ());
#else
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot load plugin: %s"), dlerror ());
#endif
		return NULL;
	}

#if defined(VISUAL_OS_WIN32)
	get_plugin_info = (VisPluginGetInfoFunc) GetProcAddress (handle, "get_plugin_info");
#else
	get_plugin_info = (VisPluginGetInfoFunc) dlsym (handle, "get_plugin_info");
#endif

	if (get_plugin_info == NULL) {
#if defined(VISUAL_OS_WIN32)
		visual_log (VISUAL_LOG_CRITICAL, "Cannot initialize plugin: win32 error code: %d", GetLastError ());

		FreeLibrary (handle);
#else
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot initialize plugin: %s"), dlerror ());

		dlclose (handle);
#endif

		return NULL;
	}

	pluginfo = VISUAL_PLUGININFO (get_plugin_info (&cnt));

	if (pluginfo == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot get plugin info while loading."));

#if defined(VISUAL_OS_WIN32)
		FreeLibrary (handle);
#else
		dlclose (handle);
#endif

		return NULL;
	}

	plugin = visual_plugin_new ();
	plugin->ref = ref;
	plugin->info = &pluginfo[ref->index];

	visual_object_ref (VISUAL_OBJECT (ref));

	ref->usecount++;
	plugin->realized = FALSE;
	plugin->handle = handle;

	/* Now the plugin is set up and ready to be realized, also random seed its random context */
	visual_time_get (&time_);
	visual_random_context_set_seed (&plugin->random, time_.tv_usec);

	return plugin;
}

/**
 * Private function to realize the plugin. This initializes the plugin.
 *
 * @param plugin Pointer to the VisPluginData that needs to be realized.
 * 
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_PLUGIN_ALREADY_REALIZED on failure.
 */
int visual_plugin_realize (VisPluginData *plugin)
{
	VisParamContainer *paramcontainer;

	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	if (plugin->realized == TRUE)
		return -VISUAL_ERROR_PLUGIN_ALREADY_REALIZED;

	paramcontainer = visual_plugin_get_params (plugin);
	visual_param_container_set_eventqueue (paramcontainer, &plugin->eventqueue);

	plugin->info->init (plugin);
	plugin->realized = TRUE;

	return VISUAL_OK;
}

/**
 * Private function to create VisPluginRefs from plugins.
 *
 * @param pluginpath The full path and filename to the plugin of which a reference
 *	needs to be obtained.
 * @param count Int pointer that will contain the number of VisPluginRefs returned.
 *
 * @return The optionally newly allocated VisPluginRefs for the plugin.
 */
VisPluginRef **visual_plugin_get_references (const char *pluginpath, int *count)
{
	VisPluginRef **ref;
	VisPluginInfo *plug_info;
	VisPluginInfo *dup_info;
	const char *plug_name;
	VisPluginGetInfoFunc get_plugin_info;
	int *plugin_version;
#if defined(VISUAL_OS_WIN32)
	HMODULE handle;
#else /* !VISUAL_OS_WIN32 */
	void *handle;
#endif
	int cnt = 1, i;

	visual_log_return_val_if_fail (pluginpath != NULL, NULL);

#if defined(VISUAL_OS_WIN32)
	handle = LoadLibrary (pluginpath);
#else
	handle = dlopen (pluginpath, RTLD_LAZY);
#endif

	if (handle == NULL) {
#if defined(VISUAL_OS_WIN32)
		visual_log (VISUAL_LOG_CRITICAL, "Cannot load plugin: win32 error code: %d", GetLastError());
#else
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot load plugin: %s"), dlerror ());
#endif

		return NULL;
	}

#if defined(VISUAL_OS_WIN32)
	plugin_version = (int *) GetProcAddress (handle, VISUAL_PLUGIN_VERSION_TAG_STRING);
#else
	plugin_version = (int *) dlsym (handle, VISUAL_PLUGIN_VERSION_TAG_STRING);
#endif

	if (plugin_version == NULL || *plugin_version != VISUAL_PLUGIN_API_VERSION) {
		visual_log (VISUAL_LOG_CRITICAL, _("Plugin %s is not compatible with version %s of libvisual"),
				pluginpath, visual_get_version ());

#if defined(VISUAL_OS_WIN32)
		FreeLibrary (handle);
#else
		dlclose (handle);
#endif

		return NULL;
	}

#if defined(VISUAL_OS_WIN32)
	get_plugin_info = (VisPluginGetInfoFunc) GetProcAddress (handle, "get_plugin_info");
#else
	get_plugin_info = (VisPluginGetInfoFunc) dlsym (handle, "get_plugin_info");
#endif

	if (get_plugin_info == NULL) {
#if defined(VISUAL_OS_WIN32)
		visual_log (VISUAL_LOG_CRITICAL, "Cannot initialize plugin: win32 error code: %d", GetLastError ());

		FreeLibrary (handle);
#else
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot initialize plugin: %s"), dlerror ());

		dlclose (handle);
#endif

		return NULL;
	}

	plug_info = VISUAL_PLUGININFO (get_plugin_info (&cnt));

	if (plug_info == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, _("Cannot get plugin info"));

#if defined(VISUAL_OS_WIN32)
		FreeLibrary (handle);
#else
		dlclose (handle);
#endif

		return NULL;
	}

	ref = visual_mem_new0 (VisPluginRef *, cnt);

	for (i = 0; i < cnt; i++) {
		ref[i] = visual_plugin_ref_new ();

		dup_info = visual_plugin_info_new ();
		visual_plugin_info_copy (dup_info, &plug_info[i]);

		ref[i]->index = i;
		ref[i]->info = dup_info;
		ref[i]->file = strdup (pluginpath);

		visual_object_unref (plug_info[i].plugin);
		visual_object_unref (VISUAL_OBJECT (&plug_info[i]));
	}

#if defined(VISUAL_OS_WIN32)
	FreeLibrary (handle);
#else
	dlclose (handle);
#endif

	*count = cnt;

	return ref;
}

/**
 * Private function to create the complete plugin registry from a set of paths.
 *
 * @param paths A pointer array to a set of paths.
 * @param ignore_non_existing A flag that can be set with TRUE or FALSE to ignore non existing dirs.
 *
 * @return A newly allocated VisHashmap containing the plugin registry for the set of paths.
 */
VisHashmap *visual_plugin_get_map (const char **paths, int ignore_non_existing)
{
	VisHashmap *hashmap;
	int i = 0;

	hashmap = visual_hashmap_new (visual_object_collection_destroyer);

	while (paths[i] != NULL) {
		if (plugin_add_dir_to_map (hashmap, paths[i]) < 0) {
			if (ignore_non_existing == FALSE)
				visual_log (VISUAL_LOG_WARNING, _("Failed to add the %s directory to the plugin registry"), paths[i]);
		}

		i++;
	}

	return hashmap;
}

/**
 * Private function to find a plugin in a plugin registry.
 *
 * @param map Pointer to a VisHashmap containing VisPluginRefs in which
 *	the search is done.
 * @param name The name of the plugin we're looking for.
 *
 * @return The VisPluginRef for the plugin if found, or NULL when not found.
 */
VisPluginRef *visual_plugin_find (VisHashmap *map, const char *name)
{
	visual_log_return_val_if_fail (map != NULL, NULL);
	visual_log_return_val_if_fail (name != NULL, NULL);

	return visual_hashmap_get_string (map, name);
}

/**
 * Gives the VISUAL_PLUGIN_API_VERSION value for which the library is compiled.
 * This can be used to check against for API/ABI compatibility check.
 *
 * @return The VISUAL_PLUGIN_API_VERSION define value.
 */
int visual_plugin_get_api_version ()
{
	return VISUAL_PLUGIN_API_VERSION;
}

/**
 * Get the domain part from a plugin type string.
 *
 * @param type The type string.
 *
 * @return A newly allocated string containing the domain part of this plugin type, or NULL on failure.
 */
const char *visual_plugin_type_get_domain (const char *type)
{
	visual_log_return_val_if_fail (type != NULL, NULL);

	return get_delim_node (type, ':', 0);
}

/**
 * Get the package part from a plugin type string.
 *
 * @param type The type string.
 *
 * @return A newly allocated string containing the package part of this plugin type, or NULL on failure.
 */
const char *visual_plugin_type_get_package (const char *type)
{
	visual_log_return_val_if_fail (type != NULL, NULL);

	return get_delim_node (type, ':', 1);
}

/**
 * Get the type part from a plugin type string.
 *
 * @param type The type string.
 *
 * @return A newly allocated string containing the type part of this plugin type, or NULL on failure.
 */
const char *visual_plugin_type_get_type (const char *type)
{
	char *str;
	char *flags;
	char *typestr;

	visual_log_return_val_if_fail (type != NULL, NULL);

	str = get_delim_node (type, ':', 2);

	flags = strchr (str, '.');

	if (flags != NULL) {
		typestr = visual_mem_malloc0 (flags - str);

		strncpy (typestr, str, flags - str);

		visual_mem_free (str);

		return typestr;
	}

	return str;
}

/**
 * Get the depth of a plugin type string.
 *
 * @param type The type string.
 *
 * @return A VisPluginTypeDepth enum value that describes out of how many parts this plugin
 *	type string consists, -VISUAL_ERROR_NULL on failure.
 */
VisPluginTypeDepth visual_plugin_type_get_depth (const char *type)
{
	int i = 0;

	visual_log_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

	while (i < VISUAL_PLUGIN_TYPE_DEPTH_TYPE) {
		char *part;

		part = get_delim_node (type, ':', i);

		if (part == NULL)
			break;

		i++;

		// FIXME can't free, why!! ?
//		visual_mem_free (part);
	}

	return i;
}

/**
 * Check if a certain plugin type string falls within the domain of the other.
 *
 * @param domain The domain in which the type string should fall.
 * @param type The type string that is checked against the given domain.
 *
 * @return TRUE if it falls within the domain, FALSE when not, -VISUAL_ERROR_NULL on failure
 */
int visual_plugin_type_member_of (const char *domain, const char *type)
{
	char *ndomain;
	char *tmp;
	char *comp1;
	char *comp2;
	int diff = 0;
	int i = 0;

	visual_log_return_val_if_fail (domain != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

	ndomain = visual_mem_malloc0 (strlen (domain) + 1);
	tmp = strchr (domain, '.');

	if (tmp != NULL)
		strncpy (ndomain, domain, tmp - domain);
	else
		strcpy (ndomain, domain);

	while (i < visual_plugin_type_get_depth (ndomain)) {
		comp1 = get_delim_node (ndomain, ':', i);
		comp2 = get_delim_node (type, ':', i);

		if (comp1 == NULL || comp2 == NULL) {
			if (comp1 != NULL)
				visual_mem_free (comp1);

			if (comp2 != NULL)
				visual_mem_free (comp2);

			visual_mem_free (ndomain);

			return FALSE;
		}

		if (strcmp (comp1, comp2) != 0)
			diff++;

		visual_mem_free (comp1);
		visual_mem_free (comp2);

		i++;
	}

	visual_mem_free (ndomain);

	if (diff > 0)
		return FALSE;

	return TRUE;
}

/**
 * Retrieves the flags section from the plugin type string.
 *
 * @param type The type string, containing the plugin type and optional flags.
 *
 * return NULL if no flags are found, the flags between the '[' and ']' braces on succes.
 *	for example when the plugin type string is "Libvisual:core:actor.[special|something]"
 *	the returned flag string would be "special|something". Keep in mind that the string is
 *	allocated and should be freed after it's not being used anylonger.
 */
const char *visual_plugin_type_get_flags (const char *type)
{
	char *flagsret;
	char *flagstr;

	visual_log_return_val_if_fail (type != NULL, NULL);

	flagstr = strstr (type, ".[");

	if (flagstr == NULL)
		return NULL;

	flagstr += 2; /* Skip the ".[" */

	flagsret = visual_mem_malloc0 (strlen (flagstr) - 1);

	strncpy (flagsret, flagstr, strlen (flagstr) - 1);
	flagsret[strlen (flagstr) - 1] = '\0';

	return flagsret;
}

/**
 * Checks if a certain flag is found within a plugin type string.
 *
 * @param type The type string, containing the plugin type and optional flags.
 * @param flag The flag string to check for within the type string.
 *
 * @return TRUE in found, FALSE if not found, -VISUAL_ERROR_NULL on failure.
 */
int visual_plugin_type_has_flag (const char *type, const char *flag)
{
	char *flags;
	char *nflag, *s;

	visual_log_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);
	visual_log_return_val_if_fail (flag != NULL, -VISUAL_ERROR_NULL);

	nflag = flags = (char *) visual_plugin_type_get_flags (type);

	if (flags == NULL)
		return FALSE;

	do {
		s = strchr (nflag, '|');

		if (s != NULL) {
			if (strncmp (nflag, flag, s - nflag - 1) == 0) {
				visual_mem_free (flags);

				return TRUE;
			}
		} else {
			if (strcmp (nflag, flag) == 0) {
				visual_mem_free (flags);

				return TRUE;
			}
		}

	} while ((nflag = strchr (nflag, '|') + 1));

	visual_mem_free (flags);

	return FALSE;
}

/**
 * Creates a VisPluginEnviron structure.
 *
 * @param type The Environ type that is requested.
 * @param envobj The VisObject connected to this Environ type.
 *
 * @return A newly allocated VisPluginEnviron, or NULL on failure.
 */
VisPluginEnviron *visual_plugin_environ_new (const char *type, VisObject *envobj)
{
	VisPluginEnviron *enve;

	enve = visual_mem_new0 (VisPluginEnviron, 1);

	visual_plugin_environ_init (enve, type, envobj);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (enve), TRUE);
	visual_object_ref (VISUAL_OBJECT (enve));

	return enve;
}

int visual_plugin_environ_init (VisPluginEnviron *enve, const char *type, VisObject *envobj)
{
	visual_log_return_val_if_fail (enve != NULL, -VISUAL_ERROR_PLUGIN_ENVIRON_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (enve));
	visual_object_set_dtor (VISUAL_OBJECT (enve), plugin_environ_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (enve), FALSE);

	/* Set the VisPluginRef data */
	visual_object_clean (VISUAL_OBJECT (enve), VisPluginEnviron);

	enve->type = type;
	enve->environment = envobj;

	return VISUAL_OK;
}

/**
 * Adds a VisPluginEnviron to the plugin it's environment map.
 *
 * @param plugin Pointer to the VisPluginData to which the VisPluginEnviron is added.
 * @param enve Pointer to the VisPluginEnviron that is added to the VisPluginData.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_NULL, -VISUAL_ERROR_PLUGIN_ENVIRON_NULL,
 *	-VISUAL_ERROR_NULL or error values returned by visual_hashmap_put_string() on failure.
 */
int visual_plugin_environ_add (VisPluginData *plugin, VisPluginEnviron *enve)
{
	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
	visual_log_return_val_if_fail (enve != NULL, -VISUAL_ERROR_PLUGIN_ENVIRON_NULL);
	visual_log_return_val_if_fail (enve->type != NULL, -VISUAL_ERROR_NULL);

	visual_plugin_environ_remove (plugin, enve->type);

	return visual_hashmap_put_string (&plugin->environment, enve->type, enve);
}

/**
 * Removes a VisPluginEnviron from the plugin it's environment map.
 *
 * @param plugin Pointer to the VisPluginData from which the VisPluginEnviron is removed.
 * @param type The Environ type that is removed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_NULL on failure.
 */
int visual_plugin_environ_remove (VisPluginData *plugin, const char *type)
{
	visual_log_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
	visual_log_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

	visual_hashmap_remove_string (&plugin->environment, type, TRUE);

	return VISUAL_OK;
}

/**
 * Retrieves a VisPluginEnviron from the plugin it's environment map.
 *
 * @param plugin Pointer to the VisPluginData from which the VisPluginEnviron is requested.
 * @param type The Environ type that is requested.
 *
 * @return The requested VisPluginEnviron it's environ specific VisObject, or NULL on failure
 */
VisObject *visual_plugin_environ_get (VisPluginData *plugin, const char *type)
{
	VisPluginEnviron *enve;

	visual_log_return_val_if_fail (plugin != NULL, NULL);
	visual_log_return_val_if_fail (type != NULL, NULL);

	return visual_hashmap_get_string (&plugin->environment, type);
}

/**
 * @}
 */

