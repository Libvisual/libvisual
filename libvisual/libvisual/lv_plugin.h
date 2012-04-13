/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_plugin.h,v 1.51 2006/02/13 20:54:08 synap Exp $
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

#ifndef _LV_PLUGIN_H
#define _LV_PLUGIN_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_event.h>
#include <libvisual/lv_param.h>
#include <libvisual/lv_random.h>

/**
 * @defgroup VisPlugin VisPlugin
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_PLUGINREF(obj)				(VISUAL_CHECK_CAST ((obj), VisPluginRef))
#define VISUAL_PLUGININFO(obj)				(VISUAL_CHECK_CAST ((obj), VisPluginInfo))
#define VISUAL_PLUGINDATA(obj)				(VISUAL_CHECK_CAST ((obj), VisPluginData))
#define VISUAL_PLUGINENVIRON(obj)			(VISUAL_CHECK_CAST ((obj), VisPluginEnviron))

/**
 * Indicates at which version the plugin API is.
 */
#define VISUAL_PLUGIN_API_VERSION	3004

/**
 * Standard defination for GPLv1 plugins, use this for the .license entry in VisPluginInfo
 */
#define VISUAL_PLUGIN_LICENSE_GPLv1 "GPLv1"

/**
 * Standard defination for GPLv2 plugins, use this for the .license entry in VisPluginInfo
 */
#define VISUAL_PLUGIN_LICENSE_GPL   "GPLv2"

/**
 * Standard defination for GPLv3 plugins, use this for the .license entry in VisPluginInfo
 */

#define VISUAL_PLUGIN_LICENSE_GPLv3 "GPLv3"

/**
 * Standard defination for LGPL plugins, use this for the .license entry in VisPluginInfo
 */
#define VISUAL_PLUGIN_LICENSE_LGPL	"LGPL"
/**
 * Standard defination for BSD plugins, use this for the .license entry in VisPluginInfo
 */
#define VISUAL_PLUGIN_LICENSE_BSD	"BSD"

#define VISUAL_PLUGIN_VERSION_TAG		"__lv_plugin_libvisual_api_version"
#define VISUAL_PLUGIN_API_VERSION_VALIDATOR	VISUAL_C_LINKAGE const int __lv_plugin_libvisual_api_version = \
						VISUAL_PLUGIN_API_VERSION;


/**
 * Enumerate to define the plugin flags. Plugin flags can be used to
 * define some of the plugin it's behavior.
 */
typedef enum {
	VISUAL_PLUGIN_FLAG_NONE          = 0,   /**< Used to set no flags. */
	VISUAL_PLUGIN_FLAG_NOT_REENTRANT = 1,   /**< Used to tell the plugin loader that this plugin
	                                           * is not reentrant, and can be loaded only once. */
	VISUAL_PLUGIN_FLAG_SPECIAL       = 2    /**< Used to tell the plugin loader that this plugin has
	                                           * special purpose, like the GdkPixbuf plugin, or a webcam
	                                           * plugin. */
} VisPluginFlags;

/**
 * Enumerate to check the depth of the type wildcard/defination used, used together with the visual_plugin_type functions.
 */
typedef enum {
	VISUAL_PLUGIN_TYPE_DEPTH_NONE		= 0,    /**< No type found.*/
	VISUAL_PLUGIN_TYPE_DEPTH_DOMAIN		= 1,    /**< Only domain in type. */
	VISUAL_PLUGIN_TYPE_DEPTH_PACKAGE	= 2,    /**< Domain and package in type. */
	VISUAL_PLUGIN_TYPE_DEPTH_TYPE		= 3     /**< Domain, package and type found in type. */
} VisPluginTypeDepth;

typedef enum {
    VISUAL_PLUGIN_TYPE_ACTOR,
    VISUAL_PLUGIN_TYPE_INPUT,
    VISUAL_PLUGIN_TYPE_MORPH,
    VISUAL_PLUGIN_TYPE_TRANSFORM
} VisPluginType;

typedef struct _VisPluginRef VisPluginRef;
typedef struct _VisPluginInfo VisPluginInfo;
typedef struct _VisPluginData VisPluginData;
typedef struct _VisPluginEnviron VisPluginEnviron;


/* Plugin standard get_plugin_info method */

/**
 * This is the signature for the 'get_plugin_info' function every libvisual plugin needs to have. The 
 * 'get_plugin_info' function provides libvisual plugin data and all the detailed information regarding
 * the plugin. This function is compulsory without it libvisual won't load the plugin.
 *
 * @arg count An int pointer in which the number of VisPluginData entries within the plugin. Plugins can have
 * 	multiple 'features' and thus the count is needed.
 *
 * @return Pointer to the VisPluginInfo array which contains information about the plugin.
 */
