#ifndef _LV_PLUGIN_H
#define _LV_PLUGIN_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <libvisual/lv_video.h>
#include <libvisual/lv_audio.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_songinfo.h>
#include <libvisual/lv_event.h>

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

typedef struct _VisPluginInfo VisPluginInfo;
typedef struct _VisPluginRef VisPluginRef;

typedef struct _VisActorPlugin VisActorPlugin;
typedef struct _VisInputPlugin VisInputPlugin;
typedef struct _VisMorphPlugin VisMorphPlugin;

typedef struct _LVPlugin LVPlugin;

/* Actor plugin methods */
typedef int (*plugin_actor_init_func_t)(VisActorPlugin *);
typedef int (*plugin_actor_cleanup_func_t)(VisActorPlugin *);
typedef int (*plugin_actor_requisition_func_t)(VisActorPlugin *, int *, int *);
typedef int (*plugin_actor_events_func_t)(VisActorPlugin *, VisEventQueue *);
typedef VisPalette *(*plugin_actor_palette_func_t)(VisActorPlugin *);
typedef int (*plugin_actor_render_func_t)(VisActorPlugin *, VisVideo *, VisAudio *);

/* Input plugin methods */
typedef int (*plugin_input_init_func_t)(VisInputPlugin *);
typedef int (*plugin_input_cleanup_func_t)(VisInputPlugin *);
typedef int (*plugin_input_upload_func_t)(VisInputPlugin *, VisAudio *);

/* Morph plugin methods */
typedef int (*plugin_morph_init_func_t)(VisMorphPlugin *);
typedef int (*plugin_morph_cleanup_func_t)(VisMorphPlugin *);
typedef int (*plugin_morph_palette_func_t)(VisMorphPlugin *, float, VisAudio *, VisPalette *, VisVideo *, VisVideo *);
typedef int (*plugin_morph_apply_func_t)(VisMorphPlugin *, float, VisAudio *, VisVideo *, VisVideo *, VisVideo *);

/* Plugin methods */
typedef LVPlugin *(*plugin_load_func_t)(VisPluginRef *);

/**
 * The VisPluginInfo data structure contains information about a plugin
 * and is filled within the plugin itself.
 */
struct _VisPluginInfo {
	char		*name;		/**< Contains the name of the plugin. */
	char		*author;	/**< Contains information about the authors. */
	char		*version;	/**< Contains information about the plugin it's version. */
	char		*about;		/**< Contains some about description. */
	char		*help;		/**< Contains an explanation for the plugin. */
};

/**
 * The VisPluginRef data structure contains information for the plugin loader.
 */
struct _VisPluginRef {
	char		*file;		/**< Location of the plugin. */
	char		*name;		/**< Name of the plugin. */
	int		 usecount;	/**< Use count that holds the number of instances loaded. */
	VisPluginType	 type;		/**< Holds the plugin type. */
	VisPluginInfo	*info;		/**< Pointer to the VisPluginInfo data structure. */
};

/**
 * The VisActorPlugin structure is the main data structure
 * for the actor (visualisation) plugin.
 *
 * The actor plugin is the visualisation plugin.
 */
struct _VisActorPlugin {
	char				*file;		/**< Location of the plugin. */
	char				*name;		/**< Name of the plugin. */
	VisPluginRef			*ref;		/**< Pointer to the plugin reference. */
	VisPluginInfo			*info;		/**< Pointer to the VisPluginInfo data structure
							 * containing information about the plugin. */
	plugin_actor_init_func_t	 init;		/**< The plugin it's initialize function. */
	plugin_actor_cleanup_func_t	 cleanup;	/**< The plugin it's cleanup function. */
	plugin_actor_requisition_func_t	 requisition;	/**< The requisition function. This is used to
							 * get the desired VisVideo surface size of the plugin. */
	plugin_actor_events_func_t	 events;	/**< The plugin it's event handler.
							 * Resize events also come through the event handler first. */
	plugin_actor_palette_func_t	 palette;	/**< Used to retrieve the desired palette from the plugin. */
	plugin_actor_render_func_t	 render;	/**< The main render loop. This is called to draw a frame. */
	void				*handle;	/**< Contains the handle that is given by dlopen. */
	int				 depth;		/**< A flag that contains the supported depths.
							 * This is made up by ORred values of the VisVideoDepth type.
							 * @see VisVideoDepth */
	int				 schedflags;	/**< Scheduler flags are hint flags for the auto plugin scheduler
							 * which is not yet supported. */
	int				 plugflags;	/**< Plugin flags are hint flags for the plugin loader,
							 * which is not yet supported. */
	VisSongInfo			*songinfo;	/**< Pointer to the VisSongInfo structure containing
							 * information about the current song being played. */
	void				*private;	/**< Private to interchange data
							 * between the plugin it's methods and functions.
							 * It's highly adviced that all data is encapsulated in a 
							 * private so the plugin is reentrant. */
};

/**
 * The VisInputPlugin structure is the main data structure
 * for the input plugin.
 *
 * The input plugin is used to retrieve PCM samples from
 * certain sources.
 */
