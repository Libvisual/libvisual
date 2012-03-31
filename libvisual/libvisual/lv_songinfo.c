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

static int songinfo_dtor (VisObject *object)
{
	VisSongInfo *songinfo = VISUAL_SONGINFO (object);

	visual_songinfo_free_strings (songinfo);

	if (songinfo->cover != NULL)
		visual_object_unref (VISUAL_OBJECT (songinfo->cover));

	songinfo->cover = NULL;

	return VISUAL_OK;
}

VisSongInfo *visual_songinfo_new (VisSongInfoType type)
{
	VisSongInfo *songinfo;

	songinfo = visual_mem_new0 (VisSongInfo, 1);

	visual_songinfo_init (songinfo, type);

	/* Do the VisObject initialization */
	visual_object_set_allocated (VISUAL_OBJECT (songinfo), TRUE);
	visual_object_ref (VISUAL_OBJECT (songinfo));

	return songinfo;
}

int visual_songinfo_init (VisSongInfo *songinfo, VisSongInfoType type)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	/* Do the VisObject initialization */
	visual_object_clear (VISUAL_OBJECT (songinfo));
	visual_object_set_dtor (VISUAL_OBJECT (songinfo), songinfo_dtor);
	visual_object_set_allocated (VISUAL_OBJECT (songinfo), FALSE);

	/* Set the VisSongInfo data */
	songinfo->type = type;

	return VISUAL_OK;
}

int visual_songinfo_free_strings (VisSongInfo *songinfo)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (songinfo->songname != NULL)
		visual_mem_free (songinfo->songname);

	if (songinfo->artist != NULL)
		visual_mem_free (songinfo->artist);

	if (songinfo->album != NULL)
		visual_mem_free (songinfo->album);

	if (songinfo->song != NULL)
		visual_mem_free (songinfo->song);

	songinfo->songname = NULL;
	songinfo->artist = NULL;
	songinfo->album = NULL;
	songinfo->song = NULL;

	return VISUAL_OK;
}

int visual_songinfo_set_type (VisSongInfo *songinfo, VisSongInfoType type)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	songinfo->type = type;

	return VISUAL_OK;
}

int visual_songinfo_set_length (VisSongInfo *songinfo, int length)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	songinfo->length = length;

	return VISUAL_OK;
}

int visual_songinfo_set_elapsed (VisSongInfo *songinfo, int elapsed)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	songinfo->elapsed = elapsed;

	return VISUAL_OK;
}

int visual_songinfo_set_simple_name (VisSongInfo *songinfo, char *name)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (songinfo->songname != NULL)
		visual_mem_free (songinfo->songname);

	songinfo->songname = visual_strdup (name);

	return VISUAL_OK;
}

int visual_songinfo_set_artist (VisSongInfo *songinfo, char *artist)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (songinfo->artist != NULL)
		visual_mem_free (songinfo->artist);

	songinfo->artist = visual_strdup (artist);

	return VISUAL_OK;
}

int visual_songinfo_set_album (VisSongInfo *songinfo, char *album)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (songinfo->album != NULL)
		visual_mem_free (songinfo->album);

	songinfo->album = visual_strdup (album);

	return VISUAL_OK;
}

int visual_songinfo_set_song (VisSongInfo *songinfo, char *song)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (songinfo->song != NULL)
		visual_mem_free (songinfo->song);

	songinfo->song = visual_strdup (song);

	return VISUAL_OK;
}

int visual_songinfo_set_cover (VisSongInfo *songinfo, VisVideo *cover)
{
	VisParamContainer *params;
	VisParamEntry *xparam;
	VisParamEntry *yparam;
	int cawidth = 64;
	int caheight = 64;

	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (songinfo->cover != NULL)
		visual_object_unref (VISUAL_OBJECT (songinfo->cover));

	/* Get the desired cover art size */
	params = visual_get_params ();
	xparam = visual_param_container_get (params, "songinfo cover size x");
	yparam = visual_param_container_get (params, "songinfo cover size y");

	if (xparam != NULL && yparam != NULL) {
		cawidth = visual_param_entry_get_integer (xparam);
		caheight = visual_param_entry_get_integer (yparam);
	}

	/* The coverart image */
	songinfo->cover = visual_video_scale_depth_new (cover, cawidth, caheight,
			VISUAL_VIDEO_DEPTH_32BIT,
			VISUAL_VIDEO_SCALE_BILINEAR);

	return VISUAL_OK;
}

int visual_songinfo_mark (VisSongInfo *songinfo)
{
	visual_return_val_if_fail (songinfo != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	visual_timer_start (&songinfo->timer);

	return VISUAL_OK;
}

long visual_songinfo_age (VisSongInfo *songinfo)
{
	VisTime cur;

	visual_time_get (&cur);

	/* Clock has been changed into the past */
	if (cur.sec < songinfo->timer.start.sec)
		visual_songinfo_mark (songinfo);

	visual_time_difference (&cur, &songinfo->timer.start, &cur);

	return cur.sec;
}

int visual_songinfo_copy (VisSongInfo *dest, VisSongInfo *src)
{
	visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_SONGINFO_NULL);
	visual_return_val_if_fail (src != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	dest->type = src->type;
	dest->length = src->length;
	dest->elapsed = src->elapsed;

	visual_mem_copy (&dest->timer, &src->timer, sizeof (VisTimer));

	if (src->songname != NULL)
		dest->songname = visual_strdup (src->songname);

	if (src->artist != NULL)
		dest->artist = visual_strdup (src->artist);

	if (src->album != NULL)
		dest->album = visual_strdup (src->album);

	if (src->song != NULL)
		dest->song = visual_strdup (src->song);

	/* Point the coverart VisVideo to that of the original and up it's refcount */
	dest->cover = src->cover;

	if (src->cover != NULL)
		visual_object_ref (VISUAL_OBJECT (src->cover));

	return VISUAL_OK;
}

int visual_songinfo_compare (VisSongInfo *s1, VisSongInfo *s2)
{
	int diff = 0;

	visual_return_val_if_fail (s1 != NULL, -VISUAL_ERROR_SONGINFO_NULL);
	visual_return_val_if_fail (s2 != NULL, -VISUAL_ERROR_SONGINFO_NULL);

	if (s1->songname != NULL && s2->songname != NULL) {
		if (strcmp (s1->songname, s2->songname) != 0)
			diff++;
	} else if ((s1->songname == NULL || s2->songname == NULL) &&
			!(s1->songname == NULL && s2->songname == NULL)) {
		diff++;
	}

	if (s1->artist != NULL && s2->artist != NULL) {
		if (strcmp (s1->artist, s2->artist) != 0)
			diff++;
	} else if ((s1->artist == NULL || s2->artist == NULL) &&
			!(s1->artist == NULL && s2->artist == NULL)) {
		diff++;
	}

	if (s1->album != NULL && s2->album != NULL) {
		if (strcmp (s1->album, s2->album) != 0)
			diff++;
	} else if ((s1->album == NULL || s2->album == NULL) &&
			!(s1->album == NULL && s2->album == NULL)) {
		diff++;
	}

	if (s1->song != NULL && s2->song != NULL) {
		if (strcmp (s1->song, s2->song) != 0)
			diff++;
	} else if ((s1->song == NULL || s2->song == NULL) &&
			!(s1->song == NULL && s2->song == NULL)) {
		diff++;
	}

	return (diff > 0 ? FALSE : TRUE);
}
