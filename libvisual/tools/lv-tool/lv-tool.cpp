// lv-tool - Libvisual commandline tool
//
// Copyright (C) 2012-2013 Libvisual team
//               2004-2006 Dennis Smit
//
// Authors: Daniel Hiepler <daniel@niftylight.de>
//          Chong Kai Xiong <kaixiong@codeleft.sg>
//          Dennis Smit <ds@nerds-incorporated.org>
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
#include "gettext.h"
#include <libvisual/libvisual.h>
#include <SDL/SDL.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <csignal>
#include <getopt.h>

// Defaults
#define DEFAULT_ACTOR   "lv_analyzer"
#define DEFAULT_INPUT   "debug"
#define DEFAULT_WIDTH   320
#define DEFAULT_HEIGHT  200
#define DEFAULT_FPS     30
#define DEFAULT_COLOR_DEPTH 0

namespace {

  std::string actor_name = DEFAULT_ACTOR;
  std::string input_name = DEFAULT_INPUT;
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

  volatile std::sig_atomic_t terminate_process = false;

  enum class CycleDir
  {
      PREV,
      NEXT
  };

  /** Class to manage LV's lifecycle */
  class Libvisual
  {
  public:

      Libvisual (int& argc, char**& argv)
      {
          visual_init (&argc, &argv);
      }

      ~Libvisual ()
      {
          visual_quit ();
      }
  };

  class Display
  {
      SDL_Surface * m_screen;
      VisVideoDepth m_requested_depth;
      VisVideo * m_screen_video;
      bool m_resizable;

      void destroy() {
          if (m_screen_video) {
              visual_object_unref(VISUAL_OBJECT(m_screen_video));
              m_screen_video = nullptr;
          }
          if (m_screen) {
              // NOTE: The surface returned by SDL_SetVideoMode is either
              //       freed by the next call to SDL_SetVideoMode or
              //       when calling SDL_QUIT.  So nothing to free here.
              //       https://www.libsdl.org/release/SDL-1.2.15/docs/html/sdlsetvideomode.html
          }
      }

  public:
      Display() : m_screen(nullptr), m_requested_depth(VISUAL_VIDEO_DEPTH_NONE), m_screen_video(nullptr),
                  m_resizable(false) {
      }

      ~Display() {
          destroy();

          if (SDL_WasInit (SDL_INIT_VIDEO)) {
              SDL_Quit();
          }
      }

      VisVideo * create (VisVideoDepth depth,
                         VisVideoAttributeOptions const* vidoptions,
                         unsigned int width,
                         unsigned int height,
                         bool resizable) {
          int videoflags = 0;

          if (resizable)
              videoflags |= SDL_RESIZABLE;

          if (!SDL_WasInit (SDL_INIT_VIDEO)) {
              if (SDL_Init (SDL_INIT_VIDEO) == -1) {
                  visual_log (VISUAL_LOG_ERROR, "Unable to init SDL VIDEO: %s", SDL_GetError ());
                  return nullptr;
              }
          }

          m_resizable = resizable;
          m_requested_depth = depth;

          destroy();

          int bpp;

          if (depth == VISUAL_VIDEO_DEPTH_GL) {
              SDL_VideoInfo const* videoinfo = SDL_GetVideoInfo ();

              if (!videoinfo) {
                  return nullptr;
              }

              videoflags |= SDL_OPENGL;

              if (videoinfo->hw_available)
                  videoflags |= SDL_HWSURFACE;

              // TODO apply vidoptions here

              bpp = videoinfo->vfmt->BitsPerPixel;

              SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
          }
          else
          {
              bpp = 8 * visual_video_bpp_from_depth (depth);
          }

          // Create surface
          m_screen = SDL_SetVideoMode (width, height, bpp, videoflags);
          visual_log_return_val_if_fail( m_screen != nullptr, nullptr );

          // Recreate video object
          m_screen_video = visual_video_new();
          visual_log_return_val_if_fail( m_screen_video != nullptr, nullptr );
          visual_video_set_attributes(m_screen_video, m_screen->w, m_screen->h, m_screen->pitch, depth);
          visual_video_allocate_buffer(m_screen_video);

          SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

          return m_screen_video;
      }

