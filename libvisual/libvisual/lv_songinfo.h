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
  class LV_API SongInfo
  {
  public:

      SongInfo (SongInfoType type);

      ~SongInfo ();

      friend bool operator== (SongInfo const& lhs, SongInfo const& rhs);

      /**
       * Sets the song information type.
       *
       * @note The type determines the set of information supplied by
       * the SongInfo object.
       *
       * @param type Interface type that is set against the VisSongInfo.
       *
       * @return 0 on succes -1 on failure.
       */
      // FIXME: Type should only be set at creation time. Ideally we
      // should be able prevent accidental access to invalid fields.
      void set_type (SongInfoType type);

      SongInfoType get_type () const;

      /**
       * Sets a simple song name. Used when the simple interface is
       * being used to set a song name.
       *
       * @param name The simple song name.
       */
      void set_simple_name (std::string const& name);

      std::string get_simple_name () const;

      /**
       * Sets the length of a song.
       *
       * @note Advanced interface only
       *
       * @param length length in seconds to set
       */
      void set_length (int length);

      int get_length () const;

      /**
       * Sets the elapsed time of a song.
       *
       * @note Advanced interface only
       *
       * @param elapsed The elapsed time in seconds.
       */
      void set_elapsed (int elapsed);

      int get_elapsed () const;

      /**
       * Sets the artist name.
       *
       * @note Advanced interface only
       *
       * @param artist artist name to set
       */
      void set_artist (std::string const& artist);

      std::string get_artist () const;

      /**
       * Sets the album name.
       *
       * @note Advanced interface only
       *
       * @param album album name to set
       */
      void set_album (std::string const& album);

      std::string get_album () const;

      /**
       * Sets the song name.
       *
       * @note Advanced interface only
       *
       * @param name song name to set
       */
      void set_song (std::string const& name);

      std::string get_song () const;

      /**
       * Sets the cover art.
       *
       * @note Advanced interface only
       *
       * @param cover VisVideo object containing the cover art.
       *
       * @return 0 on succes -1 on failure.
       */
      void set_cover (VisVideo* cover);

      VisVideo* get_cover () const;

      /**
       * Resets the age timer. Use this to timestamp a song to the
       * current time.
       */
      void mark ();

      /**
       * Returns the age of the song.
       *
       * @return age in seconds
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

#define VISUAL_SONGINFO(obj)                (VISUAL_CHECK_CAST ((obj), VisSongInfo))

#ifdef __cplusplus
typedef ::LV::SongInfo VisSongInfo;
#else
typedef struct _VisSongInfo VisSongInfo;
struct _VisSongInfo;
#endif

LV_BEGIN_DECLS

LV_API VisSongInfo *visual_songinfo_new (VisSongInfoType type);

LV_API void visual_songinfo_free (VisSongInfo *songinfo);

LV_API void visual_songinfo_copy (VisSongInfo *dest, VisSongInfo const *src);

LV_API int visual_songinfo_compare (VisSongInfo const *s1, VisSongInfo const *s2);

LV_API void            visual_songinfo_set_type (VisSongInfo *songinfo, VisSongInfoType type);
LV_API VisSongInfoType visual_songinfo_get_type (VisSongInfo *songinfo);

LV_API void visual_songinfo_set_length (VisSongInfo *songinfo, int length);
LV_API int  visual_songinfo_get_length (VisSongInfo *songinfo);

LV_API void visual_songinfo_set_elapsed (VisSongInfo *songinfo, int elapsed);
LV_API int  visual_songinfo_get_elapsed (VisSongInfo *songinfo);

LV_API void        visual_songinfo_set_simple_name (VisSongInfo *songinfo, const char *name);
LV_API const char *visual_songinfo_get_simple_name (VisSongInfo *songinfo);

LV_API void        visual_songinfo_set_artist (VisSongInfo *songinfo, const char *artist);
LV_API const char *visual_songinfo_get_artist (VisSongInfo *songinfo);

LV_API void        visual_songinfo_set_album (VisSongInfo *songinfo, const char *album);
LV_API const char *visual_songinfo_get_album (VisSongInfo *songinfo);

LV_API void        visual_songinfo_set_song (VisSongInfo *songinfo, const char *song);
LV_API const char *visual_songinfo_get_song (VisSongInfo *songinfo);

LV_API void        visual_songinfo_set_cover (VisSongInfo *songinfo, VisVideo *cover);
LV_API VisVideo   *visual_songinfo_get_cover (VisSongInfo *songinfo);

LV_API void visual_songinfo_mark (VisSongInfo *songinfo);

LV_API long visual_songinfo_get_age (VisSongInfo *songinfo);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_SONGINFO_H */
