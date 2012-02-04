/* When using the sdl driver and Xgl, sdl will show a nice empty window. Be sure to set the:
 * XLIB_SKIP_ARGB_VISUALS environment variable to 1
 */

#include <libvisual/libvisual.h>

#include <stdlib.h>
#include <SDL.h>

#define SCREEN_H 200
#define SCREEN_W 500

static SDL_GLattr __lv_sdl_gl_attribute_map[] = {
    [VISUAL_GL_ATTRIBUTE_NONE]      = -1,
    [VISUAL_GL_ATTRIBUTE_BUFFER_SIZE]   = SDL_GL_BUFFER_SIZE,
    [VISUAL_GL_ATTRIBUTE_LEVEL]     = -1,
    [VISUAL_GL_ATTRIBUTE_RGBA]      = -1,
    [VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER]  = SDL_GL_DOUBLEBUFFER,
    [VISUAL_GL_ATTRIBUTE_STEREO]        = SDL_GL_STEREO,
    [VISUAL_GL_ATTRIBUTE_AUX_BUFFERS]   = -1,
    [VISUAL_GL_ATTRIBUTE_RED_SIZE]      = SDL_GL_RED_SIZE,
    [VISUAL_GL_ATTRIBUTE_GREEN_SIZE]    = SDL_GL_GREEN_SIZE,
    [VISUAL_GL_ATTRIBUTE_BLUE_SIZE]     = SDL_GL_BLUE_SIZE,
    [VISUAL_GL_ATTRIBUTE_ALPHA_SIZE]    = SDL_GL_ALPHA_SIZE,
    [VISUAL_GL_ATTRIBUTE_DEPTH_SIZE]    = SDL_GL_DEPTH_SIZE,
    [VISUAL_GL_ATTRIBUTE_STENCIL_SIZE]  = SDL_GL_STENCIL_SIZE,
    [VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE]    = SDL_GL_ACCUM_RED_SIZE,
    [VISUAL_GL_ATTRIBUTE_ACCUM_GREEN_SIZE]  = SDL_GL_ACCUM_GREEN_SIZE,
    [VISUAL_GL_ATTRIBUTE_ACCUM_BLUE_SIZE]   = SDL_GL_ACCUM_BLUE_SIZE,
    [VISUAL_GL_ATTRIBUTE_ACCUM_ALPHA_SIZE]  = SDL_GL_ACCUM_ALPHA_SIZE,
    [VISUAL_GL_ATTRIBUTE_LAST]      = -1
};

void lock(SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface))
        SDL_LockSurface(surface);
}

