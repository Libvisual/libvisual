/***************************************************************************
 *   Based on work by Max Howell <max.howell@methylblue.com>               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <sys/types.h>  //this must be _before_ sys/socket on freebsd
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libvisual/libvisual.h>
#include <SDL.h>

#define ACTOR "lv_analyzer"
#define INPUT "pulseaudio"
#define MORPH "alphablend"

#define x_exit(msg) \
    printf ("Error: %s\n", msg); \
    exit (EXIT_FAILURE);

/* SDL */
SDL_Surface *screen = 0;
SDL_Color    pal[256];

static void sdl_init(void);
static int sdl_event_handler(void);
static void sdl_quit(void);

static inline void sdl_lock(void) { if( SDL_MUSTLOCK( screen ) == SDL_TRUE ) SDL_LockSurface( screen ); }
static inline void sdl_unlock(void) { if( SDL_MUSTLOCK( screen ) == SDL_TRUE ) SDL_UnlockSurface( screen ); }

static inline int
sdl_isFullScreen(void)
{
    return (screen->flags & SDL_FULLSCREEN) > 0;
}

static inline void
sdl_toggleFullScreen(void)
{
    SDL_WM_ToggleFullScreen( screen );
    SDL_ShowCursor( (screen->flags & SDL_FULLSCREEN) > 0 ? SDL_DISABLE : SDL_ENABLE );
}

/* LIBVISUAL */
struct {
    VisVideo   *video;
    VisPalette *pal;
    VisBin     *bin;
    const char *plugin;
    const char *input;
    const char *morph;
    int         pluginIsGL;
} v;

static void v_init (int, char**);
static uint v_render (void);
static void v_resize (int, int);

static void
v_cycleActor (int prev)
{
    v.plugin = (prev ? visual_actor_get_prev_by_name (v.plugin)
                     : visual_actor_get_next_by_name (v.plugin));
    if (!v.plugin) {
        v.plugin = (prev ? visual_actor_get_prev_by_name (0)
                         : visual_actor_get_next_by_name (0));
    }
    if (!strcmp (v.plugin, "gstreamer") || !strcmp (v.plugin, "gdkpixbuf")) {
        v_cycleActor (prev);
    }
}

int
main (int argc, char** argv)
{
    puts ("Controls: Arrow keys switch between plugins, TAB toggles fullscreen, ESC quits.");
    puts ("          Each plugin can has its own mouse/key bindings, too.");
    if (argc > 1) {
        v.plugin = argv[1];
    } else {
        puts ("Note: you can give your favourite libvisual plugin as command line argument.");
    }
    v.pluginIsGL = 0;

    //init
    sdl_init ();
    v_init (argc, argv);

    //main loop
    uint render_time = 10;
    int samples = 0;

    while (sdl_event_handler()) {
        render_time = v_render();
    }

    return EXIT_SUCCESS;
}

void
sdl_init (void)
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        x_exit (SDL_GetError());
    }
    atexit (sdl_quit);
}

void
sdl_quit (void)
{
    //FIXME crashes!
    //visual_bin_destroy( v.bin );
    //visual_quit();

    SDL_FreeSurface (screen);
    SDL_Quit();
}

static inline void
sdl_set_pal(void)
{
    if (v.pal) {
        int i;
        for (i = 0; i < 256; i++) {
            pal[i].r = v.pal->colors[i].r;
            pal[i].g = v.pal->colors[i].g;
            pal[i].b = v.pal->colors[i].b;
        }
    }
    SDL_SetColors( screen, pal, 0, 256 );
}

static void
sdl_create (int width, int height) {
    SDL_FreeSurface (screen);

    if (v.pluginIsGL)
    {
        const SDL_VideoInfo *videoinfo = SDL_GetVideoInfo();
        int videoflags;

        if (!videoinfo) {
            x_exit ("Could not get video info.");
        }

        videoflags  = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE | SDL_RESIZABLE;
        videoflags |= videoinfo->hw_available ? SDL_HWSURFACE : SDL_SWSURFACE;
        if (videoinfo->blit_hw) videoflags |= SDL_HWACCEL;

        SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
        screen = SDL_SetVideoMode (width, height, 16, videoflags);
    } else {
        screen = SDL_SetVideoMode (width, height, v.video->bpp * 8, SDL_RESIZABLE);
    }
    visual_video_set_buffer (v.video, screen->pixels);
    visual_video_set_pitch (v.video, screen->pitch);
}

