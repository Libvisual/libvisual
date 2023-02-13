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

#ifndef _LV_SONGINFO_H
#define _LV_SONGINFO_H

#include <libvisual/lv_types.h>
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
#include <string_view>

namespace LV {

  enum SongInfoType
  {
      SONG_INFO_TYPE_NULL     = ::VISUAL_SONGINFO_TYPE_NULL,
      SONG_INFO_TYPE_SIMPLE   = ::VISUAL_SONGINFO_TYPE_SIMPLE,
      SONG_INFO_TYPE_ADVANCED = ::VISUAL_SONGINFO_TYPE_ADVANCED
  };

  //!
  //! Song information class.
  //!
  //! Contains information about a song such as artist, album, song, elapsed
  //! time and cover art.
  //!
  class LV_API SongInfo
  {
  public:

      explicit SongInfo (SongInfoType type);

      ~SongInfo ();

      friend bool operator== (SongInfo const& lhs, SongInfo const& rhs);

      /**
       * Sets the song information type.
       *
       * @note The type determines the set of information supplied by
       * the SongInfo object.
       *
       * @param type Interface type that is set against the VisSongInfo.
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
      void set_simple_name (std::string_view name);

      /**
       * Returns the simple song name.
       *
       * @return name
       */
      std::string const& get_simple_name () const;

      /**
       * Sets the length of a song.
       *
       * @note Advanced interface only
       *
       * @param length length in seconds
       */
      void set_length (int length);

      /**
       * Returns the length of the song
       *
       * @return length in seconds
       */
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
      void set_artist (std::string_view artist);

      std::string const& get_artist () const;

      /**
       * Sets the album name.
       *
       * @note Advanced interface only
       *
       * @param album album name to set
       */
      void set_album (std::string_view album);

      std::string const& get_album () const;

      /**
       * Sets the song name.
       *
       * @note Advanced interface only
       *
       * @param name song name to set
       */
      void set_song (std::string_view name);

      std::string const& get_song () const;

      /**
       * Sets the cover art.
       *
       * @note Advanced interface only
       *
       * @param cover VisVideo object containing the cover art.
       */
      void set_cover (VideoPtr const& cover);

      VideoPtr get_cover () const;

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

      SongInfoType m_type;
      int          m_length;
      int          m_elapsed;
      std::string  m_song_name;
      std::string  m_artist;
      std::string  m_album;
      std::string  m_song;
      Timer        m_timer;
      VideoPtr     m_cover;
  };

} // LV namespace

#endif /* __cplusplus */

#ifdef __cplusplus
typedef ::LV::SongInfo VisSongInfo;
#else
typedef struct _VisSongInfo VisSongInfo;
struct _VisSongInfo;
#endif

LV_BEGIN_DECLS

LV_NODISCARD LV_API VisSongInfo *visual_songinfo_new   (VisSongInfoType type);
LV_NODISCARD LV_API VisSongInfo *visual_songinfo_clone (VisSongInfo const *src);

LV_API void visual_songinfo_free (VisSongInfo *songinfo);

LV_API void visual_songinfo_copy    (VisSongInfo *dest, VisSongInfo const *src);
LV_API int  visual_songinfo_compare (VisSongInfo const *s1, VisSongInfo const *s2);

LV_API void            visual_songinfo_set_type (VisSongInfo *songinfo, VisSongInfoType type);
LV_API VisSongInfoType visual_songinfo_get_type (VisSongInfo const *songinfo);

LV_API void visual_songinfo_set_length (VisSongInfo *songinfo, int length);
LV_API int  visual_songinfo_get_length (VisSongInfo const *songinfo);

LV_API void visual_songinfo_set_elapsed (VisSongInfo *songinfo, int elapsed);
LV_API int  visual_songinfo_get_elapsed (VisSongInfo const *songinfo);

LV_API void        visual_songinfo_set_simple_name (VisSongInfo *songinfo, const char *name);
LV_API const char *visual_songinfo_get_simple_name (VisSongInfo const *songinfo);

LV_API void        visual_songinfo_set_artist (VisSongInfo *songinfo, const char *artist);
LV_API const char *visual_songinfo_get_artist (VisSongInfo const *songinfo);

LV_API void        visual_songinfo_set_album (VisSongInfo *songinfo, const char *album);
LV_API const char *visual_songinfo_get_album (VisSongInfo const *songinfo);

LV_API void        visual_songinfo_set_song (VisSongInfo *songinfo, const char *song);
LV_API const char *visual_songinfo_get_song (VisSongInfo const *songinfo);

LV_API void        visual_songinfo_set_cover (VisSongInfo *songinfo, VisVideo *cover);
LV_API VisVideo   *visual_songinfo_get_cover (VisSongInfo const *songinfo);

LV_API void visual_songinfo_mark (VisSongInfo *songinfo);

LV_API long visual_songinfo_get_age (VisSongInfo *songinfo);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_SONGINFO_H */
