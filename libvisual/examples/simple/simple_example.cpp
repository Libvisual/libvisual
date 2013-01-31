/*
 * Simple example of using Libvisual.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Scott Sibley <sisibley@gmail.com>
 *          Max Howell <max.howell@methylblue.com>
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

#include "simple_example.hpp"

#include <functional>

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
    , m_morph_name (INPUT_PLUGIN_NAME)
{
    m_bin.set_supported_depth (VISUAL_VIDEO_DEPTH_ALL);

    create_display (640, 480, m_bin.get_depth ());
    m_bin.set_video (m_screen);
    m_bin.connect (m_actor_name, m_input_name);

    SDL_WM_SetCaption (m_actor_name.c_str (), 0);

    m_bin.depth_changed ();

    m_bin.realize ();
    m_bin.sync (false);

    m_bin.switch_set_style (VISUAL_SWITCH_STYLE_MORPH);
    m_bin.switch_set_steps (10);
    m_bin.set_morph (m_morph_name);
}

SimpleExample::~SimpleExample ()
{
    // nothing
}

LV::VideoPtr SimpleExample::create_display (int width, int height, VisVideoDepth depth)
{
    if (depth == VISUAL_VIDEO_DEPTH_GL) {
        int flags = SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_RESIZABLE;

        SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

        m_sdl_screen = SDL_SetVideoMode (width, height, 24, flags);
    } else {
        int flags = SDL_RESIZABLE | SDL_HWPALETTE;

        m_sdl_screen = SDL_SetVideoMode (width, height, visual_video_depth_value_from_enum (depth), flags);
    }

    if (!m_sdl_screen) {
        m_screen.reset ();
        return nullptr;
    }

    m_screen = LV::Video::wrap (m_sdl_screen->pixels, false, width, height, depth);

    return m_screen;
}

void SimpleExample::resize_display (int width, int height)
{
    create_display (width, height, m_bin.get_depth ());

    m_bin.sync (false);
}

void SimpleExample::set_palette (LV::Palette const& palette)
{
    SDL_Color sdl_palette[256];

    for (unsigned int i = 0; i < 256; i++) {
        sdl_palette[i].r = m_palette.colors[i].r;
        sdl_palette[i].g = m_palette.colors[i].g;
        sdl_palette[i].b = m_palette.colors[i].b;
    }

    SDL_SetColors (m_sdl_screen, sdl_palette, 0, 256);
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
    m_bin.set_morph (morph_name);
    m_bin.switch_actor (actor_name);

    m_actor_name = actor_name;
    SDL_WM_SetCaption (m_actor_name.c_str (), 0);
}

void SimpleExample::render ()
{
    bool is_opengl = (m_bin.get_depth () == VISUAL_VIDEO_DEPTH_GL);

    if (is_opengl) {
        m_bin.run ();
        SDL_GL_SwapBuffers ();
    } else {
        SDL_LockSurface (m_sdl_screen);
        m_bin.run ();
        SDL_UnlockSurface (m_sdl_screen);

        set_palette (m_bin.get_palette ());
        SDL_Flip (m_sdl_screen);
    }
}

void SimpleExample::run ()
{
    while (handle_events ()) {
        render ();
    }
}
