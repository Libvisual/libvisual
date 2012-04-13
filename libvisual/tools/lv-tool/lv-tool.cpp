/* Libvisual - The audio visualisation framework cli tool
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>,
 * Copyright (C) 2012 Daniel Hiepler <daniel@niftylight.de>
         *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Daniel Hiepler <daniel@niftylight.de>
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
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <getopt.h>

/* defaults */
#define DEFAULT_ACTOR   "lv_analyzer"
#define DEFAULT_INPUT   "debug"
#define DEFAULT_MORPH   "slide_left"
#define DEFAULT_WIDTH   320
#define DEFAULT_HEIGHT  200
#define DEFAULT_FPS     30

#if HAVE_SDL
# define DEFAULT_DRIVER "sdl"
#else
# define DEFAULT_DRIVER "stdout"
#endif

/* local variables */
namespace {

  std::string actor_name = DEFAULT_ACTOR;
  std::string input_name = DEFAULT_INPUT;
  std::string morph_name = DEFAULT_MORPH;
  std::string driver_name = DEFAULT_DRIVER;
  int width  = DEFAULT_WIDTH;
  int height = DEFAULT_HEIGHT;
  int framerate = DEFAULT_FPS;
  int have_seed = 0;
  uint32_t seed = 0;

} // anonymous namespace

/******************************************************************************/

/** print info about libvisual plugin */
static void _print_plugin_info(VisPluginInfo const& info)
{
    std::printf(
        "Plugin: \"%s\" (%s)\n"
        "\tauthor:\t%s\n"
        "\tversion:\t%s\n"
        "\tlicense:\t%s\n"
        "%s\n"
        "%s\n\n",
        info.name, info.plugname,
        info.author, info.version, info.license,
        info.about, info.help);
}


/** print help for plugins */
static void _print_plugin_help()
{
    LV::PluginList const& list =
        LV::PluginRegistry::instance()->get_plugins_by_type (VISUAL_PLUGIN_TYPE_ACTOR);

    /* print actors */
    if(!list.empty())
    {
        for (unsigned int i = 0; i < list.size (); i++)
        {
            _print_plugin_info(*list[i]->info);
        }
    }
    else
    {
        std::cerr << "No actors found\n";
    }

    /* print morphs */
}

/** print commandline help */
static void _print_help(const char *name)
{
    std::printf("libvisual commandline tool - %s\n"
                "Usage: %s [options]\n\n"
                "Valid options:\n"
                "\t--help\t\t\t-h\t\tThis help text\n"
                "\t--plugin-help\t\t-p\t\tList of installed plugins + information\n"
                "\t--dimensions <wxh>\t-D <wxh>\tRequest dimensions from display driver (no guarantee) [%dx%d]\n"
                "\t--driver <driver>\t-d <driver>\tUse this output driver [%s]\n"
                "\t--input <input>\t\t-i <input>\tUse this input plugin [%s]\n"
                "\t--actor <actor>\t\t-a <actor>\tUse this actor plugin [%s]\n"
                "\t--morph <morph>\t\t-m <morph>\tUse this morph plugin [%s]\n"
                "\t--seed <seed>\t\t-s <seed>\tSet random seed\n"
                "\t--fps <n>\t\t-f <n>\t\tLimit output to n frames per second (if display driver supports it) [%d]\n\n",
                "http://github.com/StarVisuals/libvisual",
                name,
                width, height,
                driver_name.c_str (),
                input_name.c_str (),
                actor_name.c_str (),
                morph_name.c_str (),
                framerate);
}


/** parse commandline arguments */
static int _parse_args(int argc, char *argv[])
{
    int index, argument;

    static struct option loptions[] =
    {
        {"help",        no_argument,       0, 'h'},
        {"plugin-help", no_argument,       0, 'p'},
        {"dimensions",  required_argument, 0, 'D'},
        {"driver",      required_argument, 0, 'd'},
        {"input",       required_argument, 0, 'i'},
        {"actor",       required_argument, 0, 'a'},
        {"morph",       required_argument, 0, 'm'},
        {"fps",         required_argument, 0, 'f'},
        {"seed",        required_argument, 0, 's'},
        {0,             0,                 0,  0 }
    };

    while((argument = getopt_long(argc, argv, "hpD:d:i:a:m:f:s:", loptions, &index)) >= 0)
    {

        switch(argument)
        {
            /* --help */
            case 'h':
            {
                _print_help(argv[0]);
                return EXIT_FAILURE;
            }

            /* --plugin-help */
            case 'p':
            {
                _print_plugin_help();
                return EXIT_FAILURE;
            }

            /* --dimensions */
            case 'D':
            {
                if (std::sscanf (optarg, "%dx%d", &width, &height) != 2)
                {
                    std::cerr << "Invalid dimensions: '" << optarg << "'. Use <width>x<height> (e.g. 320x200)\n";
                    return EXIT_FAILURE;
                }
                break;
            }

            /* --driver */
            case 'd':
            {
                if (!DisplayDriverFactory::instance().has_driver (optarg))
                {
                    std::cerr << "Unsupported display driver: " << optarg << "\n";
                    return EXIT_FAILURE;
                }

                driver_name = optarg;
                break;
            }

            /* --input */
            case 'i':
            {
                /* save name for later */
                input_name = optarg;
                break;
            }

            /* --actor */
            case 'a':
            {
                /* save name for later */
                actor_name = optarg;
                break;
            }

            /* --morph */
            case 'm':
            {
                /* save filename for later */
                morph_name = optarg;
                break;
            }

            /* --fps */
            case 'f':
            {
                /* set framerate */
                std::sscanf(optarg, "%d", &framerate);
                break;
            }

            /* --seed */
            case 's':
            {
                 have_seed = 1;
                 std::sscanf(optarg, "%u", &seed);
                 break;
            }

            /* invalid argument */
            case '?':
            {
                _print_help(argv[0]);
                return EXIT_FAILURE;
            }

            /* unhandled arguments */
            default:
            {
                abort ();
            }
        }
    }


    return EXIT_SUCCESS;
}

