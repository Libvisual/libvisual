#ifndef _LV_PLUGIN_H
#define _LV_PLUGIN_H

#include <libvisual/lv_video.h>
#include <libvisual/lv_audio.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_songinfo.h>
#include <libvisual/lv_event.h>
#include <libvisual/lv_param.h>
#include <libvisual/lv_random.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Indicates at which version the plugin API is. */
#define VISUAL_PLUGIN_API_VERSION	1

/**
 * Enumerate to define the plugin type. Especially used
 * within the VisPlugin system and also used within the plugin
 * themselves so they can tell the plugin system what kind of
 * plugin they are.
 *
 * There are three different plugins being:
 * 	-# Actor plugins: These are the actual
 * 	visualisation plugins.
 * 	-# Input plugins: These can be used to obtain
 * 	PCM data through, for example different sound servers.
 * 	-# Morph plugins: These are capable of morphing
 * 	between different plugins.
 */
typedef enum {
	VISUAL_PLUGIN_TYPE_NULL,	/**< Used when there is no plugin. */
	VISUAL_PLUGIN_TYPE_ACTOR,	/**< Used when the plugin is an actor plugin. */
	VISUAL_PLUGIN_TYPE_INPUT,	/**< Used when the plugin is an input plugin. */
	VISUAL_PLUGIN_TYPE_MORPH	/**< Used when the plugin is a morph plugin. */
} VisPluginType;

typedef struct _VisPluginRef VisPluginRef;
typedef struct _VisPluginInfo VisPluginInfo;
typedef struct _VisPluginData VisPluginData;

typedef struct _VisActorPlugin VisActorPlugin;
typedef struct _VisInputPlugin VisInputPlugin;
typedef struct _VisMorphPlugin VisMorphPlugin;

/* Actor plugin methods */
typedef int (*plugin_actor_requisition_func_t)(VisPluginData *, int *, int *);
typedef VisPalette *(*plugin_actor_palette_func_t)(VisPluginData *);
typedef int (*plugin_actor_render_func_t)(VisPluginData *, VisVideo *, VisAudio *);

/* Input plugin methods */
typedef int (*plugin_input_upload_func_t)(VisPluginData *, VisAudio *);

/* Morph plugin methods */
typedef int (*plugin_morph_palette_func_t)(VisPluginData *, float, VisAudio *, VisPalette *, VisVideo *, VisVideo *);
typedef int (*plugin_morph_apply_func_t)(VisPluginData *, float, VisAudio *, VisVideo *, VisVideo *, VisVideo *);

/* Plugin standard get_plugin_info method */
typedef const VisPluginInfo *(*plugin_get_info_func_t)(int *);

/* Standard plugin methods */
typedef int (*plugin_init_func_t)(VisPluginData *);
typedef int (*plugin_cleanup_func_t)(VisPluginData *);
typedef int (*plugin_events_func_t)(VisPluginData *, VisEventQueue *);

/**
 * The VisPluginRef data structure contains information about the plugins
 * and does refcounting. It is also used as entries in the plugin registry.
 */
struct _VisPluginRef {
	char			*file;		/**< The file location of the plugin. */
	int			 index;		/**< Contains the index number for the entry in the VisPluginInfo table. */
	int			 usecount;	/**< The use count, this indicates how many instances are loaded. */
	VisPluginInfo		*info;		/**< A copy of the VisPluginInfo structure. */
};

/**
 * The VisPluginInfo data structure contains information about a plugin
 * and is filled within the plugin itself.
 */
struct _VisPluginInfo {
	uint32_t		 struct_size;	/**< Struct size, should always be set for compatability checks. */
	uint32_t		 api_version;	/**< API version, compile plugins always with .api_version = VISUAL_PLUGIN_API_VERSION. */
	VisPluginType		 type;		/**< Plugin type. */

	char			*plugname;	/**< The plugin name as it's saved in the registry. */

	char			*name;		/**< Long name */
	char			*author;	/**< Author */
	char			*version;	/**< Version */
	char			*about;		/**< About */
	char			*help;		/**< Help */

	plugin_init_func_t	 init;		/**< The standard init function, every plugin has to implement this. */
	plugin_cleanup_func_t	 cleanup;	/**< The standard cleanup function, every plugin has to implement this. */
	plugin_events_func_t	 events;	/**< The standard event function, implementation is optional. */

	void			*plugin;	/**< Pointer to the plugin specific data structures. */
};

/**
 * The VisPluginData structure is the main plugin structure, every plugin
 * is encapsulated in this.
 */
struct _VisPluginData {
	VisPluginRef		*ref;		/**< Pointer to the plugin references corresponding to this VisPluginData. */
	const VisPluginInfo	*info;		/**< Pointer to the VisPluginInfo that is obtained from the plugin. */

