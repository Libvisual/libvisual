/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
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
#include "lv_bin.h"
#include "lv_common.h"
#include "lv_list.h"

namespace LV {

  class Bin::Impl
  {
  public:

      bool managed;

      VisActor* actor;
      VideoPtr  actvideo;
      VideoPtr  privvid;

      bool      actmorphmanaged;
      VideoPtr  actmorphvideo;
      VisActor* actmorph;

      bool      inputmanaged;
      VisInput* input;

      bool         morphmanaged;
      VisMorph*    morph;
      int          morphstyle;
      bool         morphing;
      bool         morphautomatic;
      int          morphsteps;
      int          morphstepsdone;
      float        morphrate;
      VisMorphMode morphmode;
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
  };

  VisVideoDepth Bin::Impl::get_suitable_depth (VisVideoDepth depthflag)
  {
      VisVideoDepth depth;

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
      : actor           (0)
      , actmorphmanaged (false)
      , actmorph        (0)
      , inputmanaged    (0)
      , input           (0)
      , morphmanaged    (false)
      , morph           (0)
      , morphstyle      (0)
      , morphing        (false)
      , morphautomatic  (true)
      , morphsteps      (0)
      , morphstepsdone  (0)
      , morphrate       (0.0)
      , morphmode       (VISUAL_MORPH_MODE_TIME)
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
      if (actor)
          visual_object_unref (VISUAL_OBJECT (actor));

      if (input)
          visual_object_unref (VISUAL_OBJECT (input));

      if (morph)
          visual_object_unref (VISUAL_OBJECT (morph));

      if (actmorph)
          visual_object_unref (VISUAL_OBJECT (actmorph));
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
          visual_actor_realize (m_impl->actor);

      if (m_impl->input)
          visual_input_realize (m_impl->input);

      if (m_impl->morph)
          visual_morph_realize (m_impl->morph);
  }

  void Bin::set_actor (VisActor *actor)
  {
      if (m_impl->actor) {
          visual_object_unref (VISUAL_OBJECT (m_impl->actor));
      }

      m_impl->actor = actor;
      m_impl->managed = false;

      if (m_impl->actor) {
          visual_object_ref (VISUAL_OBJECT (m_impl->actor));
      }
  }

  VisActor* Bin::get_actor () const
  {
      return m_impl->actor;
  }

  void Bin::set_input (VisInput* input)
  {
      if (m_impl->input) {
          visual_object_unref (VISUAL_OBJECT (m_impl->input));
      }

      m_impl->input = input;
      m_impl->inputmanaged = false;

      if (m_impl->input) {
          visual_object_ref (VISUAL_OBJECT (m_impl->input));
      }
  }

  VisInput *Bin::get_input () const
  {
      return m_impl->input;
  }

  void Bin::set_morph (std::string const& morphname)
  {
      if (m_impl->morph)
          visual_object_unref (VISUAL_OBJECT (m_impl->morph));

      VisMorph* morph = visual_morph_new (morphname.c_str ());

      m_impl->morph = morph;
      m_impl->morphmanaged = true;

      visual_return_if_fail (morph->plugin != NULL);

      VisVideoDepth depthflag = visual_morph_get_supported_depth (morph);

      if (visual_video_depth_is_supported (depthflag, m_impl->actvideo->get_depth ()) <= 0) {
          visual_object_unref (VISUAL_OBJECT (morph));
          m_impl->morph = NULL;

          return;
      }
  }

  VisMorph* Bin::get_morph () const
  {
      return m_impl->morph;
  }

  void Bin::connect (VisActor *actor, VisInput *input)
  {
      visual_return_if_fail (actor != NULL);
      visual_return_if_fail (input != NULL);

      set_actor (actor);
      set_input (input);

      VisVideoDepth depthflag = visual_actor_get_supported_depth (actor);

      if (depthflag == VISUAL_VIDEO_DEPTH_GL) {
          set_depth (VISUAL_VIDEO_DEPTH_GL);
      } else {
          set_depth (m_impl->get_suitable_depth(depthflag));
      }

      m_impl->depthforcedmain = m_impl->depth;
  }

  void Bin::connect (std::string const& actname, std::string const& inname)
  {
      /* Create the actor */
      VisActor* actor = visual_actor_new (actname.c_str ());
      visual_return_if_fail (actor != NULL);

      /* Create the input */
      VisInput* input = visual_input_new (inname.c_str ());
      visual_return_if_fail (input != NULL);

      /* Connect */
      connect (actor, input);

      m_impl->managed = true;
      m_impl->inputmanaged = true;
  }