typedef const VisPluginInfo *(*VisPluginGetInfoFunc)(void);

/* Standard plugin methods */

/**
 * Every libvisual plugin that is loaded by the libvisual plugin loader needs this signature for it's
 * intialize function.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginInitFunc)(VisPluginData *plugin);

/**
 * Every libvisual plugin that is loaded by the libvisual plugin loader needs this signature for it's
 * cleanup function.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginCleanupFunc)(VisPluginData *plugin);

/**
 * This is the signature for the event handler within libvisual plugins. An event handler is not mandatory because
 * it has no use in some plugin classes but some plugin types require it nonetheless.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 * @arg events Pointer to the VisEventQueue that might contain events that need to be handled.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginEventsFunc)(VisPluginData *plugin, VisEventQueue *events);

/**
 * The VisPluginInfo data structure contains information about a plugin
 * and is filled within the plugin itself.
 */
struct _VisPluginInfo {
	VisObject     object; /**< The VisObject data. */

	VisPluginType type;   /**< Plugin type, in the format of "domain:package:type", as example,
	                         * this could be "Libvisual:core:actor". It's adviced to use the defination macros here
	                         * instead of filling in the string yourself. */
	const char *plugname; /**< The plugin name as it's saved in the registry. */

	const char *name;     /**< Long name */
	const char *author;   /**< Author */
	const char *version;  /**< Version */
	const char *about;    /**< About */
	const char *help;     /**< Help */
	const char *license;  /**< License */

	VisPluginInitFunc	 init;     /**< The standard init function, every plugin has to implement this. */
	VisPluginCleanupFunc cleanup;  /**< The standard cleanup function, every plugin has to implement this. */
	VisPluginEventsFunc	 events;   /**< The standard event function, implementation is optional. */

	int flags;            /**< Plugin flags from the VisPluginFlags enumerate. */

	VisObject           *plugin;   /**< Pointer to the plugin specific data structures. */
};

/**
 * The VisPluginData structure is the main plugin structure, every plugin
 * is encapsulated in this.
 */
struct _VisPluginData {
	VisObject            object;      /**< The VisObject data. */

	VisPluginInfo const *info;        /**< Pointer to the VisPluginInfo that is obtained from the plugin. */

	VisEventQueue       *eventqueue;  /**< The plugin it's VisEventQueue for queueing events. */
	VisParamContainer   *params;      /**< The plugin it's VisParamContainer in which VisParamEntries can be placed. */
	int                  plugflags;   /**< Plugin flags, currently unused but will be used in the future. */

	VisRandomContext    *random;      /**< Pointer to the plugin it's private random context. It's highly adviced to use
	                                     * the plugin it's randomize functions. The reason is so more advanced apps can
	                                     * semi reproduce visuals. */

	int                  realized;    /**< Flag that indicates if the plugin is realized. */

	VisList              environment; /**< Misc environment specific data. */
};

/**
 * The VisPluginEnviron is used to setup a pre realize/init environment for plugins.
 * Some types of plugins might need this internally and thus this system provides this function.
 */
struct _VisPluginEnviron {
	VisObject   object;       /**< The VisObject data. */

	const char *type;         /**< Almost the same as _VisPluginInfo.type. */

	VisObject  *environment;  /**< VisObject that contains environ specific data. */
};

/**
 * Creates a new VisPluginInfo structure.
 *
 * @return A newly allocated VisPluginInfo
 */
VisPluginInfo *visual_plugin_info_new (void);

/**
 * Copies data from one VisPluginInfo to another, this does not copy everything
 * but only things that are needed in the local copy for the plugin registry.
 *
 * @param dest Pointer to the destination VisPluginInfo in which some data is copied.
 * @param src Pointer to the source VisPluginInfo from which some data is copied.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PLUGIN_INFO_NULL on failure.
 */
int visual_plugin_info_copy (VisPluginInfo *dest, VisPluginInfo const* src);

/**
 * Pumps the queued events into the plugin it's event handler if it has one.
 *
 * @param plugin Pointer to a VisPluginData of which the events need to be pumped into
 *	the handler.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER on failure.
 */
int visual_plugin_events_pump (VisPluginData *plugin);

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
VisEventQueue *visual_plugin_get_eventqueue (VisPluginData *plugin);

/**
 * Gives the VisPluginInfo related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisPluginInfo is requested.
 *
 * @return The VisPluginInfo within the VisPluginData, or NULL on failure.
 */
const VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin);

/**
 * Gives the VisParamContainer related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisParamContainer is requested.
 *
 * @return The VisParamContainer within the VisPluginData, or NULL on failure.
 */
