#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>

#include <lvconfig.h>
#include "lv_plugin.h"
#include "lv_log.h"
#include "lv_mem.h"

extern VisList *__lv_plugins;

static void ref_list_destroy (void *ref);

static int plugin_add_dir_to_list (VisList *list, char *dir);
static int plugin_init (LVPlugin *plugin);
static int plugin_cleanup (LVPlugin *plugin);
static char *plugin_get_name (LVPlugin *plugin);
static VisPluginInfo *plugin_get_info (LVPlugin *plugin);
static int plugin_destroy (LVPlugin *plugin);

static void ref_list_destroy (void *data)
{
	VisPluginRef *ref;

	if (data == NULL)
		return;

	ref = (VisPluginRef*) data;
	if (ref->info != NULL)
		visual_plugin_info_free (ref->info);

	visual_plugin_ref_free (ref);
}

/**
 * @defgroup VisPlugin VisPlugin
 * @{
 */

/**
 * Pumps the queued events into the plugin it's event handler if it has one.
 *
 * @param plugin Pointer to a LVPlugin of which the events need to be pumped into
 *	the handler.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_events_pump (LVPlugin *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);

	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return -1;
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			if (plugin->plugin.actorplugin->events != NULL)
				plugin->plugin.actorplugin->events (plugin->plugin.actorplugin, &plugin->eventqueue);

			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			return -1;
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			return -1;
			break;
			
		default:
			return -1;
			break;
	}

	return 0;
}

/**
 * Gives the event queue from a LVPlugin. This queue needs to be used
 * when you want to send events to the plugin.
 *
 * @see visual_plugin_events_pump
 *
 * @param plugin Pointer to the LVPlugin from which we want the queue.
 *
 * @return A pointer to the requested VisEventQueue or NULL on error.
 */
VisEventQueue *visual_plugin_get_eventqueue (LVPlugin *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	return &plugin->eventqueue;
}

/**
 * Creates a new VisPluginInfo structure. Creates a VisPluginInfo and add information to it.
 *
 * @param name The name of the plugin.
 * @param author The author of the plugin.
 * @param version The version string for the plugin.
 * @param about An about text, that tells something about the plugin.
 * @param help A help text, that has some generic help information.
 *
 * @return A newly allocated VisPluginInfo or NULL on error.
 */
VisPluginInfo *visual_plugin_info_new (char *name, char *author, char *version, char *about, char *help)
{
	VisPluginInfo *pluginfo;

	visual_log_return_val_if_fail (name != NULL, NULL);
	visual_log_return_val_if_fail (author != NULL, NULL);
	visual_log_return_val_if_fail (version != NULL, NULL);
	visual_log_return_val_if_fail (about != NULL, NULL);
	visual_log_return_val_if_fail (help != NULL, NULL);
	
	pluginfo = visual_mem_new0 (VisPluginInfo, 1);

	pluginfo->name = strdup (name);
	pluginfo->author = strdup (author);
	pluginfo->version = strdup (version);
	pluginfo->about = strdup (about);
	pluginfo->help = strdup (help);

	return pluginfo;
}

/**
 * Duplicates a VisPluginInfo.
 *
 * @param pluginfo Pointer to the VisPluginInfo that needs to be duplicated.
 *
 * @return Newly allocated duplicate of the plugin information.
 */
VisPluginInfo *visual_plugin_info_duplicate (VisPluginInfo *pluginfo)
{
	VisPluginInfo *plugnew;

	visual_log_return_val_if_fail (pluginfo != NULL, NULL);

	plugnew = visual_plugin_info_new (pluginfo->name, pluginfo->author,
			pluginfo->version, pluginfo->about, pluginfo->help);

	return plugnew;
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
 * Gives the VisPluginInfo related to a LVPlugin.
 *
 * @param plugin The LVPlugin of which the VisPluginInfo is requested.
 *
 * @return The VisPluginInfo within the LVPlugin, or NULL on error.
 */
VisPluginInfo *visual_plugin_get_info (LVPlugin *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, NULL);

	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return NULL;
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			return plugin->plugin.actorplugin->info;
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			return plugin->plugin.inputplugin->info;
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			return plugin->plugin.morphplugin->info;
			break;

		default:
			return NULL;
			break;
	}

	return NULL;
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

	if (ref->name != NULL)
		visual_mem_free (ref->name);

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
 * Creates a new LVPlugin structure.
 *
 * @return A newly allocated LVPlugin.
 */
