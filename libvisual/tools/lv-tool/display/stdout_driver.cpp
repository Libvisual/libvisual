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

  class StdoutDriver
      : public SADisplayDriver
  {
  public:

      StdoutDriver (SADisplay& display)
          : m_display (display)
      {}

      virtual ~StdoutDriver ()
      {
          close ();
      }

      virtual LV::VideoPtr create (VisVideoDepth depth,
                                   VisVideoAttrOptions const* vidoptions,
                                   unsigned int width,
                                   unsigned int height,
                                   bool resizable)
      {
          m_screen_video = LV::Video::create (width, height, VISUAL_VIDEO_DEPTH_24BIT);

          return m_screen_video;
      }

      virtual void close ()
      {
          m_screen_video.reset ();
      }

      virtual void lock ()
      {
          // nothing to do
      }

      virtual void unlock ()
      {
          // nothing to do
      }

      virtual void set_fullscreen (bool fullscreen, bool autoscale)
      {
          // nothing to do
      }

      virtual LV::VideoPtr get_video () const
      {
          return m_screen_video;
      }

      virtual void set_title(std::string const& title)
      {
          // nothing to do
      }

      virtual void update_rect (LV::Rect const& rect)
      {
          write (STDOUT_FILENO, m_screen_video->get_pixels (), m_screen_video->get_size ());
      }

      virtual void drain_events (VisEventQueue& eventqueue)
      {
          // nothing to do
      }

  private:

      SADisplay&   m_display;
      LV::VideoPtr m_screen_video;
  };

} // anonymous namespace

// creator
SADisplayDriver* stdout_driver_new (SADisplay& display)
{
    return new StdoutDriver (display);
}