      void drain_events(VisEventQueue & eventqueue) {
          // NOTE: SDL_VIDEORESIZE calls create(..) here as well
          SDL_Event event;
          while (SDL_PollEvent (&event)) {
              switch (event.type) {
                  case SDL_KEYUP:
                      visual_event_queue_add_keyboard (&eventqueue,
                                                       VisKey(event.key.keysym.sym),
                                                       VisKeyMod(event.key.keysym.mod),
                                                       VISUAL_KEY_UP);
                      break;

                  case SDL_KEYDOWN:
                      visual_event_queue_add_keyboard (&eventqueue, VisKey(event.key.keysym.sym),
                                                       VisKeyMod(event.key.keysym.mod),
                                                       VISUAL_KEY_DOWN);
                      break;

                  case SDL_VIDEORESIZE:
                      visual_event_queue_add_resize (&eventqueue, NULL, event.resize.w, event.resize.h);
                      // TODO non-NULL vidoptions here?
                      create (m_requested_depth, nullptr, event.resize.w, event.resize.h, m_resizable);
                      break;

                  case SDL_MOUSEMOTION:
                      visual_event_queue_add_mousemotion (&eventqueue, event.motion.x,
                                                          event.motion.y);
                      break;

                  case SDL_MOUSEBUTTONDOWN:
                      visual_event_queue_add_mousebutton (&eventqueue, event.button.button,
                                                          VISUAL_MOUSE_DOWN,
                                                          event.button.x,
                                                          event.button.y);
                      break;

                  case SDL_MOUSEBUTTONUP:
                      visual_event_queue_add_mousebutton (&eventqueue, event.button.button,
                                                          VISUAL_MOUSE_UP,
                                                          event.button.x,
                                                          event.button.y);
                      break;

                  case SDL_QUIT:
                      visual_event_queue_add_quit (&eventqueue, 0);
                      break;

                  default:
                      break;
              }
          }
      }

      VisVideo * get_video() const {
          visual_log_return_val_if_fail( m_screen_video != nullptr, nullptr );
          return m_screen_video;
      }

      static void set_title(std::string const& title) {
          SDL_WM_SetCaption (title.c_str(), nullptr);
      }

      void update_all() {
          visual_log_return_if_fail( m_screen != nullptr );

          if (m_screen->format->BitsPerPixel == 8) {
              visual_log_return_if_fail( m_screen_video != nullptr );
              VisPalette * const pal = m_screen_video->pal;

              if (pal != nullptr && pal->ncolors <= 256) {
                  SDL_Color colors[256];

                  for (int i = 0; i < 256; i++) {
                      colors[i].r = pal->colors[i].r;
                      colors[i].g = pal->colors[i].g;
                      colors[i].b = pal->colors[i].b;
                  }

                  SDL_SetColors (m_screen, colors, 0, 256);
              }
          }

          if (m_requested_depth == VISUAL_VIDEO_DEPTH_GL) {
              SDL_GL_SwapBuffers();
          } else {
              visual_mem_copy(m_screen->pixels,
                              visual_video_get_pixels(m_screen_video),
                              visual_video_get_size(m_screen_video));
              SDL_Flip(m_screen);
          }
      }

      void lock () {
          visual_log_return_if_fail( m_screen != nullptr );
          if (SDL_MUSTLOCK (m_screen))
              SDL_LockSurface (m_screen);
      }

      void unlock () {
          visual_log_return_if_fail( m_screen != nullptr );
          if (SDL_MUSTLOCK (m_screen))
              SDL_UnlockSurface (m_screen);
      }
  };

  class DisplayLock
  {
      Display * m_display;

  public:
      DisplayLock(Display & display) : m_display(&display) {
          visual_log_return_if_fail( m_display != nullptr );
          m_display->lock();
      }

