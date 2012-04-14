/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_actor.h,v 1.19 2006/01/27 20:18:26 synap Exp $
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

#ifndef _LV_ACTOR_H
#define _LV_ACTOR_H

#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>
#include <libvisual/lv_audio.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_songinfo.h>
#include <libvisual/lv_event.h>

/**
 * @defgroup VisActor VisActor
 * @{
 */

#define VISUAL_ACTOR(obj)				(VISUAL_CHECK_CAST ((obj), VisActor))
#define VISUAL_ACTOR_PLUGINENVIRON(obj) (VISUAL_CHECK_CAST ((obj), VisActorPluginEnviron))
#define VISUAL_ACTOR_PLUGIN(obj)		(VISUAL_CHECK_CAST ((obj), VisActorPlugin))

/**
 * Name defination of the standard VisActorPluginEnviron element for an actor plugin.
 */
#define VISUAL_ACTOR_PLUGIN_ENVIRON	"Libvisual:core:actor:environ"

typedef struct _VisActor VisActor;
typedef struct _VisActorPluginEnviron VisActorPluginEnviron;
typedef struct _VisActorPlugin VisActorPlugin;

/* Actor plugin methods */

/**
 * An actor plugin needs this signature for the requisition function. The requisition function
 * is used to determine the size required by the plugin for a given width/height value.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 * @arg width Pointer to an int containing the width requested, will be altered to the nearest
 * 	supported width.
 * @arg height Pointer to an int containing the height requested, will be altered to the nearest
 * 	supported height.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginActorRequisitionFunc)(VisPluginData *plugin, int *width, int *height);

/**
 * An actor plugin needs this signature for the palette function. The palette function
 * is used to retrieve the desired palette from the plugin.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 *
 * @return Pointer to the VisPalette used by the plugin, this should be a VisPalette with 256
 *	VisColor entries, NULL is also allowed to be returned.
 */
typedef VisPalette *(*VisPluginActorPaletteFunc)(VisPluginData *plugin);

/**
 * An actor plugin needs this signature for the render function. The render function
 * is used to render the frame for the visualisation.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 * @arg video Pointer to the VisVideo containing all information about the display surface.
 *	Params like height and width won't suddenly change, this is always notified as an event
 *	so the plugin can adjust to the new dimension.
 * @arg audio Pointer to the VisAudio containing all the data regarding the current audio sample.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginActorRenderFunc)(VisPluginData *plugin, VisVideo *video, VisAudio *audio);

/**
 * The VisActor structure encapsulates the actor plugin and provides
 * abstract interfaces to the actor. The VisActor system
 * it's methods are also capable of doing automatic size fitting
 * and depth transformations, and it keeps track on songinfo and events.
 *
 * Members in the structure shouldn't be accessed directly but instead
 * it's adviced to use the methods provided.
 *
 * @see visual_actor_new
 */
struct _VisActor {
	VisObject	 object;		/**< The VisObject data. */

	VisPluginData	*plugin;		/**< Pointer to the plugin itself. */

	/* Video management and fake environments when needed */
	VisVideo	*video;			/**< Pointer to the target display video.
						 * @see visual_actor_set_video */
	VisVideo	*transform;		/**< Private member which is used for depth transformation. */
	VisVideo	*fitting;		/**< Private member which is used to fit the plugin. */
	VisPalette	*ditherpal;		/**< Private member in which a palette is set when transforming
						 * depth from true color to indexed.
						 * @see visual_actor_get_palette */

	/* Songinfo management */
	VisSongInfo	*songcompare;		/**< Private member which is used to compare with new songinfo
						  * to check if a new song event should be emitted. */
};

/**
 * The VisActorPluginEnviron structure is the main environmental element for a VisActorPlugin. The environmental name
 * is stored in the VISUAL_ACTOR_PLUGIN_ENVIRON define. The structure is used to set environmental data like,
 * desired frames per second. The VisActorPluginEnviron element should be polled by either libvisual-display
 * or a custom target to check for changes.
 */
struct _VisActorPluginEnviron {
	VisObject			object;		/**< The VisObject data. */

	int				fps;		/**< The desired fps, set by the plugin, optionally read by
							 * the display target. */
};

/**
 * The VisActorPlugin structure is the main data structure
 * for the actor (visualisation) plugin.
 *
 * The actor plugin is the visualisation plugin.
 */
