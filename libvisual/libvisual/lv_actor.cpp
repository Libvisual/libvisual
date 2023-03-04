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

#include "config.h"
#include "lv_actor.h"
#include "lv_common.h"
#include "lv_plugin_registry.h"
#include <stdexcept>
#include <string>

namespace LV {

  class Actor::Impl
  {
  public:

      VisPluginData* plugin;
      VideoPtr       video;
      VideoPtr       to_scale;
      VideoPtr       to_convert;
      SongInfo       songcompare;
      VisVideoDepth  run_depth;

      Impl ();
      ~Impl ();

      VisActorPlugin* get_actor_plugin () const;

      VisVideoDepth get_supported_depths ();
  };

  Actor::Impl::Impl ()
      : plugin      (nullptr)
      , songcompare {SONG_INFO_TYPE_NULL}
  {
      // nothing
  }

  Actor::Impl::~Impl ()
  {
      if (plugin) {
          delete get_actor_plugin ()->songinfo;
          visual_plugin_unload (plugin);
      }
  }

  VisActorPlugin* Actor::Impl::get_actor_plugin () const
  {
      return static_cast<VisActorPlugin*> (visual_plugin_get_info (plugin)->plugin);
  }

  VisVideoDepth Actor::Impl::get_supported_depths ()
  {
      auto actor_plugin = get_actor_plugin ();

      return actor_plugin ? actor_plugin->vidoptions.depth : VISUAL_VIDEO_DEPTH_NONE;
  }

