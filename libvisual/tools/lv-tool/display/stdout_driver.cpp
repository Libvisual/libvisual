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
#include <vector>
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

      virtual bool create (VisVideoDepth depth,
                           VisVideoAttributeOptions const* vidoptions,
                           unsigned int width,
                           unsigned int height,
                           bool resizable)
      {
          unsigned int pixel_size = visual_video_depth_value_from_enum (VISUAL_VIDEO_DEPTH_24BIT) / 8;

          m_area.resize (width * height * pixel_size);
          visual_mem_set (&m_area[0], 0, m_area.size ());

          // save dimensions
          m_width  = width;
          m_height = height;
          m_depth  = depth;

          return true;
      }

      virtual void close ()
      {
          // nothing to do
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

      virtual void get_video (VisVideo* screen)
      {
          visual_video_set_depth (screen, VISUAL_VIDEO_DEPTH_24BIT);
          visual_video_set_dimension (screen, m_width, m_height);
          visual_video_set_buffer (screen, &m_area[0]);
      }

      virtual void update_rect (LV::Rect const& rect)
      {
          write(STDOUT_FILENO, &m_area[0], m_area.size());
      }

      virtual void drain_events (VisEventQueue& eventqueue)
      {
          // nothing to do
      }

  private:

      SADisplay&           m_display;
      unsigned int         m_width;
      unsigned int         m_height;
      VisVideoDepth        m_depth;
      std::vector<uint8_t> m_area;
  };

} // anonymous namespace

// creator
SADisplayDriver* stdout_driver_new (SADisplay& display)
{
    return new StdoutDriver (display);
}
