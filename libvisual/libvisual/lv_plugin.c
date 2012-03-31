/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_plugin.c,v 1.80.2.1 2006/03/04 12:32:47 descender Exp $
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

#define _SVID_SOURCE

#include "config.h"
#include "lv_plugin.h"
#include "lv_common.h"
#include "lv_libvisual.h"
#include "lv_util.h"
#include "gettext.h"
#include <stdio.h>
#include <dirent.h>

#if defined(VISUAL_OS_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

extern VisList *__lv_plugins;

static int plugin_info_dtor (VisObject *object);
static int plugin_ref_dtor (VisObject *object);
static int plugin_environ_dtor (VisObject *object);
static int plugin_dtor (VisObject *object);

static int plugin_add_dir_to_list (VisList *list, const char *dir);
static char *get_delim_node (const char *str, char delim, int index);

static int plugin_info_dtor (VisObject *object)
{
	VisPluginInfo *pluginfo = VISUAL_PLUGININFO (object);

	pluginfo->plugname = NULL;
	pluginfo->type = NULL;
	pluginfo->name = NULL;
	pluginfo->author = NULL;
	pluginfo->version = NULL;
	pluginfo->about = NULL;
	pluginfo->help = NULL;

	return VISUAL_OK;
}

static int plugin_ref_dtor (VisObject *object)
{
	VisPluginRef *ref = VISUAL_PLUGINREF (object);

	if (ref->file != NULL)
		visual_mem_free (ref->file);

	if (ref->usecount > 0)
		visual_log (VISUAL_LOG_ERROR, _("A plugin reference with %d instances has been destroyed."), ref->usecount);

	if (ref->info != NULL)
		visual_object_unref (VISUAL_OBJECT (ref->info));

	ref->file = NULL;
	ref->info = NULL;

	return VISUAL_OK;
}

static int plugin_environ_dtor (VisObject *object)
{
	VisPluginEnviron *enve = VISUAL_PLUGINENVIRON (object);

	if (enve->environment != NULL)
		visual_object_unref (enve->environment);

	enve->environment = NULL;

	return VISUAL_OK;
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

	return VISUAL_OK;
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

VisPluginInfo *visual_plugin_info_new ()
{
	VisPluginInfo *pluginfo;

	pluginfo = visual_mem_new0 (VisPluginInfo, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (pluginfo), TRUE, plugin_info_dtor);

	return pluginfo;
}


int visual_plugin_info_copy (VisPluginInfo *dest, VisPluginInfo *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_PLUGIN_INFO_NULL);

	visual_mem_copy (dest, src, sizeof (VisPluginInfo));

	dest->plugname = visual_strdup (src->plugname);
	dest->type = visual_strdup (src->type);
	dest->name = visual_strdup (src->name);
	dest->author = visual_strdup (src->author);
	dest->version = visual_strdup (src->version);
	dest->about = visual_strdup (src->about);
	dest->help = visual_strdup (src->help);

	return VISUAL_OK;
}

int visual_plugin_events_pump (VisPluginData *plugin)
{
	visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	if (plugin->info->events != NULL) {
		plugin->info->events (plugin, &plugin->eventqueue);

		return VISUAL_OK;
	}

	return -VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER;
}

VisEventQueue *visual_plugin_get_eventqueue (VisPluginData *plugin)
{
	visual_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->eventqueue;
}

VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin)
{
	visual_return_val_if_fail (plugin != NULL, NULL);

	return plugin->info;
}

VisParamContainer *visual_plugin_get_params (VisPluginData *plugin)
{
	visual_return_val_if_fail (plugin != NULL, NULL);

	return plugin->params;
}

VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin)
{
	visual_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->random;
}

void *visual_plugin_get_specific (VisPluginData *plugin)
{
	VisPluginInfo *pluginfo;

	visual_return_val_if_fail (plugin != NULL, NULL);

	pluginfo = visual_plugin_get_info (plugin);
	visual_return_val_if_fail (pluginfo != NULL, NULL);

	return pluginfo->plugin;
}

VisPluginRef *visual_plugin_ref_new ()
{
	VisPluginRef *ref;

	ref = visual_mem_new0 (VisPluginRef, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (ref), TRUE, plugin_ref_dtor);

	return ref;
}

VisPluginData *visual_plugin_new ()
{
	VisPluginData *plugin;

	plugin = visual_mem_new0 (VisPluginData, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (plugin), TRUE, plugin_dtor);

	plugin->params = visual_param_container_new ();

	return plugin;
}

VisList *visual_plugin_get_registry ()
{
	return __lv_plugins;
}