      ~DisplayLock() {
          m_display->unlock();
      }
  };

  /** print info about libvisual plugin */
  void print_plugin_info(VisPluginInfo const& info)
  {
      printf("Plugin: \"%s\" (%s)\n"
                  "\tAuthor: %s\n\tVersion: %s\t\n"
                  "\t%s - %s\n\n",
                  info.name, info.plugname,
                  info.author, info.version,
                  info.about, info.help);
  }


  /** print help for plugins */
  void print_plugin_help()
  {
      VisList * const all_plugins = visual_plugin_get_registry();

      VisListEntry *le = NULL;
      VisPluginRef *ref = NULL;

      printf("===== INPUTS =====\n");
      VisList * const inputs = visual_plugin_registry_filter(all_plugins, "Libvisual:core:input");
      ref = static_cast<VisPluginRef*>(visual_list_next (inputs, &le));

      // print inputs
      if(! ref)
      {
          std::cerr << "No input plugins found\n";
      }
      else
      {
          do {
              visual_plugin_load(ref);
              print_plugin_info(*(ref->info));
              ref = static_cast<VisPluginRef*>(visual_list_next (inputs, &le));
          } while (ref != nullptr);
      }

      printf("===== ACTORS =====\n");
      VisList * const actors = visual_plugin_registry_filter(all_plugins, "Libvisual:core:actor");
      ref = static_cast<VisPluginRef*>(visual_list_next (actors, &le));

      // print actors
      if(! ref)
      {
          std::cerr << "No actor plugins found\n";
      }
      else
      {
          do {
              visual_plugin_load(ref);
              print_plugin_info(*(ref->info));
              ref = static_cast<VisPluginRef*>(visual_list_next (inputs, &le));
          } while (ref != nullptr);
      }
  }

  /** print commandline help */
  void print_help(std::string const& name)
  {
      printf("Usage: %s [options]\n\n"
                  "Valid options:\n"
                  "\t--help\t\t\t-h\t\tThis help text\n"
                  "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
                  "\t--verbose\t\t-v\t\tIncrease log verbosity (may be used multiple times)\n"
                  "\t--dimensions <wxh>\t-D <wxh>\tRequest dimensions from display driver (no guarantee) [%dx%d]\n"
                  "\t--depth <depth> \t-c <depth>\tSet output colour depth (automatic by default)\n"
                  "\t--input <input>\t\t-i <input>\tUse this input plugin [%s]\n"
                  "\t--actor <actor>\t\t-a <actor>\tUse this actor plugin [%s]\n"
                  "\t--seed <seed>\t\t-s <seed>\tSet random seed\n"
                  "\t--fps <n>\t\t-f <n>\t\tLimit output to n frames per second (if display driver supports it) [%d]\n"
                  "\t--framecount <n>\t-F <n>\t\tOutput n frames, then exit.\n"
                  "\t--switch <n>\t\t-S <n>\t\tSwitch actor after n frames.\n"
                  "\t--exclude <actors>\t-x <actors>\tProvide a list of actors to exclude.\n"
                  "\n",
                  name.c_str (),
                  width, height,
                  input_name.c_str (),
                  actor_name.c_str (),
                  frame_rate);
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
          {"input",       required_argument, 0, 'i'},
          {"actor",       required_argument, 0, 'a'},
          {"fps",         required_argument, 0, 'f'},
          {"seed",        required_argument, 0, 's'},
          {"exclude",     required_argument, 0, 'x'},
          {"framecount",  required_argument, 0, 'F'},
          {"switch",      required_argument, 0, 'S'},
          {"depth",       required_argument, 0, 'c'},
          {0,             0,                 0,  0 }
      };

      int index, argument;

