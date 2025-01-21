/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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
#include "lv_bin.h"
#include "lv_common.h"
#include <string>

namespace LV {

  Time const min_switch_time = Time::from_msecs (100);

  class Bin::Impl
  {
  public:

      bool is_realized;

      VisVideoDepth  supported_depths;
      VisVideoDepth  output_depth;
      VisBinDepth    depth_policy;
      bool           need_sync;

      VideoPtr output_video;

      ActorPtr actor;
      ActorPtr morph_target;

      InputPtr input;

      // Morph-specific pipeline state
      MorphPtr morph;
      Time     morph_time;
      bool     use_morph;
      bool     is_morphing;

      Impl ();

      ~Impl ();

      void configure_pipeline ();

      void setup_actor_output (ActorPtr const& actor, VideoPtr const& video);

      void cut_to_morph_target ();
  };

  VisVideoDepth find_depth_policy_best (VisBinDepth policy, VisVideoDepth depth_set)
  {
      auto depth {VISUAL_VIDEO_DEPTH_NONE};

      switch (policy) {
          case VISUAL_BIN_DEPTH_LOWEST:
              depth = visual_video_depth_get_lowest (depth_set);
              break;

          case VISUAL_BIN_DEPTH_HIGHEST:
              depth = visual_video_depth_get_highest (depth_set);
              break;
      }

      return depth;
  }

  VideoPtr create_matching_format_video (LV::VideoPtr const& source)
  {
      auto const width  {source->get_width ()};
      auto const height {source->get_height ()};
      auto const depth  {source->get_depth ()};

      return Video::create (width, height, depth);
  }

  Bin::Impl::Impl ()
      : is_realized      {false}
      , supported_depths {VISUAL_VIDEO_DEPTH_ALL}
      , output_depth     {VISUAL_VIDEO_DEPTH_NONE}
      , depth_policy     {VISUAL_BIN_DEPTH_HIGHEST}
      , need_sync        {false}
      , morph_time       {Time::from_msecs (500)}
      , use_morph        {true}
      , is_morphing      {false}
  {
      // empty
  }

  Bin::Impl::~Impl ()
  {
      // nothing
  }

  void Bin::Impl::setup_actor_output (ActorPtr const& actor, VideoPtr const& video)
  {
      auto depth_set {actor->get_supported_depths ()};
      auto run_depth {find_depth_policy_best (depth_policy, depth_set)};
      actor->set_video (video);
      actor->video_negotiate (run_depth, false, true);
      video->fill_color (Color {0, 0, 0});
  }

  void Bin::Impl::cut_to_morph_target ()
  {
      if (!morph_target)
          return;

      is_morphing = false;

      auto next_actor {morph_target};
      auto actor_output_video {morph->get_video ()};
      setup_actor_output (next_actor, actor_output_video);

      actor = next_actor;

      morph_target.reset ();
      morph->set_video (nullptr);
  }

  Bin::Bin ()
      : m_impl (new Impl)
  {
      // empty
  }

  Bin::~Bin ()
  {
      // empty
  }

  ActorPtr const& Bin::get_actor () const
  {
      return m_impl->actor;
  }

  InputPtr const& Bin::get_input () const
  {
      return m_impl->input;
  }

  MorphPtr const& Bin::get_morph () const
  {
      return m_impl->morph;
  }

