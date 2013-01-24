/* Libvisual - The audio visualisation framework cli tool
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Daniel Hiepler <daniel@niftylight.de>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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
#include "display/display.hpp"
#include "display/display_driver_factory.hpp"
#include <libvisual/libvisual.h>
#include <string>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

// Defaults
#define DEFAULT_ACTOR   "lv_analyzer"
#define DEFAULT_INPUT   "debug"
#define DEFAULT_MORPH   "slide_left"
#define DEFAULT_WIDTH   320
#define DEFAULT_HEIGHT  200
#define DEFAULT_FPS     30
#define DEFAULT_COLOR_DEPTH 0

#if HAVE_SDL
# define DEFAULT_DRIVER "sdl"
#else
# define DEFAULT_DRIVER "null"
#endif

namespace {

  std::string actor_name = DEFAULT_ACTOR;
  std::string input_name = DEFAULT_INPUT;
  std::string morph_name = DEFAULT_MORPH;
  std::string driver_name = DEFAULT_DRIVER;
  std::string exclude_actors;

  unsigned int width  = DEFAULT_WIDTH;
  unsigned int height = DEFAULT_HEIGHT;
  unsigned int color_depth = DEFAULT_COLOR_DEPTH;

  unsigned int frame_rate  = DEFAULT_FPS;
  unsigned int frame_count = 0;

  bool have_seed = 0;
  uint32_t seed = 0;

  /** print info about libvisual plugin */
  void print_plugin_info(VisPluginInfo const& info)
  {
      std::printf("Plugin: \"%s\" (%s)\n"
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
      std::printf("===== INPUTS =====\n");
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

      
      std::printf("===== ACTORS =====\n");
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
      
      
      std::printf("===== MORPHS =====\n");
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
  void print_help(std::string const& name)
  {
      std::printf("libvisual commandline tool - %s\n"
                  "Usage: %s [options]\n\n"
                  "Valid options:\n"
                  "\t--help\t\t\t-h\t\tThis help text\n"
                  "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
                  "\t--verbose\t\t-v\t\tIncrease verbosity (use multiple times for more effect)\n"
                  "\t--dimensions <wxh>\t-D <wxh>\tRequest dimensions from display driver (no guarantee) [%dx%d]\n"
                  "\t--depth <depth> \t-c <depth>\tSet output colour depth (automatic by default)\n"
                  "\t--driver <driver>\t-d <driver>\tUse this output driver [%s]\n"
                  "\t--input <input>\t\t-i <input>\tUse this input plugin [%s]\n"
                  "\t--actor <actor>\t\t-a <actor>\tUse this actor plugin [%s]\n"
                  "\t--morph <morph>\t\t-m <morph>\tUse this morph plugin [%s]\n"
                  "\t--seed <seed>\t\t-s <seed>\tSet random seed\n"
                  "\t--fps <n>\t\t-f <n>\t\tLimit output to n frames per second (if display driver supports it) [%d]\n"
                  "\t--framecount <n>\t-F <n>\t\tOutput n frames, then exit.\n"
                  "\t--exclude <actors>\t-x <actors>\tProvide a list of actors to exclude.\n"
                  "\n",
                  "http://libvisual.org",
                  name.c_str (),
                  width, height,
                  driver_name.c_str (),
                  input_name.c_str (),
                  actor_name.c_str (),
                  morph_name.c_str (),
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
		  {"driver",      required_argument, 0, 'd'},
		  {"input",       required_argument, 0, 'i'},
		  {"actor",       required_argument, 0, 'a'},
		  {"morph",       required_argument, 0, 'm'},
		  {"fps",         required_argument, 0, 'f'},
		  {"seed",        required_argument, 0, 's'},
          	  {"exclude",     required_argument, 0, 'x'},
		  {"framecount",  required_argument, 0, 'F'},
		  {"depth",       required_argument, 0, 'c'},
		  {0,             0,                 0,  0 }
	  };

      int index, argument;

      while ((argument = getopt_long(argc, argv, "hpvD:d:i:a:m:f:s:F:x:c:", loptions, &index)) >= 0) {

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
				  VisLogSeverity v = visual_log_get_verbosity();
				  v = (VisLogSeverity) ((int) v-1);
				  
				  if(v <= VISUAL_LOG_MIN)
					  break;
				  
                  visual_log_set_verbosity(v);
                  break;
              }

              // --dimensions
              case 'D': {
                  if (std::sscanf (optarg, "%dx%d", &width, &height) != 2)
                  {
                      std::cerr << "Invalid dimensions: '" << optarg << "'. Use <width>x<height> (e.g. 320x200)\n";
                      return -1;
                  }
                  break;
              }

              // --depth
              case 'c': {
                  if (std::sscanf (optarg, "%d", &color_depth) != 1 ||
                      visual_video_depth_enum_from_value(color_depth) == VISUAL_VIDEO_DEPTH_NONE)
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
                  std::sscanf(optarg, "%d", &frame_rate);
                  break;
              }

              // --seed
              case 's': {
                  have_seed = true;
                  std::sscanf(optarg, "%u", &seed);
                  break;
              }

              // --framecount
              case 'F': {
                  // set framecount
                  std::sscanf(optarg, "%d", &frame_count);
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

  void v_cycleActor (int prev)
  {
      auto name = prev ? visual_actor_get_prev_by_name(actor_name.c_str())
                       : visual_actor_get_next_by_name(actor_name.c_str());

      if (!name) {
          name = prev ? visual_actor_get_prev_by_name(0)
                      : visual_actor_get_next_by_name(0);
      }

      actor_name = name;

      if (std::strstr (exclude_actors.c_str(), name) != 0)
          v_cycleActor(prev);
  }

#if 0
  void v_cycleMorph ()
  {
      auto name = visual_morph_get_next_by_name(morph_name.c_str());

      if(!name) {
          name = visual_morph_get_next_by_name(0);
      }

      morph_name = name;
  }
#endif

} // anonymous namespace


int main (int argc, char **argv)
{
    // print warm welcome
    std::cerr << argv[0] << " v0.1\n";

    // default loglevel
    visual_log_set_verbosity (VISUAL_LOG_ERROR);
	
	// initialize libvisual once (this is meant to be called only once,
    // visual_init() after visual_quit() results in undefined state)
	LV::System::init (argc, argv);

    try {
        // parse commandline arguments
        int parseRes = parse_args(argc, argv);
        if (parseRes < 0)
            throw std::runtime_error ("Failed to parse arguments");
        else if (parseRes > 0)
            throw std::runtime_error ("");

        // create new VisBin for video output
        LV::Bin bin;
        bin.set_supported_depth(VISUAL_VIDEO_DEPTH_ALL);
        bin.switch_set_style(VISUAL_SWITCH_STYLE_DIRECT);

        // Let the bin manage plugins. There's a bug otherwise.
        bin.connect(actor_name, input_name);

        // initialize actor plugin
        std::cerr << "Loading actor '" << actor_name << "'...\n";
        auto actor = bin.get_actor();
        if (!actor)
            throw std::runtime_error ("Failed to load actor '" + actor_name + "'");

        // Set random seed
        if (have_seed) {
            auto  plugin_data = visual_actor_get_plugin(actor);
            auto& r_context   = *visual_plugin_get_random_context (plugin_data);

            r_context.set_seed (seed);
            seed++;
        }

        // initialize input plugin
        std::cerr << "Loading input '" << input_name << "'...\n";
        auto input = bin.get_input();
        if (!input) {
            throw std::runtime_error ("Failed to load input '" + input_name + "'");
        }

        // Select output colour depth

        VisVideoDepth depth;

        if (color_depth == 0) {
            // Pick the best display depth directly supported by actor

            int depthflag = visual_actor_get_supported_depth (actor);

            if (depthflag == VISUAL_VIDEO_DEPTH_GL) {
                depth = visual_video_depth_get_highest (depthflag);
            }
            else {
                depth = visual_video_depth_get_highest_nogl (depthflag);
            }
        } else {
            depth = visual_video_depth_enum_from_value (color_depth);
        }

        bin.set_depth (depth);

        auto vidoptions = visual_actor_get_video_attribute_options(actor);

        // initialize display
        Display display (driver_name);

        // create display
        auto video = display.create(depth, vidoptions, width, height, true);
        if(!video)
            throw std::runtime_error("Failed to get VisVideo from display");

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

        // main loop
        bool running = true;
        bool visible = true;

        while (running)
        {
            LV::Event ev;

            // Handle all events
            display.drain_events(localqueue);

            auto pluginqueue = visual_plugin_get_eventqueue(visual_actor_get_plugin (bin.get_actor()));

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
                        display.lock();
                        width = ev.event.resize.width;
                        height = ev.event.resize.height;
                        video = display.create(depth, vidoptions, width, height, true);

                        bin.set_video (video);
                        bin.sync(false);

                        display.unlock();

                        break;
                    }

                    case VISUAL_EVENT_MOUSEMOTION:
                    {
                        break;
                    }

                    case VISUAL_EVENT_MOUSEBUTTONDOWN:
                    {
                        // switch to next actor
                        v_cycleActor(1);

                        std::cerr << "Switching to actor '" << actor_name << "'...\n";
                        bin.switch_actor(actor_name);

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
                        running = FALSE;
                        break;
                    }

                    case VISUAL_EVENT_VISIBILITY:
                    {
                        visible = ev.event.visibility.is_visible;
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
                display.lock();
                int depthflag = bin.get_depth();
                VisVideoDepth depth = visual_video_depth_get_highest(depthflag);

                display.create(depth, vidoptions, width, height, true);

                video = display.get_video();
                bin.set_video(video);

                bin.sync(true);

                display.unlock();
            }

            // Do a run cycle
            if (!visible)
                continue;

            display.lock();

            bin.run();

            // All frames rendered?
            frames_drawn++;
            if (frame_count > 0 && frames_drawn >= frame_count)
                running = false;

            display.update_all();
            display.unlock();
        }

        /* Cleanup */
        //visual_plugin_unload(visual_actor_get_plugin(actor));
        //visual_plugin_unload(visual_input_get_plugin(input));

    }
    catch (std::exception& error) {
        std::cerr << error.what () << std::endl;
    }

    visual_quit ();

    return EXIT_SUCCESS;
}
