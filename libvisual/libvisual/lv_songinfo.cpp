/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012 Libvisual team
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
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
#include "lv_songinfo.h"
#include "lv_common.h"
#include "lv_libvisual.h"

namespace LV {

  SongInfo::SongInfo (SongInfoType type)
      : m_type    (type)
      , m_length  (0)
      , m_elapsed (0)
  {
      // empty
  }

  SongInfo::~SongInfo ()
  {
      // empty
  }

  void SongInfo::set_type (SongInfoType type)
  {
      m_type = type;
  }

  SongInfoType SongInfo::get_type () const
  {
      return m_type;
  }

  void SongInfo::set_length (int length)
  {
      m_length = length;
  }

  int SongInfo::get_length () const
  {
      return m_length;
  }

  void SongInfo::set_elapsed (int elapsed)
  {
      m_elapsed = elapsed;
  }

  int SongInfo::get_elapsed () const
  {
      return m_elapsed;
  }

  void SongInfo::set_simple_name (std::string_view name)
  {
      m_song_name = name;
  }

  std::string const& SongInfo::get_simple_name () const
  {
      return m_song_name;
  }

  void SongInfo::set_artist (std::string_view artist)
  {
      m_artist = artist;
  }

  std::string const& SongInfo::get_artist () const
  {
      return m_artist;
  }

  void SongInfo::set_album (std::string_view album)
  {
      m_album = album;
  }

  std::string const& SongInfo::get_album () const
  {
      return m_album;
  }

  void SongInfo::set_song (std::string_view song)
  {
      m_song = song;
  }

  std::string const& SongInfo::get_song () const
  {
      return m_song;
  }

  void SongInfo::set_cover (VideoPtr const& cover)
  {
      // Get the desired cover art size
      auto& system_params = LV::System::instance()->get_params ();
      auto  xparam = system_params.get ("songinfo-cover-width");
      auto  yparam = system_params.get ("songinfo-cover-height");

      int cover_width = 64;
      int cover_height = 64;

      if (xparam && yparam) {
          cover_width  = visual_param_get_value_integer (xparam);
          cover_height = visual_param_get_value_integer (yparam);
      }

      // The coverart image
      m_cover = Video::create_scale_depth (cover, cover_width, cover_height,
                                           VISUAL_VIDEO_DEPTH_32BIT,
                                           VISUAL_VIDEO_SCALE_BILINEAR);
  }

  void SongInfo::mark ()
  {
      m_timer.start ();
  }

  long SongInfo::get_age ()
  {
      auto cur = Time::now ();
      auto start_time = m_timer.get_start_time ();

      // Clock has been changed into the past
      if (cur < start_time)
          mark ();

      cur -= start_time;

      return cur.sec;
  }

  bool operator== (SongInfo const& lhs, SongInfo const& rhs)
  {
      if (lhs.m_song_name != rhs.m_song_name)
          return false;

      if (lhs.m_artist != rhs.m_artist)
          return false;

      if (lhs.m_album != rhs.m_album)
          return false;

      if (lhs.m_song != rhs.m_song)
          return false;

      return true;
  }

} // LV namespace
