#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

#include "lvconfig.h"
#include "lv_libvisual.h"
#include "lv_plugin.h"
#include "lv_log.h"
#include "lv_mem.h"

extern VisList *__lv_plugins;

static void ref_list_destroy (void *ref);

static int plugin_add_dir_to_list (VisList *list, char *dir);

static void ref_list_destroy (void *data)
{
	VisPluginRef *ref;

	if (data == NULL)
		return;

	ref = (VisPluginRef *) data;

	visual_plugin_info_free (ref->info);
	visual_plugin_ref_free (ref);
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

	return pluginfo;
}

/**
 * Frees the VisPluginInfo. This frees the VisPluginInfo data structure.
 *
 * @param pluginfo Pointer to the VisPluginInfo that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_info_free (VisPluginInfo *pluginfo)
{
	visual_log_return_val_if_fail (pluginfo != NULL, -1);

	if (pluginfo->plugname != NULL)
		visual_mem_free (pluginfo->plugname);

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

	visual_mem_free (pluginfo);

	return 0;
}

/**
 * Copies data from one VisPluginInfo to another, this does not copy everything
 * but only things that are needed in the local copy for the plugin registry.
 *
 * @param dest Pointer to the destination VisPluginInfo in which some data is copied.
 * @param src Pointer to the source VisPluginInfo from which some data is copied.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_info_copy (VisPluginInfo *dest, VisPluginInfo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -1);
	visual_log_return_val_if_fail (src != NULL, -1);

	memcpy (dest, src, sizeof (VisPluginInfo));

	dest->plugname = strdup (src->plugname);
	dest->name = strdup (src->name);
	dest->author = strdup (src->author);
	dest->version = strdup (src->version);
	dest->about = strdup (src->about);
	dest->help = strdup (src->help);

	return 0;
}


/**
 * Pumps the queued events into the plugin it's event handler if it has one.
 *
 * @param plugin Pointer to a VisPluginData of which the events need to be pumped into
 *	the handler.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_events_pump (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);

	if (plugin->info->events != NULL) {
		plugin->info->events (plugin, &plugin->eventqueue);

		return 0;
	}

	return -1;
}

/**
 * Gives the event queue from a VisPluginData. This queue needs to be used
 * when you want to send events to the plugin.
 *
 * @see visual_plugin_events_pump
 *
 * @param plugin Pointer to the VisPluginData from which we want the queue.
 *
 * @return A pointer to the requested VisEventQueue or NULL on error.
 */
VisEventQueue *visual_plugin_get_eventqueue (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->eventqueue;
}

/**
 * Gives the VisPluginInfo related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisPluginInfo is requested.
 *
 * @return The VisPluginInfo within the VisPluginData, or NULL on error.
 */
const VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return plugin->info;
}

/**
 * Gives the VisParamContainer related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisParamContainer is requested.
 *
 * @return The VisParamContainer within the VisPluginData, or NULL on error.
 */
VisParamContainer *visual_plugin_get_params (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->params;
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
	return (visual_mem_new0 (VisPluginRef, 1));
}

/**
 * Frees the VisPluginRef. This frees the VisPluginRef data structure.
 *
 * @param ref Pointer to the VisPluginRef that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_ref_free (VisPluginRef *ref)
{
	visual_log_return_val_if_fail (ref != NULL, -1);

	if (ref->file != NULL)
		visual_mem_free (ref->file);

	if (ref->usecount > 0)
		visual_log (VISUAL_LOG_CRITICAL, "A plugin reference with %d references has been destroyed.", ref->usecount);
	
	visual_mem_free (ref);

	return 0;
}

/**
 * Destroys a VisList of plugin references. This frees all the
 * references in a list and the list itself. This is used internally
 * to destroy the plugin registry.
 *
 * @param list The list of VisPluginRefs that need to be destroyed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_ref_list_destroy (VisList *list)
{
	int ret1;

	visual_log_return_val_if_fail (list != NULL, -1);
	
	ret1 = visual_list_destroy (list, ref_list_destroy);

	list = NULL;

	return ((ret1 == 0) ? 0 : -1);
}

/**
 * Creates a new VisActorPlugin structure.
 *
 * @return A newly allocated VisActorPlugin.
 */
