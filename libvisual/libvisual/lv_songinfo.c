#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "lv_songinfo.h"

/**
 * @defgroup VisSongInfo VisSongInfo
 * @{
 */

/**
 * Creates a new VisSongInfo structure.
 *
 * @param type Type of interface being used.
 *
 * @return 0 on succes -1 on error.
 */
VisSongInfo *visual_songinfo_new (VisSongInfoType type)
{
	VisSongInfo *songinfo;

	songinfo = visual_mem_new0 (VisSongInfo, 1);

	songinfo->type = type;

	return songinfo;
}

/**
 * Frees the VisSongInfo. This frees the VisSongInfo data structure.
 *
 * @param songinfo Pointer to a VisSongInfo that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_free (VisSongInfo *songinfo)
{
	if (songinfo == NULL)
		return -1;

	visual_songinfo_free_strings (songinfo);

	visual_video_free_with_buffer (songinfo->cover);

	free (songinfo);

	return 0;
}

/**
 * Frees all the strings within the structure. This frees all the
 * strings used by the structure.
 *
 * @param songinfo Pointer to a VisSongInfo of which the strings need to
 * 	freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_free_strings (VisSongInfo *songinfo)
{
	if (songinfo == NULL)
		return -1;

	if (songinfo->songname != NULL)
		free (songinfo->songname);

	if (songinfo->artist != NULL)
		free (songinfo->artist);

	if (songinfo->album != NULL)
		free (songinfo->album);

	if (songinfo->song != NULL)
		free (songinfo->song);

	return 0;
}

/**
 * Sets the interface type to a VisSongInfo. Used to set the interface
 * type to the VisSongInfo structure. The interface type defines if we're
 * providing a simple string containing the song name or an separated
 * set of data containing all the information about a song.
 *
 * @param songinfo Pointer to a VisSongInfo to which the interface type is set.
 * @param type Interface type that is set against the VisSongInfo.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_type (VisSongInfo *songinfo, VisSongInfoType type)
{
	if (songinfo == NULL)
		return -1;
	
	songinfo->type = type;

	return 0;
}

/**
 * Sets the length of a song. Used to set the length of a song when the
 * advanced interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the song length is set.
 * @param length The length in seconds.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_length (VisSongInfo *songinfo, int length)
{
	if (songinfo == NULL)
		return -1;
	
	songinfo->length = length;

	return 0;
}

/**
 * Sets the elapsed time of a song. Used to set the elapsed time of a song when
 * the advanced interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the elapsed time is set.
 * @param elapsed The elapsed time in seconds.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_elapsed (VisSongInfo *songinfo, int elapsed)
{
	if (songinfo == NULL)
		return -1;
	
	songinfo->elapsed = elapsed;

	return 0;
}

/**
 * Sets a simple song name. Used when the simple interface is being used
 * to set a song name.
 *
 * @param songinfo Pointer to a VisSongInfo to which the simple song name is set.
 * @param name The simple song name.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_simple_name (VisSongInfo *songinfo, char *name)
{
	if (songinfo == NULL)
		return -1;
	
	if (songinfo->songname != NULL)
		free (songinfo->songname);

	songinfo->songname = strdup (name);

	return 0;
}

/**
 * Sets the artist name. Used to set the artist name when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the artist name is set.
 * @param artist The artist name.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_artist (VisSongInfo *songinfo, char *artist)
{
	if (songinfo == NULL)
		return -1;

	if (songinfo->artist != NULL)
		free (songinfo->artist);

	songinfo->artist = strdup (artist);

	return 0;
}

/**
 * Sets the album name. Used to set the album name when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the album name is set.
 * @param album The album name.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_album (VisSongInfo *songinfo, char *album)
{
	if (songinfo == NULL)
		return -1;

	if (songinfo->album != NULL)
		free (songinfo->album);

	songinfo->album = strdup (album);

	return 0;
}

/**
 * Sets the song name. Used to set the song name when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the song name is set.
 * @param song The song name.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_song (VisSongInfo *songinfo, char *song)
{
	if (songinfo == NULL)
		return -1;

	if (songinfo->song != NULL)
		free (songinfo->song);

	songinfo->song = strdup (song);

	return 0;
}

/**
 * Sets the cover art. Used to set the cover art when the advanced
 * interface is being used.
 *
 * @param songinfo Pointer to a VisSongInfo to which the cover art is set.
 * @param cover Pointer to a VisVideo containing the cover art.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_set_cover (VisSongInfo *songinfo, VisVideo *cover)
{
	if (songinfo == NULL)
		return -1;

	songinfo->cover = cover;

	return 0;
}

/**
 * Resets the age timer. Used to timestamp a song to the current
 * time.
 *
 * @param songinfo Pointer to a VisSongInfo that is timestamped.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_mark (VisSongInfo *songinfo)
{
	if (songinfo == NULL)
		return -1;
	
	songinfo->start = time (NULL);

	return 0;
}

/**
 * Gives the age of the VisSongInfo. Returns the age in seconds
 * stored in a time_t.
 *
 * @param songinfo Pointer to a VisSongInfo of which the age is requested.
 *
 * @return The age in seconds.
 */
time_t visual_songinfo_age (VisSongInfo *songinfo)
{
	time_t cur;

	cur = time (NULL);

	/* Clock has been changed */
	if (cur < songinfo->start)
		songinfo->start = cur;

	return cur - songinfo->start;
}

/**
 * Copies the content of a VisSongInfo. Used to copy the content of
 * a VisSongInfo in that of another.
 *
 * @param dest Pointer to the destination VisSongInfo.
 * @param src Pointer to the source VisSongInfo.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_copy (VisSongInfo *dest, VisSongInfo *src)
{
	dest->type = src->type;
	dest->length = src->length;
	dest->elapsed = src->elapsed;
	dest->start = src->start;

	if (dest == NULL || src == NULL)
		return -1;

	if (src->songname != NULL)
		dest->songname = strdup (src->songname);
	
	if (src->artist != NULL)
		dest->artist = strdup (src->artist);
	
	if (src->album != NULL)
		dest->album = strdup (src->album);
	
	if (src->song != NULL)
		dest->song = strdup (src->song);

	/* FIXME copy the cover art video as well */

	return 0;
}

/**
 * Compares the VisSongInfo strings. Used to compare the content
 * of two VisSongInfos by comparing their strings. This can be used
 * to detect if there is a song change.
 *
 * @param s1 Pointer to the first VisSongInfo.
 * @param s2 Pointer to the second VisSongInfo.
 *
 * @return 0 on succes -1 on error.
 */
int visual_songinfo_compare (VisSongInfo *s1, VisSongInfo *s2)
{
	int diff = 0;

	if (s1 == NULL || s2 == NULL)
		return -1;

	if (s1->songname != NULL && s2->songname != NULL) {
		if (strcmp (s1->songname, s2->songname) != 0)
			diff++;
	}

	if (s1->artist != NULL && s2->artist != NULL) {
		if (strcmp (s1->artist, s2->artist) != 0)
			diff++;
	}

	if (s1->album != NULL && s2->album != NULL) {
		if (strcmp (s1->album, s2->album) != 0)
			diff++;
	}

	if (s1->song != NULL && s2->song != NULL) {
		if (strcmp (s1->song, s2->song) != 0)
			diff++;
	}
	
	return diff;
}

/**
 * @}
 */

