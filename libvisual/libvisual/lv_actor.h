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
#include <libvisual/lv_video.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_audio.h>

/**
 * @defgroup VisActor VisActor
 * @{
 */

#ifdef __cplusplus

#include <libvisual/lv_intrusive_ptr.hpp>
#include <memory>
#include <string_view>

namespace LV
{

  class Actor;

  typedef LV::IntrusivePtr<Actor> ActorPtr;

  //! Actor class.
  class LV_API Actor
  {
  public:

      /**
       * Determines if an actor plugin by the given name is available.
       *
       * @param name Name of actor plugin to check for
       *
       * @return True if an actor plugin by that name is available, else false
       */
      static bool available (std::string_view name);

      /**
       * Creates a new Actor with a plugin of a given name.
       *
       * @see realize()
       *
       * @param name Name of plugin to load
       *
       * @return New actor, or nullptr if plugin failed to load
       */
      static ActorPtr load (std::string_view name);

      Actor (Actor const&) = delete;

      ~Actor ();

      Actor& operator= (Actor const&) = delete;

      /**
       * Returns the plugin object.
       *
       * @return Plugin object
       */
      VisPluginData* get_plugin ();

      /**
       * Realizes this Actor.
       *
       * @return true on success, false otherwise
       */
      bool realize ();

      /**
       * Returns the song information used by this Actor for display.
       *
       * @return Song information
       */
      SongInfo const* get_songinfo ();

      /**
       * Returns the colour palette used.
       *
       * @note Only 8-bit renders will have colour palettes.
       *
       * @return Colour palette, or null if there is none
       */
      Palette const* get_palette ();

      /**
       * Configures actor to automatically work with the video target set with set_video().
       *
       * @note This method setups any necessary scaling and depth conversion necessary.
       *
       * @see set_video()
       *
       * @param run_depth Desired colour depth used for rendering. Use VISUAL_VIDEO_DEPTH_NONE for auto selection
       * @param noevent   Set to TRUE to stop events
       * @param forced    Set to TRUE If run_depth is set
       *
       * @return true on success, false otherwise
       */
      bool video_negotiate (VisVideoDepth run_depth, bool noevent, bool forced);

      /**
       * Returns the set of colour depths natively supported.
       *
       * @return Set of supported colour depths
       */
      VisVideoDepth get_supported_depths ();

      VisVideoAttrOptions const* get_video_attribute_options ();

      /**
       * Sets the video target for rendering.
       *
       * @see video_negotiate()
       *
       * @param video Video target
       */
      void set_video (VideoPtr const& video);

      VideoPtr const& get_video ();

      /**
       * Runs this actor.
       *
       * Each call pumps events to the actor for handling and passes
       * in the audio data for visualising.
       *
       * @note Scaling and depth conversions are automatically performed.
       *
       * @param audio Audio data to visualise
       */
      void run (Audio const& audio);

  private:

      friend void intrusive_ptr_add_ref (Actor const* actor);
      friend void intrusive_ptr_release (Actor const* actor);

      class Impl;
      const std::unique_ptr<Impl> m_impl;

      mutable unsigned int m_ref_count;

      explicit Actor (std::string_view name);
  };

  inline void intrusive_ptr_add_ref (Actor const* actor)
  {
      actor->m_ref_count++;
  }

  inline void intrusive_ptr_release (Actor const* actor)
  {
      if (--actor->m_ref_count == 0) {
          delete actor;
      }
  }

} // LV namespace

typedef LV::Actor VisActor;

#else

typedef struct _VisActor VisActor;
struct _VisActor;

#endif /* __cplusplus */

/**
 * Function signature and type of the Actor requisition() method.
 *
 * The requisition() method is called to query and configure the preferred rendering dimensions.
 *
 * @param         plugin Plugin object
 * @param[in,out] width  Suggested width
 * @param[in,out] height Suggested height
 */
typedef void (*VisPluginActorRequisitionFunc)(VisPluginData *plugin, int *width, int *height);

/**
 * Function signature and type of the Actor palette() method.
 *
 * The palette() method is called to obtain the colour palette used for 8-bit rendering modes.
 *
 * @param plugin Plugin object
 *
 * @return Color palette used by Actor, or NULL
 */
typedef VisPalette *(*VisPluginActorPaletteFunc)(VisPluginData *plugin);

/**
 * Function signature and type of the Actor render() method.
 *
 * The render() method is called to render the actor.
 *
 * @param plugin Plugin object
 * @param video  Video to render to
 * @param audio  Audio data to visualise
 */
typedef void (*VisPluginActorRenderFunc)(VisPluginData *plugin, VisVideo *video, VisAudio *audio);

/**
 * Actor plugin class.
 */
typedef struct _VisActorPlugin VisActorPlugin;
struct _VisActorPlugin {
	VisPluginActorRequisitionFunc requisition;    /**< Returns the preferred rendering dimensions */
	VisPluginActorPaletteFunc     palette;        /**< Returns the colour palette of the actor */
	VisPluginActorRenderFunc      render;         /**< Renders the actor */

	VisSongInfo                  *songinfo;       /**< Information on the currently playing song */
	VisVideoAttrOptions           vidoptions;     /**< Video attributes */
};

LV_BEGIN_DECLS

LV_API VisPluginData *visual_actor_get_plugin (VisActor *actor);

LV_API const char *visual_actor_get_prev_by_name_gl   (const char *name);
LV_API const char *visual_actor_get_next_by_name_gl   (const char *name);
LV_API const char *visual_actor_get_prev_by_name_nogl (const char *name);
LV_API const char *visual_actor_get_next_by_name_nogl (const char *name);
LV_API const char *visual_actor_get_prev_by_name      (const char *name);
LV_API const char *visual_actor_get_next_by_name      (const char *name);

LV_NODISCARD LV_API VisActor *visual_actor_new (const char *name);
LV_API int  visual_actor_realize (VisActor *actor);
LV_API void visual_actor_run     (VisActor *actor, VisAudio *audio);
LV_API void visual_actor_ref     (VisActor *actor);
LV_API void visual_actor_unref   (VisActor *actor);

LV_API VisSongInfo *visual_actor_get_songinfo (VisActor *actor);
LV_API VisPalette  *visual_actor_get_palette  (VisActor *actor);

LV_API VisVideoDepth        visual_actor_get_supported_depths        (VisActor *actor);
LV_API VisVideoAttrOptions *visual_actor_get_video_attribute_options (VisActor *actor);

LV_API void      visual_actor_set_video (VisActor *actor, VisVideo *video);
LV_API VisVideo *visual_actor_get_video (VisActor *actor);

LV_API int visual_actor_video_negotiate (VisActor *actor, VisVideoDepth run_depth, int noevent, int forced);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_ACTOR_H */
