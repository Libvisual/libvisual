// lv-tool - Libvisual commandline tool
//
// Copyright (C) 2012-2013 Libvisual team
//               2004-2006 Dennis Smit
//
// Authors: Daniel Hiepler <daniel@niftylight.de>
//          Chong Kai Xiong <kaixiong@codeleft.sg>
//          Dennis Smit <ds@nerds-incorporated.org>
//          Scott Sibley <sisibley@gmail.com>
//
// This file is part of lv-tool.
//
// lv-tool is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// lv-tool is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with lv-tool.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"
#include "version.h"
#include "display/display.hpp"
#include "display/display_driver_factory.hpp"
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <atomic>
#include <stdexcept>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <getopt.h>

// Defaults
#define DEFAULT_ACTOR_GL "lv_gltest"
#define DEFAULT_ACTOR_NONGL "lv_scope"
#define DEFAULT_INPUT   "debug"
#define DEFAULT_MORPH   "slide_left"
#define DEFAULT_WIDTH   640
#define DEFAULT_HEIGHT  480
#define DEFAULT_FPS     60
#define DEFAULT_COLOR_DEPTH 0

#if HAVE_SDL
# define DEFAULT_DRIVER "sdl"
#else
# define DEFAULT_DRIVER "null"
#endif

namespace {

  const std::unordered_set<std::string> actors_to_skip {
      "gdkpixbuf",
      "gstreamer",
      "lv_analyzer"
  };

  std::string actor_name;
  std::string input_name = DEFAULT_INPUT;
  std::string morph_name = DEFAULT_MORPH;
  std::string driver_name = DEFAULT_DRIVER;
  std::string exclude_actors;

  unsigned int width  = DEFAULT_WIDTH;
  unsigned int height = DEFAULT_HEIGHT;
  unsigned int color_depth = DEFAULT_COLOR_DEPTH;

  unsigned int frame_rate  = DEFAULT_FPS;
  unsigned int frame_count = 0;
  unsigned int actor_switch_after_frames = 0;
  unsigned int actor_switch_framecount = 0;

  bool have_seed = 0;
  uint32_t seed = 0;

  std::atomic_bool terminate_process = false;

  enum class CycleDir
  {
      PREV,
      NEXT
  };

  /** Class to manage LV's lifecycle */
  class Libvisual final
  {
  public:

      Libvisual (int& argc, char**& argv)
      {
          LV::System::init (argc, argv);
      }

      ~Libvisual ()
      {
          LV::System::destroy ();
      }
  };

  class ResizeRequest
  {
  public:
      ResizeRequest()
          : m_width {0}
          , m_height {0}
      {}

      unsigned int get_width() const { return m_width; }
      unsigned int get_height() const { return m_height; }

      bool is_due() const
      {
          if (m_applied_at >= m_received_at) {
              return false;
          }
          const double seconds_passed = (LV::Time::now() - m_received_at).to_secs();
          return seconds_passed >= 0.2; // >=300ms is known to feel slow to humans
      }

      void store(unsigned int width, unsigned int height)
      {
          m_width = width;
          m_height = height;
          m_received_at = LV::Time::now();
      }

      void mark_as_applied()
      {
          m_applied_at = LV::Time::now();
      }

  private:
      unsigned int m_width;
      unsigned int m_height;
      LV::Time m_received_at;
      LV::Time m_applied_at;
  };

  /** print info about libvisual plugin */
  void print_plugin_info(VisPluginInfo const& info)
  {
      printf("Plugin: \"%s\" (%s)\n"
                  "\tURL: %s\n"
                  "\tAuthor: %s\n\tVersion: %s\tLicense: %s\n"
                  "\t%s - %s\n\n",
                  info.name, info.plugname,
                  info.url ? info.url : "<n/a>",
                  info.author, info.version, info.license,
                  info.about, info.help);
  }


  /** print help for plugins */
  void print_plugin_help()
  {
      printf("===== INPUTS =====\n");
      auto const& inputs =
          LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_INPUT);