static void v_cycleActor (int prev)
{
    const char *name;

    name = prev ? visual_actor_get_prev_by_name(actor_name.c_str())
                : visual_actor_get_next_by_name(actor_name.c_str());

    if (!name) {
        name = prev ? visual_actor_get_prev_by_name(0)
                    : visual_actor_get_next_by_name(0);
    }

    actor_name = name;
}

static void v_cycleMorph ()
{
    const char *name;

    name = visual_morph_get_next_by_name(morph_name.c_str());
    if(!name) {
        name = visual_morph_get_next_by_name(0);
    }

    morph_name = name;
}

/******************************************************************************
 ******************************************************************************
 ******************************************************************************/
int main (int argc, char **argv)
{
    // print warm welcome
    std::cerr << argv[0] << " v0.1\n";

    // initialize libvisual once (this is meant to be called only once,
    // visual_init() after visual_quit() results in undefined state)
    visual_log_set_verbosity(VISUAL_LOG_DEBUG);
    visual_init (&argc, &argv);

    try {
        // parse commandline arguments
        if (_parse_args(argc, argv) != EXIT_SUCCESS)
            throw std::runtime_error ("Failed to parse arguments");

        // create new VisBin for video output
        VisBin *bin = visual_bin_new();
        visual_bin_set_supported_depth(bin, VISUAL_VIDEO_DEPTH_ALL);
        visual_bin_switch_set_style(bin, VISUAL_SWITCH_STYLE_MORPH);

        // initialize actor plugin
        std::cerr << "Loading actor '" << actor_name << "'...\n";
        VisActor *actor = visual_actor_new (actor_name.c_str ());
        if (!actor)
            throw std::runtime_error ("Failed to load actor '" + actor_name + "'");

        // Set random seed
        if (have_seed) {
            VisPluginData    *plugin_data = visual_actor_get_plugin(actor);
            VisRandomContext *r_context   = visual_plugin_get_random_context (plugin_data);

            visual_random_context_set_seed (r_context, seed);
            seed++;
        }

        // initialize input plugin
        std::cerr << "Loading input '" << input_name << "'...\n";
        VisInput *input = visual_input_new(input_name.c_str());
        if (!input) {
            throw std::runtime_error ("Failed to load input '" + input_name + "'");
        }

        // Pick the best display depth

        int depthflag = visual_actor_get_supported_depth (actor);

        VisVideoDepth depth;

        if (depthflag == VISUAL_VIDEO_DEPTH_GL) {
            depth = visual_video_depth_get_highest (depthflag);
        }
        else {
            depth = visual_video_depth_get_highest_nogl (depthflag);
        }

        visual_bin_set_depth (bin, depth);

        VisVideoAttributeOptions const* vidoptions =
            visual_actor_get_video_attribute_options(actor);

        // initialize display
        SADisplay display (driver_name);

        // create display
        display.create(depth, vidoptions, width, height, true);

        VisVideo *video = display.get_video();
        if(!video)
            throw std::runtime_error("Failed to get VisVideo from display");

        // put it all together
        visual_bin_connect(bin, actor, input);
        visual_bin_set_video(bin, video);
        visual_bin_realize(bin);
        visual_bin_sync(bin, FALSE);
        visual_bin_depth_changed(bin);

        // get a queue to handle events
        VisEventQueue localqueue;

        // main loop
        bool running = true;
        bool visible = true;

        while (running)
        {
            LV::Event ev;

            // Handle all events
            display.drain_events(localqueue);

            LV::EventQueue* pluginqueue = visual_plugin_get_eventqueue(visual_actor_get_plugin (bin->actor));

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
                        display.create(depth, vidoptions, width, height, true);
                        video = display.get_video ();

                        visual_bin_set_video (bin, video);
                        visual_actor_video_negotiate (bin->actor, depth, FALSE, FALSE);

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
                        v_cycleMorph();

                        visual_bin_set_morph_by_name(bin, morph_name.c_str());
                        visual_bin_switch_actor_by_name(bin, actor_name.c_str());

                        // get new actor
                        actor = visual_bin_get_actor(bin);

                        // handle depth of new actor
                        depthflag = visual_actor_get_supported_depth(actor);
                        if (depthflag == VISUAL_VIDEO_DEPTH_GL)
                        {
                            visual_bin_set_depth(bin, VISUAL_VIDEO_DEPTH_GL);
                        }
                        else
                        {
                            depth = visual_video_depth_get_highest(depthflag);
                            if ((bin->depthflag & depth) > 0)
                                visual_bin_set_depth(bin, depth);
                            else
                                visual_bin_set_depth(bin, visual_video_depth_get_highest_nogl(bin->depthflag));
                        }
                        bin->depthforcedmain = bin->depth;
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

            if (visual_bin_depth_changed(bin))
            {
                display.lock();
                display.create(depth, vidoptions, width, height, true);
                VisVideo *video = display.get_video();
                visual_bin_set_video(bin, video);
                visual_bin_sync(bin, TRUE);
                display.unlock();
            }

            // Do a run cycle
            if (!visible)
                continue;

            display.lock();
            visual_bin_run(bin);
            display.unlock();
            display.update_all();
            display.set_fps_limit(framerate);
        }
    }
    catch (std::exception& error) {
        std::cerr << error.what () << std::endl;
    }

    //printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));

    visual_quit ();

    return EXIT_SUCCESS;
}
