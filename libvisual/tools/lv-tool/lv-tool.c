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
#include "display/display.h"
#include <libvisual/libvisual.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

/* defaults */
#define DEFAULT_ACTOR   "lv_analyzer"
#define DEFAULT_INPUT   "debug"
#define DEFAULT_MORPH   "slide_left"
#define DEFAULT_WIDTH   320
#define DEFAULT_HEIGHT  200
#define DEFAULT_FPS     30


/* local variables */
static char actor_name[128];
static char input_name[128];
static char morph_name[128];
static int  width;
static int  height;
static int  framerate;
static int  driver;
static int  have_seed;
static uint32_t seed;

/* list of available driver-creators - register new drivers here */
typedef struct
{
        const char name[64];
        SADisplayDriver *(*creator)(void);
}SADisplayDriverDescription;

SADisplayDriverDescription all_display_drivers[] =
{
#ifdef HAVE_SDL
        { .name = "sdl",      .creator = &sdl_driver_new      },
#endif
#ifdef HAVE_GLX
        { .name = "glx",      .creator = &glx_driver_new      },
#endif
        { .name = "stdout",   .creator = &stdout_driver_new   },
};




/******************************************************************************/

/** print info about libvisual plugin */
static void _print_plugin_info(VisPluginData *p)
{
   /** @todo: make this work
    VisPluginInfo *i = visual_plugin_get_info(p);
    printf(
        "Plugin: \"%s\" (%s)\n"
        "\tauthor:\t%s\n"
        "\tversion:\t%s\n"
        "\tlicense:\t%s\n"
        "%s\n"
        "\%s\n\n",
        i->name, i->plugname,
        i->author, i->version, i->license,
        i->about, i->help); */
}


/** print help for plugins */
static void _print_plugin_help()
{
    VisList *list;

    /* print inputs */


    /* print actors */
    if(!(list = visual_actor_get_list()))
    {
        fprintf(stderr, "No actors found\n");
    }
    else
    {
        int n = 0;
        VisActor *a;
        while((a = visual_list_get(list, n++)))
        {
            VisPluginData *p = visual_actor_get_plugin(a);
            _print_plugin_info(p);
        }
    }

    /* print morphs */
}

/** print commandline help */
static void _print_help(char *name)
{
    printf("libvisual commandline tool - %s\n"
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
           all_display_drivers[0].name,
           input_name,
           actor_name,
           morph_name,
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
                    if(sscanf(optarg, "%dx%d", &width, &height) != 2)
                    {
                        fprintf(stderr,
                                "Invalid dimensions: \"%s\". Use <width>x<height> (e.g. 320x200)\n", optarg);
                        return EXIT_FAILURE;
                    }
                    break;
            }

            /* --driver */
            case 'd':
            {
                int n;
                for(n = 0;
                    n < sizeof(all_display_drivers)/sizeof(SADisplayDriverDescription);
                    n++)
                {
                    /* is this our driver? */
                    if(strcmp(optarg, all_display_drivers[n].name) == 0)
                    {
                        /* stop search */
                        break;
                    }
                }

                /* found something? */
                if(n < sizeof(all_display_drivers)/sizeof(SADisplayDriverDescription))
                {
                    driver = n;
                    break;
                }

                fprintf(stderr, "Unsupported display driver: %s\n", optarg);
                return EXIT_FAILURE;
            }

            /* --input */
            case 'i':
            {
                /* save name for later */
                strncpy(input_name, optarg, sizeof(input_name)-1);
                break;
            }

            /* --actor */
            case 'a':
            {
                /* save name for later */
                strncpy(actor_name, optarg, sizeof(actor_name)-1);
                break;
            }

            /* --morph */
            case 'm':
            {
                /* save filename for later */
                strncpy(morph_name, optarg, sizeof(morph_name)-1);
                break;
            }

            /* --fps */
            case 'f':
            {
                /* set framerate */
                sscanf(optarg, "%d", &framerate);
                break;
            }

            /* --seed */
            case 's':
            {
                 have_seed = 1;
                 sscanf(optarg, "%u", &seed);
				 break;
            }

            /* invalid argument */
            case '?':
            {
                fprintf(stderr, "argument %d is invalid\n", index);
                _print_help(argv[0]);
                return EXIT_FAILURE;
            }

            /* unhandled arguments */
            default:
            {
                fprintf(stderr, "argument %d is invalid\n", index);
                break;
            }
        }
    }


    return EXIT_SUCCESS;
}

