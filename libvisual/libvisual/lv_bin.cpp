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

  class Bin::Impl
  {
  public:

      ActorPtr  actor;
      VideoPtr  actvideo;
      VideoPtr  privvid;

      VideoPtr  actmorphvideo;
      ActorPtr  actmorph;

      InputPtr  input;

      bool         use_morph;
      MorphPtr     morph;
      bool         morphing;
      Time         morphtime;

      VisBinDepth   depthpreferred;    /* Prefered depth, highest or lowest */
      VisVideoDepth depthflag;         /* Supported depths */
      VisVideoDepth depthold;          /* Previous depth */
      VisVideoDepth depth;             /* Depth we're running in */
      bool          depthchanged;      /* Set true if the depth has changed */
      bool          depthfromGL;       /* Set when switching away from openGL */
      VisVideoDepth depthforced;       /* Contains forced depth value, for the actmorph so we've got smooth transformations */
      VisVideoDepth depthforcedmain;   /* Contains forced depth value, for the main actor */

      Impl ();

      ~Impl ();

      VisVideoDepth get_suitable_depth (VisVideoDepth depth);

	  void set_actor (ActorPtr const& actor);
	  void set_input (InputPtr const& input);
  };

  VisVideoDepth Bin::Impl::get_suitable_depth (VisVideoDepth depthflag)
  {
      VisVideoDepth depth = VISUAL_VIDEO_DEPTH_NONE;

      switch (depthpreferred) {
          case VISUAL_BIN_DEPTH_LOWEST:
              depth = visual_video_depth_get_lowest (depthflag);
              break;

          case VISUAL_BIN_DEPTH_HIGHEST:
              depth = visual_video_depth_get_highest (depthflag);
              break;
      }

      // Is supported within bin natively
      if (depthflag & depth)
          return depth;

      // Not supported by the bin, taking the highest depth from the bin
      return visual_video_depth_get_highest_nogl (depthflag);
  }


  Bin::Impl::Impl ()
      : use_morph       (false)
      , morphing        (false)
      , morphtime       (4, 0)
      , depthpreferred  (VISUAL_BIN_DEPTH_HIGHEST)
      , depthflag       (VISUAL_VIDEO_DEPTH_NONE)
      , depthold        (VISUAL_VIDEO_DEPTH_NONE)
      , depth           (VISUAL_VIDEO_DEPTH_NONE)
      , depthchanged    (false)
      , depthfromGL     (false)
      , depthforced     (VISUAL_VIDEO_DEPTH_NONE)
      , depthforcedmain (VISUAL_VIDEO_DEPTH_NONE)
  {
      // empty
  }

  Bin::Impl::~Impl ()
  {
      // nothing
  }

  void Bin::Impl::set_actor (ActorPtr const& new_actor)
  {
      actor = new_actor;
  }

  void Bin::Impl::set_input (InputPtr const& new_input)
  {
      input = new_input;
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

  void Bin::realize ()
  {
      if (m_impl->actor)
          m_impl->actor->realize ();

      if (m_impl->input)
          m_impl->input->realize ();

      if (m_impl->morph)
          m_impl->morph->realize ();
  }

  ActorPtr const& Bin::get_actor () const
  {
      return m_impl->actor;
  }

  InputPtr const& Bin::get_input () const
  {
      return m_impl->input;
  }

  void Bin::set_morph (std::string_view morph_name)
  {
      m_impl->morph = Morph::load (morph_name);
      visual_return_if_fail (m_impl->morph);

      VisVideoDepth depthflag = m_impl->morph->get_supported_depths ();

      if (visual_video_depth_is_supported (depthflag, m_impl->actvideo->get_depth ()) <= 0) {
          m_impl->morph.reset ();
          return;
      }
  }

  MorphPtr const& Bin::get_morph () const
  {
      return m_impl->morph;
  }

  bool Bin::connect (ActorPtr const& actor, InputPtr const& input)
  {
      visual_return_val_if_fail (actor, false);
      visual_return_val_if_fail (input, false);

      m_impl->set_actor (actor);
      m_impl->set_input (input);

      auto depthflag = actor->get_supported_depths ();

      if (depthflag == VISUAL_VIDEO_DEPTH_GL) {
          set_depth (VISUAL_VIDEO_DEPTH_GL);
      } else {
          set_depth (m_impl->get_suitable_depth (depthflag));
      }

      m_impl->depthforcedmain = m_impl->depth;

      return true;
  }

  bool Bin::connect (std::string_view actor_name, std::string_view input_name)
  {
      // Create the actor
      auto actor = Actor::load (actor_name);
      visual_return_val_if_fail (actor, false);

      // Create the input
      auto input = Input::load (input_name);
      visual_return_val_if_fail (input, false);

      // Connect
      if (!connect (actor, input)) {
          return false;
      }

      return true;
  }

  void Bin::sync (bool noevent)
  {
      visual_log (VISUAL_LOG_DEBUG, "starting sync");

      VideoPtr video;

      /* Sync the actor regarding morph */
      if (m_impl->morphing && m_impl->use_morph &&
          m_impl->actvideo->get_depth () != VISUAL_VIDEO_DEPTH_GL && !m_impl->depthfromGL) {

          m_impl->morph->set_video (m_impl->actvideo);

          video = m_impl->privvid;
          if (!video) {
              visual_log (VISUAL_LOG_DEBUG, "Private video data nullptr");
              return;
          }

          video->free_buffer ();
          video->copy_attrs (m_impl->actvideo);

          visual_log (VISUAL_LOG_DEBUG, "pitches actvideo %d, new video %d",
                      m_impl->actvideo->get_pitch (), video->get_pitch ());

          visual_log (VISUAL_LOG_DEBUG, "phase1 m_impl->privvid %p", (void *) m_impl->privvid.get ());
          if (m_impl->actmorph->get_video ()->get_depth () == VISUAL_VIDEO_DEPTH_GL) {
              video = m_impl->actvideo;
          } else
              video->allocate_buffer ();

          visual_log (VISUAL_LOG_DEBUG, "phase2");
      } else {
          video = m_impl->actvideo;
          if (!video) {
              visual_log (VISUAL_LOG_DEBUG, "Actor video is nullptr");
              return;
          }

          visual_log (VISUAL_LOG_DEBUG, "setting new video from actvideo %d %d",
                      video->get_depth (), video->get_bpp ());
      }

      // Main actor

      m_impl->actor->set_video (video);

      visual_log (VISUAL_LOG_DEBUG, "one last video pitch check %d depth old %d forcedmain %d noevent %d",
                  video->get_pitch (), m_impl->depthold,
                  m_impl->depthforcedmain, noevent);

      if (m_impl->depthold == VISUAL_VIDEO_DEPTH_GL) {
          m_impl->actor->video_negotiate (m_impl->depthforcedmain, false, true);
      }
      else {
          m_impl->actor->video_negotiate (m_impl->depthforcedmain, noevent, true);
      }

      visual_log (VISUAL_LOG_DEBUG, "pitch after main actor negotiate %d", video->get_pitch ());

      // Morphing actor

      if (m_impl->morphing && m_impl->use_morph) {

          auto actvideo = m_impl->actmorphvideo;
          if (!actvideo) {
              visual_log (VISUAL_LOG_DEBUG, "Morph video is nullptr");
              return;
          }

          actvideo->free_buffer ();

          actvideo->copy_attrs (video);

          if (m_impl->actor->get_video ()->get_depth () != VISUAL_VIDEO_DEPTH_GL)
              actvideo->allocate_buffer ();

          m_impl->actmorph->realize ();

          visual_log (VISUAL_LOG_DEBUG, "phase3 pitch of real framebuffer %d",
                      m_impl->actvideo->get_pitch ());

          m_impl->actmorph->video_negotiate (m_impl->depthforced, false, true);
      }

      visual_log (VISUAL_LOG_DEBUG, "end sync function");
  }

  void Bin::set_video (VideoPtr const& video)
  {
      m_impl->actvideo = video;
  }

  void Bin::set_supported_depth (VisVideoDepth depthflag)
  {
      m_impl->depthflag = depthflag;
  }

  VisVideoDepth Bin::get_supported_depth () const
  {
      return m_impl->depthflag;
  }

  void Bin::set_preferred_depth (VisBinDepth depthpreferred)
  {
      m_impl->depthpreferred = depthpreferred;
  }

  void Bin::set_depth (VisVideoDepth depth)
  {
      m_impl->depthold = m_impl->depth;

      if (!visual_video_depth_is_supported (m_impl->depthflag, depth))
          return;

      visual_log (VISUAL_LOG_DEBUG, "old: %d new: %d", m_impl->depth, depth);

      if (m_impl->depth != depth)
          m_impl->depthchanged = true;

      if (m_impl->depth == VISUAL_VIDEO_DEPTH_GL && m_impl->depthchanged)
          m_impl->depthfromGL = true;
      else
          m_impl->depthfromGL = false;

      m_impl->depth = depth;

      if (m_impl->actvideo) {
          m_impl->actvideo->set_depth (depth);
      }
  }

  VisVideoDepth Bin::get_depth () const
  {
      return m_impl->depth;
  }

  bool Bin::depth_changed ()
  {
      if (!m_impl->depthchanged)
          return false;

      m_impl->depthchanged = false;

      return true;
  }

  Palette const& Bin::get_palette () const
  {
      if (m_impl->morphing)
          return *m_impl->morph->get_palette ();
      else
          return *m_impl->actor->get_palette ();
  }

  void Bin::switch_actor (std::string_view actor_name)
  {
      // FIXME: This is needed because visual_log() takes only null-terminated C strings.
      std::string actor_name_str {actor_name};

      visual_log (VISUAL_LOG_DEBUG, "switching to a new actor: %s, old actor: %s",
				  actor_name_str.c_str (), visual_plugin_get_info (m_impl->actor->get_plugin ())->plugname);

      if (m_impl->actmorph) {
          m_impl->actmorph.reset ();
          m_impl->actmorphvideo.reset ();
      }

      /* Create a new managed actor */
      auto actor = LV::Actor::load (actor_name);
      visual_return_if_fail (actor);

      auto video = LV::Video::create ();
      video->copy_attrs(m_impl->actvideo);

      auto depthflag = actor->get_supported_depths ();
      VisVideoDepth depth;

      if (visual_video_depth_is_supported (depthflag, VISUAL_VIDEO_DEPTH_GL)) {
          visual_log (VISUAL_LOG_INFO, "Switching to GL mode");

          depth = VISUAL_VIDEO_DEPTH_GL;

          m_impl->depthforced = depth;
          m_impl->depthforcedmain = depth;

          video->set_depth(depth);

          set_depth (depth);

          m_impl->depthchanged = true;
      } else {
          visual_log (VISUAL_LOG_INFO, "Switching away from Gl mode -- or non Gl switch");

          /* Switching from GL */
          depth = m_impl->get_suitable_depth (depthflag);
          video->set_depth(depth);

          visual_log (VISUAL_LOG_DEBUG, "after depth fixating");

          /* After a depth change, the pitch value needs an update from the client
           * if it's different from width * bpp, after a visual_bin_sync
           * the issues are fixed again */
          visual_log (VISUAL_LOG_INFO, "video depth (from fixate): %d", video->get_depth ());

          /* FIXME check if there are any unneeded depth transform environments and drop these */
          visual_log (VISUAL_LOG_DEBUG, "checking if we need to drop something: depthforcedmain: %d actvideo->depth %d",
                      m_impl->depthforcedmain, m_impl->actvideo->get_depth ());

          /* Drop a transformation environment when not needed */
          if (m_impl->depthforcedmain != m_impl->actvideo->get_depth ()) {
              m_impl->actor->video_negotiate (m_impl->depthforcedmain, true, true);
              visual_log (VISUAL_LOG_DEBUG, "[[[[optionally a bogus transform environment, dropping]]]]");
          }

          if (m_impl->actvideo->get_depth () > video->get_depth ()
              && m_impl->actvideo->get_depth () != VISUAL_VIDEO_DEPTH_GL
              && m_impl->use_morph) {

              visual_log (VISUAL_LOG_INFO, "old depth is higher, video depth %d, bin depth %d",
                          video->get_depth (), m_impl->depth);

              m_impl->depthforced = depth;;
              m_impl->depthforcedmain = m_impl->depth;

              set_depth (m_impl->actvideo->get_depth ());
              video->set_depth (m_impl->actvideo->get_depth ());

          } else if (m_impl->actvideo->get_depth () != VISUAL_VIDEO_DEPTH_GL) {

              visual_log (VISUAL_LOG_INFO, "new depth is higher, or equal: video depth %d, depth %d bin depth %d",
                          video->get_depth (), depth, m_impl->depth);

              visual_log (VISUAL_LOG_DEBUG, "depths i can locate: actvideo: %d   bin: %d     bin-old: %d",
                          m_impl->actvideo->get_depth (), m_impl->depth, m_impl->depthold);

              m_impl->depthforced = video->get_depth ();
              m_impl->depthforcedmain = m_impl->depth;

              visual_log (VISUAL_LOG_DEBUG, "depthforcedmain in switch by name: %d", m_impl->depthforcedmain);
              visual_log (VISUAL_LOG_DEBUG, "Bin::set_depth %d", video->get_depth ());
              set_depth (video->get_depth ());

          } else {
              /* Don't force ourself into a GL depth, seen we do a direct
               * switch in the run */
              m_impl->depthforced = video->get_depth ();
              m_impl->depthforcedmain = video->get_depth ();

              visual_log (VISUAL_LOG_INFO, "Switching from GL to framebuffer for real, framebuffer depth: %d",
                          video->get_depth ());
          }

          visual_log (VISUAL_LOG_INFO, "Target depth selected: %d", depth);

          video->set_pitch(video->get_width() * (visual_video_depth_bpp(depth) >> 3));

          video->allocate_buffer();
      }


      visual_log (VISUAL_LOG_INFO, "video pitch of that what connects to the new actor %d",
                  video->get_pitch ());

      actor->set_video (video);

      m_impl->actmorphvideo = video;

      visual_log (VISUAL_LOG_INFO, "switching... ******************************************");
      switch_actor (actor);

      visual_log (VISUAL_LOG_INFO, "end switch actor by name function ******************");
  }

  void Bin::switch_actor (ActorPtr const& actor)
  {
      visual_return_if_fail (actor);

      /* Set the new actor */
      m_impl->actmorph = actor;

      visual_log (VISUAL_LOG_DEBUG, "Starting actor switch...");

      /* Free the private video */
      m_impl->privvid.reset ();

      visual_log (VISUAL_LOG_INFO, "depth of the main actor: %d",
                  m_impl->actor->get_video ()->get_depth ());

      /* Starting the morph, but first check if we don't have anything todo with openGL */
      if (m_impl->use_morph &&
          m_impl->actor->get_video ()->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
          m_impl->actmorph->get_video ()->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
          !m_impl->depthfromGL) {

          if (m_impl->morph) {
              m_impl->morph->set_progress (0.0f);
              m_impl->morph->set_video (m_impl->actvideo);
              m_impl->morph->set_time (m_impl->morphtime);
          }

          visual_log (VISUAL_LOG_DEBUG, "phase 1");
          /* Allocate a private video for the main actor, so the morph
           * can draw to the framebuffer */
          auto privvid = Video::create ();

          visual_log (VISUAL_LOG_DEBUG, "actvideo->depth %d actmorph->video->depth %d",
                      m_impl->actvideo->get_depth (),
                      m_impl->actmorph->get_video ()->get_depth ());

          visual_log (VISUAL_LOG_DEBUG, "phase 2");
          privvid->copy_attrs (m_impl->actvideo);

          visual_log (VISUAL_LOG_DEBUG, "phase 3 pitch privvid %d actvideo %d",
                      privvid->get_pitch (), m_impl->actvideo->get_pitch ());

          privvid->allocate_buffer ();

          visual_log (VISUAL_LOG_DEBUG, "phase 4");
          /* Initial privvid initialize */

          visual_log (VISUAL_LOG_DEBUG, "actmorph->video->depth %d %p",
                      m_impl->actmorph->get_video ()->get_depth (),
                      m_impl->actvideo->get_pixels ());

          if (m_impl->actvideo->get_pixels () && privvid->get_pixels ())
              visual_mem_copy (privvid->get_pixels (), m_impl->actvideo->get_pixels (),
                               privvid->get_size ());
          else if (privvid->get_pixels ())
              visual_mem_set (privvid->get_pixels (), 0, privvid->get_size ());

          m_impl->actor->set_video (privvid);
          m_impl->privvid = privvid;
      } else {
          visual_log (VISUAL_LOG_DEBUG, "Pointer actvideo->pixels %p", m_impl->actvideo->get_pixels ());
          if (m_impl->actor->get_video ()->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
              m_impl->actvideo->get_pixels ()) {
              visual_mem_set (m_impl->actvideo->get_pixels (), 0, m_impl->actvideo->get_size ());
          }
      }

      visual_log (VISUAL_LOG_DEBUG, "Leaving, actor->video->depth: %d actmorph->video->depth: %d",
                  m_impl->actor->get_video ()->get_depth (),
                  m_impl->actmorph->get_video ()->get_depth ());

      m_impl->morphing = true;
  }

  void Bin::switch_finalize ()
  {
      visual_log (VISUAL_LOG_DEBUG, "Completing actor switch...");

      /* Copy over the depth to be sure, and for GL plugins */
      /* m_impl->actvideo->set_depth (m_impl->actmorphvideo->get_depth ()); */

      m_impl->actmorphvideo.reset ();
      m_impl->privvid.reset ();

      m_impl->actor = m_impl->actmorph;
      m_impl->actmorph.reset ();

      m_impl->actor->set_video (m_impl->actvideo);

      m_impl->morphing = false;
      m_impl->morph.reset ();

      visual_log (VISUAL_LOG_DEBUG, " - in finalize - fscking depth from actvideo: %d %d",
                  m_impl->actvideo->get_depth (),
                  m_impl->actvideo->get_bpp ());

      VisVideoDepth depthflag = m_impl->actor->get_supported_depths ();
      m_impl->actvideo->set_depth (m_impl->get_suitable_depth (depthflag));
      set_depth (m_impl->actvideo->get_depth ());

      m_impl->depthforcedmain = m_impl->actvideo->get_depth ();
      visual_log (VISUAL_LOG_DEBUG, "m_impl->depthforcedmain in finalize %d", m_impl->depthforcedmain);

      /* FIXME replace with a depth fixer */
      if (m_impl->depthchanged) {
          visual_log (VISUAL_LOG_INFO, "negotiate without event");
          m_impl->actor->video_negotiate (m_impl->depthforcedmain, true, true);
          visual_log (VISUAL_LOG_INFO, "end negotiate without event");
          //sync(false);
      }

      visual_log (VISUAL_LOG_DEBUG, "Leaving...");
  }

  void Bin::use_morph (bool use)
  {
      m_impl->use_morph = use;
  }

  void Bin::switch_set_time (Time const& time)
  {
      m_impl->morphtime = time;
  }

  void Bin::run ()
  {
      visual_return_if_fail (m_impl->actor);
      visual_return_if_fail (m_impl->input);

      m_impl->input->run ();

      /* If we have a direct switch, do this BEFORE we run the actor,
       * else we can get into trouble especially with GL, also when
       * switching away from a GL plugin this is needed */
      if (m_impl->morphing) {
          if (!visual_plugin_is_realized (m_impl->actmorph->get_plugin ())) {
              m_impl->actmorph->realize ();

              m_impl->actmorph->video_negotiate (m_impl->depthforced, false, true);
          }

          /* When we've got multiple switch events without a sync we need
           * to realize the main actor as well */
          if (!visual_plugin_is_realized (m_impl->actor->get_plugin ())) {
              m_impl->actor->realize ();

              m_impl->actor->video_negotiate (m_impl->depthforced, false, true);
          }

          /* When the style is DIRECT or the context is GL we shouldn't try
           * to morph and instead finalize at once */
          if (!m_impl->use_morph || m_impl->actor->get_video ()->get_depth () == VISUAL_VIDEO_DEPTH_GL) {

              switch_finalize ();

              /* We can't start drawing yet, the client needs to catch up with
               * the depth change */
              return;
          }
      }

      m_impl->actor->realize ();

      auto const& audio = m_impl->input->get_audio ();

      m_impl->actor->run (audio);

      if (m_impl->morphing) {
          if (m_impl->use_morph &&
              m_impl->actmorph->get_video ()->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
              m_impl->actor->get_video ()->get_depth () != VISUAL_VIDEO_DEPTH_GL) {

              m_impl->actmorph->run (audio);

              if (!m_impl->morph) {
                  switch_finalize ();
                  return;
              }

              /* Same goes for the morph, we realize it here for depth changes
               * (especially the OpenGL case */
              m_impl->morph->realize ();
              m_impl->morph->run (audio,
                                  m_impl->actor->get_video (),
                                  m_impl->actmorph->get_video ());

              if (m_impl->morph->is_done ()) {
                  switch_finalize ();
              }
          } else {
              /* visual_bin_switch_finalize (bin); */
          }
      }
  }

} // LV namespace