VisParamContainer *visual_plugin_get_params (VisPluginData *plugin);

/**
 * Gives the VisRandomContext related to a VisPluginData.
 *
 * @param plugin The VisPluginData of which the VisRandomContext is requested.
 *
 * @return The VisRandomContext within the VisPluginDAta, or NULL on failure.
 */
VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin);

/**
 * Retrieves the plugin specific part of a plugin.
 *
 * @param plugin The pointer to the VisPluginData from which we want the plugin specific part.
 *
 * @return Void * pointing to the plugin specific part which can be cast.
 */
void *visual_plugin_get_specific (VisPluginData *plugin);

/**
 * Creates a new VisPluginData structure.
 *
 * @return A newly allocated VisPluginData.
 */
VisPluginData *visual_plugin_new (void);

/**
 * Private function to unload a plugin. After calling this function the
 * given argument is no longer usable.
 *
 * @param plugin Pointer to the VisPluginData that needs to be unloaded.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PLUGIN_NULL, -VISUAL_ERROR_PLUGIN_HANDLE_NULL or
 *	-VISUAL_ERROR_PLUGIN_REF_NULL on failure.
 */
int visual_plugin_unload (VisPluginData *plugin);

/**
 * Private function to load a plugin.
 *
 * @param ref Pointer to the VisPluginRef containing information about
 *	the plugin that needs to be loaded.
 *
 * @return A newly created and loaded VisPluginData.
 */
VisPluginData *visual_plugin_load (VisPluginType type, const char *name);

/**
 * Private function to realize the plugin. This initializes the plugin.
 *
 * @param plugin Pointer to the VisPluginData that needs to be realized.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_PLUGIN_ALREADY_REALIZED on failure.
 */
int visual_plugin_realize (VisPluginData *plugin);

/**
 * Gives the VISUAL_PLUGIN_API_VERSION value for which the library is compiled.
 * This can be used to check against for API/ABI compatibility check.
 *
 * @return The VISUAL_PLUGIN_API_VERSION define value.
 */
int visual_plugin_get_api_version (void);

/**
 * Creates a VisPluginEnviron structure.
 *
 * @param type The Environ type that is requested.
 * @param envobj The VisObject connected to this Environ type.
 *
 * @return A newly allocated VisPluginEnviron, or NULL on failure.
 */
VisPluginEnviron *visual_plugin_environ_new (const char *type, VisObject *envobj);

/**
 * Adds a VisPluginEnviron to the plugin its environment list.
 *
 * @param plugin Pointer to the VisPluginData to which the VisPluginEnviron is added.
 * @param enve Pointer to the VisPluginEnviron that is added to the VisPluginData.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PLUGIN_NULL, -VISUAL_ERROR_PLUGIN_ENVIRON_NULL,
 *	-VISUAL_ERROR_NULL or error values returned by visual_list_add() on failure.
 */
int visual_plugin_environ_add (VisPluginData *plugin, VisPluginEnviron *enve);

/**
 * Removes a VisPluginEnviron from the plugin it's environment list.
 *
 * @param plugin Pointer to the VisPluginData from which the VisPluginEnviron is removed.
 * @param type The Environ type that is removed.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_NULL on failure.
 */
int visual_plugin_environ_remove (VisPluginData *plugin, const char *type);

/**
 * Retrieves a VisPluginEnviron from the plugin it's environment list.
 *
 * @param plugin Pointer to the VisPluginData from which the VisPluginEnviron is requested.
 * @param type The Environ type that is requested.
 *
 * @return The requested VisPluginEnviron it's environ specific VisObject, or NULL on failure
 */
VisObject *visual_plugin_environ_get (VisPluginData *plugin, const char *type);

VISUAL_END_DECLS

#ifdef __cplusplus

#include <libvisual/lv_module.hpp>
#include <vector>
#include <string>

namespace LV {

  struct PluginRef
  {
      std::string          file;
      VisPluginInfo const* info;
      ModulePtr            module;
  };

  typedef std::vector<PluginRef*> PluginList;

  /**
   * Retrieves the name of the next plugin in the given list.
   *
   * @param list a list of plugins
   * @param name name of plugin to start searching from
   *
   * @return name of the next plugin, or NULL if none can be found
   */
  char const* plugin_get_next_by_name (PluginList const& list, char const* name);

  /**
   * Retrieves the name of the previous plugin in the given list
   *
   * @param list a list of plugins
   * @param name name of plugin to start searching from
   *
   * @return name of the previous plugin, or NULL if none can be found
   */
  char const* plugin_get_prev_by_name (PluginList const& list, char const* name);

} // LV namespace

#endif /* __cplusplus */

/**
 * @}
 */

#endif /* _LV_PLUGIN_H */