LVPlugin *visual_plugin_new ()
{
	return (visual_mem_new0 (LVPlugin, 1));
}

/**
 * Frees the LVPlugin. This frees the LVPlugin data structure.
 *
 * @param plugin Pointer to the LVPlugin that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_free (LVPlugin *plugin)
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

	while ((ref = visual_list_next (pluglist, &entry)) != NULL) {
		if (ref->type == type)
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
			return ref->name;

		if (tagged == TRUE)
			return ref->name;

		if (strcmp (name, ref->name) == 0)
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
		
		return ref->name;
	}

	while ((ref = visual_list_next (list, &entry)) != NULL) {
		if (strcmp (name, ref->name) == 0) {
			if (pref != NULL)
				return pref->name;
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
	int i, n, len;

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
			ref = visual_plugin_get_reference (NULL, temp);

		if (ref != NULL)
			visual_list_add (list, ref);
		
		visual_mem_free (namelist[i]);
	}
	visual_mem_free (namelist);

	return 0;
}

static int plugin_init (LVPlugin *plugin)
{
	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return -1;
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			plugin->plugin.actorplugin->ref = plugin->ref;
			plugin->plugin.actorplugin->init (plugin->plugin.actorplugin);
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			plugin->plugin.inputplugin->ref = plugin->ref;
			plugin->plugin.inputplugin->init (plugin->plugin.inputplugin);
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			plugin->plugin.morphplugin->ref = plugin->ref;
			plugin->plugin.morphplugin->init (plugin->plugin.morphplugin);
			break;
			
		default:
			return -1;
			break;
	}

	return 0;
}

static int plugin_cleanup (LVPlugin *plugin)
{
	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return -1;
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			plugin->plugin.actorplugin->cleanup (plugin->plugin.actorplugin);
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			plugin->plugin.inputplugin->cleanup (plugin->plugin.inputplugin);
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			plugin->plugin.morphplugin->cleanup (plugin->plugin.morphplugin);
			break;

		default:
			return -1;
			break;
	}

	return 0;
}

static char *plugin_get_name (LVPlugin *plugin)
{
	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return NULL;
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			return plugin->plugin.actorplugin->name;
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			return plugin->plugin.inputplugin->name;
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			return plugin->plugin.morphplugin->name;
			break;

		default:
			return NULL;
			break;
	}

	return NULL;
}

static VisPluginInfo *plugin_get_info (LVPlugin *plugin)
{
	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			return NULL;
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			return plugin->plugin.actorplugin->info;
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			return plugin->plugin.inputplugin->info;
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			return plugin->plugin.morphplugin->info;
			break;

		default:
			return NULL;
			break;
	}

	return NULL;
}

static int plugin_destroy (LVPlugin *plugin)
{
	visual_plugin_info_free (plugin_get_info (plugin));

	switch (plugin->type) {
		case VISUAL_PLUGIN_TYPE_NULL:
			break;

		case VISUAL_PLUGIN_TYPE_ACTOR:
			visual_plugin_actor_free (plugin->plugin.actorplugin);
			break;

		case VISUAL_PLUGIN_TYPE_INPUT:
			visual_plugin_input_free (plugin->plugin.inputplugin);
			break;

		case VISUAL_PLUGIN_TYPE_MORPH:
			visual_plugin_morph_free (plugin->plugin.morphplugin);
			break;
			
		default:
			return -1;
			break;
	}

	visual_plugin_free (plugin);

	return 0;
}

/**
 * Private function to unload a plugin.
 *
 * @param plugin Pointer to the LVPlugin that needs to be unloaded.
 *
 * @return 0 on succes -1 on error.
 */
