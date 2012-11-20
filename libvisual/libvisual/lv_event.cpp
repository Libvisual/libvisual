/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
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

#include "config.h"
#include "lv_event.h"
#include "lv_common.h"
#include <vector>
#include <queue>

namespace LV {

  namespace {

    unsigned int get_event_priority (VisEvent const& event)
    {
        return event.type == VISUAL_EVENT_RESIZE ? 100 : 0;
    }

    struct EventPriorityLesser
    {
        bool operator() (VisEvent const& event1, VisEvent const& event2) const
        {
            return get_event_priority (event1) < get_event_priority (event2);
        }
    };

  } // anonymous namespace

  class EventQueue::Impl
  {
  public:

      typedef std::priority_queue<Event, std::vector<Event>, EventPriorityLesser> Queue;

      Queue events;

      // FIXME: We need custom input handlers for actors
      int           mousex;
      int           mousey;
      VisMouseState mousestate;

      Impl ()
          : mousex (0)
          , mousey (0)
          , mousestate (VISUAL_MOUSE_UP)
      {}
  };

  EventQueue::EventQueue ()
      : m_impl (new Impl)
  {
      // empty
  }

  EventQueue::~EventQueue ()
  {
      // empty
  }

  bool EventQueue::poll (VisEvent& event)
  {
      if (!m_impl->events.empty ()) {
          event = m_impl->events.top ();
          m_impl->events.pop ();
          return true;
      } else {
          return false;
      }
  }

  void EventQueue::add (VisEvent const& event)
  {
      m_impl->events.push (event);
  }

} // LV namespace
