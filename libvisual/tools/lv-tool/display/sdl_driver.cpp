/* Libvisual - The audio visualisation framework cli tool
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>,
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
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

#include "sdl_driver.hpp"
#include "display.hpp"
#include "display_driver.hpp"
#include "gettext.h"
#include <libvisual/libvisual.h>

#include <SDL/SDL.h>
#include <iostream>
#include <map>

namespace {

  SDL_GLattr const sdl_gl_attribute_map[] = {
      SDL_GLattr (-1),         // VISUAL_GL_ATTRIBUTE_NONE
      SDL_GL_BUFFER_SIZE,      // VISUAL_GL_ATTRIBUTE_BUFFER_SIZE
      SDL_GLattr (-1),         // VISUAL_GL_ATTRIBUTE_LEVEL
      SDL_GLattr (-1),         // VISUAL_GL_ATTRIBUTE_RGBA
      SDL_GL_DOUBLEBUFFER,     // VISUAL_GL_ATTRIBUTE_DOUBLEBUFFER
      SDL_GL_STEREO,           // VISUAL_GL_ATTRIBUTE_STEREO
      SDL_GLattr (-1),         // VISUAL_GL_ATTRIBUTE_AUX_BUFFERS
      SDL_GL_RED_SIZE,         // VISUAL_GL_ATTRIBUTE_RED_SIZE
      SDL_GL_GREEN_SIZE,       // VISUAL_GL_ATTRIBUTE_GREEN_SIZE
      SDL_GL_BLUE_SIZE,        // VISUAL_GL_ATTRIBUTE_BLUE_SIZE
      SDL_GL_ALPHA_SIZE,       // VISUAL_GL_ATTRIBUTE_ALPHA_SIZE
      SDL_GL_DEPTH_SIZE,       // VISUAL_GL_ATTRIBUTE_DEPTH_SIZE
      SDL_GL_STENCIL_SIZE,     // VISUAL_GL_ATTRIBUTE_STENCIL_SIZE
      SDL_GL_ACCUM_RED_SIZE,   // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GL_ACCUM_GREEN_SIZE, // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GL_ACCUM_BLUE_SIZE,  // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GL_ACCUM_ALPHA_SIZE, // VISUAL_GL_ATTRIBUTE_ACCUM_RED_SIZE
      SDL_GLattr (-1)          // VISUAL_GL_ATTRIBUTE_LAST
  };

  void get_nearest_resolution (int& width, int& height);

  class SDLDriver
      : public SADisplayDriver
  {
  public:

      SDLDriver (SADisplay& display)
          : m_display         (display)
          , m_screen          (0)
          , m_requested_depth (VISUAL_VIDEO_DEPTH_NONE)
          , m_last_width      (0)
          , m_last_height     (0)
          , m_resizable       (false)
          , m_active          (false)
          , m_running         (false)
      {}

      virtual ~SDLDriver ()
      {
          close ();
      }

      virtual bool create (VisVideoDepth depth, VisVideoAttributeOptions const* vidoptions,
                           unsigned int width, unsigned int height, bool resizable)
      {
          int videoflags = 0;

          if (resizable)
              videoflags |= SDL_RESIZABLE;

          if (!SDL_WasInit (SDL_INIT_VIDEO)) {
              if (SDL_Init (SDL_INIT_VIDEO) == -1) {
                  std::cerr << "Unable to init SDL VIDEO: " << SDL_GetError () << std::endl;
                  return false;
              }
          }

          m_resizable = resizable;
          m_requested_depth = depth;

          if (depth == VISUAL_VIDEO_DEPTH_GL) {
              SDL_VideoInfo const* videoinfo = SDL_GetVideoInfo ();

              if (!videoinfo) {
                  return -1;
              }

              videoflags |= SDL_OPENGL | SDL_GL_DOUBLEBUFFER | SDL_HWPALETTE;

              if (videoinfo->hw_available)
                  videoflags |= SDL_HWSURFACE;
              else
                  videoflags |= SDL_SWSURFACE;

              if (videoinfo->blit_hw)
                  videoflags |= SDL_HWACCEL;

              for (unsigned int i = VISUAL_GL_ATTRIBUTE_NONE; i < VISUAL_GL_ATTRIBUTE_LAST; i++) {
                  if (vidoptions->gl_attributes[i].mutated) {
                      SDL_GLattr sdl_attribute =
                          sdl_gl_attribute_map[vidoptions->gl_attributes[i].attribute];

                      if (sdl_attribute < 0)
                          continue;

                      SDL_GL_SetAttribute (sdl_attribute, vidoptions->gl_attributes[i].value);
                  }
              }

              int bpp = videoinfo->vfmt->BitsPerPixel;
              m_screen = SDL_SetVideoMode (width, height, bpp, videoflags);

          } else {
              m_screen = SDL_SetVideoMode (width, height,
                                           visual_video_depth_value_from_enum (depth),
                                           videoflags);
          }

          SDL_WM_SetCaption (_("lv-tool"), NULL);

          SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

          m_running = true;

          return true;
      }

      virtual void close ()
      {
          if (!m_running)
              return;

          SDL_Quit ();

          m_running = false;
      }

      virtual void lock ()
      {
          if (SDL_MUSTLOCK (m_screen))
              SDL_LockSurface (m_screen);
      }

      virtual void unlock ()
      {
          if (SDL_MUSTLOCK (m_screen) == SDL_TRUE)
              SDL_UnlockSurface (m_screen);
      }

      virtual void set_fullscreen (bool fullscreen, bool autoscale)
      {
          if (fullscreen) {
              if (!(m_screen->flags & SDL_FULLSCREEN)) {
                  if (autoscale) {
                      int width  = m_display.get_screen ()->width;
                      int height = m_display.get_screen ()->height;

                      m_last_width  = width;
                      m_last_height = height;

                      get_nearest_resolution (width, height);

                      create (m_requested_depth, NULL, width, height, m_resizable);
                  }

                  SDL_ShowCursor (SDL_FALSE);
                  SDL_WM_ToggleFullScreen (m_screen);
              }
          } else {
              if ((m_screen->flags & SDL_FULLSCREEN)) {
                  SDL_ShowCursor (SDL_TRUE);
                  SDL_WM_ToggleFullScreen (m_screen);

                  if (autoscale) {
                      create (m_requested_depth, NULL, m_last_width, m_last_height, m_resizable);
                  }
              }
          }
      }

      virtual void get_video (VisVideo* screen)
      {
          if (m_requested_depth == VISUAL_VIDEO_DEPTH_GL)
              visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_GL);
          else
              visual_video_set_depth (screen, visual_video_depth_enum_from_value (m_screen->format->BitsPerPixel));

          visual_video_set_dimension (screen, m_screen->w, m_screen->h);
          visual_video_set_pitch (screen, m_screen->pitch);
          visual_video_set_buffer (screen, m_screen->pixels);
      }

      virtual void update_rect (LV::Rect const& rect)
      {
          if (m_screen->format->BitsPerPixel == 8) {
              LV::Palette* pal = m_display.get_screen ()->pal;

              if (pal && pal->size() <= 256) {
                  SDL_Color colors[256];
                  visual_mem_set (colors, 0, sizeof (colors));

                  for (unsigned int i = 0; i < pal->size(); i++) {
                      colors[i].r = pal->colors[i].r;
                      colors[i].g = pal->colors[i].g;
                      colors[i].b = pal->colors[i].b;
                  }

                  SDL_SetColors (m_screen, colors, 0, 256);
              }
          }

          if (m_requested_depth == VISUAL_VIDEO_DEPTH_GL)
              SDL_GL_SwapBuffers ();
          else
              SDL_UpdateRect (m_screen, rect.x, rect.y, rect.width, rect.height);
      }

      virtual void drain_events (VisEventQueue& eventqueue)
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
                                                                     event.key.keysym.mod,
                                                                     VISUAL_KEY_UP));
                  break;

              case SDL_KEYDOWN:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_keyboard (VisKey(event.key.keysym.sym),
                                                                     event.key.keysym.mod,
                                                                     VISUAL_KEY_DOWN));
                  break;

              case SDL_VIDEORESIZE:
                  visual_event_queue_add (&eventqueue,
                                          visual_event_new_resize (event.resize.w,
                                                                   event.resize.h));

                  create (m_display.get_screen ()->depth, NULL, event.resize.w, event.resize.h, m_resizable);
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

      SADisplay&    m_display;
      SDL_Surface*  m_screen;
      VisVideoDepth m_requested_depth;

      unsigned int m_last_width;
      unsigned int m_last_height;

      bool m_resizable;

      bool m_active;
      bool m_running;
  };

  void get_nearest_resolution (int& width, int& height)
  {
      SDL_Rect **modelist = SDL_ListModes (NULL, SDL_FULLSCREEN);
      if (modelist == NULL)
          return;

      int w = width;
      int h = height;

      // Window is bigger than highest resolution
      if (modelist[0]->w <= width || modelist[0]->h <= height) {
          width  = modelist[0]->w;
          height = modelist[0]->h;
          return;
      }

      for (unsigned int i = 0; modelist[i]; i++) {
          if (modelist[i]->w >= width && modelist[i]->h >= height) {
              w = modelist[i]->w;
              h = modelist[i]->h;
              return;
          }
      }

      width  = w;
      height = h;
  }

} // anonymous namespace

SADisplayDriver* sdl_driver_new (SADisplay& display)
{
    return new SDLDriver (display);
}