  bool Actor::available(std::string_view name) {
      return LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_ACTOR, name);
  }

  ActorPtr Actor::load (std::string_view name)
  {
      try {
          return {new Actor {name}, false};
      }
      catch (std::exception& error) {
          visual_log (VISUAL_LOG_ERROR, "%s", error.what ());
          return nullptr;
      }
  }

  Actor::Actor (std::string_view name)
      : m_impl      {new Impl}
      , m_ref_count {1}
  {
      if (!available (name)) {
          throw std::runtime_error {"Actor plugin not found"};
      }

      m_impl->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, std::string {name}.c_str ());
      if (!m_impl->plugin) {
          throw std::runtime_error {"Failed to load actor plugin"};
      }

      // FIXME: Hack to initialize songinfo
      m_impl->get_actor_plugin ()->songinfo = new SongInfo {SONG_INFO_TYPE_NULL};
  }

  Actor::~Actor ()
  {
      // nothing
  }

  VisPluginData* Actor::get_plugin ()
  {
      return m_impl->plugin;
  }

  bool Actor::realize ()
  {
      return visual_plugin_realize (m_impl->plugin);
  }

  VideoPtr const& Actor::get_video ()
  {
      return m_impl->video;
  }

  SongInfo const* Actor::get_songinfo ()
  {
      return m_impl->get_actor_plugin ()->songinfo;
  }

  Palette const* Actor::get_palette ()
  {
      auto actor_plugin = m_impl->get_actor_plugin ();

      if (m_impl->run_depth == VISUAL_VIDEO_DEPTH_8BIT) {
          return actor_plugin->palette (m_impl->plugin);
      } else {
          return nullptr;
      }
  }

  bool Actor::video_negotiate (VisVideoDepth run_depth, bool noevent, bool forced)
  {
      auto output_width  = m_impl->video->get_width ();
      auto output_height = m_impl->video->get_height ();
      auto output_depth  = m_impl->video->get_depth ();

      // Ask actor for preferred rendering dimensions

      int run_width  = output_width;
      int run_height = output_height;

      m_impl->get_actor_plugin ()->requisition (m_impl->plugin, &run_width, &run_height);

      // Check to make sure requested run depth is supported. If not, pick the highest.

      auto supported_depths = get_supported_depths ();
      m_impl->run_depth = forced ? run_depth : visual_video_depth_get_highest_nogl (supported_depths);

      if (!visual_video_depth_is_supported (supported_depths, m_impl->run_depth)) {
          m_impl->run_depth = visual_video_depth_get_highest_nogl (supported_depths);
      }

      // Configure proxy videos to convert rendering

      m_impl->to_scale.reset ();
      m_impl->to_convert.reset ();

      visual_log (VISUAL_LOG_DEBUG, "Setting up any necessary video conversions..");

      if (output_depth != VISUAL_VIDEO_DEPTH_GL) {
          // Configure any necessary depth conversion
          if (m_impl->run_depth != output_depth) {
              visual_log (VISUAL_LOG_DEBUG, "Setting up depth conversion: %s -> %s",
                          visual_video_depth_name (m_impl->run_depth),
                          visual_video_depth_name (output_depth));

              m_impl->to_convert = Video::create (run_width, run_height, m_impl->run_depth);
          }

          // Configure any necessary scaling
          if (run_width != output_width || run_height != output_height) {
              visual_log (VISUAL_LOG_DEBUG, "Setting up scaling: (%dx%d) -> (%dx%d)",
                          run_width, run_height, output_width, output_height);

              m_impl->to_scale = Video::create (run_width, run_height, output_depth);
          }
      } else {
          visual_log (VISUAL_LOG_DEBUG, "Conversions skipped in OpenGL rendering mode");
      }

      // FIXME: This should be moved into the if block above. It's out
      // here because plugins depend on this to receive information
      // about initial dimensions
      if (!noevent) {
          visual_event_queue_add (visual_plugin_get_event_queue (m_impl->plugin),
                                  visual_event_new_resize (run_width, run_height));
      }

      return true;
  }

  VisVideoDepth Actor::get_supported_depths ()
  {
      return m_impl->get_supported_depths ();
  }

  VisVideoAttrOptions const* Actor::get_video_attribute_options ()
  {
      return &m_impl->get_actor_plugin ()->vidoptions;
  }

  void Actor::set_video (VideoPtr const& video)
  {
      m_impl->video = video;
  }

  void Actor::run (Audio const& audio)
  {
      visual_return_if_fail (m_impl->video);

      auto actor_plugin = m_impl->get_actor_plugin ();
      if (!actor_plugin) {
          visual_log (VISUAL_LOG_ERROR, "The given actor does not reference any actor plugin");
          return;
      }

      auto plugin = get_plugin ();

      /* Songinfo handling */
      if (!visual_songinfo_compare (&m_impl->songcompare, actor_plugin->songinfo) ||
          m_impl->songcompare.get_elapsed () != actor_plugin->songinfo->get_elapsed ()) {

          actor_plugin->songinfo->mark ();

          visual_event_queue_add (visual_plugin_get_event_queue (plugin),
                                  visual_event_new_newsong (actor_plugin->songinfo));

          visual_songinfo_copy (&m_impl->songcompare, actor_plugin->songinfo);
      }

      // Get plugin to process all events
      visual_plugin_events_pump (m_impl->plugin);

      auto const& video      = m_impl->video;
      auto const& to_convert = m_impl->to_convert;
      auto const& to_scale   = m_impl->to_scale;

      if (video->get_depth () != VISUAL_VIDEO_DEPTH_GL) {
          auto palette = get_palette ();

          if (to_convert) {
              // Have depth conversion

              // Setup any palette
              if (palette) {
                  to_convert->set_palette (*palette);
              }

              // Render first
              actor_plugin->render (m_impl->plugin, to_convert.get (), const_cast<Audio*> (&audio));

              if (to_scale) {
                  // Convert depth, then scale
                  to_scale->convert_depth (to_convert);
                  video->scale (to_scale, VISUAL_VIDEO_SCALE_NEAREST);
              }
              else {
                  // Convert depth only
                  video->convert_depth (to_convert);
              }
          } else {
              // No depth conversion

              if (to_scale) {
                  // Setup any palette
                  if (palette) {
                      to_scale->set_palette (*palette);
                  }

                  // Render, then scale
                  actor_plugin->render (m_impl->plugin, to_scale.get (), const_cast<Audio*> (&audio));
                  video->scale (to_scale, VISUAL_VIDEO_SCALE_NEAREST);
              } else {
                  // Setup any palette
                  if (palette) {
                      video->set_palette (*palette);
                  }

                  // Render directly to video target
                  actor_plugin->render (m_impl->plugin, video.get (), const_cast<Audio*> (&audio));
              }
          }
      } else {
          // Render directly to video target (OpenGL)
          actor_plugin->render (m_impl->plugin, video.get (), const_cast<Audio*> (&audio));
      }
  }

} // LV namespace
