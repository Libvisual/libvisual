/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_serialize.c,v 1.5 2006-09-19 19:05:47 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libvisual/libvisual.h>

#include "avs_serialize.h"
#include "avs_parse.h"

static int avs_data_serialize_container_dtor (VisObject *object);

/* Object destructors */
static int avs_data_serialize_container_dtor (VisObject *object)
{
	AVSSerializeContainer *scont = AVS_SERIALIZE_CONTAINER (object);

	visual_collection_set_destroyer (VISUAL_COLLECTION (&scont->layout), visual_object_collection_destroyer);
	visual_collection_destroy (VISUAL_COLLECTION (&scont->layout));

	return TRUE;
}

/* Static parser helper functions */
char *avs_serialize_retrieve_palette_from_preset_section (char *section, VisParamEntry *param)
{
	int i;
	VisPalette pal;

	visual_palette_allocate_colors (&pal, AVS_SERIALIZE_GET_BYTE (section));

	AVS_SERIALIZE_SKIP_INT (section);

	for (i = 0; i < pal.ncolors; i++) {
		pal.colors[i].r = AVS_SERIALIZE_GET_BYTE (section);
		AVS_SERIALIZE_SKIP_BYTE (section);
		pal.colors[i].g = AVS_SERIALIZE_GET_BYTE (section);
		AVS_SERIALIZE_SKIP_BYTE (section);
		pal.colors[i].b = AVS_SERIALIZE_GET_BYTE (section);
		AVS_SERIALIZE_SKIP_BYTE (section);

		AVS_SERIALIZE_SKIP_BYTE (section);
	}

	visual_param_entry_set_palette (param, &pal);

	visual_palette_free_colors (&pal);

	return section;
}

char *avs_serialize_retrieve_color_from_preset_section (char *section, VisParamEntry *param)
{
	unsigned char r, g, b;

	b = AVS_SERIALIZE_GET_BYTE (section);
	AVS_SERIALIZE_SKIP_BYTE (section);
	g = AVS_SERIALIZE_GET_BYTE (section);
	AVS_SERIALIZE_SKIP_BYTE (section);
	r = AVS_SERIALIZE_GET_BYTE (section);
	AVS_SERIALIZE_SKIP_BYTE (section);

	visual_param_entry_set_color (param, r, g, b);

	AVS_SERIALIZE_SKIP_BYTE (section);

	return section;
}

char *avs_serialize_retrieve_string_from_preset_section (char *section, VisParamEntry *param)
{
	char *string;
	int len;
    int i;

	/* FIXME should just get an int ? */
	len = AVS_SERIALIZE_GET_INT (section);
	AVS_SERIALIZE_SKIP_INT (section);

	if (len > 0) {
		string = visual_mem_malloc0 (len);

		strncpy (string, section, len);

		visual_param_entry_set_string (param, string);

		visual_mem_free (string);
	}

	AVS_SERIALIZE_SKIP_LENGTH (section, len);

    printf("after length skip :%s\n", section);
	return section;
}



AVSSerializeContainer *avs_serialize_container_new ()
{
	AVSSerializeContainer *scont;

	scont = visual_mem_new0 (AVSSerializeContainer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (scont), TRUE, avs_data_serialize_container_dtor);

	return scont;
}

int avs_serialize_container_add (AVSSerializeContainer *scont, AVSSerializeEntry *sentry)
{
	visual_list_add (&scont->layout, sentry);

	return 0;
}