int
sdl_event_handler(void)
{
    SDL_Event event;
    VisEventQueue *vevent;

    while (SDL_PollEvent (&event)) {
        vevent = visual_plugin_get_eventqueue (visual_actor_get_plugin (visual_bin_get_actor (v.bin)));

        switch (event.type) {
        case SDL_KEYUP:
            visual_event_queue_add_keyboard (vevent, (VisKey)event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_UP);
            break;

        case SDL_KEYDOWN:
            visual_event_queue_add_keyboard (vevent, (VisKey)event.key.keysym.sym, event.key.keysym.mod, VISUAL_KEY_DOWN);

            switch (event.key.keysym.sym) {
            //PLUGIN CONTROLS
            case SDLK_F11:
            case SDLK_TAB:
                sdl_toggleFullScreen ();
                break;

            case SDLK_ESCAPE:
                if (sdl_isFullScreen ()) {
                    sdl_toggleFullScreen ();
                } else {
                    return 0;
                }
                break;

            case SDLK_LEFT:
                v_cycleActor (1);
                goto morph;

            case SDLK_RIGHT:
                v_cycleActor (0);

            morph:
                sdl_lock();
                  visual_bin_set_morph_by_name (v.bin, "alphablend");
                  visual_bin_switch_actor_by_name (v.bin, (char*)v.plugin);
                sdl_unlock();

                SDL_WM_SetCaption (v.plugin, 0);

                break;

            default:
                ;
            }
            break;

        case SDL_VIDEORESIZE:
            v_resize (event.resize.w, event.resize.h);
            break;

        case SDL_MOUSEMOTION:
            visual_event_queue_add_mousemotion (vevent, event.motion.x, event.motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN:
            /*if (event.button.button == SDL_BUTTON_RIGHT)
            {
                sdl_toggleFullScreen();
                break;
            }*/
            visual_event_queue_add_mousebutton (vevent, event.button.button, VISUAL_MOUSE_DOWN, 0, 0);
            break;

        case SDL_MOUSEBUTTONUP:
            visual_event_queue_add_mousebutton (vevent, event.button.button, VISUAL_MOUSE_UP, 0, 0);
            break;

        case SDL_QUIT:
            return 0;

        default:
            ;
        }
    }

    return 1;
}

void
v_resize( int width, int height )
{
    visual_video_set_dimension( v.video, width, height );

    sdl_create( width, height );

    visual_bin_sync( v.bin, 0 );
}

void
v_init (int argc, char **argv)
{
    VisVideoDepth depth;

    visual_init (&argc, &argv);

    v.bin    = visual_bin_new ();
    depth  = visual_video_depth_enum_from_value( 24 );

    visual_bin_switch_set_style (v.bin, VISUAL_SWITCH_STYLE_MORPH);
    visual_bin_switch_set_steps (v.bin, 10);

/*
    if (v.plugin == NULL) {
        puts ("Available plugins:");
        while ((v.plugin = visual_actor_get_next_by_name (v.plugin))) {
            printf (" * %s\n", v.plugin);
        }
        v.plugin = visual_actor_get_next_by_name (0);
    }
*/

    v.plugin = ACTOR;
    v.input = INPUT;
    v.morph = MORPH;

    visual_bin_set_supported_depth (v.bin, VISUAL_VIDEO_DEPTH_ALL);

    if (!(v.video = visual_video_new ())) {
        x_exit ("Cannot create a video surface");
    }
    if (visual_video_set_depth (v.video, depth) < 0) {
        x_exit ("Cannot set video depth");
    }

    visual_video_set_dimension (v.video, 640, 480);

    if (visual_bin_set_video (v.bin, v.video)) {
        x_exit ("Cannot set video");
    }

    visual_bin_connect_by_names (v.bin, (char*)v.plugin, (char *)v.input);

    if (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL)
    {
        visual_video_set_depth (v.video, VISUAL_VIDEO_DEPTH_GL);
        v.pluginIsGL = 1;
    }

    sdl_create (640, 480);

    SDL_WM_SetCaption (v.plugin, 0);

    /* Called so the flag is set to false, seen we create the initial environment here */
    visual_bin_depth_changed (v.bin);


    visual_bin_realize (v.bin);
    visual_bin_sync (v.bin, 0);

}

uint
v_render(void)
{
    /* On depth change */
    if (visual_bin_depth_changed (v.bin)) {
        sdl_lock ();

        v.pluginIsGL = (visual_bin_get_depth (v.bin) == VISUAL_VIDEO_DEPTH_GL);

        sdl_create (screen->w, screen->h);
        visual_bin_sync (v.bin, 1);

        sdl_unlock ();
    }

    long ticks = -SDL_GetTicks ();

    if (v.pluginIsGL) {
        visual_bin_run (v.bin);
        SDL_GL_SwapBuffers ();
    } else {
        sdl_lock ();
        visual_video_set_buffer (v.video, screen->pixels);
        visual_bin_run (v.bin);
        sdl_unlock ();

        v.pal = visual_bin_get_palette (v.bin);
        sdl_set_pal ();
        SDL_Flip (screen);
    }

    ticks += SDL_GetTicks ();
    return ticks;
}
