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
#include "private/lv_audio_stream.hpp"
#include "lv_common.h"

#include <list>

namespace LV {

  namespace {

    uint64_t max_lifetime = VISUAL_USECS_PER_SEC;

  } // anonymous namespace

  class AudioStream::Impl
  {
  public:

      struct Fragment
      {
          BufferConstPtr buffer;
          Time           timestamp;
      };

      typedef std::list<Fragment> FragmentList;

      FragmentList fragments;
      std::size_t  size;

      Impl ();

      void remove_stale_fragments (Time const& time);
  };

  AudioStream::Impl::Impl ()
      : size { 0 }
  {
      // empty
  }

  void AudioStream::Impl::remove_stale_fragments (Time const& time)
  {
      fragments.remove_if ([&] (Fragment const& fragment) -> bool {
          if ((time - fragment.timestamp).to_usecs () > max_lifetime) {
              size -= fragment.buffer->get_size ();
              return true;
          }

          return false;
      });
  }

  AudioStream::AudioStream ()
      : m_impl (new Impl)
  {
      // empty
  }

  AudioStream::~AudioStream ()
  {
      // empty
  }

  std::size_t AudioStream::get_size () const
  {
      return m_impl->size;
  }

  void AudioStream::write (BufferConstPtr const& buffer, Time const& timestamp)
  {
      // Invalidate stale fragments
      m_impl->remove_stale_fragments (Time::now ());

      // Add fragment to list
      m_impl->fragments.push_back ({ buffer, timestamp });
      m_impl->size += buffer->get_size ();
  }

  std::size_t AudioStream::read (BufferPtr const& buffer, std::size_t nbytes)
  {
      visual_return_val_if_fail (nbytes > 0, 0);

      // Return if there are no fragments
      if (m_impl->fragments.empty ()) {
          return 0;
      }

      // Truncate if read buffer is too small
      nbytes = std::min (nbytes, buffer->get_size ());

      std::size_t size = 0;
      std::size_t offset = 0;

      auto last_fragment = m_impl->fragments.rbegin ();

      auto fragment = last_fragment;

      // Find the oldest fragment to read from
      if (nbytes <= m_impl->size) {
          while (fragment != m_impl->fragments.rend ()) {
              size += fragment->buffer->get_size ();
              if (size >= nbytes) {
                  offset = size - nbytes;
                  break;
              }
              ++fragment;
          }
      } else {
          fragment = m_impl->fragments.rend (); --fragment;
          nbytes = m_impl->size;
      }

      std::size_t write_offset = 0;

      // Copy out a portion of the first fragment if its data is only partially needed
      if (offset > 0) {
          std::size_t size = fragment->buffer->get_size () - offset;
          buffer->put (fragment->buffer->get_data (offset), size, write_offset);

          // Finish if this is the only fragment
          if (fragment == last_fragment) {
              return nbytes;
          }

          write_offset += size;
          --fragment;
      }

      // Copy out data in the remaining fragments
      while (fragment != last_fragment) {
          buffer->put (fragment->buffer, write_offset);
          write_offset += fragment->buffer->get_size ();
          --fragment;
      }

      // Copy out data in the last fragment
      buffer->put (fragment->buffer, write_offset);

      return nbytes;
  }

} // LV namespace