  void Bin::set_morph (std::string_view morph_name)
  {
      auto new_morph {Morph::load (morph_name)};
      visual_return_if_fail (new_morph);

      // Morph class doesn't have code to adapt its output format, so we
      // have to ensure the pipeout output depth is supported by the morph.

      auto depth_set {new_morph->get_supported_depths ()};

      if (!(depth_set & m_impl->supported_depths)) {
          visual_log (VISUAL_LOG_WARNING, "Morph does not support colour depth (morph:%d, bin:%d)",
                      depth_set, m_impl->supported_depths);

          if (m_impl->is_morphing) {
              m_impl->cut_to_morph_target ();

              m_impl->morph.reset ();
              m_impl->use_morph = false;
          }

          return;
      }

      new_morph->set_time (m_impl->morph_time);

      if (m_impl->morph) {
          // Swap in new morph
          auto output_video {m_impl->morph->get_video ()};
          m_impl->morph = new_morph;
          m_impl->morph->set_video (output_video);
      } else {
          // Set morph for the first time
          m_impl->morph = new_morph;
      }

      if (m_impl->is_realized) {
          m_impl->morph->realize ();
      }
  }

  bool Bin::connect (std::string_view actor_name, std::string_view input_name)
  {
      if (m_impl->actor || m_impl->input) {
          visual_log (VISUAL_LOG_WARNING, "Bin is already connected.");
          return false;
      }

      auto actor = Actor::load (actor_name);
      visual_return_val_if_fail (actor, false);

      auto input = Input::load (input_name);
      visual_return_val_if_fail (input, false);

      m_impl->actor = actor;
      m_impl->input = input;

      return true;
  }

  void Bin::set_preferred_depth (VisBinDepth policy)
  {
      m_impl->depth_policy = policy;
      m_impl->need_sync = true;
  }

  void Bin::set_supported_depth (VisVideoDepth depth_set)
  {
      m_impl->supported_depths = depth_set;
      m_impl->need_sync = true;
  }

  VisVideoDepth Bin::get_supported_depth () const
  {
      return m_impl->supported_depths;
  }

  VisVideoDepth Bin::get_depth () const
  {
      return m_impl->output_depth;
  }

  Palette const& Bin::get_palette () const
  {
      if (m_impl->is_morphing)
          return *m_impl->morph->get_palette ();
      else
          return *m_impl->actor->get_palette ();
  }

  void Bin::realize ()
  {
      if (m_impl->is_realized) {
          visual_log (VISUAL_LOG_WARNING, "Bin is already realized.");
          return;
      }

      visual_return_if_fail (m_impl->actor);
      visual_return_if_fail (m_impl->input);

      m_impl->input->realize ();
      m_impl->actor->realize ();

      if (m_impl->morph) {
          m_impl->morph->realize ();
      }

      m_impl->is_realized = true;
  }

  void Bin::use_morph (bool use)
  {
      if (m_impl->use_morph == use)
          return;

      // If disabling morph while it's running, immediately cut to
      // the morph target (and reconfigure pipeline).
      if (!use && m_impl->is_morphing) {
          m_impl->cut_to_morph_target ();
      }

      m_impl->use_morph = use;
  }

  void Bin::set_video (VideoPtr const& video)
  {
      if (!visual_video_depth_is_supported (m_impl->supported_depths, video->get_depth ())) {
          visual_log (VISUAL_LOG_WARNING, "Output video depth is not supported by bin, ignoring.");
          return;
      }

      visual_log (VISUAL_LOG_INFO, "New output video target set, pending sync.");
      m_impl->output_video = video;
      m_impl->output_depth = video->get_depth ();
      m_impl->need_sync = true;
  }

  void Bin::sync ()
  {
      if (!m_impl->need_sync) {
          visual_log (VISUAL_LOG_INFO, "No sync needed.");
          return;
      }

      if (!m_impl->output_video) {
          visual_log (VISUAL_LOG_INFO, "No output video set, nothing to sync.");
          return;
      }

      if (!m_impl->is_realized) {
          realize ();
      }

      visual_log (VISUAL_LOG_INFO, "Starting sync.");

      if (m_impl->is_morphing) {
          auto new_video1 {create_matching_format_video (m_impl->output_video)};
          auto new_video2 {create_matching_format_video (m_impl->output_video)};

          m_impl->setup_actor_output (m_impl->actor, new_video1);
          m_impl->setup_actor_output (m_impl->morph_target, new_video2);
          m_impl->morph->set_video (m_impl->output_video);
      } else {
          m_impl->setup_actor_output (m_impl->actor, m_impl->output_video);
      }

      m_impl->need_sync = false;
  }

