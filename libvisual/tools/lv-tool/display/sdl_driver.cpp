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

#include "sdl_driver.hpp"
#include "display.hpp"
#include "display_driver.hpp"
#include <libvisual/libvisual.h>

#include <SDL.h>
#include <array>

namespace {

  SDL_GLattr const sdl_gl_attribute_map[] = {
      static_cast<SDL_GLattr> (-1),  // VISUAL_GL_ATTRIBUTE_NONE
      SDL_GL_BUFFER_SIZE,            // VISUAL_GL_ATTRIBUTE_BUFFER_SIZE
      static_cast<SDL_GLattr> (-1),  // VISUAL_GL_ATTRIBUTE_LEVEL
      static_cast<SDL_GLattr> (-1),  // VISUAL_GL_ATTRIBUTE_RGBA
      SDL_GL_DOUBLEBUFFER,           // VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER
      SDL_GL_STEREO,                 // VISUAL_GL_ATTRIBUTE_STEREO
      static_cast<SDL_GLattr> (-1),  // VISUAL_GL_ATTRIBUTE_AUX_BUFFERS
      SDL_GL_RED_SIZE,               // VISUAL_GL_ATTRIBUTE_RED_SIZE
      SDL_GL_GREEN_SIZE,             // VISUAL_GL_ATTRIBUTE_GREEN_SIZE
      SDL_GL_BLUE_SIZE,              // VISUAL_GL_ATTRIBUTE_BLUE_SIZE
      SDL_GL_ALPHA_SIZE,             // VISUAL_GL_ATTRIBUTE_ALPHA_SIZE
      SDL_GL_DEPTH_SIZE,             // VISUAL_GL_ATTRIBUTE_DEPTH_SIZE
      SDL_GL_STENCIL_SIZE,           // VISUAL_GL_ATTRIBUTE_STENCIL_SIZE
      SDL_GL_ACCUM_RED_SIZE,         // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GL_ACCUM_GREEN_SIZE,       // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GL_ACCUM_BLUE_SIZE,        // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GL_ACCUM_ALPHA_SIZE,       // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      static_cast<SDL_GLattr> (-1)   // VISUAL_GL_ATTRIBUTE_LAST
  };

  void get_nearest_resolution (int& width, int& height);

  class SDLDriver final
      : public DisplayDriver
  {
  public:

      SDLDriver (Display& display)
          : m_display         (display)
          , m_screen          (0)
          , m_screen_video    (0)
          , m_requested_depth (VISUAL_VIDEO_DEPTH_NONE)
          , m_last_width      (0)
          , m_last_height     (0)
          , m_resizable       (false)
          , m_active          (false)
          , m_running         (false)
      {}

      ~SDLDriver () override
      {
          close ();
      }

      LV::VideoPtr create (VisVideoDepth depth,
                           VisVideoAttrOptions const* vidoptions,
                           unsigned int width,
                           unsigned int height,
                           bool resizable) override
      {

          int videoflags = 0;

          if (resizable)
              videoflags |= SDL_RESIZABLE;

          if (!SDL_WasInit (SDL_INIT_VIDEO)) {
              if (SDL_Init (SDL_INIT_VIDEO) == -1) {
                  visual_log (VISUAL_LOG_ERROR, "Unable to initialize SDL: %s", SDL_GetError ());
                  return nullptr;
              }
          }

          m_resizable = resizable;
          m_requested_depth = depth;

          m_screen_video.reset ();

          if (depth == VISUAL_VIDEO_DEPTH_GL) {
              SDL_VideoInfo const* videoinfo = SDL_GetVideoInfo ();

              if (!videoinfo) {
                  return nullptr;
              }

              videoflags |= SDL_OPENGL;

              if (videoinfo->hw_available)
                  videoflags |= SDL_HWSURFACE;

              if (vidoptions) {
                  for (unsigned int i = VISUAL_GL_ATTRIBUTE_NONE; i < VISUAL_GL_ATTRIBUTE_LAST; i++) {
                      SDL_GLattr sdl_attribute = sdl_gl_attribute_map[vidoptions->gl_attrs[i].attribute];

                      if (sdl_attribute == static_cast<SDL_GLattr> (-1)) {
                          continue;
                      }

                      SDL_GL_SetAttribute (sdl_attribute, vidoptions->gl_attrs[i].value);
                  }
              }

              SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);

              int bpp = videoinfo->vfmt->BitsPerPixel;
              m_screen = SDL_SetVideoMode (width, height, bpp, videoflags);

          } else {
              m_screen = SDL_SetVideoMode (width, height,
                                           visual_video_depth_bpp (depth),
                                           videoflags);
          }

          // Recreate video object
          m_screen_video = LV::Video::wrap (m_screen->pixels,
                                            false,
                                            m_screen->w,
                                            m_screen->h,
                                            depth,
                                            m_screen->pitch);

          SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

          m_running = true;

          return m_screen_video;
      }

      void close () override
      {
          if (!m_running)
              return;

          //m_screen_video.reset (); FIXME: Invalid pointer.

          SDL_Quit ();

          m_running = false;
      }

      void lock () override
      {
          if (SDL_MUSTLOCK (m_screen))
              SDL_LockSurface (m_screen);
      }