	VisEventQueue		 eventqueue;	/**< The plugin it's VisEventQueue for queueing events. */
	VisParamContainer	 params;	/**< The plugin it's VisParamContainer in which VisParamEntries can be placed. */

	int			 plugflags;	/**< Plugin flags, currently unused but will be used in the future. */
	VisSongInfo		*songinfo;	/**< Pointer to VisSongInfo that contains information about the current playing song.
						  * This can be NULL. */

	VisRandomContext	 random;	/**< Pointer to the plugin it's private random context. It's highly adviced to use
						  * the plugin it's randomize functions. The reason is so more advanced apps can
						  * semi reproduce visuals. */

	int			 realized;	/**< Flag that indicates if the plugin is realized. */
	void			*handle;	/**< The dlopen handle */
	
	void			*priv;		/**< Pointer to the plugin it's private. */
};

/**
 * The VisActorPlugin structure is the main data structure
 * for the actor (visualisation) plugin.
 *
 * The actor plugin is the visualisation plugin.
 */
struct _VisActorPlugin {
	plugin_actor_requisition_func_t	 requisition;	/**< The requisition function. This is used to
							 * get the desired VisVideo surface size of the plugin. */
	plugin_actor_palette_func_t	 palette;	/**< Used to retrieve the desired palette from the plugin. */
	plugin_actor_render_func_t	 render;	/**< The main render loop. This is called to draw a frame. */

	int				 depth;		/**< The depth flag for the VisActorPlugin. This contains an ORred
							  * value of depths that are supported by the plugin. */
};

/**
 * The VisInputPlugin structure is the main data structure
 * for the input plugin.
 *
 * The input plugin is used to retrieve PCM samples from
 * certain sources.
 */
struct _VisInputPlugin {
	plugin_input_upload_func_t	 upload;	/**< The sample upload function. This is the main function
							  * of the plugin which uploads sample data into
							  * libvisual. */
};

/**
 * The VisMorphPlugin structure is the main data structure
 * for the morph plugin.
 *
 * The morph plugin is capable of morphing between two VisVideo
 * sources, and thus is capable of morphing between two
 * VisActors.
 */
struct _VisMorphPlugin {
	plugin_morph_palette_func_t	 palette;	/**< The plugin's palette function. This can be used
							  * to obtain a palette for VISUAL_VIDEO_DEPTH_8BIT surfaces.
							  * However the function may be set to NULL. In this case the
							  * VisMorph system morphs between palettes itself. */
	plugin_morph_apply_func_t	 apply;		/**< The plugin it's main function. This is used to morph
							  * between two VisVideo sources. */
	int				 depth;		/**< The depth flag for the VisMorphPlugin. This contains an ORred
							  * value of depths that are supported by the plugin. */
	int				 requests_audio;/**< When set on TRUE this will indicate that the Morph plugin
							  * requires an VisAudio context in order to render properly. */
};

/* prototypes */
VisPluginInfo *visual_plugin_info_new (void);
int visual_plugin_info_free (VisPluginInfo *pluginfo);
int visual_plugin_info_copy (VisPluginInfo *dest, VisPluginInfo *src);

int visual_plugin_events_pump (VisPluginData *plugin);
VisEventQueue *visual_plugin_get_eventqueue (VisPluginData *plugin);

const VisPluginInfo *visual_plugin_get_info (VisPluginData *plugin);

VisParamContainer *visual_plugin_get_params (VisPluginData *plugin);

VisRandomContext *visual_plugin_get_random_context (VisPluginData *plugin);

VisPluginRef *visual_plugin_ref_new (void);
int visual_plugin_ref_free (VisPluginRef *ref);
int visual_plugin_ref_list_destroy (VisList *list);

VisPluginData *visual_plugin_new (void);
int visual_plugin_free (VisPluginData *plugin);

VisList *visual_plugin_get_registry (void);
VisList *visual_plugin_registry_filter (VisList *pluglist, VisPluginType type);

char *visual_plugin_get_next_by_name (VisList *list, char *name);
char *visual_plugin_get_prev_by_name (VisList *list, char *name);

int visual_plugin_unload (VisPluginData *plugin);
VisPluginData *visual_plugin_load (VisPluginRef *ref);
int visual_plugin_realize (VisPluginData *plugin);

VisPluginRef **visual_plugin_get_references (char *pluginpath, int *count);
VisList *visual_plugin_get_list (char **paths);

VisPluginRef *visual_plugin_find (VisList *list, char *name);

int visual_plugin_get_api_version (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_PLUGIN_H */
