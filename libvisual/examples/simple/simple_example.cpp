// Simple example of using Libvisual.
//
// Copyright (C) 2012-2013 Libvisual team
//               2004-2006 Dennis Smit
//
// Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
//          Scott Sibley <sisibley@gmail.com>
//          Max Howell <max.howell@methylblue.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "simple_example.hpp"

#include <functional>
#include <array>

#define DISPLAY_WIDTH     640
#define DISPLAY_HEIGHT    480
#define ACTOR_PLUGIN_NAME "lv_analyzer"
#define INPUT_PLUGIN_NAME "debug"
#define MORPH_PLUGIN_NAME "slide_left"

namespace {

  enum CycleDirection {
      CYCLE_DIR_PREV,
      CYCLE_DIR_NEXT
  };

  std::string cycle_actor (std::string const& actor, CycleDirection dir)
  {
      std::function<char const* (char const*)> func;

      switch (dir) {
          case CYCLE_DIR_PREV: {
              func = visual_actor_get_prev_by_name;
              break;
          }
          case CYCLE_DIR_NEXT: {
              func = visual_actor_get_next_by_name;
              break;
          }
      }

      auto new_actor = func (actor.c_str ());

      if (!new_actor) {
          new_actor = func (nullptr);
      }

      return new_actor;
  }

}

SimpleExample::SimpleExample ()
    : m_actor_name (ACTOR_PLUGIN_NAME)
    , m_input_name (INPUT_PLUGIN_NAME)
    , m_morph_name (MORPH_PLUGIN_NAME)
{
    m_bin.set_supported_depth (VISUAL_VIDEO_DEPTH_ALL);
    m_bin.connect (m_actor_name, m_input_name);

    create_display (DISPLAY_WIDTH, DISPLAY_HEIGHT, m_bin.get_depth ());
    m_bin.set_video (m_screen);

    SDL_WM_SetCaption (m_actor_name.c_str (), 0);

    m_bin.depth_changed ();

    m_bin.realize ();
    m_bin.sync (false);

    m_bin.use_morph (true);
    m_bin.set_morph (m_morph_name);
}

SimpleExample::~SimpleExample ()
{
    // nothing
}

LV::VideoPtr SimpleExample::create_display (int width, int height, VisVideoDepth depth)
{
    m_screen.reset ();
    m_sdl_screen = nullptr;

    if (depth == VISUAL_VIDEO_DEPTH_GL) {
        int flags = SDL_OPENGL | SDL_RESIZABLE;

        SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);

        m_sdl_screen = SDL_SetVideoMode (width, height, 24, flags);
    } else {
        int flags = SDL_RESIZABLE | SDL_HWPALETTE;

        m_sdl_screen = SDL_SetVideoMode (width, height, visual_video_depth_from_bpp (depth), flags);
    }

    if (!m_sdl_screen) {
        return nullptr;
    }

    m_screen = LV::Video::wrap (m_sdl_screen->pixels, false, width, height, depth);

    return m_screen;
}

void SimpleExample::resize_display (int width, int height)
{
    create_display (width, height, m_bin.get_depth ());

    m_bin.set_video (m_screen);
    m_bin.sync (false);
}

void SimpleExample::set_palette (LV::Palette const& pal)
{
    if (pal.empty ()) {
        return;
    }

    std::array<SDL_Color, 256> colors;
    visual_mem_set (colors.data (), 0, sizeof (colors));

    for (unsigned int i = 0; i < pal.size(); i++) {
        colors[i].r = pal.colors[i].r;
        colors[i].g = pal.colors[i].g;
        colors[i].b = pal.colors[i].b;
    }

    SDL_SetColors (m_sdl_screen, colors.data (), 0, 256);
}

bool SimpleExample::handle_events ()
{
    SDL_Event event;

    while (SDL_PollEvent (&event)) {
        switch (event.type) {
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        return false;

                    case SDLK_LEFT:
                        morph_to_actor (cycle_actor (m_actor_name, CYCLE_DIR_PREV), m_morph_name);
                        break;

                    case SDLK_RIGHT:
                        morph_to_actor (cycle_actor (m_actor_name, CYCLE_DIR_NEXT), m_morph_name);
                        break;

                    default:;
                }

                break;
            }
            case SDL_VIDEORESIZE: {
                resize_display (event.resize.w, event.resize.h);
                break;
            }
            default:;
        }
    }

    return true;
}

void SimpleExample::morph_to_actor (std::string const& actor_name, std::string const& morph_name)
{
    (void)morph_name; //m_bin.set_morph (morph_name);
    m_bin.switch_actor (actor_name);

    m_actor_name = actor_name;
    SDL_WM_SetCaption (m_actor_name.c_str (), 0);
}

void SimpleExample::render ()
{
    auto screen_depth = m_screen->get_depth ();

    if (screen_depth == VISUAL_VIDEO_DEPTH_GL) {
        m_bin.run ();
        SDL_GL_SwapBuffers ();
    } else {
        SDL_LockSurface (m_sdl_screen);
        m_bin.run ();
        SDL_UnlockSurface (m_sdl_screen);

        if (screen_depth == VISUAL_VIDEO_DEPTH_8BIT) {
            set_palette (m_screen->get_palette ());
        }

        SDL_Flip (m_sdl_screen);
    }
}

void SimpleExample::run ()
{
    while (true) {
        render ();

        if (!handle_events ()) {
            break;
        }

        if (m_bin.depth_changed ())
        {
            int depthflag = m_bin.get_depth ();
            VisVideoDepth depth = visual_video_depth_get_highest (depthflag);

            create_display (m_screen->get_width (), m_screen->get_height (), depth);
            m_bin.set_video (m_screen);

            m_bin.sync (true);
        }
    }
}
