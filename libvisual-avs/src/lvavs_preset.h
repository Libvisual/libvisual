/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
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

#ifndef _LV_LVAVS_PRESET_H
#define _LV_LVAVS_PRESET_H

#include <libvisual/libvisual.h>

#include "avs_parse.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LVAVS_PRESET(obj)				(VISUAL_CHECK_CAST ((obj), 0, LVAVSPreset))
#define LVAVS_PRESET_ELEMENT(obj)			(VISUAL_CHECK_CAST ((obj), 0, LVAVSPResetElement))
#define LVAVS_PRESET_CONTAINER(obj)			(VISUAL_CHECK_CAST ((obj), 0, LVAVSPresetContainer))

typedef struct _LVAVSPreset LVAVSPreset;
typedef struct _LVAVSElement LVAVSElement;
typedef struct _LVAVSContainer LVAVSContainer;

typedef enum {
	LVAVS_PRESET_ELEMENT_TYPE_NULL,
	LVAVS_PRESET_ELEMENT_TYPE_PLUGIN,
	LVAVS_PRESET_ELEMENT_TYPE_CONTAINER,
	LVAVS_PRESET_ELEMENT_TYPE_RENDERMODE,
	LVAVS_PRESET_ELEMENT_TYPE_COMMENT
} LVAVSPresetElementType;

/* The AVS data structure */
struct _LVAVSPreset {
	VisObject		 object;

	char			*origfile;

	LVAVSPresetContainer	*main;
};

struct _LVAVSPresetElement {
	VisObject	 	 object;

	LVAVSPresetElementType	 type;
	
	const char		*element_name;

	VisParamContainer	*pcont;
};

struct _LVAVSPresetContainer {
	LVAVSPresetElement	 element;
	
	VisList			*members;
};


/* Prototypes */
LVAVSPreset *lvavs_preset_new (void);
LVAVSPreset *lvavs_preset_new_from_preset (char *filename);
LVAVSPreset *lvavs_preset_new_from_wavs (AVSTree *wavs);

LVAVSPresetElement *lvavs_preset_element_new (LVAVSPresetElementType type, const char *name);
LVAVSPresetContainer *lvavs_preset_container_new (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LVAVS_PRESET_H */