VisList *visual_plugin_registry_filter (VisList *pluglist, const char *domain)
{
	VisList *list;
	VisListEntry *entry = NULL;
	VisPluginRef *ref;
	int ret;

	visual_return_val_if_fail (pluglist != NULL, NULL);

	list = visual_list_new (visual_object_collection_destroyer);

	if (list == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot create a new list"));

		return NULL;
	}

	while ((ref = visual_list_next (pluglist, &entry)) != NULL) {
		if ((ret = visual_plugin_type_member_of (ref->info->type, domain))) {
			if (ret == TRUE) {
				visual_object_ref (VISUAL_OBJECT (ref));

				visual_list_add (list, ref);
			} else if (ret != FALSE) {
				visual_log (VISUAL_LOG_WARNING, "%s", visual_error_to_string (ret));
			}
		}
		else if (ret != FALSE) { /* FIXME XXX TODO, patch frmo duilio check how this works */
			visual_log (VISUAL_LOG_WARNING, "%s", visual_error_to_string (ret));
		}
	}

	return list;
}

const char *visual_plugin_get_next_by_name (VisList *list, const char *name)
{
	VisListEntry *entry = NULL;
	VisPluginRef *ref;
	int tagged = FALSE;

	visual_return_val_if_fail (list != NULL, NULL);

	while ((ref = visual_list_next (list, &entry)) != NULL) {
		if (name == NULL)
			return ref->info->plugname;

		if (tagged == TRUE)
			return ref->info->plugname;

		if (strcmp (name, ref->info->plugname) == 0)
			tagged = TRUE;
	}

	return NULL;
}

const char *visual_plugin_get_prev_by_name (VisList *list, const char *name)
{
	VisListEntry *entry = NULL;
	VisPluginRef *ref, *pref = NULL;

	visual_return_val_if_fail (list != NULL, NULL);

	if (name == NULL) {
		ref = visual_list_get (list, visual_collection_size (VISUAL_COLLECTION (list)) - 1);

		if (ref == NULL)
			return NULL;

		return ref->info->plugname;
	}

	while ((ref = visual_list_next (list, &entry)) != NULL) {
		if (strcmp (name, ref->info->plugname) == 0) {
			if (pref != NULL)
				return pref->info->plugname;
			else
				return NULL;
		}

		pref = ref;
	}

	return NULL;
}

static int plugin_add_dir_to_list (VisList *list, const char *dir)
{
	VisPluginRef **ref;
	char temp[FILENAME_MAX];
	int i;
	size_t len;
	int cnt = 0;

#if defined(VISUAL_OS_WIN32)
	BOOL fFinished;
	HANDLE hList;
	TCHAR szDir[MAX_PATH+1];
	WIN32_FIND_DATA FileData;

	snprintf (szDir, MAX_PATH, "%s/*", dir);

	hList = FindFirstFile (szDir, &FileData);
	if (hList == INVALID_HANDLE_VALUE) {
		FindClose (hList);

		return 0;
	}

	visual_mem_set (temp, 0, sizeof (temp));

	fFinished = FALSE;

	while (!fFinished) {
		ref = NULL;

		if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			snprintf (temp, 1023, "%s/%s", dir, FileData.cFileName);
			len = strlen (temp);

			if (len > 5 && (strncmp (&temp[len - 4], ".dll", 4) == 0))
				ref = visual_plugin_get_references (temp, &cnt);

			if (ref != NULL) {
				for (i = 0; i < cnt; i++)
					visual_list_add (list, ref[i]);

				/* This is the pointer pointer pointer, not a ref itself */
				visual_mem_free (ref);
			}
		}

		if (!FindNextFile (hList, &FileData)) {
			if (GetLastError () == ERROR_NO_MORE_FILES) {
				fFinished = TRUE;
			}
		}
	}

	FindClose (hList);
#else
	struct dirent **namelist;
	int j, n;

	n = scandir (dir, &namelist, NULL, alphasort);

	if (n < 0)
		return -1;

	/* Free the . and .. entries */
	visual_mem_free (namelist[0]);
	visual_mem_free (namelist[1]);

	visual_mem_set (temp, 0, sizeof (temp));

	for (i = 2; i < n; i++) {
		ref = NULL;

		snprintf (temp, 1023, "%s/%s", dir, namelist[i]->d_name);

		len = strlen (temp);
		if (len > 3 && (strncmp (&temp[len - 3], ".so", 3) == 0))
			ref = visual_plugin_get_references (temp, &cnt);

		if (ref != NULL) {
			for (j = 0; j < cnt; j++)
				visual_list_add (list, ref[j]);

			/* This is the pointer pointer pointer, not a ref itself */
			visual_mem_free (ref);
		}

		visual_mem_free (namelist[i]);
	}

	visual_mem_free (namelist);