struct _VisInputPlugin {
	char				*file;		/**< Location of the plugin. */
	char				*name;		/**< Name of the plugin. */
	VisPluginRef			*ref;		/**< Pointer to the plugin reference. */
	VisPluginInfo			*info;		/**< Pointer to the VisPluginInfo data structure
							  * containing information about the plugin. */
	plugin_input_init_func_t	 init;		/**< The plugin it's initialize function. */
	plugin_input_cleanup_func_t	 cleanup;	/**< The plugin it's cleanup function. */
	plugin_input_upload_func_t	 upload;	/**< The sample upload function. This is the main function
							  * of the plugin which uploads sample data into
							  * libvisual. */
	void				*handle;	/**< Contains the handle that is given by dlopen. */
	int				 plugflags;	/**< Plugin flags are hint flags for the plugin loader,
							  * which is not yet supported. */
	void				*private;	/**< Private to interchange data
							  * between the plugin it's methods and functions.
							  * It's highly adviced that all data is encapsulated in a
							  * private so the plugin is reentrant. */
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
	char				*file;		/**< Location of the plugin. */
	char				*name;		/**< Name of the plugin. */
	VisPluginRef			*ref;		/**< Pointer to the plugin reference. */
	VisPluginInfo			*info;		/**< Pointer to the VisPluginInfo data structure
							  * containing information about the plugin. */
	plugin_morph_init_func_t	 init;		/**< The plugin it's initialize function. */
	plugin_morph_cleanup_func_t	 cleanup;	/**< The plugin it's cleanup function. */
	plugin_morph_palette_func_t	 palette;	/**< The plugin it's palette function. This can be used
							  * to obtain a palette for VISUAL_VIDEO_DEPTH_8BIT surfaces.
							  * However the function may be set to NULL. In this case the
							  * VisMorph system morphs between palettes itself. */
	plugin_morph_apply_func_t	 apply;		/**< The plugin it's main function. This is used to morph
							  * between two VisVideo sources. */
	void				*handle;	/**< Contains the handle that is given by dlopen. */
	int				 depth;		/**< A flag that contains the supported depths.
							  * This is made up by ORred values of the VisVideoDepth type.
							  * @see VisVideoDepth */
	int				 plugflags;	/**< Plugin flags are hint flags for the plugin loader,
							  * Which is not yet supproted. */
	void				*private;	/**< Private to interchange data
							  * between the plugin it's methods and functions.
							  * It's highly adviced that all data is encapsulated in a
							  * private so the plugin is reentrant. */
};

/**
 * This LVPlugin data structure is the main data structure for
 * every plugin.
 *
 * It's not called VisPlugin because XMMS has this in it's header files
 * and we're open for suggestion for a better naming using the Vis prefix.
 */
struct _LVPlugin {
	VisPluginRef	*ref;			/**< Pointer to the plugin reference. */
	void		*handle;		/**< Handle from dlopen. */
	VisPluginType	 type;			/**< Contains the plugin type. */
	int		 realized;		/**< Is set when the plugin is realized. this
						  * means that when the plugin it's init function
						  * has been called. */
	union {
		VisActorPlugin *actorplugin;	/**< Union entry used when the plugin is an actor plugin. */
		VisInputPlugin *inputplugin;	/**< Union entry used when the plugin is an input plugin. */
		VisMorphPlugin *morphplugin;	/**< Union entry used when the plugin is an morph plugin. */
	} plugin;				/**< Union that holds the three different plugin types. */
};

/* prototypes */
VisPluginInfo *visual_plugin_info_new (char *name, char *author, char *version, char *about, char *help);
VisPluginInfo *visual_plugin_info_duplicate (VisPluginInfo *pluginfo);
int visual_plugin_info_free (VisPluginInfo *pluginfo);

VisPluginRef *visual_plugin_ref_new ();
int visual_plugin_ref_free (VisPluginRef *ref);
int visual_plugin_ref_list_destroy (VisList *list);

VisActorPlugin *visual_plugin_actor_new ();
int visual_plugin_actor_free (VisActorPlugin *actorplugin);

VisInputPlugin *visual_plugin_input_new ();
int visual_plugin_input_free (VisInputPlugin *inputplugin);

VisMorphPlugin *visual_plugin_morph_new ();
int visual_plugin_morph_free (VisMorphPlugin *morphplugin);

LVPlugin *visual_plugin_new ();
int visual_plugin_free (LVPlugin *plugin);

VisList *visual_plugin_get_registry ();
VisList *visual_plugin_registry_filter (VisList *pluglist, VisPluginType type);

char *visual_plugin_get_next_by_name (VisList *list, char *name);
char *visual_plugin_get_prev_by_name (VisList *list, char *name);

int _lv_plugin_unload (LVPlugin *plugin);
LVPlugin *_lv_plugin_load (VisPluginRef *ref);
int _lv_plugin_realize (LVPlugin *plugin);
VisPluginRef *_lv_plugin_get_reference (VisPluginRef *refn, char *pluginpath);
VisList *_lv_plugin_get_list (char **paths);
VisPluginRef *_lv_plugin_find (VisList *list, char *name);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_PLUGIN_H */