VisActorPlugin *visual_plugin_actor_new ()
{
	return (visual_mem_new0 (VisActorPlugin, 1));
}

/**
 * Frees the VisActorPlugin. This frees the VisActorPlugin data structure.
 *
 * @param actorplugin Pointer to the VisActorPlugin that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_actor_free (VisActorPlugin *actorplugin)
{
	visual_log_return_val_if_fail (actorplugin != NULL, -1);

	visual_mem_free (actorplugin);

	actorplugin = NULL;

	return 0;
}

/**
 * Creates a new VisInputPlugin structure.
 *
 * @return A newly allocated VisInputPlugin.
 */
VisInputPlugin *visual_plugin_input_new ()
{
	return (visual_mem_new0 (VisInputPlugin, 1));
}

/**
 * Frees the VisInputPlugin. This frees the VisInputPlugin data structure.
 *
 * @param inputplugin Pointer to the VisInputPlugin that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_input_free (VisInputPlugin *inputplugin)
{
	visual_log_return_val_if_fail (inputplugin != NULL, -1);

	visual_mem_free (inputplugin);

	inputplugin = NULL;
	
	return 0;
}

/**
 * Creates a new VisMorphPlugin structure.
 *
 * @return A newly allocated VisMorphPlugin.
 */
VisMorphPlugin *visual_plugin_morph_new ()
{
	return (visual_mem_new0 (VisMorphPlugin, 1));
}

/**
 * Frees the VisMorphPlugin. This frees the VisMorphPlugin data structure.
 *
 * @param morphplugin Pointer to the VisMorphPlugin that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_morph_free (VisMorphPlugin *morphplugin)
{
	visual_log_return_val_if_fail (morphplugin != NULL, -1);

	visual_mem_free (morphplugin);

	morphplugin = NULL;

	return 0;
}

/**
 * Creates a new VisPluginData structure.
 *
 * @return A newly allocated VisPluginData.
 */
VisPluginData *visual_plugin_new ()
{
	return (visual_mem_new0 (VisPluginData, 1));
}

/**
 * Frees the VisPluginData. This frees the VisPluginData data structure.
 *
 * @param plugin Pointer to the VisPluginData that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_free (VisPluginData *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);

	visual_mem_free (plugin);

	plugin = NULL;

	return 0;
}

/**
 * Gives a VisList that contains references to all the plugins in the registry.
 *
 * @see VisPluginRef
 * 
 * @return VisList of references to all the libvisual plugins.
 */
VisList *visual_plugin_get_registry ()
{
	return __lv_plugins;
}

/**
 * Gives a newly allocated VisList with references for one plugin type.
 *
 * @see VisPluginRef
 *
 * @param pluglist Pointer to the VisList that contains the plugin registry.
 * @param type The plugin type that is filtered for.
 *
 * @return Newly allocated VisList that is a filtered version of the plugin registry.
 */
VisList *visual_plugin_registry_filter (VisList *pluglist, VisPluginType type)
{
	VisList *list;
	VisListEntry *entry = NULL;
	VisPluginRef *ref;

	visual_log_return_val_if_fail (pluglist != NULL, NULL);

	list = visual_list_new ();

	if (list == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot create a new list");
		return NULL;
	}

	while ((ref = visual_list_next (pluglist, &entry)) != NULL) {
		if (ref->info->type == type)
			visual_list_add (list, ref);
	}

	return list;
}

/**
 * Get the next plugin based on it's name.
 *
 * @see visual_plugin_registry_filter
 * 
 * @param list Pointer to the VisList containing the plugins. Adviced is to filter
 *	this list first using visual_plugin_registry_filter.
 * @param name Name of a plugin entry of which we want the next entry or NULL to get
 * 	the first entry.
 *
 * @return The name of the next plugin or NULL on error.
 */
