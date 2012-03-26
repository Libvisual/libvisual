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

VISUAL_BEGIN_DECLS

#define VISUAL_SONGINFO(obj)				(VISUAL_CHECK_CAST ((obj), VisSongInfo))

/**
 * Used to define the type of song info being used.
 * There are two interfaces to notify libvisual of song
 * information, being a simple and a advanced interface.
 */
typedef enum {
	VISUAL_SONGINFO_TYPE_NULL,	/**< No song info is given. */
	VISUAL_SONGINFO_TYPE_SIMPLE,	/**< Simple interface only sets a songname. */
	VISUAL_SONGINFO_TYPE_ADVANCED	/**< Advanced interface splits all the stats
					  * in separated entries */
} VisSongInfoType;

typedef struct _VisSongInfo VisSongInfo;

/**
 * Song info data structure.
 *
 * Contains information about the current played song. Information like
 * artist, album, song, elapsed time and even coverart can be set using the
 * methods of the VisSongInfo system.
 */
struct _VisSongInfo {
	VisObject	 object;	/**< The VisObject data. */
	VisSongInfoType	 type;		/**< Sets the interface type. */

	/* In seconds */
	int		 length;	/**< Total length of the song playing. */
	int		 elapsed;	/**< Elapsed time of the song playing. */

	/* Simple type */
	char		*songname;	/**< A string containing the song name using
					  * the simple interface. */

	/* Advanced type */
	char		*artist;	/**< A string containing the artist name using
					  * the advanced interface. */
	char		*album;		/**< A string containing the album name using
					  * the advanced interface. */
	char		*song;		/**< A string containing the song name using
					  * the advanced interface. */

	/* Timing */
	VisTimer	 timer;		/**< Used to internal timing to keep track on the
					  * age of the record. */
	/* Cover art */
	VisVideo	*cover;		/**< Pointer to a VisVideo that contains the cover art. */
};

/**
 * Creates a new VisSongInfo structure.
 *
 * @param type Type of interface being used.
 *
 * @return 0 on succes -1 on failure.
 */
VisSongInfo *visual_songinfo_new (VisSongInfoType type);

int visual_songinfo_init (VisSongInfo *songinfo, VisSongInfoType type);

/**
 * Frees all the strings within the structure. This frees all the
 * strings used by the structure.
 *
 * @param songinfo Pointer to a VisSongInfo of which the strings need to
 * 	freed.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_free_strings (VisSongInfo *songinfo);

/**
 * Sets the interface type to a VisSongInfo. Used to set the interface
 * type to the VisSongInfo structure. The interface type defines if
 * we're providing a simple string containing the song name or an
 * separated set of data containing all the information about a song.
 *
 * @param songinfo Pointer to a VisSongInfo to which the interface type is set.
 * @param type Interface type that is set against the VisSongInfo.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_type (VisSongInfo *songinfo, VisSongInfoType type);


/**
 * Sets the length of a song. Used to set the length of a song when
 * the advanced interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the song length is set.
 * @param length The length in seconds.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_length (VisSongInfo *songinfo, int length);

/**
 * Sets the elapsed time of a song. Used to set the elapsed time of a
 * song when the advanced interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the elapsed time is set.
 * @param elapsed The elapsed time in seconds.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_elapsed (VisSongInfo *songinfo, int elapsed);


/**
 * Sets a simple song name. Used when the simple interface is being
 * used to set a song name.
 *
 * @param songinfo Pointer to a VisSongInfo to which the simple song name is set.
 * @param name The simple song name.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_simple_name (VisSongInfo *songinfo, char *name);

/**
 * Sets the artist name. Used to set the artist name when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the artist name is set.
 * @param artist The artist name.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_artist (VisSongInfo *songinfo, char *artist);

/**
 * Sets the album name. Used to set the album name when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the album name is set.
 * @param album The album name.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_album (VisSongInfo *songinfo, char *album);

/**
 * Sets the song name. Used to set the song name when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the song name is set.
 * @param song The song name.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_song (VisSongInfo *songinfo, char *song);

/**
 * Sets the cover art. Used to set the cover art when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the cover art is set.
 * @param cover Pointer to a VisVideo containing the cover art.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_set_cover (VisSongInfo *songinfo, VisVideo *cover);

/**
 * Resets the age timer. Used to timestamp a song to the current
 * time.
 *
 * @param songinfo Pointer to a VisSongInfo that is timestamped.
 *
 * @return 0 on succes -1 on failure.
 */
int visual_songinfo_mark (VisSongInfo *songinfo);


/**
 * Gives the age of the VisSongInfo. Returns the age in seconds
 * stored in a long.
 *
 * @param songinfo Pointer to a VisSongInfo of which the age is requested.
 *
 * @return The age in seconds.
 */
long visual_songinfo_age (VisSongInfo *songinfo);


/**
 * Copies the content of a VisSongInfo. Used to copy the content of
 * a VisSongInfo in that of another.
 *
 * @param dest Pointer to the destination VisSongInfo.
 * @param src Pointer to the source VisSongInfo.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_SONGINFO_NULL on failure.
 */
int visual_songinfo_copy (VisSongInfo *dest, VisSongInfo *src);


/**
 * Compares the VisSongInfo strings. Used to compare the content
 * of two VisSongInfos by comparing their strings. This can be used
 * to detect if there is a song change.
 *
 * @param s1 Pointer to the first VisSongInfo.
 * @param s2 Pointer to the second VisSongInfo.
 *
 * @return FALSE on different, TRUE on same, -VISUAL_ERROR_SONGINFO_NULL on failure.
 */
int visual_songinfo_compare (VisSongInfo *s1, VisSongInfo *s2);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_SONGINFO_H */