#endif

	return 0;
}

int visual_plugin_unload (VisPluginData *plugin)
{
	VisPluginRef *ref;

	visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	ref = plugin->ref;

	/* Not loaded */
	if (plugin->handle == NULL) {
		visual_object_unref (VISUAL_OBJECT (plugin));

		visual_log (VISUAL_LOG_ERROR, _("Tried unloading a plugin that never has been loaded."));

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

	visual_return_val_if_fail (ref != NULL, NULL);
	visual_return_val_if_fail (ref->info != NULL, NULL);

	/* Check if this plugin is reentrant */
	if (ref->usecount > 0 && (ref->info->flags & VISUAL_PLUGIN_FLAG_NOT_REENTRANT)) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin %s, the plugin is already loaded and is not reentrant."),
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
		visual_log (VISUAL_LOG_ERROR, "Cannot load plugin: win32 error code: %ld", GetLastError ());
#else
		visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin: %s"), dlerror ());
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
		visual_log (VISUAL_LOG_ERROR, "Cannot initialize plugin: win32 error code: %ld", GetLastError ());

		FreeLibrary (handle);
#else
		visual_log (VISUAL_LOG_ERROR, _("Cannot initialize plugin: %s"), dlerror ());

		dlclose (handle);
#endif

		return NULL;
	}

	pluginfo = VISUAL_PLUGININFO (get_plugin_info (&cnt));

	if (pluginfo == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot get plugin info while loading."));

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
	visual_random_context_set_seed (&plugin->random, time_.usec);

	return plugin;
}

int visual_plugin_realize (VisPluginData *plugin)
{
	VisParamContainer *paramcontainer;

	visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);

	if (plugin->realized == TRUE)
		return -VISUAL_ERROR_PLUGIN_ALREADY_REALIZED;

	paramcontainer = visual_plugin_get_params (plugin);
	visual_param_container_set_eventqueue (paramcontainer, &plugin->eventqueue);
	plugin->info->init (plugin);
	plugin->realized = TRUE;

	return VISUAL_OK;
}

