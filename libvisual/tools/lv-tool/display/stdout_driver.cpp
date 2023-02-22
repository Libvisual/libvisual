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

#include "stdout_driver.hpp"
#include "display.hpp"
#include "display_driver.hpp"
#include <libvisual/libvisual.h>
#include <string>
#include <unistd.h>

// MinGW unistd.h doesn't have *_FILENO or SEEK_* defined
#ifdef VISUAL_WITH_MINGW
#  define STDOUT_FILENO 1
#endif

namespace {

  class StdoutDriver final
      : public DisplayDriver
  {
  public:

      StdoutDriver (Display& display)
      {
          (void)display;
      }

      ~StdoutDriver () override
      {
          close ();
      }

      LV::VideoPtr create (VisVideoDepth depth,
                           VisVideoAttrOptions const* vidoptions,
                           unsigned int width,
                           unsigned int height,
                           bool resizable) override
      {
          (void)vidoptions;
          (void)resizable;

          if (depth == VISUAL_VIDEO_DEPTH_GL)
          {
              visual_log (VISUAL_LOG_ERROR, "Cannot use stdout driver for OpenGL rendering");
              return nullptr;
          }

          m_screen_video = LV::Video::create (width, height, depth);

          return m_screen_video;
      }

      void close () override
      {
          m_screen_video.reset ();
      }

      void lock () override
      {
          // nothing to do
      }

      void unlock () override
      {
          // nothing to do
      }

      bool is_fullscreen () const override
      {
          return false;
      }

      void set_fullscreen (bool fullscreen, bool autoscale) override
      {
          (void)fullscreen;
          (void)autoscale;

          // nothing to do
      }

      LV::VideoPtr get_video () const override
      {
          return m_screen_video;
      }

      void set_title (std::string_view title) override
      {
          (void)title;

          // nothing to do
      }

      void update_rect (LV::Rect const& rect) override
      {
          (void)rect;

          if (write (STDOUT_FILENO, m_screen_video->get_pixels (), m_screen_video->get_size ()) == -1)
              visual_log (VISUAL_LOG_ERROR, "Failed to write pixels to stdout");
      }

      void drain_events (VisEventQueue& eventqueue) override
      {
          (void)eventqueue;

          // nothing to do
      }

  private:

      LV::VideoPtr m_screen_video;
  };

} // anonymous namespace

// creator
DisplayDriver* stdout_driver_new (Display& display)
{
    return new StdoutDriver (display);
}