struct _VisActorPlugin {
	VisObject			 object;	/**< The VisObject data. */
	VisPluginActorRequisitionFunc	 requisition;	/**< The requisition function. This is used to
							 * get the desired VisVideo surface size of the plugin. */
	VisPluginActorPaletteFunc	 palette;	/**< Used to retrieve the desired palette from the plugin. */
	VisPluginActorRenderFunc	 render;	/**< The main render loop. This is called to draw a frame. */

	VisSongInfo			 *songinfo;	/**< Pointer to VisSongInfo that contains information about
							 *the current playing song. This can be NULL. */

	VisVideoAttributeOptions	 vidoptions;
};

LV_BEGIN_DECLS

/**
 * Gives the encapsulated VisPluginData from a VisActor.
 *
 * @param actor Pointer of a VisActor of which the VisPluginData needs to be returned.
 *
 * @return VisPluginData that is encapsulated in the VisActor, possibly NULL.
 */
LV_API VisPluginData *visual_actor_get_plugin (VisActor *actor);

/**
 * Gives the next actor plugin based on the name of a plugin but skips non
 * GL plugins.
 *
 * @see visual_actor_get_prev_by_name_gl
 *
 * @param name The name of the current plugin or NULL to get the first.
 *
 * @return The name of the next plugin within the list that is a GL plugin.
 */
LV_API const char *visual_actor_get_next_by_name_gl (const char *name);

/**
 * Gives the previous actor plugin based on the name of a plugin but skips non
 * GL plugins.
 *
 * @see visual_actor_get_next_by_name_gl
 *
 * @param name The name of the current plugin or NULL to get the last.
 *
 * @return The name of the previous plugin within the list that is a GL plugin.
 */
LV_API const char *visual_actor_get_prev_by_name_gl (const char *name);

/**
 * Gives the next actor plugin based on the name of a plugin but skips
 * GL plugins.
 *
 * @see visual_actor_get_prev_by_name_nogl
 *
 * @param name The name of the current plugin or NULL to get the first.
 *
 * @return The name of the next plugin within the list that is not a GL plugin.
 */
LV_API const char *visual_actor_get_next_by_name_nogl (const char *name);

/**
 * Gives the previous actor plugin based on the name of a plugin but skips
 * GL plugins.
 *
 * @see visual_actor_get_next_by_name_nogl
 *
 * @param name The name of the current plugin or NULL to get the last.
 *
 * @return The name of the previous plugin within the list that is not a GL plugin.
 */
LV_API const char *visual_actor_get_prev_by_name_nogl (const char *name);

/**
 * Gives the next actor plugin based on the name of a plugin.
 *
 * @see visual_actor_get_prev_by_name
 *
 * @param name The name of the current plugin, or NULL to get the first.
 *
 * @return The name of the next plugin within the list.
 */
LV_API const char *visual_actor_get_next_by_name (const char *name);

/**
 * Gives the previous actor plugin based on the name of a plugin.
 *
 * @see visual_actor_get_next_by_name
 *
 * @param name The name of the current plugin. or NULL to get the last.
 *
 * @return The name of the previous plugin within the list.
 */
LV_API const char *visual_actor_get_prev_by_name (const char *name);

/**
 * Creates a new actor from name, the plugin will be loaded but won't be realized.
 *
 * @param actorname
 * 	The name of the plugin to load, or NULL to simply allocate a new
 * 	actor.
 *
 * @return A newly allocated VisActor, optionally containing a loaded plugin. Or NULL on failure.
 */
LV_API VisActor *visual_actor_new (const char *actorname);

/**
 * Initializes a VisActor, this will set the allocated flag for the object to FALSE. Should not
 * be used to reset a VisActor, or on a VisActor created by visual_actor_new().
 *
 * @see visual_actor_new
 *
 * @param actor Pointer to the VisActor that is initialized.
 * @param actorname The name of the plugin to load, or NULL to simply initialize a new actor.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_ACTOR_NULL or -VISUAL_ERROR_PLUGIN_NO_LIST on failure.
 */
LV_API int visual_actor_init (VisActor *actor, const char *actorname);

/**
 * Realize the VisActor. This also calls the plugin init function.
 *
 * @param actor Pointer to a VisActor that needs to be realized.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_ACTOR_NULL, -VISUAL_ERROR_PLUGIN_NULL or
 *	error values returned by visual_plugin_realize () on failure.
 *
 */
LV_API int visual_actor_realize (VisActor *actor);

/**
 * Gives a pointer to the song info data within the VisActor. This song info data can be used
 * to set name, artist and even coverart which can be used by the plugins and the framework itself.
 *
 * @see VisSongInfo
 *
 * @param actor Pointer to a VisActor of which the song info is needed.
 *
 * @return Pointer to the song info structure on succes or NULL on failure.
 */
