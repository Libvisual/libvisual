/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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
#include <libvisual/lv_event.h>
#include <libvisual/lv_param.h>
#include <libvisual/lv_random.h>

/**
 * @defgroup VisPlugin VisPlugin
 * @{
 */

/** Compile-time macro indicating the Plugin API version */
#define VISUAL_PLUGIN_API_VERSION	3005

#define VISUAL_PLUGIN_LICENSE_GPLv1 "GPLv1"
#define VISUAL_PLUGIN_LICENSE_GPL   "GPLv2"
#define VISUAL_PLUGIN_LICENSE_GPLv3 "GPLv3"
#define VISUAL_PLUGIN_LICENSE_LGPL	"LGPL"
#define VISUAL_PLUGIN_LICENSE_BSD	"BSD"

#define VISUAL_PLUGIN_VERSION_TAG   "__lv_plugin_libvisual_api_version"
#define VISUAL_PLUGIN_API_VERSION_VALIDATOR \
    LV_C_LINKAGE LV_PLUGIN_EXPORT const int __lv_plugin_libvisual_api_version = VISUAL_PLUGIN_API_VERSION; \
    LV_C_LINKAGE LV_PLUGIN_EXPORT const VisPluginInfo* get_plugin_info (void);

/** Plugin flags */
typedef enum {
	VISUAL_PLUGIN_FLAG_NONE      = 0,   /**< Used to indicate the absence of special flags */
	VISUAL_PLUGIN_FLAG_REENTRANT = 1    /**< Indicate that plugin is safe for multiple instantiation */
} VisPluginFlags;

/** Plugin type */
typedef enum {
    VISUAL_PLUGIN_TYPE_ACTOR,
    VISUAL_PLUGIN_TYPE_INPUT,
    VISUAL_PLUGIN_TYPE_MORPH
} VisPluginType;

#ifdef __cplusplus

namespace LV {

  //! Plugin class
  class LV_API PluginData;

} // LV namespace

typedef LV::PluginData VisPluginData;

#else

typedef struct _VisPluginData VisPluginData;
struct _VisPluginData;

#endif

typedef struct _VisPluginRef VisPluginRef;
typedef struct _VisPluginInfo VisPluginInfo;

/**
 * Function signature and type for the Plugin init() method.
 *
 * The init() method is called to initialise the plugin state and pre-allocate needed resources
 * for the Plugin to run.
 *
 * @param plugin Plugin object
 *
 * @return TRUE on success, FALSE on failure
 */
typedef int (*VisPluginInitFunc)(VisPluginData *plugin);

/**
 * Function signature and tpye for the Plugin cleanup() method.
 *
 * The cleanup() method is called to clean up the plugin state and free any allocated resources.
 *
 * @param plugin Plugin object
 */
typedef void (*VisPluginCleanupFunc)(VisPluginData *plugin);

/**
 * Function signature and type for the Plugin events() method.
 *
 * The events() method is called to process events.
 *
 * @arg plugin Plugin object
 * @arg events Event queue
 *
 * @return TRUE on success, FALSE on failure
 */
typedef int (*VisPluginEventsFunc)(VisPluginData *plugin, VisEventQueue *events);

/**
 * The VisPluginInfo data structure contains information about a plugin
 * and is filled within the plugin itself.
 */
struct _VisPluginInfo {
	VisPluginType  type;     /**< Plugin type  */
	const char    *plugname; /**< The plugin name as it's saved in the registry. */

	const char    *name;     /**< Long name */
	const char    *author;   /**< Author */
	const char    *version;  /**< Version */
	const char    *about;    /**< About */
	const char    *help;     /**< Help */
	const char    *license;  /**< License */
	const char    *url;	     /**< URL that points to further plugin info */

	VisPluginInitFunc	 init;     /**< init() method */
	VisPluginCleanupFunc cleanup;  /**< cleanup() method */
	VisPluginEventsFunc	 events;   /**< events() method (may be NULL) */

