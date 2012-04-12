/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_songinfo.c,v 1.24 2006/01/22 13:23:37 synap Exp $
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
#include "lv_util.h"
#include <string.h>

namespace LV {

  SongInfo::SongInfo (SongInfoType type_)
      : type    (type_)
      , length  (0)
      , elapsed (0)
      , cover   (0)
  {
    // empty
  }

  SongInfo::~SongInfo ()
  {
      if (cover)
          visual_object_unref (VISUAL_OBJECT (cover));
  }

  void SongInfo::set_type (SongInfoType type_)
  {
      type = type_;
  }

  SongInfoType SongInfo::get_type () const
  {
      return type;
  }

  void SongInfo::set_length (int length_)
  {
      length = length_;
  }

  int SongInfo::get_length () const
  {
      return length;
  }

  void SongInfo::set_elapsed (int elapsed_)
  {
      elapsed = elapsed_;
  }

  int SongInfo::get_elapsed () const
  {
      return elapsed;
  }

  void SongInfo::set_simple_name (std::string const& name)
  {
      song_name = name;
  }

  std::string SongInfo::get_simple_name () const
  {
      return song_name;
  }

  void SongInfo::set_artist (std::string const& artist_)
  {
      artist = artist_;
  }

  std::string SongInfo::get_artist () const
  {
      return artist;
  }

  void SongInfo::set_album (std::string const& album_)
  {
      album = album_;
  }

  std::string SongInfo::get_album () const
  {
      return album;
  }

  void SongInfo::set_song (std::string const& song_)
  {
      song = song_;
  }

  std::string SongInfo::get_song () const
  {
      return song;
  }

  void SongInfo::set_cover (VisVideo *cover_)
  {
      if (cover)
          visual_object_unref (VISUAL_OBJECT (cover));

      // Get the desired cover art size
      VisParamContainer* params = visual_get_params ();
      VisParamEntry* xparam = visual_param_container_get (params, "songinfo cover size x");
      VisParamEntry* yparam = visual_param_container_get (params, "songinfo cover size y");

      int cawidth = 64;
      int caheight = 64;

      if (xparam && yparam) {
          cawidth = visual_param_entry_get_integer (xparam);
          caheight = visual_param_entry_get_integer (yparam);
      }

      // The coverart image
      cover = visual_video_scale_depth_new (cover, cawidth, caheight,
                                            VISUAL_VIDEO_DEPTH_32BIT,
                                            VISUAL_VIDEO_SCALE_BILINEAR);
  }

  void SongInfo::mark ()
  {
      timer.start ();
  }

  long SongInfo::get_age ()
  {
      Time cur = Time::now ();
      Time start_time = timer.get_start_time();

      // Clock has been changed into the past
      if (cur < start_time)
          mark ();

      cur -= start_time;

      return cur.sec;
  }

  bool operator== (SongInfo const& lhs, SongInfo const& rhs)
  {
      if (lhs.song_name != rhs.song_name)
          return false;

      if (lhs.artist != rhs.artist)
          return false;

      if (lhs.album != rhs.album)
          return false;

      if (lhs.song != rhs.song)
          return false;

      return true;
  }

} // LV namespace