void unlock(SDL_Surface *surface)
{
    if(SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
}

SDL_Surface *create(SDL_Surface *surface, VisVideoDepth depth, VisVideoAttributeOptions *vidoptions, int width, int height, int resizable)
{
    const SDL_VideoInfo *videoinfo;
    int videoflags = 0;
    int bpp;   
    int i;
    if(surface != NULL)
        SDL_FreeSurface(surface);
    videoflags |= resizable ? SDL_RESIZABLE : 0;
   
    if(depth == VISUAL_VIDEO_DEPTH_GL)
    {
        videoinfo  = SDL_GetVideoInfo();
        if(!videoinfo)
            return NULL;
        videoflags |= SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;

        if(videoinfo->hw_available)
            videoflags |= SDL_HWSURFACE;
        else
            videoflags |= SDL_SWSURFACE;

        if(videoinfo->blit_hw)
            videoflags |= SDL_HWACCEL;

        if(vidoptions != NULL)
        {
            for(i = VISUAL_GL_ATTRIBUTE_NONE; i < VISUAL_GL_ATTRIBUTE_LAST; i++)
            {
                if(vidoptions->gl_attributes[i].mutated == TRUE)
                {
                    SDL_GLattr sdl_attribute =
                        __lv_sdl_gl_attribute_map[
                        vidoptions->gl_attributes[i].attribute];
                    if(sdl_attribute < 0)
                        continue;
                    SDL_GL_SetAttribute(sdl_attribute, vidoptions->gl_attributes[i].value);
                }
            }
        }

        bpp = videoinfo->vfmt->BitsPerPixel;
        surface = SDL_SetVideoMode(width, height, bpp, videoflags);
    } else {
            surface = SDL_SetVideoMode(width, height, 
               visual_video_depth_value_from_enum(depth), videoflags);
    }

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
    return surface;
}

VisVideo *new_video(SDL_Surface *surface)
{
    VisVideo *video = visual_video_new();
    visual_video_set_depth(video, visual_video_depth_enum_from_value(surface->format->BitsPerPixel));
    visual_video_set_dimension(video, surface->w, surface->w);
    visual_video_set_pitch(video, surface->pitch);
    visual_video_set_buffer(video, surface->pixels);
    return video;
}

int main (int argc, char **argv)
{
    SDL_Surface *surface;
	VisVideo *video;

    VisVideo *surface_video;
    VisBin *bin;
	VisEventQueue *eventqueue;
    VisParamContainer *params;
    VisParamEntry *param;
	VisVideoAttributeOptions *vidoptions;
    const char *morph_name = "slide", *actor_name = "lv_scope";

	int running = TRUE;
	int fullscreen = FALSE;
	int visible = TRUE;
    int depthflag;
	VisVideoDepth depth;
    int do_morph = FALSE;

    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        visual_log(VISUAL_LOG_CRITICAL, "Unable to init SDL: %s", SDL_GetError());
        return 0;
    }

    if(!(surface = SDL_SetVideoMode(SCREEN_W, SCREEN_H, 32, SDL_RESIZABLE)))
    {
        visual_log(VISUAL_LOG_CRITICAL, "Unable to set video: %s", SDL_GetError());
        return 0;
    }

    SDL_WM_SetCaption("SDL Test", "SDL Test");
 
    visual_log_set_verboseness(VISUAL_LOG_VERBOSENESS_HIGH);

	visual_init (&argc, &argv);

    video = visual_video_new_with_buffer(SCREEN_W, SCREEN_H, VISUAL_VIDEO_DEPTH_8BIT);

    bin = visual_bin_new();

    visual_bin_set_supported_depth(bin, VISUAL_VIDEO_DEPTH_ALL);
    visual_bin_switch_set_style(bin, VISUAL_SWITCH_STYLE_MORPH);

    visual_bin_set_video(bin, video);
    visual_bin_connect_by_names(bin, "lv_scope", "xmms2");
    visual_bin_realize(bin);
    visual_bin_sync(bin, FALSE);
    visual_bin_depth_changed(bin);

    surface_video = new_video(surface);

    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

    vidoptions = visual_actor_get_video_attribute_options(bin->actor);

    create(surface, VISUAL_VIDEO_DEPTH_32BIT, vidoptions, SCREEN_W, SCREEN_H, TRUE);
/*
    params = visual_plugin_get_params(bin->input->plugin);

    param = visual_param_container_get(params, "songinfo");

    if(param != NULL)
        visual_param_entry_set_object(param, 
            VISUAL_OBJECT(visual_actor_get_songinfo(bin->actor)));
*/

    //params = visual_plugin_get_params(actor->plugin);

    //string = visual_string_new_with_value("filename");
    //param = visual_param_container_get(params, string);
    //visual_param_entry_set_string(param, "/home/starlon/Projects/libvisual-svn/trunk/libvisual-avs/testpresets/ring10.avs");

	//localqueue = visual_event_queue_new ();

	while (running) {
        SDL_Flip(surface);
        SDL_Event event;
        eventqueue = visual_plugin_get_eventqueue(visual_actor_get_plugin(bin->actor));
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    goto to_exit;
                    break;
                case SDL_MOUSEMOTION:
                    visual_event_queue_add_mousemotion(eventqueue,  event.motion.x, event.motion.y);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    do_morph = TRUE;
                    //visual_event_queue_add_mousebutton(eventqueue, event.button.button, VISUAL_MOUSE_DOWN, event.button.x, event.button.y);
                    break;
                case SDL_MOUSEBUTTONUP:
                    visual_event_queue_add_mousebutton(eventqueue, event.button.button, VISUAL_MOUSE_UP, event.button.x, event.button.y);
                    break;
                case SDL_VIDEORESIZE:

                    goto to_exit;
                    create(surface, VISUAL_VIDEO_DEPTH_8BIT, NULL, event.resize.w, event.resize.h, TRUE);

                    visual_event_queue_add_resize(eventqueue, video, event.resize.w, event.resize.h);
                    break;
                default:
                    break;
            }
        }
        if(do_morph)
        {
            actor_name = visual_actor_get_next_by_name_nogl(actor_name);
            if(!actor_name)
                actor_name = visual_actor_get_next_by_name(0);
            morph_name = visual_morph_get_next_by_name(morph_name);
            if(!morph_name)
                morph_name = visual_morph_get_next_by_name(0);
            visual_bin_set_morph_by_name(bin, (char *)morph_name);
            visual_bin_switch_actor_by_name(bin, (char *)actor_name);
            do_morph = FALSE;
        }
        if(visual_bin_depth_changed(bin)) 
        {

            depthflag = visual_bin_get_depth(bin);
            depth = visual_video_depth_get_highest(depthflag);
            visual_video_free_buffer(video);
            visual_video_set_dimension(video, SCREEN_W, SCREEN_H);
            visual_video_set_depth(video, depth);
            visual_video_set_pitch(video, SCREEN_W * visual_video_bpp_from_depth(depth));
            visual_video_allocate_buffer(video);
            vidoptions = visual_actor_get_video_attribute_options(bin->actor);
            create(surface, depth, vidoptions, SCREEN_W, SCREEN_H, TRUE);
            visual_bin_sync(bin, TRUE);
        }
		visual_bin_run (bin);
        lock(surface);
        visual_video_depth_transform(surface_video, video);
        unlock(surface);
	}

to_exit:
    visual_video_free_buffer(video);
    visual_object_unref(VISUAL_OBJECT(video));
    visual_object_unref(VISUAL_OBJECT(surface_video));
	visual_quit ();
    SDL_Quit();
	return 0;
}