      while ((argument = getopt_long(argc, argv, "hpvD:i:a:f:s:F:S:x:c:", loptions, &index)) >= 0) {

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
                  VisLogVerboseness level = visual_log_get_verboseness ();
                  level = VisLogVerboseness (int (level) + 1);
                  if (int (level) > VISUAL_LOG_VERBOSENESS_HIGH) {
                      visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_HIGH);
                  }
                  else {
                      visual_log_set_verboseness (level);
                  }
                  break;
              }

              // --dimensions
              case 'D': {
                  if (std::sscanf (optarg, "%ux%u", &width, &height) != 2)
                  {
                      std::cerr << "Invalid dimensions: '" << optarg << "'. Use <width>x<height> (e.g. 320x200)\n";
                      return -1;
                  }
                  break;
              }

              // --depth
              case 'c': {
                  if (std::sscanf (optarg, "%u", &color_depth) != 1 ||
                      visual_video_depth_enum_from_value(color_depth) == VISUAL_VIDEO_DEPTH_ERROR)
                  {
                      std::cerr << "Invalid depth: '" << optarg << "'. Use integer value (e.g. 24)\n";
                      return -1;
                  }
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

      // FIXME: this won't work if an actor's name is used as part of
      // another actor's name
      if (exclude_actors.find (new_name) != std::string::npos) {
          return cycle_actor_name (new_name, dir);
      }

      return new_name;
  }

} // anonymous namespace


int main (int argc, char **argv)
{
    try {
        // print warm welcome
        std::cerr << "lv-tool" << " - "
                  << "Libvisual " << PACKAGE_VERSION
                  << " commandline tool - "
                  << PACKAGE_URL << "\n";

        // setup signal handlers
        setup_signal_handlers ();

        // default loglevel
        visual_log_set_verboseness (VISUAL_LOG_VERBOSENESS_LOW);

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
            visual_random_context_set_seed(&__lv_internal_random_context, seed);
        }

        // create new VisBin for video output
        LV::Bin bin;
        bin.set_supported_depth(VISUAL_VIDEO_DEPTH_ALL);

        // Let the bin manage plugins. There's a bug otherwise.
        if (!bin.connect(actor_name, input_name)) {
            throw std::runtime_error ("Failed to start pipeline with actor '" + actor_name + "' and input '" + input_name + "'");
        }

        auto actor = bin.get_actor();

        // Select output colour depth

        VisVideoDepth depth;
        int depthflag = visual_actor_get_supported_depth(actor);

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
                depth = visual_video_depth_enum_from_value (color_depth);
            }
        }
        /* GL actor */
        else
        {
            depth = visual_video_depth_get_highest (depthflag);
        }

        bin.set_depth (depth);

        auto vidoptions = visual_actor_get_video_attribute_options(actor);

        // initialize display
        Display display;

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

        // get a queue to handle events
        VisEventQueue * const localqueue = visual_event_queue_new();
        visual_log_return_val_if_fail( localqueue != nullptr, EXIT_FAILURE );

        // rendering statistics
        uint64_t frames_drawn = 0;

        // frame rate control state
        uint64_t const frame_period_us = frame_rate > 0 ? VISUAL_USEC_PER_SEC / frame_rate : 0;
        LV::Time last_frame_time;
        bool draw_frame = true;

        // main loop
        bool running = true;
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

            VisEvent ev;

            // Handle all events
            display.drain_events(*localqueue);

            auto pluginqueue = visual_plugin_get_eventqueue (visual_actor_get_plugin(bin.get_actor()));

            while (visual_event_queue_poll(localqueue, &ev))
            {
                if(ev.type != VISUAL_EVENT_RESIZE)
                    visual_event_queue_add(pluginqueue, &ev);

                switch (ev.type)
                {
                    case VISUAL_EVENT_PARAM:
                    {
                        break;
                    }

                    case VISUAL_EVENT_RESIZE:
                    {
                        DisplayLock lock {display};

                        width = ev.event.resize.width;
                        height = ev.event.resize.height;

                        video = display.create(depth, vidoptions, width, height, true);
                        display.set_title(_("lv-tool"));

                        bin.set_video (video);
                        bin.sync(false);

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
