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
#define VISUAL_ACTOR_PLUGIN(obj)		(VISUAL_CHECK_CAST ((obj), VisActorPlugin))

typedef struct _VisActor VisActor;
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
 * Function type and signature for the actor render() method.
 *
 * @param plugin Plugin
 * @param video  Video to render to
 * @param audio  Audio data to visualise
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginActorRenderFunc)(VisPluginData *plugin, VisVideo *video, VisAudio *audio);

/**
 * Actor class.
 */
struct _VisActor {
	VisObject      object;      /**< Parent */
	VisPluginData *plugin;      /**< Plugin object */
	VisVideo      *video;       /**< Video target */

	VisVideo      *transform;
	VisVideo      *fitting;
	VisPalette    *ditherpal;
	VisSongInfo   *songcompare;
};

/**
 * Actor plugin class.
 */
struct _VisActorPlugin {
	VisObject                     object;         /**< The VisObject data. */
	VisPluginActorRequisitionFunc requisition;    /**< Returns the preferred rendering dimensions */
	VisPluginActorPaletteFunc     palette;        /**< Returns the colour palette of the actor */
	VisPluginActorRenderFunc      render;         /**< Renders the actor */

	VisSongInfo                  *songinfo;       /**< Information on the currently playing song */
	VisVideoAttrOptions           vidoptions;     /**< Video attributes */
};

LV_BEGIN_DECLS

/**
 * Returns the plugin object that backs an actor.
 *
 * @param actor Actor object
 *
 * @return Plugin object, or NULL if none
 */
LV_API VisPluginData *visual_actor_get_plugin (VisActor *actor);

/**
 * Returns the name of the previous available GL actor plugin.
 *
 * @see visual_actor_get_next_by_name_gl()
 *
 * @param name Name of current actor plugin, or NULL to retrieve the first
 *
 * @return Name of previous available GL actor plugin
 */
LV_API const char *visual_actor_get_prev_by_name_gl (const char *name);

/**
 * Returns the name of the next available GL actor plugin.
 *
 * @see visual_actor_get_prev_by_name_gl()
 *
 * @param name Name of current actor plugin, or NULL to retrieve the last.
 *
 * @return Name of next available GL actor plugin
 */
LV_API const char *visual_actor_get_next_by_name_gl (const char *name);

/**
 * Returns the name of the previous available non-GL actor plugin.
 *
 * @see visual_actor_get_next_by_name_nogl()
 *
 * @param name Name of current actor plugin, or NULL to retrieve the first.
 *
 * @return Name of previous available non-GL actor plugin
 */
LV_API const char *visual_actor_get_prev_by_name_nogl (const char *name);

/**
 * Returns the name of the next available non-GL actor plugin.
 *
 * @see visual_actor_get_prev_by_name_nogl()
 *
 * @param name Name of current actor plugin, or NULL to retrieve the last.
 *
 * @return Name of next available non-GL actor plugin
 */
LV_API const char *visual_actor_get_next_by_name_nogl (const char *name);

/**
 * Returns the name of the next available actor plugin.
 *
 * @see visual_actor_get_next_by_name()
 *
 * @param name Name of current actor plugin, or NULL to retrieve the last.
 *
 * @return Name of next available actor plugin
 */
LV_API const char *visual_actor_get_prev_by_name (const char *name);

/**
 * Returns the name of the next available actor plugin.
 *
 * @see visual_actor_get_prev_by_name()
 *
 * @param name Name of current actor plugin, or NULL to retrieve the last.
 *
 * @return Name of next available actor plugin
 */
LV_API const char *visual_actor_get_next_by_name (const char *name);


/**
 * Creates a new Actor with a plugin of a given name.
 *
 * @see visual_actor_realize()
 *
 * @param name Name of plugin to load, or NULL to allocate an empty object
 *
 * @return A new Actor, or NULL on failure
 */
LV_API VisActor *visual_actor_new (const char *name);

/**
 * Realize an Actor.
 *
 * @param actor Actor object
 *
 * @return VISUAL_OK on success
 */
LV_API int visual_actor_realize (VisActor *actor);

/**
 * Returns the song information used by the Actor for display.
 *
 * @param actor Actor object
 *
 * @return Song information
 */
LV_API VisSongInfo *visual_actor_get_songinfo (VisActor *actor);

/**
 * Returns the color palette of an Actor.
 *
 * @note Called only when rendering in 8-bit colour mode.
 *
 * @param actor Actor object
 *
 * @return Palette
 */
LV_API VisPalette *visual_actor_get_palette (VisActor *actor);

/**
 * Configures an actor to automatically work with the video target set with visual_actor_set_video().
 *
 * @note This function setups any necessary scaling and depth conversion necessary.
 *
 * @see visual_actor_set_video
 *
 * @param actor     Actor object
 * @param run_depth Desired colour depth used for rendering. Use VISUAL_VIDEO_DEPTH_NONE for auto selection
 * @param noevent   Set to TRUE to stop events
 * @param forced    Set to TRUE If run_depth is set
 *
 * @return VISUAL_OK on success
 */
LV_API int visual_actor_video_negotiate (VisActor *actor, VisVideoDepth run_depth, int noevent, int forced);

/**
 * Returns colour depths natively supported by the actor.
 *
 * @param actor Actor object
 *
 * @return Set of supported colour depths
 */
LV_API VisVideoDepth visual_actor_get_supported_depth (VisActor *actor);

LV_API VisVideoAttrOptions *visual_actor_get_video_attribute_options (VisActor *actor);

/**
 * Sets the video target for an actor.
 *
 * @see visual_actor_video_negotiate
 *
 * @param actor Actor
 * @param video Video target
 */
LV_API void visual_actor_set_video (VisActor *actor, VisVideo *video);

/**
 * Runs an actor.
 *
 * At each call, visual_actor_run() pumps events to the actor for handling and passes in the audio data for visualising.
 *
 * @note Scaling and depth conversions are automatically performed.
 *
 * @param actor Actor object
 * @param audio Audio data to visualise
 */
LV_API void visual_actor_run (VisActor *actor, VisAudio *audio);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_ACTOR_H */