LV_API VisSongInfo *visual_actor_get_songinfo (VisActor *actor);

/**
 * Gives a pointer to the palette within the VisActor. This can be needed to set a palette on the target
 * display when it's in index mode.
 *
 * @see VisPalette
 *
 * @param actor Pointer to a VisActor of which the palette is needed.
 *
 * @return Pointer to the palette structure on succes or NULL on failure. Also it's possible that NULL
 * is returned when the plugin is running in a full color mode or openGL. The returned palette is
 * read only.
 */
LV_API VisPalette *visual_actor_get_palette (VisActor *actor);

/**
 * This function negotiates the VisActor with it's target video that is set by visual_actor_set_video.
 * When needed it also sets up size fitting environment and depth transformation environment.
 *
 * The function has a few extra arguments that are mainly to be used from within internal code.
 *
 * This function needs to be called everytime there is a change within either the size or depth of
 * the target video.
 *
 * The main method of calling this function is: "visual_actor_video_negotiate (actor, 0, FALSE, FALSE)"
 *
 * @see visual_actor_set_video
 *
 * @param actor Pointer to a VisActor that needs negotiation.
 * @param rundepth An depth in the form of the VISUAL_VIDEO_DEPTH_* style when a depth is forced.
 * 	  This could be needed when for example a plugin has both a 8 bits and a 32 bits display method
 * 	  but while the target video is in 32 bits you still want to run the plugin in 8 bits. If this
 * 	  is desired the "forced" argument also needs to be set on TRUE.
 * @param noevent When set on TRUE this does only renegotiate depth transformation environments. For example
 * 	  when the target display was running in 32 bits and switched to 8 bits while the plugin was already
 * 	  in 8 bits it doesn't need an events, which possibly reinitializes the plugin.
 * @param forced This should be set if the rundepth argument is set, so it forces the plugin in a certain
 * 	  depth.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_ACTOR_NULL, -VISUAL_ERROR_PLUGIN_NULL, -VISUAL_ERROR_PLUGIN_REF_NULL,
 * 	-VISUAL_ERROR_ACTOR_VIDEO_NULL or -VISUAL_ERROR_ACTOR_GL_NEGOTIATE on failure.
 */
LV_API int visual_actor_video_negotiate (VisActor *actor, VisVideoDepth rundepth, int noevent, int forced);

/**
 * Gives the by the plugin natively supported depths
 *
 * @param actor Pointer to a VisActor of which the supported depth of it's
 * 	  encapsulated plugin is requested.
 *
 * @return an OR value of the VISUAL_VIDEO_DEPTH_* values which can be checked against using AND on success,
 * 	-VISUAL_ERROR_ACTOR_NULL, -VISUAL_ERROR_PLUGIN_NULL or -VISUAL_ERROR_ACTOR_PLUGIN_NULL on failure.
 */
LV_API int visual_actor_get_supported_depth (VisActor *actor);

LV_API VisVideoAttributeOptions *visual_actor_get_video_attribute_options (VisActor *actor);

/**
 * Used to connect the target display it's VisVideo structure to the VisActor.
 *
 * Using the visual_video methods the screenbuffer, it's depth and dimension and optionally it's pitch
 * can be set so the actor plugins know about their graphical environment and have a place to draw.
 *
 * After this function it's most likely that visual_actor_video_negotiate needs to be called.
 *
 * @see visual_video_new
 * @see visual_actor_video_negotiate
 *
 * @param actor Pointer to a VisActor to which the VisVideo needs to be set.
 * @param video Pointer to a VisVideo which contains information about the target display and the pointer
 * 	  to it's screenbuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_ACTOR_NULL on failure.
 */
LV_API int visual_actor_set_video (VisActor *actor, VisVideo *video);

/**
 * This is called to run a VisActor. It also pump it's events when needed, checks for new song events and also does the fitting
 * and depth transformation actions when needed.
 *
 * Every run cycle one frame is created, so this function needs to be used in the main draw loop of the application.
 *
 * @param actor Pointer to a VisActor that needs to be runned.
 * @param audio Pointer to a VisAudio that contains all the audio data.
 *
 * return VISUAL_OK on success, -VISUAL_ERROR_ACTOR_NULL, -VISUAL_ERROR_ACTOR_VIDEO_NULL, -VISUAL_ERROR_NULL or
 * 	-VISUAL_ERROR_ACTOR_PLUGIN_NULL on failure.
 */
LV_API int visual_actor_run (VisActor *actor, VisAudio *audio);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_ACTOR_H */