char *visual_plugin_get_next_by_name (VisList *list, char *name)
{
	VisListEntry *entry = NULL;
	VisPluginRef *ref;
	int tagged = FALSE;

	visual_log_return_val_if_fail (list != NULL, NULL);

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

/**
 * Get the previous plugin based on it's name.
 *
 * @see visual_plugin_registry_filter
 * 
 * @param list Pointer to the VisList containing the plugins. Adviced is to filter
 *	this list first using visual_plugin_registry_filter.
 * @param name Name of a plugin entry of which we want the previous entry or NULL to get
 * 	the last entry.
 *
 * @return The name of the next plugin or NULL on error.
 */
char *visual_plugin_get_prev_by_name (VisList *list, char *name)
{
	VisListEntry *entry = NULL;
	VisPluginRef *ref, *pref = NULL;
	
	visual_log_return_val_if_fail (list != NULL, NULL);

	if (name == NULL) {
		ref = visual_list_get (list, visual_list_count (list) - 1);
		
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

static int plugin_add_dir_to_list (VisList *list, char *dir)
{
	VisPluginRef *ref;
	char temp[1024];
	struct dirent **namelist;
	int i, j, n, len;
	int cnt = 0;

	n = scandir (dir, &namelist, 0, alphasort);

	if (n < 0)
		return -1;

	/* Free the . and .. entries */
	visual_mem_free (namelist[0]);
	visual_mem_free (namelist[1]);

	for (i = 2; i < n; i++) {
		ref = NULL;

		snprintf (temp, 1023, "%s/%s", dir, namelist[i]->d_name);

		len = strlen (temp);
		if (len > 3 && (strncmp (&temp[len - 3], ".so", 3)) == 0)
			ref = visual_plugin_get_references (temp, &cnt);

		if (ref != NULL) {
			for (j = 0; j < cnt; j++) 
				visual_list_add (list, &ref[j]);
		}

		visual_mem_free (namelist[i]);
	}

	visual_mem_free (namelist);

	return 0;
}

/**
 * Private function to unload a plugin. After calling this function the
 * given argument is no longer usable.
 *
 * @param plugin Pointer to the VisPluginData that needs to be unloaded.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_unload (VisPluginData *plugin)
{
	VisPluginRef *ref;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	ref = plugin->ref;

	/* Not loaded */
	if (plugin->handle == NULL) {
		visual_mem_free (plugin);

		visual_log (VISUAL_LOG_CRITICAL, "Tried unloading a plugin that never has been loaded.");
		return -1;
	}
	
	if (plugin->realized == TRUE)
		plugin->info->cleanup (plugin);

	dlclose (plugin->handle);

	visual_mem_free (plugin);
	
	visual_log_return_val_if_fail (ref != NULL, -1);
	
	if (ref->usecount > 0)
		ref->usecount--;

	return 0;
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
	const VisPluginInfo *pluginfo;
	plugin_get_info_func_t get_plugin_info;
	void *handle;
	int cnt;

	visual_log_return_val_if_fail (ref != NULL, NULL);

	handle = dlopen (ref->file, RTLD_LAZY);

	if (handle == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot load plugin: %s", dlerror ());

		return NULL;
	}
	
	get_plugin_info = (plugin_get_info_func_t) dlsym (handle, "get_plugin_info");
	
	if (get_plugin_info == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot initialize plugin: %s", dlerror ());

		dlclose (handle);

		return NULL;
	}

	pluginfo = get_plugin_info (&cnt);

	/* XXX support array */
	if (pluginfo == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot get plugin info while loading.");

		dlclose (handle);
		
		return NULL;
	}

	plugin = visual_mem_new0 (VisPluginData, 1);
	plugin->ref = ref;
	plugin->info = pluginfo;

	ref->usecount++;
	plugin->realized = FALSE;
	plugin->handle = handle;

	return plugin;
}

/**
 * Private function to realize the plugin. This initializes the plugin.
 *
 * @param plugin Pointer to the VisPluginData that needs to be realized.
 * 
 * @return 0 on succes -1 on error.
 */
int visual_plugin_realize (VisPluginData *plugin)
{
	VisParamContainer *paramcontainer;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	if (plugin->realized == TRUE)
		return -1;

	plugin->info->init (plugin);
	plugin->realized = TRUE;

	paramcontainer = visual_plugin_get_params (plugin);
	visual_param_set_eventqueue (paramcontainer, &plugin->eventqueue);

	return 0;
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
VisPluginRef *visual_plugin_get_references (char *pluginpath, int *count)
{
	VisPluginRef *ref;
	const VisPluginInfo *plug_info;
	VisPluginInfo *dup_info;
	const char *plug_name;
	plugin_get_info_func_t get_plugin_info;
	void *handle;
	int cnt = 1, i;

	visual_log_return_val_if_fail (pluginpath != NULL, NULL);

	handle = dlopen (pluginpath, RTLD_LAZY);
	
	if (handle == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot load plugin: %s", dlerror ());

		return NULL;
	}

	get_plugin_info = (plugin_get_info_func_t) dlsym (handle, "get_plugin_info");

	if (get_plugin_info == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot initialize plugin: %s", dlerror ());

		dlclose (handle);

		return NULL;
	}

	plug_info = get_plugin_info (&cnt);

	/* XXX, check api version and struct size */

	if (plug_info == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot get plugin info");

		dlclose (handle);
		
		return NULL;
	}

	/* Check for API and struct size */
	if (plug_info[0].struct_size != sizeof (VisPluginInfo) ||
			plug_info[0].api_version != VISUAL_PLUGIN_API_VERSION) {

		visual_log (VISUAL_LOG_CRITICAL, "Plugin %s is not compatible with version %s of libvisual",
				pluginpath, visual_get_version ());

		dlclose (handle);

		return NULL;
	}

	ref = visual_mem_new0 (VisPluginRef, cnt);
	
	for (i = 0; i < cnt; i++) {
		dup_info = visual_plugin_info_new ();
		visual_plugin_info_copy (dup_info, (VisPluginInfo *) &plug_info[i]);
		
		ref[i].info = dup_info;
		ref[i].file = strdup (pluginpath);
	}

	dlclose (handle);
	
	*count = cnt;	

	return ref;
}

/**
 * Private function to create the complete plugin registry from a set of paths.
 *
 * @param paths A pointer list to a set of paths.
 *
 * @return A newly allocated VisList containing the plugin registry for the set of paths.
 */
VisList *visual_plugin_get_list (char **paths)
{
	VisList *list = visual_list_new();
	int i = 0;

	while (paths[i] != NULL) {
		if (plugin_add_dir_to_list (list, paths[i++]) < 0) {
			visual_list_destroy (list, NULL);
			return NULL;
		}
	}
	
	return list;
}

/**
 * Private function to find a plugin in a plugin registry.
 *
 * @param list Pointer to a VisList containing VisPluginRefs in which
 *	the search is done.
 * @param name The name of the plugin we're looking for.
 *
 * @return The VisPluginRef for the plugin if found, or NULL when not found.
 */
VisPluginRef *visual_plugin_find (VisList *list, char *name)
{
	VisListEntry *entry = NULL;
	VisPluginRef *ref;

	while ((ref = visual_list_next (list, &entry)) != NULL) {

		if (ref->info->plugname == NULL)
			continue;

		if (strcmp (name, ref->info->plugname) == 0)
			return ref;
	}

	return NULL;
}

/**
 *
 *
 */
int visual_plugin_get_api_version ()
{
	return VISUAL_PLUGIN_API_VERSION;
}

/**
 * @}
 */