static void v_cycleActor (int prev)
{
    const char *name;
    name = (prev ? visual_actor_get_prev_by_name ((char *)actor_name)
                     : visual_actor_get_next_by_name ((char *)actor_name));
    if (name == NULL) {
        name = (prev ? visual_actor_get_prev_by_name (0)
                         : visual_actor_get_next_by_name (0));
    }
    memset(actor_name, 0, sizeof(actor_name));
    memcpy(actor_name, name, strlen(name));
}

static void v_cycleMorph ()
{
    const char *name;
    name = visual_morph_get_next_by_name((char *)morph_name);
    if(name == NULL) {
        name = visual_morph_get_next_by_name(0);
    }
    memset(morph_name, 0, sizeof(morph_name));
    memcpy(morph_name, name, strlen(name));
}

/******************************************************************************
 ******************************************************************************
 ******************************************************************************/
int main (int argc, char **argv)
{
        int depthflag;
        VisVideoDepth depth;

        /* set defaults */
        width = DEFAULT_WIDTH;
        height = DEFAULT_HEIGHT;
        driver = 0;
        strncpy(actor_name, DEFAULT_ACTOR, sizeof(actor_name)-1);
        strncpy(input_name, DEFAULT_INPUT, sizeof(input_name)-1);
        strncpy(morph_name, DEFAULT_MORPH, sizeof(morph_name)-1);
        framerate = DEFAULT_FPS;

        /* print warm welcome */
        fprintf(stderr, "%s v0.1\n", argv[0]);

        /**
         * initialize libvisual once (this is meant to be called only once,
         * visual_init() after visual_quit() results in undefined state)
         */
        visual_log_set_verbosity(VISUAL_LOG_DEBUG);
        visual_init (&argc, &argv);

        /* parse commandline arguments */
        if(_parse_args(argc, argv) != EXIT_SUCCESS)
                goto _m_exit;

        /* create new VisBin for video output */
        VisBin *bin;
        bin = visual_bin_new();
        visual_bin_set_supported_depth(bin, VISUAL_VIDEO_DEPTH_ALL);
        visual_bin_switch_set_style(bin, VISUAL_SWITCH_STYLE_MORPH);

        /* initialize actor plugin */
        fprintf(stderr, "Loading actor \"%s\"...\n", actor_name);
        VisActor *actor;
        if(!(actor = visual_actor_new(actor_name)))
        {
                fprintf(stderr, "Failed to load actor \"%s\"\n", actor_name);
                goto _m_exit;
        }

        /* Set random seed */
        if (have_seed) {
                VisPluginData    *plugin_data = visual_actor_get_plugin(actor);
                VisRandomContext *r_context   = visual_plugin_get_random_context (plugin_data);

                visual_random_context_set_seed (r_context, seed);
                seed++;
        }

        /* initialize input plugin */
        fprintf(stderr, "Loading input \"%s\"...\n", input_name);
        VisInput *input;
        if(!(input = visual_input_new(input_name)))
        {
                fprintf(stderr, "Failed to load input \"%s\"\n", input_name);
                goto _m_exit;
        }

        /* handle depth? */
        if((depthflag = visual_actor_get_supported_depth(actor))
           == VISUAL_VIDEO_DEPTH_GL)
        {
                depth = visual_video_depth_get_highest(depthflag);
                visual_bin_set_depth(bin, VISUAL_VIDEO_DEPTH_GL);
        }
        else
        {
                depth = visual_video_depth_get_highest_nogl(depthflag);
                if((bin->depthflag & depth) > 0)
                {
                        visual_bin_set_depth(bin, depth);
                }
                else
                {
                        visual_bin_set_depth(bin,
                                             visual_video_depth_get_highest_nogl(
                                                                                 bin->depthflag));
                }
        }

        bin->depthforcedmain = bin->depth;

        VisVideoAttributeOptions *vidoptions;
        vidoptions = visual_actor_get_video_attribute_options(actor);


        /* initialize display */
        SADisplay *display;
        if(!(display = display_new(all_display_drivers[driver].creator())))
        {
                fprintf(stderr, "Failed to initialize display.\n");
                goto _m_exit;
        }

        /* create display */
        display_create(display, depth, vidoptions, width, height, TRUE);
        VisVideo *video;
        if(!(video = display_get_video(display)))
        {
                fprintf(stderr, "Failed to get VisVideo from display.\n");
                goto _m_exit_display;
        }

        /* put it all together */
        visual_bin_connect(bin, actor, input);
        visual_bin_set_video(bin, video);
        visual_bin_realize(bin);
        visual_bin_sync(bin, FALSE);
        visual_bin_depth_changed(bin);

        /* get a queue to handle events */
        VisEventQueue *localqueue;
        localqueue = visual_event_queue_new ();


        /* main loop */
        int running = TRUE;
        int visible = TRUE;
        while (running)
        {
                VisEventQueue *pluginqueue;
                VisEvent *ev;

                /* Handle all events */
                display_drain_events(display, localqueue);

                pluginqueue = visual_plugin_get_eventqueue(visual_actor_get_plugin (bin->actor));
                while(visual_event_queue_poll_by_reference(localqueue, &ev))
                {

                        if(ev->type != VISUAL_EVENT_RESIZE)
                                visual_event_queue_add (pluginqueue, ev);

                        switch (ev->type)
                        {
                                case VISUAL_EVENT_PARAM:
                                {
                                        break;
                                }

                                case VISUAL_EVENT_RESIZE:
                                {
                                        display_lock(display);
                                        width = ev->event.resize.width;
                                        height = ev->event.resize.height;
                                        display_create(display, depth, vidoptions, width, height, TRUE);
                                        video = display_get_video (display);

                                        visual_bin_set_video (bin, video);
                                        visual_actor_video_negotiate (bin->actor, depth, FALSE, FALSE);

                                        display_unlock(display);
                                        break;
                                }

                                case VISUAL_EVENT_MOUSEMOTION:
                                {
                                        break;
                                }

                                case VISUAL_EVENT_MOUSEBUTTONDOWN:
                                {
                                        /* switch to next actor */
                                        v_cycleActor(1);
                                        v_cycleMorph();

                                        visual_bin_set_morph_by_name(bin, morph_name);
                                        visual_bin_switch_actor_by_name(bin, actor_name);

                                        /* get new actor */
                                        actor = visual_bin_get_actor(bin);

                                        /* handle depth of new actor */
                                        depthflag = visual_actor_get_supported_depth(actor);
                                        if(depthflag == VISUAL_VIDEO_DEPTH_GL)
                                                visual_bin_set_depth(bin, VISUAL_VIDEO_DEPTH_GL);
                                        else
                                        {
                                                depth = visual_video_depth_get_highest(depthflag);
                                                if((bin->depthflag & depth) > 0)
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
                                        switch(ev->event.keyboard.keysym.sym)
                                        {
                                                case VKEY_ESCAPE:
                                                {
                                                        running = FALSE;
                                                        break;
                                                }

                                                case VKEY_TAB:
                                                {
                                                        break;
                                                }

                                                default:
                                                {
                                                        fprintf(stderr, "keypress: %c\n", ev->event.keyboard.keysym.sym);
                                                        break;
                                                }
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
                                        visible = ev->event.visibility.is_visible;
                                        break;
                                }

                                default:
                                {
                                        break;
                                }
                        }
                }

                if(visual_bin_depth_changed(bin))
                {
                    display_lock(display);
                    display_create(display, depth, vidoptions, width, height, TRUE);
                    VisVideo *video = display_get_video(display);
                    visual_bin_set_video(bin, video);
                    visual_bin_sync(bin, TRUE);
                    display_unlock(display);
                }


                /* Do a run cycle */
                if(!visible)
                        continue;

                display_lock(display);
                visual_bin_run(bin);
                display_unlock(display);
                display_update_all(display);
                display_fps_limit(display, framerate);
        }


_m_exit_display:
                /* cleanup display stuff */
                display_set_fullscreen(display, FALSE, TRUE);
                display_close(display);

_m_exit:
                /* cleanup resources allocated by visual_init() */
                visual_quit ();

                //printf ("Total frames: %d, average fps: %f\n", display_fps_total (display), display_fps_average (display));
                return EXIT_SUCCESS;
}