      void unlock () override
      {
          if (SDL_MUSTLOCK (m_screen) == SDL_TRUE)
              SDL_UnlockSurface (m_screen);
      }

      bool is_fullscreen () const override
      {
          return m_screen->flags & SDL_FULLSCREEN;
      }

      void set_fullscreen (bool fullscreen, bool autoscale) override
      {
          if (fullscreen) {
              if (!(m_screen->flags & SDL_FULLSCREEN)) {
                  if (autoscale) {
                      int width  = m_display.get_video ()->get_width ();
                      int height = m_display.get_video ()->get_height ();

                      m_last_width  = width;
                      m_last_height = height;

                      get_nearest_resolution (width, height);

                      create (m_requested_depth, nullptr, width, height, m_resizable);
                  }

                  SDL_ShowCursor (SDL_FALSE);
                  SDL_WM_ToggleFullScreen (m_screen);
              }
          } else {
              if ((m_screen->flags & SDL_FULLSCREEN)) {
                  SDL_ShowCursor (SDL_TRUE);
                  SDL_WM_ToggleFullScreen (m_screen);

                  if (autoscale) {
                      create (m_requested_depth, nullptr, m_last_width, m_last_height, m_resizable);
                  }
              }
          }
      }

      LV::VideoPtr get_video () const override
      {
          return m_screen_video;
      }

      void set_title (std::string_view title) override
      {
          std::string const title_str {title};
          SDL_WM_SetCaption (title_str.c_str (), nullptr);
      }

      void update_rect (LV::Rect const& rect) override
      {
          if (m_screen->format->BitsPerPixel == 8) {
              auto const& pal = m_display.get_video ()->get_palette ();

              if (!pal.empty () && pal.size() <= 256) {
                  std::array<SDL_Color, 256> colors;
                  visual_mem_set (colors.data (), 0, sizeof (colors));

                  for (unsigned int i = 0; i < pal.size(); i++) {
                      colors[i].r = pal.colors[i].r;
                      colors[i].g = pal.colors[i].g;
                      colors[i].b = pal.colors[i].b;
                  }

                  SDL_SetColors (m_screen, colors.data (), 0, 256);
              }
          }

          if (m_requested_depth == VISUAL_VIDEO_DEPTH_GL)
              SDL_GL_SwapBuffers ();
          else
              SDL_UpdateRect (m_screen, rect.x, rect.y, rect.width, rect.height);
      }

      void drain_events (VisEventQueue& eventqueue) override
      {
          // Visible or not

          if (((SDL_GetAppState () & SDL_APPACTIVE) == 0) && m_active) {
              m_active = false;
              visual_event_queue_add (&eventqueue,
                                      visual_event_new_visibility (FALSE));
          } else if (((SDL_GetAppState () & SDL_APPACTIVE) != 0) && !m_active) {
              m_active = true;
              visual_event_queue_add (&eventqueue,
                                      visual_event_new_visibility (TRUE));
          }

          // Events

          SDL_Event event;

          while (SDL_PollEvent (&event)) {

              switch (event.type) {
              case SDL_KEYUP:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_keyboard (VisKey(event.key.keysym.sym),
                                                                     VisKeyMod(event.key.keysym.mod),
                                                                     VISUAL_KEY_UP));
                  break;

              case SDL_KEYDOWN:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_keyboard (VisKey(event.key.keysym.sym),
                                                                     VisKeyMod(event.key.keysym.mod),
                                                                     VISUAL_KEY_DOWN));
                  break;

              case SDL_VIDEORESIZE:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_resize (event.resize.w,
                                                                   event.resize.h));
                  break;

              case SDL_MOUSEMOTION:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_mousemotion (event.motion.x,
                                                                        event.motion.y));
                  break;

              case SDL_MOUSEBUTTONDOWN:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_mousebutton (event.button.button,
                                                                        VISUAL_MOUSE_DOWN,
                                                                        event.button.x,
                                                                        event.button.y));
                  break;

              case SDL_MOUSEBUTTONUP:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_mousebutton (event.button.button,
                                                                        VISUAL_MOUSE_UP,
                                                                        event.button.x,
                                                                        event.button.y));
                  break;

              case SDL_QUIT:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_quit ());
                  break;

              default:
                  break;
              }
          }
      }

  private:

      Display&    m_display;
      SDL_Surface*  m_screen;
      LV::VideoPtr  m_screen_video;
      VisVideoDepth m_requested_depth;

      unsigned int m_last_width;
      unsigned int m_last_height;

      bool m_resizable;

      bool m_active;
      bool m_running;
  };

  void get_nearest_resolution (int& width, int& height)
  {
      auto modelist = SDL_ListModes (nullptr, SDL_FULLSCREEN);
      if (!modelist)
          return;

      // Window is bigger than highest resolution
      if (modelist[0]->w <= width || modelist[0]->h <= height) {
          width  = modelist[0]->w;
          height = modelist[0]->h;
          return;
      }

      int w = width;
      int h = height;

      for (unsigned int i = 0; modelist[i]; i++) {
          if (modelist[i]->w >= width && modelist[i]->h >= height) {
              w = modelist[i]->w;
              h = modelist[i]->h;
              break;
          }
      }

      width  = w;
      height = h;
  }

} // anonymous namespace

DisplayDriver* sdl_driver_new (Display& display)
{
    return new SDLDriver (display);
}