  void Bin::switch_actor (std::string_view actor_name)
  {
      // FIXME: This is needed because visual_log() takes only null-terminated C strings.
      std::string actor_name_str {actor_name};

      visual_log (VISUAL_LOG_INFO, "Switching to a new actor: %s", actor_name_str.c_str ());

      // Create a new managed actor
      auto new_actor {Actor::load (actor_name)};
      visual_return_if_fail (new_actor);

      if (m_impl->is_morphing) {
          // Morph in progress => complete running morph immediately and update the morph target
          // the newly loaded actor

          // Save reference to video to be reused for new actor.
          auto temp_video {m_impl->actor->get_video ()};

          // Replace current actor with morph target
          m_impl->actor.reset ();
          m_impl->actor = m_impl->morph_target;

          // Setup new actor to render to video #2.
          m_impl->setup_actor_output (new_actor, temp_video);

          // Set morph target to newly added actor and start morphing process
          m_impl->morph_target = new_actor;
          m_impl->morph_time = Time::now ();

          new_actor->realize ();

      } else if (m_impl->use_morph && m_impl->morph) {
          // Morph enabled, but no morph process is running => setup morph process
          visual_log (VISUAL_LOG_DEBUG, "Setting up morph process.");

          auto actor_output_video {m_impl->actor->get_video ()};
          auto video1 {create_matching_format_video (actor_output_video)};
          auto video2 {create_matching_format_video (actor_output_video)};

          // Set current actor to render to video #1 instead.
          // Rendering target is already in the correct format, so there's nothing else to do
          m_impl->actor->set_video (video1);

          // Setup new actor to render to video #2.
          m_impl->setup_actor_output (new_actor, video2);

          // Redirect morph to render to output
          m_impl->morph->set_video (actor_output_video);

          // Start morph target to newly added actor and start morphing procss
          visual_log (VISUAL_LOG_DEBUG, "Morph start.");
          m_impl->morph_target = new_actor;
          m_impl->is_morphing = true;
          m_impl->morph_time = Time::now ();

          new_actor->realize ();
      } else {
          // Morphing is disabled => Immediately replace actor
          visual_log (VISUAL_LOG_DEBUG, "No morph is running, replacing actor");

          auto actor_output_video {m_impl->actor->get_video ()};
          m_impl->actor.reset ();
          m_impl->setup_actor_output (new_actor, actor_output_video);
          m_impl->actor = new_actor;

          new_actor->realize ();
      }
  }

  void Bin::switch_set_time (LV::Time const& time)
  {
      visual_return_if_fail (m_impl->is_morphing);
      m_impl->morph_time = std::max (time, min_switch_time);
  }

  void Bin::run ()
  {
      visual_return_if_fail (m_impl->actor);
      visual_return_if_fail (m_impl->input);
      visual_return_if_fail (m_impl->output_video);

      if (m_impl->need_sync) {
          sync ();
      }

      // Read audio data from input
      m_impl->input->run ();
      auto const& audio = m_impl->input->get_audio ();

      if (m_impl->is_morphing) {
          // Run actors to render inputs for morph
          m_impl->actor->run (audio);
          m_impl->morph_target->run (audio);

          // Run morph
          auto source1 {m_impl->actor->get_video ()};
          auto source2 {m_impl->morph_target->get_video ()};
          m_impl->morph->run (audio, source1, source2);

          // Reconfigure pipeline to a single running actor
          if (m_impl->morph->is_done ()) {
              visual_log (VISUAL_LOG_INFO, "Morph is complete, simplifying pipeline.");
              m_impl->cut_to_morph_target ();
          }
      } else {
          m_impl->actor->run (audio);
      }
  }

} // LV namespace
