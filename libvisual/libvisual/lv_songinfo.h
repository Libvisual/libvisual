/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_songinfo.h,v 1.14 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_SONGINFO_H
#define _LV_SONGINFO_H

#include <libvisual/lv_time.h>
#include <libvisual/lv_video.h>


/**
 * @defgroup VisSongInfo VisSongInfo
 * @{
 */


/**
 * Used to define the type of song info being used.
 * There are two interfaces to notify libvisual of song
 * information, being a simple and a advanced interface.
 */
typedef enum {
    VISUAL_SONGINFO_TYPE_NULL,      /**< No song info is given. */
    VISUAL_SONGINFO_TYPE_SIMPLE,    /**< Simple interface only sets a songname. */
    VISUAL_SONGINFO_TYPE_ADVANCED   /**< Advanced interface splits all the stats
                                       * in separated entries */
} VisSongInfoType;


#ifdef __cplusplus

#include <string>

namespace LV {

  enum SongInfoType
  {
      SONG_INFO_TYPE_NULL     = ::VISUAL_SONGINFO_TYPE_NULL,
      SONG_INFO_TYPE_SIMPLE   = ::VISUAL_SONGINFO_TYPE_SIMPLE,
      SONG_INFO_TYPE_ADVANCED = ::VISUAL_SONGINFO_TYPE_ADVANCED
  };

  /**
   * Song info class. Contains information about the current played
   * song. Information like artist, album, song, elapsed time and even
   * coverart can be set using the methods of the VisSongInfo system.
   */
  class SongInfo
  {
  public:

      SongInfo (SongInfoType type);

      ~SongInfo ();

      friend bool operator== (SongInfo const& lhs, SongInfo const& rhs);

      /**
       * Sets the interface type to a VisSongInfo. Used to set the interface
       * type to the VisSongInfo structure. The interface type defines if
       * we're providing a simple string containing the song name or an
       * separated set of data containing all the information about a song.
       *
       * @param type Interface type that is set against the VisSongInfo.
       *
       * @return 0 on succes -1 on failure.
       */
      void set_type (SongInfoType type);

      SongInfoType get_type () const;

      /**
       * Sets the length of a song. Used to set the length of a song
       * when the advanced interface is being used.
       *
       * @param length The length in seconds.
       */
      void set_length (int length);

      int get_length () const;

      /**
       * Sets the elapsed time of a song. Used to set the elapsed time
       * of a song when the advanced interface is being used.
       *
       * @param elapsed The elapsed time in seconds.
       */
      void set_elapsed (int elapsed);

      int get_elapsed () const;

      /**
       * Sets a simple song name. Used when the simple interface is
       * being used to set a song name.
       *
       * @param name The simple song name.
       */
      void set_simple_name (std::string const& name);

      std::string get_simple_name () const;

      /**
       * Sets the artist name. Used to set the artist name when the advanced
       * interface is being used.
       *
       * @param songinfo Pointer to a VisSongInfo to which the artist name is set.
       * @param artist The artist name.
       *
       * @return 0 on succes -1 on failure.
       */
      void set_artist (std::string const& artist);

      std::string get_artist () const;

      /**
       * Sets the album name. Used to set the album name when the
       * advanced interface is being used.
       *
       * @param album The album name.
       */
      void set_album (std::string const& album);

      std::string get_album () const;

      /**
       * Sets the song name. Used to set the song name when the advanced
       * interface is being used.
       *
       * @param song The song name.
       */
      void set_song (std::string const& name);

      std::string get_song () const;

      /**
       * Sets the cover art. Used to set the cover art when the advanced
       * interface is being used.
       *
       * @param cover VisVideo object containing the cover art.
       *
       * @return 0 on succes -1 on failure.
       */
      void set_cover (VisVideo* cover);

      VisVideo* get_cover () const;

      /**
       * Resets the age timer. Used to timestamp a song to the current
       * time.
       *
       * @param songinfo Pointer to a VisSongInfo that is timestamped.
       *
       * @return 0 on succes -1 on failure.
       */
      void mark ();

      /**
       * Returns the age of the VisSongInfo.
       *
       * @return The age in seconds.
       */
      long get_age (); // FIXME: should be const

  private:

      SongInfoType     type;
      int              length;
      int              elapsed;
      std::string      song_name;
      std::string      artist;
      std::string      album;
      std::string      song;
      VisTimer         timer;
      VisVideo        *cover;
  };

} // LV namespace

#endif /* __cplusplus */

VISUAL_BEGIN_DECLS

#define VISUAL_SONGINFO(obj)                (VISUAL_CHECK_CAST ((obj), VisSongInfo))

#ifdef __cplusplus
typedef ::LV::SongInfo VisSongInfo;
#else
typedef struct _VisSongInfo VisSongInfo;
struct _VisSongInfo;
#endif

VisSongInfo *visual_songinfo_new (VisSongInfoType type);

void visual_songinfo_free (VisSongInfo *songinfo);

void visual_songinfo_copy (VisSongInfo *dest, VisSongInfo const *src);

int visual_songinfo_compare (VisSongInfo const *s1, VisSongInfo const *s2);

void            visual_songinfo_set_type (VisSongInfo *songinfo, VisSongInfoType type);
VisSongInfoType visual_songinfo_get_type (VisSongInfo *songinfo);

void visual_songinfo_set_length (VisSongInfo *songinfo, int length);
int  visual_songinfo_get_length (VisSongInfo *songinfo);

void visual_songinfo_set_elapsed (VisSongInfo *songinfo, int elapsed);
int  visual_songinfo_get_elapsed (VisSongInfo *songinfo);

void        visual_songinfo_set_simple_name (VisSongInfo *songinfo, const char *name);
const char *visual_songinfo_get_simple_name (VisSongInfo *songinfo);

void        visual_songinfo_set_artist (VisSongInfo *songinfo, const char *artist);
const char *visual_songinfo_get_artist (VisSongInfo *songinfo);

void        visual_songinfo_set_album (VisSongInfo *songinfo, const char *album);
const char *visual_songinfo_get_album (VisSongInfo *songinfo);

void        visual_songinfo_set_song (VisSongInfo *songinfo, const char *song);
const char *visual_songinfo_get_song (VisSongInfo *songinfo);

void        visual_songinfo_set_cover (VisSongInfo *songinfo, VisVideo *cover);
VisVideo   *visual_songinfo_get_cover (VisSongInfo *songinfo);

void visual_songinfo_mark (VisSongInfo *songinfo);

long visual_songinfo_get_age (VisSongInfo *songinfo);


VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_SONGINFO_H */