VisPluginRef **visual_plugin_get_references (const char *pluginpath, int *count)
{
	VisPluginRef **ref;
	VisPluginInfo *plug_info;
	VisPluginInfo *dup_info;
	VisPluginGetInfoFunc get_plugin_info;
	int *plugin_version;
#if defined(VISUAL_OS_WIN32)
	HMODULE handle;
#else /* !VISUAL_OS_WIN32 */
	void *handle;
#endif
	int cnt = 1, i;

	visual_return_val_if_fail (pluginpath != NULL, NULL);

#if defined(VISUAL_OS_WIN32)
	handle = LoadLibrary (pluginpath);
#else
	handle = dlopen (pluginpath, RTLD_LAZY);
#endif

	if (handle == NULL) {
#if defined(VISUAL_OS_WIN32)
		visual_log (VISUAL_LOG_ERROR, "Cannot load plugin: win32 error code: %ld", GetLastError());
#else
		visual_log (VISUAL_LOG_ERROR, _("Cannot load plugin: %s"), dlerror ());
#endif

		return NULL;
	}

#if defined(VISUAL_OS_WIN32)
	plugin_version = (int *) GetProcAddress (handle, VISUAL_PLUGIN_VERSION_TAG);
#else
	plugin_version = (int *) dlsym (handle, VISUAL_PLUGIN_VERSION_TAG);
#endif

	if (plugin_version == NULL || *plugin_version != VISUAL_PLUGIN_API_VERSION) {
		visual_log (VISUAL_LOG_ERROR, _("Plugin %s is not compatible with version %s of libvisual"),
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
		visual_log (VISUAL_LOG_ERROR, "Cannot initialize plugin: win32 error code: %ld", GetLastError ());

		FreeLibrary (handle);
#else
		visual_log (VISUAL_LOG_ERROR, _("Cannot initialize plugin: %s"), dlerror ());

		dlclose (handle);
#endif

		return NULL;
	}

	plug_info = VISUAL_PLUGININFO (get_plugin_info (&cnt));

	if (plug_info == NULL) {
		visual_log (VISUAL_LOG_ERROR, _("Cannot get plugin info"));

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
		ref[i]->file = visual_strdup (pluginpath);

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

VisList *visual_plugin_get_list (const char **paths, int ignore_non_existing)
{
	VisList *list;
	int i = 0;

	list = visual_list_new (visual_object_collection_destroyer);

	while (paths[i] != NULL) {
		if (plugin_add_dir_to_list (list, paths[i]) < 0) {
			if (ignore_non_existing == FALSE)
				visual_log (VISUAL_LOG_WARNING, _("Failed to add the %s directory to the plugin registry"), paths[i]);
		}

		i++;
	}

	return list;
}

VisPluginRef *visual_plugin_find (VisList *list, const char *name)
{
	VisListEntry *entry = NULL;
	VisPluginRef *ref;

	visual_return_val_if_fail (list != NULL, NULL);
	visual_return_val_if_fail (name != NULL, NULL);

	while ((ref = visual_list_next (list, &entry)) != NULL) {

		if (ref->info->plugname == NULL)
			continue;

		if (strcmp (name, ref->info->plugname) == 0)
			return ref;
	}

	return NULL;
}

int visual_plugin_get_api_version ()
{
	return VISUAL_PLUGIN_API_VERSION;
}

const char *visual_plugin_type_get_domain (const char *type)
{
	visual_return_val_if_fail (type != NULL, NULL);

	return get_delim_node (type, ':', 0);
}

const char *visual_plugin_type_get_package (const char *type)
{
	visual_return_val_if_fail (type != NULL, NULL);

	return get_delim_node (type, ':', 1);
}

const char *visual_plugin_type_get_type (const char *type)
{
	char *str;
	char *flags;
	char *typestr;

	visual_return_val_if_fail (type != NULL, NULL);

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

VisPluginTypeDepth visual_plugin_type_get_depth (const char *type)
{
	int i = 0;

	visual_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

	while (i < VISUAL_PLUGIN_TYPE_DEPTH_TYPE) {
		char *part;

		part = get_delim_node (type, ':', i);

		if (part == NULL)
			break;

		i++;

		visual_mem_free (part);
	}

	return i;
}

int visual_plugin_type_member_of (const char *domain, const char *type)
{
	char *ndomain;
	char *tmp;
	char *comp1;
	char *comp2;
	int diff = 0;
	int i = 0;

	visual_return_val_if_fail (domain != NULL, -VISUAL_ERROR_NULL);
	visual_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

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

const char *visual_plugin_type_get_flags (const char *type)
{
	char *flagsret;
	char *flagstr;

	visual_return_val_if_fail (type != NULL, NULL);

	flagstr = strstr (type, ".[");

	if (flagstr == NULL)
		return NULL;

	flagstr += 2; /* Skip the ".[" */

	flagsret = visual_mem_malloc0 (strlen (flagstr) - 1);

	strncpy (flagsret, flagstr, strlen (flagstr) - 1);
	flagsret[strlen (flagstr) - 1] = '\0';

	return flagsret;
}

int visual_plugin_type_has_flag (const char *type, const char *flag)
{
	char *flags;
	char *nflag, *s;

	visual_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);
	visual_return_val_if_fail (flag != NULL, -VISUAL_ERROR_NULL);

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

VisPluginEnviron *visual_plugin_environ_new (const char *type, VisObject *envobj)
{
	VisPluginEnviron *enve;

	enve = visual_mem_new0 (VisPluginEnviron, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (enve), TRUE, plugin_environ_dtor);

	enve->type = type;
	enve->environment = envobj;

	return enve;
}


int visual_plugin_environ_add (VisPluginData *plugin, VisPluginEnviron *enve)
{
	visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
	visual_return_val_if_fail (enve != NULL, -VISUAL_ERROR_PLUGIN_ENVIRON_NULL);
	visual_return_val_if_fail (enve->type != NULL, -VISUAL_ERROR_NULL);

	visual_plugin_environ_remove (plugin, enve->type);

	return visual_list_add (&plugin->environment, enve);
}

int visual_plugin_environ_remove (VisPluginData *plugin, const char *type)
{
	VisPluginEnviron *enve;
	VisListEntry *le = NULL;

	visual_return_val_if_fail (plugin != NULL, -VISUAL_ERROR_PLUGIN_NULL);
	visual_return_val_if_fail (type != NULL, -VISUAL_ERROR_NULL);

	while ((enve = visual_list_next (&plugin->environment, &le)) != NULL) {

		/* Remove from list */
		if (strcmp (enve->type, type) == 0) {
			visual_list_delete (&plugin->environment, &le);

			visual_object_unref (VISUAL_OBJECT (enve));

			return VISUAL_OK;
		}
	}

	return VISUAL_OK;
}

VisObject *visual_plugin_environ_get (VisPluginData *plugin, const char *type)
{
	VisPluginEnviron *enve;
	VisListEntry *le = NULL;

	visual_return_val_if_fail (plugin != NULL, NULL);
	visual_return_val_if_fail (type != NULL, NULL);

	while ((enve = visual_list_next (&plugin->environment, &le)) != NULL) {

		if (strcmp (enve->type, type) == 0)
			return enve->environment;
	}

	return NULL;
}