int avs_serialize_container_add_string (AVSSerializeContainer *scont, VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_string (param);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_byte (AVSSerializeContainer *scont, VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_byte (param);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_byte_with_boundry (AVSSerializeContainer *scont, VisParamEntry *param, int boundry)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_byte_with_boundry (param, boundry);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_byte_int_skip (AVSSerializeContainer *scont, VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_byte_int_skip (param);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_byte_int_skip_with_boundry (AVSSerializeContainer *scont, VisParamEntry *param, int boundry)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_byte_int_skip_with_boundry (param, boundry);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_int (AVSSerializeContainer *scont, VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_int (param);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_int_with_boundry (AVSSerializeContainer *scont, VisParamEntry *param, int boundry)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_int_with_boundry (param, boundry);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_palette (AVSSerializeContainer *scont, VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_palette (param);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

int avs_serialize_container_add_color (AVSSerializeContainer *scont, VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new_color (param);

	avs_serialize_container_add (scont, sentry);

	return 0;
}

char *avs_serialize_container_deserialize (AVSSerializeContainer *scont, char *section)
{
	AVSSerializeEntry *sentry;
	VisListEntry *le = NULL;

	while ((sentry = visual_list_next (&scont->layout, &le)) != NULL) {
		switch (sentry->type) {
			case AVS_SERIALIZE_ENTRY_TYPE_BYTE:
				if (sentry->param != NULL) {
					visual_param_entry_set_integer (sentry->param, AVS_SERIALIZE_GET_BYTE (section));

					if (visual_param_entry_get_integer (sentry->param) > sentry->boundry && sentry->boundry > 0) {
						visual_log (VISUAL_LOG_WARNING, "A serialized entry did hit the upper value boundry");

						visual_param_entry_set_integer (sentry->param, 0);
					}
				}

				AVS_SERIALIZE_SKIP_BYTE (section);

				break;

			case AVS_SERIALIZE_ENTRY_TYPE_BYTE_WITH_INT_SKIP:
				if (sentry->param != NULL) {
					visual_param_entry_set_integer (sentry->param, AVS_SERIALIZE_GET_BYTE (section));

					if (visual_param_entry_get_integer (sentry->param) > sentry->boundry && sentry->boundry > 0) {
						visual_log (VISUAL_LOG_WARNING, "A serialized entry did hit the upper value boundry");

						visual_param_entry_set_integer (sentry->param, 0);
					}
				}

				AVS_SERIALIZE_SKIP_INT (section);

				break;

			case AVS_SERIALIZE_ENTRY_TYPE_INT:
				if (sentry->param != NULL) {
					// use get_int here... instead of get_byte
					visual_param_entry_set_integer (sentry->param, AVS_SERIALIZE_GET_BYTE (section));

					if (visual_param_entry_get_integer (sentry->param) > sentry->boundry && sentry->boundry > 0) {
						visual_log (VISUAL_LOG_WARNING, "A serialized entry did hit the upper value boundry");

						visual_param_entry_set_integer (sentry->param, 0);
					}
				}

				AVS_SERIALIZE_SKIP_INT (section);

				break;

			case AVS_SERIALIZE_ENTRY_TYPE_STRING:

				section = avs_serialize_retrieve_string_from_preset_section (section, sentry->param);

				break;

			case AVS_SERIALIZE_ENTRY_TYPE_COLOR:

				section = avs_serialize_retrieve_color_from_preset_section (section, sentry->param);

				break;

			case AVS_SERIALIZE_ENTRY_TYPE_PALETTE:

				section = avs_serialize_retrieve_palette_from_preset_section (section, sentry->param);

				break;

			default:
				printf ("INVALID SERIALIZE TYPE, BAILING OUT VERY HARDLY\n");
				return NULL;

				break;

		}

	}

	return section;
}

AVSSerializeEntry *avs_serialize_entry_new (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = visual_mem_new0 (AVSSerializeEntry, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (sentry), TRUE, NULL);

	sentry->param = param;

	/* No boundry */
	sentry->boundry = -1;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_string (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_STRING;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_byte (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_BYTE;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_byte_with_boundry (VisParamEntry *param, int boundry)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_BYTE;

	sentry->boundry = boundry;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_byte_int_skip (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_BYTE_WITH_INT_SKIP;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_byte_int_skip_with_boundry (VisParamEntry *param, int boundry)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_BYTE_WITH_INT_SKIP;

	sentry->boundry = boundry;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_int (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_INT;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_int_with_boundry (VisParamEntry *param, int boundry)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_INT;

	sentry->boundry = boundry;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_palette (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_PALETTE;

	return sentry;
}

AVSSerializeEntry *avs_serialize_entry_new_color (VisParamEntry *param)
{
	AVSSerializeEntry *sentry;

	sentry = avs_serialize_entry_new (param);

	sentry->type = AVS_SERIALIZE_ENTRY_TYPE_COLOR;

	return sentry;
}

