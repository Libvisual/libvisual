/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_serialize.h,v 1.6 2006-09-19 19:05:47 synap Exp $
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

#ifndef _LV_AVS_SERIALIZE_H
#define _LV_AVS_SERIALIZE_H

#include <libvisual/libvisual.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AVS_SERIALIZE_CONTAINER(obj)			(VISUAL_CHECK_CAST ((obj), AVSSerializeContainer))

/* Byte array retrieving / traversing helper macros */
#define AVS_SERIALIZE_GET_BYTE(f)		(*(f))
#define AVS_SERIALIZE_GET_INT(f)		(*((int *) f))
#define AVS_SERIALIZE_SKIP_INT(f)		((f) += 4)
#define AVS_SERIALIZE_SKIP_BYTE(f)		((f)++)
#define AVS_SERIALIZE_SKIP_LENGTH(f,i)		((f) += (i))
#define AVS_SERIALIZE_GET_NEXT_SECTION(f)	((f) + *f + 4)

typedef struct _AVSSerializeContainer AVSSerializeContainer;
typedef struct _AVSSerializeEntry AVSSerializeEntry;

typedef enum {
	AVS_SERIALIZE_ENTRY_TYPE_BYTE,
	AVS_SERIALIZE_ENTRY_TYPE_BYTE_WITH_INT_SKIP,
	AVS_SERIALIZE_ENTRY_TYPE_INT,
	AVS_SERIALIZE_ENTRY_TYPE_STRING,
	AVS_SERIALIZE_ENTRY_TYPE_COLOR,
	AVS_SERIALIZE_ENTRY_TYPE_PALETTE
} AVSSerializeEntryType;

/* The AVS data layout description structures for the automatic serializer / deserializer */
struct _AVSSerializeContainer {
	VisObject object;

	VisList layout;
};

struct _AVSSerializeEntry {
	VisObject object;

	AVSSerializeEntryType type;

	VisParamEntry *param;

	int boundry;
};

/* Prototypes */
char *avs_serialize_retrieve_palette_from_preset_section (char *section, VisParamEntry *param);
char *avs_serialize_retrieve_color_from_preset_section (char *section, VisParamEntry *param);
char *avs_serialize_retrieve_string_from_preset_section (char *section, VisParamEntry *param);

AVSSerializeContainer *avs_serialize_container_new (void);
int avs_serialize_container_add_string (AVSSerializeContainer *scont, VisParamEntry *param);
int avs_serialize_container_add_byte (AVSSerializeContainer *scont, VisParamEntry *param);
int avs_serialize_container_add_byte_with_boundry (AVSSerializeContainer *scont, VisParamEntry *param, int boundry);
int avs_serialize_container_add_byte_int_skip (AVSSerializeContainer *scont, VisParamEntry *param);
int avs_serialize_container_add_byte_int_skip_with_boundry (AVSSerializeContainer *scont, VisParamEntry *param, int boundry);
int avs_serialize_container_add_int (AVSSerializeContainer *scont, VisParamEntry *param);
int avs_serialize_container_add_int_with_boundry (AVSSerializeContainer *scont, VisParamEntry *param, int boundry);
int avs_serialize_container_add_palette (AVSSerializeContainer *scont, VisParamEntry *param);
int avs_serialize_container_add_color (AVSSerializeContainer *scont, VisParamEntry *param);
int avs_serialize_container_add (AVSSerializeContainer *scont, AVSSerializeEntry *sentry);
char *avs_serialize_container_deserialize (AVSSerializeContainer *scont, char *section);

AVSSerializeEntry *avs_serialize_entry_new (VisParamEntry *param);
AVSSerializeEntry *avs_serialize_entry_new_string (VisParamEntry *param);
AVSSerializeEntry *avs_serialize_entry_new_byte (VisParamEntry *param);
AVSSerializeEntry *avs_serialize_entry_new_byte_with_boundry (VisParamEntry *param, int boundry);
AVSSerializeEntry *avs_serialize_entry_new_byte_int_skip (VisParamEntry *param);
AVSSerializeEntry *avs_serialize_entry_new_byte_int_skip_with_boundry (VisParamEntry *param, int boundry);
AVSSerializeEntry *avs_serialize_entry_new_int (VisParamEntry *param);
AVSSerializeEntry *avs_serialize_entry_new_int_with_boundry (VisParamEntry *param, int boundry);
AVSSerializeEntry *avs_serialize_entry_new_palette (VisParamEntry *param);
AVSSerializeEntry *avs_serialize_entry_new_color (VisParamEntry *param);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_AVS_SERIALIZE_H */
