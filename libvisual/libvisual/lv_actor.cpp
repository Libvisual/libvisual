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

namespace LV {

  class Actor::Impl
  {
  public:

      VisPluginData* plugin;
      VideoPtr       video;
      VideoPtr       transform;
      VideoPtr       fitting;
      Palette*       ditherpal;
      SongInfo       songcompare;

      Impl ();
      ~Impl ();

      bool negotiate_video_with_unsupported_depth (VisVideoDepth rundepth, bool noevent, bool forced);
      bool negotiate_video (bool noevent);

      VisActorPlugin* get_actor_plugin () const;

      VisVideoDepth get_supported_depths ();
  };

  Actor::Impl::Impl ()
      : plugin      (nullptr)
      , ditherpal   {nullptr}
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

      delete ditherpal;
  }

  VisActorPlugin* Actor::Impl::get_actor_plugin () const
  {
      return VISUAL_ACTOR_PLUGIN (visual_plugin_get_info (plugin)->plugin);
  }

  VisVideoDepth Actor::Impl::get_supported_depths ()
  {
      auto actplugin = get_actor_plugin ();

      return actplugin ? actplugin->vidoptions.depth : VISUAL_VIDEO_DEPTH_NONE;
  }

  ActorPtr Actor::load (std::string const& name)
  {
      try {
          return {new Actor (name), false};
      }
      catch (std::exception& error) {
          visual_log (VISUAL_LOG_ERROR, "%s", error.what ());
          return nullptr;
      }
  }

  Actor::Actor (std::string const& name)
      : m_impl      {new Impl}
      , m_ref_count {1}
  {
      if (!LV::PluginRegistry::instance()->has_plugin (VISUAL_PLUGIN_TYPE_ACTOR, name)) {
          throw std::runtime_error {"Actor plugin not found"};
      }

      m_impl->plugin = visual_plugin_load (VISUAL_PLUGIN_TYPE_ACTOR, name.c_str ());
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
      auto actplugin = m_impl->get_actor_plugin ();

      if (m_impl->transform &&
          m_impl->video->get_depth () == VISUAL_VIDEO_DEPTH_8BIT) {
          return m_impl->ditherpal;
      } else {
          return actplugin->palette (get_plugin ());
      }

      return {};
  }

  bool Actor::video_negotiate (VisVideoDepth rundepth, bool noevent, bool forced)
  {
      visual_return_val_if_fail (m_impl->video, false);

      visual_log (VISUAL_LOG_INFO, "Negotiating plugin %s", visual_plugin_get_info (m_impl->plugin)->name);

      m_impl->transform.reset ();
      m_impl->fitting.reset ();
      delete m_impl->ditherpal;

      // Set up any required intermediary pixel buffers

      auto supported_depths = get_supported_depths ();

      if (!visual_video_depth_is_supported (supported_depths, m_impl->video->get_depth ()) ||
          (forced && m_impl->video->get_depth () != rundepth)) {
          return m_impl->negotiate_video_with_unsupported_depth (rundepth, noevent, forced);
      }
      else {
          return m_impl->negotiate_video (noevent);
      }

      return false;
  }

  bool Actor::Impl::negotiate_video_with_unsupported_depth (VisVideoDepth rundepth, bool noevent, bool forced)
  {
      auto actplugin = get_actor_plugin ();

      if (forced && rundepth == VISUAL_VIDEO_DEPTH_GL) {
          return false;
      }

      auto supported_depths = get_supported_depths ();

      if (supported_depths == VISUAL_VIDEO_DEPTH_NONE) {
          visual_log (VISUAL_LOG_ERROR, "Cannot find supported colour depth for rendering actor!");
          return false;
      }

      auto req_depth = forced ? rundepth : visual_video_depth_get_highest_nogl (supported_depths);

      int req_width  = video->get_width ();
      int req_height = video->get_height ();

      actplugin->requisition (plugin, &req_width, &req_height);

      transform = Video::create (req_width, req_height, req_depth);

      if (video->get_depth () == VISUAL_VIDEO_DEPTH_8BIT) {
          ditherpal = new Palette {256};
      }

      if (!noevent) {
          visual_event_queue_add (visual_plugin_get_event_queue (plugin),
                                  visual_event_new_resize (req_width, req_height));
      }

      return true;
  }

  bool Actor::Impl::negotiate_video (bool noevent)
  {
      auto actplugin = get_actor_plugin ();

      int req_width  = video->get_width ();
      int req_height = video->get_height ();

      actplugin->requisition (plugin, &req_width, &req_height);

      // Size fitting enviroment
      if (req_width != video->get_width () || req_height != video->get_height ()) {
          if (video->get_depth () != VISUAL_VIDEO_DEPTH_GL) {
              fitting = Video::create (req_width, req_height, video->get_depth ());
          }

          video->set_dimension (req_width, req_height);
      }

      // FIXME: This should be moved into the if block above. It's out
      // here because plugins depend on this to receive information
      // about initial dimensions
      if (!noevent) {
          visual_event_queue_add (visual_plugin_get_event_queue (plugin),
                                  visual_event_new_resize (req_width, req_height));
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

      auto actplugin = m_impl->get_actor_plugin ();
      if (!actplugin) {
          visual_log (VISUAL_LOG_ERROR, "The given actor does not reference any actor plugin");
          return;
      }

      auto plugin = get_plugin ();

      /* Songinfo handling */
      if (!visual_songinfo_compare (&m_impl->songcompare, actplugin->songinfo) ||
          m_impl->songcompare.get_elapsed () != actplugin->songinfo->get_elapsed ()) {

          actplugin->songinfo->mark ();

          visual_event_queue_add (visual_plugin_get_event_queue (plugin),
                                  visual_event_new_newsong (actplugin->songinfo));

          visual_songinfo_copy (&m_impl->songcompare, actplugin->songinfo);
      }

      // Get plugin to process all events
      visual_plugin_events_pump (m_impl->plugin);

      auto video = m_impl->video;

      // Set the palette to the target video
      auto palette = get_palette ();
      if (palette) {
          video->set_palette (*palette);
      }

      auto transform = m_impl->transform;
      auto fitting   = m_impl->fitting;

      if (transform && (transform->get_depth () != video->get_depth ())) {
          actplugin->render (plugin, transform.get (), const_cast<Audio*> (&audio));

          if (transform->get_depth () == VISUAL_VIDEO_DEPTH_8BIT) {
              transform->set_palette (*get_palette ());
          } else {
              transform->set_palette (*m_impl->ditherpal);
          }

          video->convert_depth (transform);
      } else {
          if (fitting && (fitting->get_width () != video->get_width () || fitting->get_height () != video->get_height ())) {
              actplugin->render (plugin, fitting.get (), const_cast<Audio*> (&audio));
              video->blit (fitting, 0, 0, false);
          } else {
              actplugin->render (plugin, video.get (), const_cast<Audio*> (&audio));
          }
      }
  }

} // LV namespace