      // print inputs
      if(inputs.empty())
      {
          std::cerr << "No input plugins found\n";
      }
      else
      {
          for (auto input : inputs)
              print_plugin_info(*input.info);
      }


      printf("===== ACTORS =====\n");
      auto const& actors =
          LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_ACTOR);

      // print actors
      if(actors.empty())
      {
          std::cerr << "No actor plugins found\n";
      }
      else
      {
          for (auto actor : actors)
              print_plugin_info(*actor.info);
      }


      printf("===== MORPHS =====\n");
      auto const& morphs =
          LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_MORPH);

      // print morphs
      if(morphs.empty())
      {
          std::cerr << "No morph plugins found\n";
      }
      else
      {
          for (auto morph : morphs)
              print_plugin_info(*morph.info);
      }

  }

  /** print commandline help */
  void print_help (std::string_view name)
  {
      std::string const name_str {name};
      printf("Usage: %s [options]\n\n"
                  "Valid options:\n"
                  "\t--help\t\t\t-h\t\tThis help text\n"
                  "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
                  "\t--verbose\t\t-v\t\tIncrease log verbosity (may be used multiple times)\n"
                  "\t--dimensions <wxh>\t-D <wxh>\tRequest dimensions from display driver (no guarantee) [%dx%d]\n"
                  "\t--depth <depth> \t-c <depth>\tSet output colour depth (automatic by default)\n"
                  "\t--driver <driver>\t-d <driver>\tUse this output driver [%s]\n"
                  "\t--input <input>\t\t-i <input>\tUse this input plugin [%s]\n"
                  "\t--actor <actor>\t\t-a <actor>\tUse this actor plugin [%s/%s]\n"
                  "\t--morph <morph>\t\t-m <morph>\tUse this morph plugin [%s]\n"
                  "\t--seed <seed>\t\t-s <seed>\tSet random seed\n"
                  "\t--fps <n>\t\t-f <n>\t\tLimit output to n frames per second (if display driver supports it) [%d]\n"
                  "\t--framecount <n>\t-F <n>\t\tOutput n frames, then exit.\n"
                  "\t--switch <n>\t\t-S <n>\t\tSwitch actor after n frames.\n"
                  "\t--exclude <actors>\t-x <actors>\tProvide a list of actors to exclude.\n"
                  "\n",
                  name_str.c_str (),
                  width, height,
                  driver_name.c_str (),
                  input_name.c_str (),
                  DEFAULT_ACTOR_GL,
                  DEFAULT_ACTOR_NONGL,
                  morph_name.c_str (),
                  frame_rate);

        printf("Available output drivers:\n");
        for(auto driver_name : DisplayDriverFactory::instance().get_driver_list())
        {
            printf("\t%s\n", driver_name.c_str());
        }
  }


  /**
   * parse commandline arguments
   *
   * @param argc from main()
   * @param argv from main()
   * @result 0 upon success, <0 upon failure, >0 if app should exit without error */
  int parse_args(int argc, char *argv[])
  {
      static struct option loptions[] = {
          {"help",        no_argument,       0, 'h'},
          {"plugin-help", no_argument,       0, 'p'},
          {"verbose",     no_argument,       0, 'v'},
          {"dimensions",  required_argument, 0, 'D'},
          {"driver",      required_argument, 0, 'd'},
          {"input",       required_argument, 0, 'i'},
          {"actor",       required_argument, 0, 'a'},
          {"morph",       required_argument, 0, 'm'},
          {"fps",         required_argument, 0, 'f'},
          {"seed",        required_argument, 0, 's'},
          {"exclude",     required_argument, 0, 'x'},
          {"framecount",  required_argument, 0, 'F'},
          {"switch",      required_argument, 0, 'S'},
          {"depth",       required_argument, 0, 'c'},
          {0,             0,                 0,  0 }
      };

      int index, argument;

      while ((argument = getopt_long(argc, argv, "hpvD:d:i:a:m:f:s:F:S:x:c:", loptions, &index)) >= 0) {

          switch(argument) {
              // --help
              case 'h': {
                  print_help(argv[0]);
                  return 1;
              }

              // --plugin-help
              case 'p': {
                  print_plugin_help();
                  return 1;
              }

              // --verbose
              case 'v': {
                  VisLogSeverity level = visual_log_get_verbosity ();
                  level = VisLogSeverity (int (level) - 1);

                  // 0 is maximum verbosity, don't go further
                  if (int (level) <= 0) {
                      visual_log_set_verbosity ((VisLogSeverity) 0);
                  }
                  else {
                      visual_log_set_verbosity (level);
                  }
                  break;
              }

              // --dimensions
              case 'D': {
                  if (std::sscanf (optarg, "%ux%u", &width, &height) != 2)
                  {
                      std::cerr << "Invalid dimensions: '" << optarg << "'. Use <width>x<height> (e.g. 640x480)\n";
                      return -1;
                  }
                  break;
              }

              // --depth
              case 'c': {
                  if (std::sscanf (optarg, "%u", &color_depth) != 1 ||
                      visual_video_depth_from_bpp(color_depth) == VISUAL_VIDEO_DEPTH_NONE)
                  {
                      std::cerr << "Invalid depth: '" << optarg << "'. Use integer value (e.g. 24)\n";
                      return -1;
                  }
                  break;
              }

              // --driver
              case 'd': {
                  if (!DisplayDriverFactory::instance().has_driver (optarg)) {
                      std::cerr << "Unsupported display driver: " << optarg << "\n";
                      return -1;
                  }

                  driver_name = optarg;
                  break;
              }

              // --input
              case 'i': {
                  // save name for later
                  input_name = optarg;
                  break;
              }

              // --actor
              case 'a': {
                  // save name for later
                  actor_name = optarg;
                  break;
              }

              // --morph
              case 'm': {
                  /* save filename for later */
                  morph_name = optarg;
                  break;
              }

              // --fps
              case 'f': {
                  // set frame_rate
                  std::sscanf(optarg, "%u", &frame_rate);
                  break;
              }

              // --seed
              case 's': {
                  have_seed = true;
                  std::sscanf(optarg, "%d", &seed);
                  break;
              }

              // --framecount
              case 'F': {
                  // set framecount
                  std::sscanf(optarg, "%u", &frame_count);
                  break;
              }

              // --switch
              case 'S': {
                  // switch actor after n frames
                  std::sscanf(optarg, "%u", &actor_switch_after_frames);
                  break;
              }

              // --exclude
              case 'x': {
                  exclude_actors = optarg;
                  break;
              }

              // invalid argument
              case '?': {
                  print_help(argv[0]);
                  return -1;
              }

              // unhandled arguments
              default:
                  std::abort ();
          }
      }

      return 0;
  }

  void handle_termination_signal (int signal)
  {
      (void)signal;

      terminate_process = true;
  }

  /**
   * Handle process termination signals.
   */
  void setup_signal_handlers ()
  {
      std::signal (SIGINT, handle_termination_signal);
      std::signal (SIGTERM, handle_termination_signal);
  }

  std::string cycle_actor_name (std::string const& name, CycleDir dir)
  {
      auto cycler = (dir == CycleDir::NEXT) ? visual_actor_get_next_by_name
                                            : visual_actor_get_prev_by_name;

      auto new_name = cycler (name.c_str ());
      if (!new_name) {
          new_name = cycler (nullptr);
      }

      // Always skip actors that are of little interest to end users,
      // while allowing explicit "--actor (gdkpixbuf|gstreamer)".
      if (actors_to_skip.contains (new_name)) {
          return cycle_actor_name (new_name, dir);
      }

      // FIXME: this won't work if an actor's name is used as part of
      // another actor's name
      if (exclude_actors.find (new_name) != std::string::npos) {
          return cycle_actor_name (new_name, dir);
      }

      return new_name;
  }