  void Bin::sync (bool noevent)
  {
      visual_log (VISUAL_LOG_DEBUG, "starting sync");

      VideoPtr video;

      /* Sync the actor regarding morph */
      if (m_impl->morphing && m_impl->morphstyle == VISUAL_SWITCH_STYLE_MORPH &&
          m_impl->actvideo->get_depth () != VISUAL_VIDEO_DEPTH_GL && !m_impl->depthfromGL) {

          visual_morph_set_video (m_impl->morph, m_impl->actvideo.get ());

          video = m_impl->privvid;
          if (!video) {
              visual_log (VISUAL_LOG_DEBUG, "Private video data NULL");
              return;
          }

          video->free_buffer ();
          video->copy_attrs (m_impl->actvideo);

          visual_log (VISUAL_LOG_DEBUG, "pitches actvideo %d, new video %d",
                      m_impl->actvideo->get_pitch (), video->get_pitch ());

          visual_log (VISUAL_LOG_DEBUG, "phase1 m_impl->privvid %p", (void *) m_impl->privvid.get ());
          if (m_impl->actmorph->video->get_depth () == VISUAL_VIDEO_DEPTH_GL) {
              video->set_buffer (NULL);
              video = m_impl->actvideo;
          } else
              video->allocate_buffer ();

          visual_log (VISUAL_LOG_DEBUG, "phase2");
      } else {
          video = m_impl->actvideo;
          if (!video) {
              visual_log (VISUAL_LOG_DEBUG, "Actor video is NULL");
              return;
          }

          visual_log (VISUAL_LOG_DEBUG, "setting new video from actvideo %d %d",
                      video->get_depth (), video->get_bpp ());
      }

      /* Main actor */
      /*    visual_actor_realize (m_impl->actor); */
      visual_actor_set_video (m_impl->actor, video.get ());

      visual_log (VISUAL_LOG_DEBUG, "one last video pitch check %d depth old %d forcedmain %d noevent %d",
                  video->get_pitch (), m_impl->depthold,
                  m_impl->depthforcedmain, noevent);

      if (m_impl->managed) {
          if (m_impl->depthold == VISUAL_VIDEO_DEPTH_GL)
              visual_actor_video_negotiate (m_impl->actor, m_impl->depthforcedmain, false, true);
          else
              visual_actor_video_negotiate (m_impl->actor, m_impl->depthforcedmain, noevent, true);
      } else {
          if (m_impl->depthold == VISUAL_VIDEO_DEPTH_GL)
              visual_actor_video_negotiate (m_impl->actor, VISUAL_VIDEO_DEPTH_NONE, false, true);
          else
              visual_actor_video_negotiate (m_impl->actor, VISUAL_VIDEO_DEPTH_NONE, noevent, false);
      }

      visual_log (VISUAL_LOG_DEBUG, "pitch after main actor negotiate %d", video->get_pitch ());

      /* Morphing actor */
      if (m_impl->actmorphmanaged && m_impl->morphing &&
          m_impl->morphstyle == VISUAL_SWITCH_STYLE_MORPH) {

          VideoPtr actvideo = m_impl->actmorphvideo;
          if (!actvideo) {
              visual_log (VISUAL_LOG_DEBUG, "Morph video is NULL");
              return;
          }

          actvideo->free_buffer ();

          actvideo->copy_attrs (video);

          if (m_impl->actor->video->get_depth () != VISUAL_VIDEO_DEPTH_GL)
              actvideo->allocate_buffer ();

          visual_actor_realize (m_impl->actmorph);

          visual_log (VISUAL_LOG_DEBUG, "phase3 pitch of real framebuffer %d",
                      m_impl->actvideo->get_pitch ());

          if (m_impl->actmorphmanaged)
              visual_actor_video_negotiate (m_impl->actmorph, m_impl->depthforced, false, true);
          else
              visual_actor_video_negotiate (m_impl->actmorph, VISUAL_VIDEO_DEPTH_NONE, false, false);
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

  void Bin::force_actor_depth (VisVideoDepth depth)
  {
      m_impl->depthforcedmain = depth;
  }

  Palette const& Bin::get_palette () const
  {
      if (m_impl->morphing)
          return *visual_morph_get_palette (m_impl->morph);
      else
          return *visual_actor_get_palette (m_impl->actor);
  }

  void Bin::switch_actor (std::string const& actname)
  {
      visual_log (VISUAL_LOG_DEBUG, "switching to a new actor: %s, old actor: %s",
				  actname.c_str (), m_impl->actor->plugin->info->plugname);

      /* Destroy if there already is a managed one */
      if (m_impl->actmorphmanaged) {
          if (m_impl->actmorph) {
              visual_object_unref (VISUAL_OBJECT (m_impl->actmorph));
			  m_impl->actmorphvideo = NULL;
          }
      }

      /* Create a new managed actor */
      VisActor *actor = visual_actor_new (actname.c_str ());
      visual_return_if_fail (actor != NULL);

      VideoPtr video = LV::Video::create();

      int width  = m_impl->actvideo->get_width ();
      int height = m_impl->actvideo->get_height ();

      VisVideoDepth depth = m_impl->actvideo->get_depth ();

      VisVideoDepth depthflag = visual_actor_get_supported_depth (actor);
      if (visual_video_depth_is_supported (depthflag, VISUAL_VIDEO_DEPTH_GL)) {
          visual_log (VISUAL_LOG_INFO, "Switching to GL mode");

          m_impl->depthforced = VISUAL_VIDEO_DEPTH_GL;
          m_impl->depthforcedmain = VISUAL_VIDEO_DEPTH_GL;

          depth = VISUAL_VIDEO_DEPTH_GL;

          set_depth (VISUAL_VIDEO_DEPTH_GL);
          m_impl->depthchanged = true;
      } else {
          visual_log (VISUAL_LOG_INFO, "Switching away from Gl mode -- or non Gl switch");

          /* Switching from GL */
          video->set_depth (m_impl->get_suitable_depth (depthflag));

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
              visual_actor_video_negotiate (m_impl->actor, m_impl->depthforcedmain, true, true);
              visual_log (VISUAL_LOG_DEBUG, "[[[[optionally a bogus transform environment, dropping]]]]");
          }

          if (m_impl->actvideo->get_depth () > video->get_depth ()
              && m_impl->actvideo->get_depth () != VISUAL_VIDEO_DEPTH_GL
              && m_impl->morphstyle == VISUAL_SWITCH_STYLE_MORPH) {

              visual_log (VISUAL_LOG_INFO, "old depth is higher, video depth %d, depth %d, bin depth %d",
                          video->get_depth (), depth, m_impl->depth);

              m_impl->depthforced = depth;
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
      }

      video = Video::create (width, height, depth);

      visual_log (VISUAL_LOG_INFO, "video pitch of that what connects to the new actor %d",
                  video->get_pitch ());

      visual_actor_set_video (actor, video.get ());

      m_impl->actmorphvideo = video;
      m_impl->actmorphmanaged = true;

      visual_log (VISUAL_LOG_INFO, "switching... ******************************************");
      switch_actor (actor);

      visual_log (VISUAL_LOG_INFO, "end switch actor by name function ******************");
  }

  void Bin::switch_actor (VisActor *actor)
  {
      visual_return_if_fail (actor != NULL);

      /* Set the new actor */
      m_impl->actmorph = actor;

      visual_log (VISUAL_LOG_DEBUG, "Starting actor switch...");

      /* Free the private video */
      m_impl->privvid = NULL;

      visual_log (VISUAL_LOG_INFO, "depth of the main actor: %d",
                  m_impl->actor->video->get_depth ());

      /* Starting the morph, but first check if we don't have anything todo with openGL */
      if (m_impl->morphstyle == VISUAL_SWITCH_STYLE_MORPH &&
          m_impl->actor->video->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
          m_impl->actmorph->video->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
          !m_impl->depthfromGL) {

          if (m_impl->morph != NULL && m_impl->morph->plugin != NULL) {
              visual_morph_set_rate (m_impl->morph, 0);

              visual_morph_set_video (m_impl->morph, m_impl->actvideo.get ());

              if (m_impl->morphautomatic)
                  visual_morph_set_mode (m_impl->morph, m_impl->morphmode);
              else
                  visual_morph_set_mode (m_impl->morph, VISUAL_MORPH_MODE_SET);

              visual_morph_set_time (m_impl->morph, &m_impl->morphtime);
              visual_morph_set_steps (m_impl->morph, m_impl->morphsteps);
          }

          m_impl->morphrate = 0;
          m_impl->morphstepsdone = 0;

          visual_log (VISUAL_LOG_DEBUG, "phase 1");
          /* Allocate a private video for the main actor, so the morph
           * can draw to the framebuffer */
          VideoPtr privvid = Video::create ();

          visual_log (VISUAL_LOG_DEBUG, "actvideo->depth %d actmorph->video->depth %d",
                      m_impl->actvideo->get_depth (),
                      m_impl->actmorph->video->get_depth ());

          visual_log (VISUAL_LOG_DEBUG, "phase 2");
          privvid->copy_attrs (m_impl->actvideo);

          visual_log (VISUAL_LOG_DEBUG, "phase 3 pitch privvid %d actvideo %d",
                      privvid->get_pitch (), m_impl->actvideo->get_pitch ());

          privvid->allocate_buffer ();

          visual_log (VISUAL_LOG_DEBUG, "phase 4");
          /* Initial privvid initialize */

          visual_log (VISUAL_LOG_DEBUG, "actmorph->video->depth %d %p",
                      m_impl->actmorph->video->get_depth (),
                      m_impl->actvideo->get_pixels ());

          if (m_impl->actvideo->get_pixels () && privvid->get_pixels ())
              visual_mem_copy (privvid->get_pixels (), m_impl->actvideo->get_pixels (),
                               privvid->get_size ());
          else if (privvid->get_pixels ())
              visual_mem_set (privvid->get_pixels (), 0, privvid->get_size ());

          visual_actor_set_video (m_impl->actor, privvid.get ());
          m_impl->privvid = privvid;
      } else {
          visual_log (VISUAL_LOG_DEBUG, "Pointer actvideo->pixels %p", m_impl->actvideo->get_pixels ());
          if (m_impl->actor->video->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
              m_impl->actvideo->get_pixels ()) {
              visual_mem_set (m_impl->actvideo->get_pixels (), 0, m_impl->actvideo->get_size ());
          }
      }

      visual_log (VISUAL_LOG_DEBUG, "Leaving, actor->video->depth: %d actmorph->video->depth: %d",
                  m_impl->actor->video->get_depth (), m_impl->actmorph->video->get_depth ());

      m_impl->morphing = true;
  }

  void Bin::switch_finalize ()
  {
      visual_log (VISUAL_LOG_DEBUG, "Completing actor switch...");

      if (m_impl->managed) {
          visual_object_unref (VISUAL_OBJECT (m_impl->actor));
      }

      /* Copy over the depth to be sure, and for GL plugins */
      /* m_impl->actvideo->set_depth (m_impl->actmorphvideo->get_depth ()); */

      if (m_impl->actmorphmanaged) {
          m_impl->actmorphvideo = NULL;
      }

      if (m_impl->privvid) {
          m_impl->privvid = NULL;
      }

      m_impl->actor = m_impl->actmorph;
      m_impl->actmorph = NULL;

      visual_actor_set_video (m_impl->actor, m_impl->actvideo.get ());

      m_impl->morphing = false;

      if (m_impl->morphmanaged) {
          visual_object_unref (VISUAL_OBJECT (m_impl->morph));
          m_impl->morph = NULL;
      }

      visual_log (VISUAL_LOG_DEBUG, " - in finalize - fscking depth from actvideo: %d %d",
                  m_impl->actvideo->get_depth (),
                  m_impl->actvideo->get_bpp ());

      VisVideoDepth depthflag = visual_actor_get_supported_depth (m_impl->actor);
      m_impl->actvideo->set_depth (m_impl->get_suitable_depth (depthflag));
      set_depth (m_impl->actvideo->get_depth ());

      m_impl->depthforcedmain = m_impl->actvideo->get_depth ();
      visual_log (VISUAL_LOG_DEBUG, "m_impl->depthforcedmain in finalize %d", m_impl->depthforcedmain);

      /* FIXME replace with a depth fixer */
      if (m_impl->depthchanged) {
          visual_log (VISUAL_LOG_INFO, "negotiate without event");
          visual_actor_video_negotiate (m_impl->actor, m_impl->depthforcedmain, true, true);
          visual_log (VISUAL_LOG_INFO, "end negotiate without event");
          /*    visual_bin_sync (bin); */
      }

      visual_log (VISUAL_LOG_DEBUG, "Leaving...");
  }

  void Bin::switch_set_style (VisBinSwitchStyle style)
  {
      m_impl->morphstyle = style;
  }

  void Bin::switch_set_steps (int steps)
  {
      m_impl->morphsteps = steps;
  }

  void Bin::switch_set_automatic (bool automatic)
  {
      m_impl->morphautomatic = automatic;
  }

  void Bin::switch_set_rate (float rate)
  {
      m_impl->morphrate = rate;
  }

  void Bin::switch_set_mode (VisMorphMode mode)
  {
      m_impl->morphmode = mode;
  }

  void Bin::switch_set_time (Time const& time)
  {
      m_impl->morphtime = time;
  }

  void Bin::run ()
  {
      visual_return_if_fail (m_impl->actor != NULL);
      visual_return_if_fail (m_impl->input != NULL);

      visual_input_run (m_impl->input);

      /* If we have a direct switch, do this BEFORE we run the actor,
       * else we can get into trouble especially with GL, also when
       * switching away from a GL plugin this is needed */
      if (m_impl->morphing) {
          /* We realize here, because it doesn't realize
           * on switch, the reason for this is so that after a
           * switch call, especially in a managed bin the
           * depth can be requested and set, this is important
           * for openGL plugins, the realize method checks
           * for double realize itself so we don't have
           * to check this, it's a bit hacky */
          visual_return_if_fail (m_impl->actmorph != NULL);
          visual_return_if_fail (m_impl->actmorph->plugin != NULL);

          if (!m_impl->actmorph->plugin->realized) {
              visual_actor_realize (m_impl->actmorph);

              if (m_impl->actmorphmanaged)
                  visual_actor_video_negotiate (m_impl->actmorph, m_impl->depthforced, false, true);
              else
                  visual_actor_video_negotiate (m_impl->actmorph, VISUAL_VIDEO_DEPTH_NONE, false, false);
          }

          /* When we've got multiple switch events without a sync we need
           * to realize the main actor as well */
          visual_return_if_fail (m_impl->actor->plugin != NULL);
          if (!m_impl->actor->plugin->realized) {
              visual_actor_realize (m_impl->actor);

              if (m_impl->managed)
                  visual_actor_video_negotiate (m_impl->actor, m_impl->depthforced, false, true);
              else
                  visual_actor_video_negotiate (m_impl->actor, VISUAL_VIDEO_DEPTH_NONE, false, false);
          }

          /* When the style is DIRECT or the context is GL we shouldn't try
           * to morph and instead finalize at once */
          visual_return_if_fail (m_impl->actor->video != NULL);
          if (m_impl->morphstyle == VISUAL_SWITCH_STYLE_DIRECT ||
              m_impl->actor->video->get_depth () == VISUAL_VIDEO_DEPTH_GL) {

              switch_finalize ();

              /* We can't start drawing yet, the client needs to catch up with
               * the depth change */
              return;
          }
      }

      /* We realize here because in a managed bin the depth for openGL is
       * requested after the connect, thus we can realize there yet */
      visual_actor_realize (m_impl->actor);

      visual_actor_run (m_impl->actor, m_impl->input->audio);

      if (m_impl->morphing) {
          visual_return_if_fail (m_impl->actmorph != NULL);
          visual_return_if_fail (m_impl->actmorph->video != NULL);
          visual_return_if_fail (m_impl->actor->video != NULL);

          if (m_impl->morphstyle == VISUAL_SWITCH_STYLE_MORPH &&
              m_impl->actmorph->video->get_depth () != VISUAL_VIDEO_DEPTH_GL &&
              m_impl->actor->video->get_depth () != VISUAL_VIDEO_DEPTH_GL) {

              visual_actor_run (m_impl->actmorph, m_impl->input->audio);

              if (m_impl->morph == NULL || m_impl->morph->plugin == NULL) {
                  switch_finalize ();
                  return;
              }

              /* Same goes for the morph, we realize it here for depth changes
               * (especially the openGL case */
              visual_morph_realize (m_impl->morph);
              visual_morph_run (m_impl->morph, m_impl->input->audio, m_impl->actor->video, m_impl->actmorph->video);

              if (visual_morph_is_done (m_impl->morph))
                  switch_finalize ();
          } else {
              /*            visual_bin_switch_finalize (bin); */
          }
      }
  }

} // LV namespace