int visual_plugin_unload (LVPlugin *plugin)
{
	VisPluginRef *ref;

	visual_log_return_val_if_fail (plugin != NULL, -1);

	/* Not loaded */
	if (plugin->handle == NULL) {
		visual_log (VISUAL_LOG_DEBUG, "The plugin was never loaded");
		return -1;
	}

	ref = plugin->ref;
	visual_log_return_val_if_fail (ref != NULL, -1);
	
	if (plugin->realized == TRUE)
		plugin_cleanup (plugin);

	visual_plugin_info_free (plugin_get_info (plugin));

	dlclose (plugin->handle);

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
 * @return A newly created and loaded LVPlugin.
 */
LVPlugin *visual_plugin_load (VisPluginRef *ref)
{
	LVPlugin *plugin;
	plugin_load_func_t init;
	void *handle;

	visual_log_return_val_if_fail (ref != NULL, NULL);

	handle = dlopen (ref->file, RTLD_LAZY);

	if (handle == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot load plugin: %s", dlerror ());

		return NULL;
	}

	init = (plugin_load_func_t) dlsym (handle, "get_plugin_info");

	if (init == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot initialize plugin: %s", dlerror ());

		dlclose (handle);

		return NULL;
	}

	plugin = init (ref);

	if (plugin != NULL) {
		plugin->ref = ref;
		plugin->handle = handle;

		ref->usecount++;

		plugin->realized = FALSE;

		return plugin;
	} else {
		return NULL;
	}
}

/**
 * Private function to realize the plugin. This initializes the plugin.
 *
 * @param plugin Pointer to the LVPlugin that needs to be realized.
 * 
 * @return 0 on succes -1 on error.
 */
int visual_plugin_realize (LVPlugin *plugin)
{
	visual_log_return_val_if_fail (plugin != NULL, -1);

	if (plugin->realized == TRUE)
		return -1;

	plugin_init (plugin);
	plugin->realized = TRUE;

	return 0;
}

/**
 * Private function to create VisPluginRefs from plugins.
 *
 * @param refn Optionally a pointer to a VisPluginRef if it is not desired to
 *	allocate a new one. If not used pass NULL.
 * @param pluginpath The full path and filename to the plugin of which a reference
 *	needs to be obtained.
 *
 * @return The optionally newly allocated VisPluginRef for the plugin.
 */
VisPluginRef *visual_plugin_get_reference (VisPluginRef *refn, char *pluginpath)
{
	LVPlugin *plugin;
	VisPluginRef *ref;
	VisPluginInfo *plug_info;
	char *plug_name;
	plugin_load_func_t init;
	void *handle;

	visual_log_return_val_if_fail (pluginpath != NULL, NULL);

	handle = dlopen (pluginpath, RTLD_LAZY);
	if (handle == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot load plugin: %s", dlerror ());

		return NULL;
	}

	init = (plugin_load_func_t) dlsym (handle, "get_plugin_info");

	if (init == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot initialize plugin: %s", dlerror ());

		dlclose (handle);

		return NULL;
	}

	if (refn == NULL) {
		ref = visual_plugin_ref_new ();
	} else {
		ref = refn;
	}

	plugin = init (ref);

	if (plugin == NULL) {
		if (refn == NULL && ref != NULL)
			visual_mem_free (ref);

		dlclose (handle);
		return NULL;
	}

	plug_name = plugin_get_name (plugin);
	if (plug_name == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot get plugin name");
		dlclose (handle);
		return NULL;
	}

	plug_info = plugin_get_info (plugin);
	if (plug_info == NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "Cannot get plugin info");
		dlclose (handle);
		return NULL;
	}

	ref->name = strdup (plug_name);
	ref->info = visual_plugin_info_duplicate (plug_info);
	ref->file = strdup (pluginpath);
	ref->type = plugin->type;

	plugin_cleanup (plugin);
	plugin_destroy (plugin);

	dlclose (handle);

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
	int i = 0, ret;

	visual_log_return_val_if_fail (paths != NULL, NULL);

	while (paths[i] != NULL) {
		ret = plugin_add_dir_to_list (list, paths[i++]);
		if (ret < 0) {
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
		if (ref->name == NULL)
			continue;

		if (strcmp (name, ref->name) == 0)
			return ref;
	}

	return NULL;
}

/**
 * @}
 */

