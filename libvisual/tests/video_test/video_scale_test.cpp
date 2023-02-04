#include <libvisual/libvisual.h>
#include <SDL.h>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#define DISPLAY_WIDTH  640
#define DISPLAY_HEIGHT 480

namespace {

  VisVideoScaleMethod cycle_scale_method (VisVideoScaleMethod method)
  {
      switch (method) {
          case VISUAL_VIDEO_SCALE_NEAREST:
              return VISUAL_VIDEO_SCALE_BILINEAR;
          case VISUAL_VIDEO_SCALE_BILINEAR:
              return VISUAL_VIDEO_SCALE_NEAREST;
          default:
              return VISUAL_VIDEO_SCALE_NEAREST;
      }
  }

  LV::VideoPtr lv_video_wrap_sdl_surface (SDL_Surface* surface)
  {
      return LV::Video::wrap (surface->pixels,
                              false,
                              surface->w,
                              surface->h,
                              visual_video_depth_from_bpp (surface->format->BitsPerPixel),
                              surface->pitch);
  }

} // anonymous namespace

int main (int argc, char *argv[])
{
    LV::System::init (argc, argv);

    int exit_code = EXIT_SUCCESS;

    try {
        // Initialize SDL

        if (SDL_Init (SDL_INIT_VIDEO)) {
            throw std::runtime_error {std::string {"Failed to initialize SDL: "} + SDL_GetError ()};
        }
        std::atexit (SDL_Quit);

        // Setup window for drawing

        int sdl_video_flags = SDL_RESIZABLE | SDL_DOUBLEBUF;

        SDL_Surface* sdl_screen = SDL_SetVideoMode (DISPLAY_WIDTH, DISPLAY_HEIGHT, 24, sdl_video_flags);
        if (!sdl_screen) {
            throw std::runtime_error {std::string {"Failed to create window"} + SDL_GetError ()};
        }

        SDL_WM_SetCaption ("LV::Video scale test", NULL);

        auto screen = lv_video_wrap_sdl_surface (sdl_screen);

        // Load image

        auto const image_file_path = (argc > 1) ? argv[1] : "../images/landscape.png";

        auto image = LV::Video::create_from_file (image_file_path);

        if (!image) {
            throw std::runtime_error {std::string {"Failed to load file '"} + image_file_path + "'"};
        }

        if (image->get_depth () != VISUAL_VIDEO_DEPTH_24BIT) {
            throw std::runtime_error {"Image must be in 24-bit RGB"};
        }

        // Initialize draw settings

        auto scale_method = VISUAL_VIDEO_SCALE_NEAREST;

        // Main loop

        bool running = true;

        while (running) {
            screen->scale (image, scale_method);
            SDL_Flip (sdl_screen);

            // Handle events

            SDL_Event event;

            while (SDL_PollEvent (&event)) {
                switch (event.type) {
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym) {
                            case SDLK_SPACE: {
                                scale_method = cycle_scale_method (scale_method);
                                break;
                            }
                            case SDLK_ESCAPE: {
                                running = false;
                                break;
                            }

                            default:;
                        }
                        break;

                    case SDL_VIDEORESIZE:
                        sdl_screen = SDL_SetVideoMode (event.resize.w, event.resize.h, 24, sdl_video_flags);
                        if (!sdl_screen) {
                            throw std::runtime_error {"Failed to resize window"};
                        }
                        screen = lv_video_wrap_sdl_surface (sdl_screen);
                        break;

                    case SDL_QUIT:
                        running = false;
                        break;

                    default:;
                }
            }
        }
    }
    catch (std::exception& error) {
        std::cerr << "Error! " << error.what () << std::endl;
        exit_code = EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown error caught!\n";
        exit_code = EXIT_FAILURE;
    }

    LV::System::destroy ();

    return exit_code;
}
