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
#include "lv_util.hpp"

extern "C" {

  VisSongInfo *visual_songinfo_new (VisSongInfoType type)
  {
      return new LV::SongInfo (LV::SongInfoType (type));
  }

  VisSongInfo *visual_songinfo_clone (VisSongInfo const *src)
  {
      visual_return_val_if_fail (src != nullptr, nullptr);

      return new LV::SongInfo (*src);
  }

  void visual_songinfo_free (VisSongInfo *self)
  {
      delete self;
  }

  void visual_songinfo_set_type (VisSongInfo *self, VisSongInfoType type)
  {
      visual_return_if_fail (self != nullptr);

      self->set_type (LV::SongInfoType (type));
  }

  VisSongInfoType visual_songinfo_get_type (VisSongInfo const *self)
  {
      visual_return_val_if_fail (self != nullptr, VISUAL_SONGINFO_TYPE_NULL);

      return VisSongInfoType (self->get_type ());
  }

  void visual_songinfo_set_length (VisSongInfo *self, int length)
  {
      visual_return_if_fail (self != nullptr);

      self->set_length (length);
  }

  int visual_songinfo_get_length (VisSongInfo const *self)
  {
	  visual_return_val_if_fail (self != nullptr, 0);

      return self->get_length ();
  }

  void visual_songinfo_set_elapsed (VisSongInfo *self, int elapsed)
  {
      visual_return_if_fail (self != nullptr);

      self->set_elapsed (elapsed);
  }

  int visual_songinfo_get_elapsed (VisSongInfo const *self)
  {
      return self->get_elapsed ();
  }

  void visual_songinfo_set_simple_name (VisSongInfo *self, const char *name)
  {
      visual_return_if_fail (self != nullptr);

      self->set_simple_name (name);
  }

  const char *visual_songinfo_get_simple_name (VisSongInfo const *self)
  {
      visual_return_val_if_fail (self != nullptr, nullptr);

      return LV::string_to_c (self->get_simple_name ());
  }

  void visual_songinfo_set_artist (VisSongInfo *self, const char *artist)
  {
      visual_return_if_fail (self != nullptr);

      self->set_artist (artist);
  }

  const char *visual_songinfo_get_artist (VisSongInfo const *self)
  {
      visual_return_val_if_fail (self != nullptr, nullptr);

      return LV::string_to_c (self->get_artist ());
  }

  void visual_songinfo_set_album (VisSongInfo *self, const char *album)
  {
      visual_return_if_fail (self != nullptr);

      self->set_album (album);
  }

  const char *visual_songinfo_get_album (VisSongInfo const *self)
  {
      visual_return_val_if_fail (self != nullptr, nullptr);

      return LV::string_to_c (self->get_album ());
  }

  void visual_songinfo_set_song (VisSongInfo *self, const char *song)
  {
      visual_return_if_fail (self != nullptr);

      self->set_song (song);
  }

  const char *visual_songinfo_get_song (VisSongInfo const *self)
  {
	  visual_return_val_if_fail (self != nullptr, nullptr);

      return LV::string_to_c (self->get_song ());
  }

  void visual_songinfo_set_cover (VisSongInfo *self, VisVideo *cover)
  {
      visual_return_if_fail (self  != nullptr);
      visual_return_if_fail (cover != nullptr);

      self->set_cover (cover);
  }

  void visual_songinfo_mark (VisSongInfo *self)
  {
      visual_return_if_fail (self != nullptr);

      self->mark ();
  }

  long visual_songinfo_get_age (VisSongInfo *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->get_age ();
  }

  void visual_songinfo_copy (VisSongInfo *lhs, const VisSongInfo *rhs)
  {
      visual_return_if_fail (lhs != nullptr);
      visual_return_if_fail (rhs != nullptr);

      *lhs = *rhs;
  }

  int visual_songinfo_compare (const VisSongInfo *lhs, const VisSongInfo *rhs)
  {
      visual_return_val_if_fail (lhs != nullptr, FALSE);
      visual_return_val_if_fail (rhs != nullptr, FALSE);

      return *lhs == *rhs;
  }

} // C extern