#if 0
  std::string cycle_morph_name (std::string const& name, CycleDir dir)
  {
      auto cycler = (dir == CycleDir::NEXT) ? visual_morph_get_next_by_name
                                            : visual_morph_get_prev_by_name;

      auto new_name = cycler (name.c_str ());
      if (!new_name) {
          new_name = cycler (nullptr);
      }

      return new_name;
  }
#endif

} // anonymous namespace


int main (int argc, char **argv)
{
    try {
        // print warm welcome
        std::cerr << visual_truncate_path (argv[0], 1) << " - "
                  << PACKAGE_STRING
                  << " (" << LV_REVISION << ") commandline tool - "
                  << PACKAGE_URL << "\n";

        // setup signal handlers
        setup_signal_handlers ();

        // default loglevel
        visual_log_set_verbosity (VISUAL_LOG_ERROR);

        // initialize LV
        Libvisual main {argc, argv};

        // parse commandline arguments
        int parse_result = parse_args (argc, argv);
        if (parse_result < 0) {
            throw std::runtime_error ("Failed to parse arguments");
        }
        if (parse_result > 0) {
            return EXIT_SUCCESS;
        }

        // Set system-wide random seed
        if (have_seed) {
            LV::System::instance()->set_rng_seed (seed);
        }

        // create new VisBin for video output
        LV::Bin bin;
        bin.set_supported_depth(VISUAL_VIDEO_DEPTH_ALL);
        bin.use_morph(false);

        // Apply dynamic actor default, as needed
        if (actor_name.empty()) {
            actor_name = DEFAULT_ACTOR_NONGL;

            // Upgrade default to a more appealing OpenGL actor if available
            if (LV::Actor::available(DEFAULT_ACTOR_GL)) {
                actor_name = DEFAULT_ACTOR_GL;
            }
        }

        // Let the bin manage plugins. There's a bug otherwise.
        if (!bin.connect(actor_name, input_name)) {
            throw std::runtime_error ("Failed to start pipeline with actor '" + actor_name + "' and input '" + input_name + "'");
        }

        auto actor = bin.get_actor();

        // Select output colour depth

        VisVideoDepth depth;
        int depthflag = actor->get_supported_depths ();

        // Pick the best display depth directly supported by non GL actor
        if(depthflag != VISUAL_VIDEO_DEPTH_GL)
        {
            if (color_depth == 0)
            {
                depth = visual_video_depth_get_highest_nogl (depthflag);
            }
            // Pick user chosen colordepth
            else
            {
                depth = visual_video_depth_from_bpp (color_depth);
            }
        }
        /* GL actor */
        else
        {
            depth = visual_video_depth_get_highest (depthflag);
        }

        bin.set_depth (depth);

        auto vidoptions = actor->get_video_attribute_options ();

        // initialize display
        Display display (driver_name);

        // create display
        auto video = display.create(depth, vidoptions, width, height, true);
        if(!video) {
            throw std::runtime_error("Failed to setup display for rendering");
        }

        // Set the display title
        display.set_title(_("lv-tool"));

        // put it all together
        bin.set_video(video);
        bin.realize();
        bin.sync(false);
        bin.depth_changed();

        bin.set_morph(morph_name);

        // get a queue to handle events
        LV::EventQueue localqueue;

        // rendering statistics
        uint64_t frames_drawn = 0;

        // frame rate control state
        uint64_t const frame_period_us = frame_rate > 0 ? VISUAL_USECS_PER_SEC / frame_rate : 0;
        LV::Time last_frame_time;
        bool draw_frame = true;

        // main loop
        bool running = true;
        ResizeRequest resize_request;
        //bool visible = true;

        while (running)
        {
            // Check if process termination was signaled
            if (terminate_process) {
                std::cerr << "Received signal to terminate process, exiting..\n";
                return EXIT_SUCCESS;
            }

            // Control frame rate
            if (frame_rate > 0) {
                if (frames_drawn > 0) {
                    draw_frame = (LV::Time::now () - last_frame_time).to_usecs () >= frame_period_us;
                }
            }

            if (draw_frame) {
                DisplayLock lock {display};

                // Draw audio data and render
                bin.run();

                // Display rendering
                display.update_all ();

                // Record frame time
                last_frame_time = LV::Time::now ();

                // All frames rendered?
                frames_drawn++;
                if (frame_count > 0 && frames_drawn >= frame_count) {
                    break;
                }

                // switch actor?
                if (actor_switch_after_frames > 0 &&
                    frames_drawn >= actor_switch_after_frames + actor_switch_framecount)
                {
                    actor_switch_framecount += actor_switch_after_frames;

                    actor_name = cycle_actor_name (actor_name, CycleDir::NEXT);
                    std::cerr << "Switching to actor '" << actor_name << "'...\n";
                    bin.switch_actor (actor_name);
                }
            }

            LV::Event ev;

            // Handle all events
            display.drain_events(localqueue);

            auto pluginqueue = visual_plugin_get_event_queue (bin.get_actor()->get_plugin ());

            while (localqueue.poll(ev))
            {
                if(ev.type != VISUAL_EVENT_RESIZE)
                    pluginqueue->add (ev);

                switch (ev.type)
                {
                    case VISUAL_EVENT_PARAM:
                    {
                        break;
                    }

                    case VISUAL_EVENT_RESIZE:
                    {
                        resize_request.store(ev.event.resize.width, ev.event.resize.height);
                        break;
                    }

                    case VISUAL_EVENT_MOUSEMOTION:
                    {
                        break;
                    }

                    case VISUAL_EVENT_MOUSEBUTTONDOWN:
                    {
                        // switch to next actor
                        actor_name = cycle_actor_name (actor_name, CycleDir::NEXT);

                        std::cerr << "Switching to actor '" << actor_name << "'...\n";
                        bin.switch_actor (actor_name);

                        break;
                    }

                    case VISUAL_EVENT_MOUSEBUTTONUP:
                    {
                        break;
                    }

                    case VISUAL_EVENT_KEYDOWN:
                    {
                        switch(ev.event.keyboard.keysym.sym)
                        {
                            case VKEY_ESCAPE:
                            {
                                running = false;
                                break;
                            }

                            case VKEY_TAB:
                            {
                                break;
                            }

                            case VKEY_F11:
                            {
                                const bool currently_fullscreen = display.is_fullscreen();
                                display.set_fullscreen (!currently_fullscreen, true);
                                break;
                            }

                            default:
                                break;
                        }

                        break;
                    }

                    case VISUAL_EVENT_KEYUP:
                    {
                        break;
                    }

                    case VISUAL_EVENT_QUIT:
                    {
                        running = false;
                        break;
                    }

                    case VISUAL_EVENT_VISIBILITY:
                    {
                        //visible = ev.event.visibility.is_visible;
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }
            }

            if (resize_request.is_due ()) {
                DisplayLock lock {display};

                width = resize_request.get_width();
                height = resize_request.get_height();

                video = display.create(depth, vidoptions, width, height, true);
                display.set_title(_("lv-tool"));

                bin.set_video (video);
                bin.sync(false);

                resize_request.mark_as_applied();
            }

            if (bin.depth_changed())
            {
                DisplayLock lock {display};

                int depthflag = bin.get_depth();
                VisVideoDepth depth = visual_video_depth_get_highest(depthflag);

                display.create(depth, vidoptions, width, height, true);

                video = display.get_video();
                bin.set_video(video);

                bin.sync(true);
            }
        }

        return EXIT_SUCCESS;
    }
    catch (std::exception& error) {
        std::cerr << error.what () << std::endl;

        return EXIT_FAILURE;
    }
}