	int flags;            /**< Plugin flags */

	void *plugin;         /**< Pointer to type-specific data */
};

LV_BEGIN_DECLS

/**
 * Returns the version of the Plugin API.
 *
 * @return Plugin API version
 */
LV_API int visual_plugin_get_api_version (void);

/**
 * Returns a Plugin's event queue.
 *
 * @see visual_plugin_events_pump
 *
 * @param plugin Plugin object
 *
 * @return Event queue
 */
LV_API VisEventQueue *visual_plugin_get_event_queue (VisPluginData *plugin);

/**
 * Pumps all queued events to a Plugin for handling.
 *
 * @param plugin Plugin object
 */
LV_API void visual_plugin_events_pump (VisPluginData *plugin);

/**
 * Returns information on a Plugin.
 *
 * @param plugin Plugin object
 *
 * @return Information
 */
LV_API const VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin);

/**
 * Retrieves the type-specific information and state of a Plugin.
 *
 * @param plugin Plugin object
 *
 * @return Type-specific data
 */
LV_API void *visual_plugin_get_specific (VisPluginData *plugin);

/**
 * Returns the parameter list of a Plugin.
 *
 * @param plugin Plugin object
 *
 * @return Parameter list, or NULL if none
 */
LV_API VisParamList *visual_plugin_get_params (VisPluginData *plugin);

/**
 * Returns the random number generator assigned to a Plugin.
 *
 * @param plugin Plugin object
 *
 * @return Random number generator
 */
LV_API VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin);

/**
 * Attaches a data pointer to a Plugin.
 *
 * @note The caller is expected to free the data.
 *
 * @see visual_plugin_get_private()
 *
 * @param plugin Plugin object
 * @param priv   Data pointer
 */
LV_API void visual_plugin_set_private (VisPluginData *plugin, void *priv);

/**
 * Returns the private Plugin data pointer set with visual_plugin_set_private().
 *
 * @param plugin Plugin object
 *
 * @return Data pointer
 */
LV_API void *visual_plugin_get_private (VisPluginData *plugin);

/**
 * Loads a Plugin.
 *
 * @note The loaded plugin will need to be initialised with visual_plugin_realize().
 *
 * @see visual_plugin_unload()
 *
 * @param type Plugin type
 * @param name Plugin name
 *
 * @return A newly loaded Plugin
 */
LV_API VisPluginData *visual_plugin_load (VisPluginType type, const char *name);

/**
 * Unloads a Plugin.
 *
 * @param plugin Plugin to unload
 */
LV_API void visual_plugin_unload (VisPluginData *plugin);

/**
 * Realizes a Plugin.
 *
 * Calling this function will initialise the plugin for running.
 *
 * @param plugin Plugin object
 *
 * @return TRUE on success, FALSE otherwise
 */
LV_API int visual_plugin_realize (VisPluginData *plugin);

/**
 * Determines if a Plugin has been realised.
 *
 * @see visual_plugin_realize()
 *
 * @param plugin Plugin object
 *
 * @return TRUE if plugin has been realised, FALSE otherwise
 */
LV_API int visual_plugin_is_realized (VisPluginData *plugin);


LV_END_DECLS

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

  typedef std::vector<PluginRef> PluginList;

  /**
   * Retrieves the name of the next plugin in the given list.
   *
   * @param list a list of plugins
   * @param name name of plugin to start searching from
   *
   * @return name of the next plugin, or NULL if none can be found
   */
  LV_API char const* plugin_get_next_by_name (PluginList const& list, char const* name);

  /**
   * Retrieves the name of the previous plugin in the given list
   *
   * @param list a list of plugins
   * @param name name of plugin to start searching from
   *
   * @return name of the previous plugin, or NULL if none can be found
   */
  LV_API char const* plugin_get_prev_by_name (PluginList const& list, char const* name);

} // LV namespace

#endif /* __cplusplus */

/**
 * @}
 */

#endif /* _LV_PLUGIN_H */
