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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libvisual/libvisual.h>

#include "lvavs_preset.h"

/* Prototypes */
static int lvavs_preset_dtor (VisObject *object);
static int lvavs_preset_element_dtor (VisObject *object);
static int lvavs_preset_container_dtor (VisObject *object);

static int preset_convert_from_wavs (LVAVSPresetContainer *presetcont, AVSContainer *cont);

LVAVSPresetElement *wavs_convert_main_new (AVSElement *avselem);
LVAVSPresetElement *wavs_convert_ring_new (AVSElement *avselem);


/* Object destructors */
static int lvavs_preset_dtor (VisObject *object)
{
	LVAVSPreset *preset = LVAVS_PRESET (object);

	if (preset->origfile != NULL)
		visual_mem_free (preset->origfile);

	if (preset->main != NULL)
		visual_object_unref (VISUAL_OBJECT (preset->main));
	
	preset->origfile = NULL;
	preset->main = NULL;

	return VISUAL_OK;
}

static int lvavs_preset_element_dtor (VisObject *object)
{
	LVAVSPresetElement *element = LVAVS_PRESET_ELEMENT (object);

	if (element->pcont != NULL)
		visual_object_unref (VISUAL_OBJECT (element->pcont));
	
	element->pcont = NULL;

	return VISUAL_OK;
}

static int lvavs_preset_container_dtor (VisObject *object)
{
	LVAVSPresetContainer *container = LVAVS_PRESET_CONTAINER (object);

	if (container->members != NULL)
		visual_object_unref (VISUAL_OBJECT (container->members));

	container->members = NULL;

	lvavs_element_dtor (object);

	return VISUAL_OK;
}


/* LVAVS Preset */
LVAVSPreset *lvavs_preset_new ()
{
	LVAVSPreset *preset;

	preset = visual_mem_new0 (LVAVSPreset, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (preset), TRUE, lvavs_preset_dtor);

	return preset;
}

LVAVSPreset *lvavs_preset_new_from_preset (char *filename)
{
	LVAVSPreset *preset;

	preset = lvavs_preset_new ();
	
	/* FIXME make */
	
	return preset;
}

LVAVSPreset *lvavs_preset_new_from_wavs (AVSTree *wavs)
{
	LVAVSPreset *preset;

	preset = lvavs_preset_new ();

	preset->main = LVAVS_PRESET_CONTAINER (wavs_convert_main_new (AVS_ELEMENT (wavs->main)));

	preset_convert_from_wavs (preset->main, wavs->main);

	return preset;
}

LVAVSPresetElement *lvavs_preset_element_new (LVAVSPresetElementType type, const char *name)
{
	LVAVSPresetElement *element;

	element = visual_mem_new0 (LVAVSPresetElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (element), TRUE, lvavs_preset_element_dtor);

	element->type = type;
	element->element_name = name;

	return element;
}

LVAVSPresetContainer *lvavs_preset_container_new ()
{
	LVAVSPresetContainer *container;

	container = visual_mem_new0 (LVAVSPresetContainer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (container), TRUE, lvavs_preset_container_dtor);

	container->members = visual_list_new (visual_object_list_destroyer);

	return container;
}


/* Internal functions */
static int preset_convert_from_wavs (LVAVSPresetContainer *presetcont, AVSContainer *cont)
{
	AVSElement *avselem;
	VisListEntry *le = NULL;

	while ((avselem = visual_list_next (cont->members, &le)) != NULL) {

		switch (avselem->type) {
			case AVS_ELEMENT_TYPE_MAIN:
				visual_list_add (presetcont->members, wavs_convert_main_new (avselem));

				break;

			case AVS_ELEMENT_TYPE_MISC_COMMENT:

				break;

			case AVS_ELEMENT_TYPE_RENDER_BASSSPIN:

				break;

			case AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN:

				break;

			case AVS_ELEMENT_TYPE_RENDER_RING:
				visual_list_add (presetcont->members, wavs_convert_ring_new (avselem));

				break;

			case AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE:
			
				break;

			case AVS_ELEMENT_TYPE_TRANS_BLUR:
			
				break;

			case AVS_ELEMENT_TYPE_TRANS_FASTBRIGHTNESS:

				break;

			case AVS_ELEMENT_TYPE_TRANS_INVERT:

				break;

			default:
				visual_log (VISUAL_LOG_CRITICAL, "Unhandled winamp AVS type %d\n", avselem->type);

				break;

		}
	}

	return VISUAL_OK;
}

LVAVSPresetElement *wavs_convert_main_new (AVSElement *avselem)
{
	LVAVSPresetContainer *container;
	VisParamContainer *pcont;
	VisParamContainer *pcontw;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("clear screen"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();
	visual_param_container_add_many (pcont, params);

	pcontw = avselem->pcont;

	/* Copy all the matching */
	visual_param_container_copy_match (pcont, pcontw);

	container = lvavs_preset_container_new ();
	LVAVS_PRESET_ELEMENT (container)->pcont = pcont;

	return LVAVS_PRESET_ELEMENT (container);
}

LVAVSPresetElement *wavs_convert_ring_new (AVSElement *avselem)
{
	LVAVSPresetElement *element;
	VisParamContainer *pcont;
	VisParamContainer *pcontw;
	int sourceplace;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("source"),
		VISUAL_PARAM_LIST_ENTRY ("place"),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY ("size"),
		VISUAL_PARAM_LIST_ENTRY ("type"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();
	visual_param_container_add_many (pcont, params);

	pcontw = avselem->pcont;

	/* Copy all the matching */
	visual_param_container_copy_match (pcont, pcontw);

	sourceplace = visual_param_entry_get_integer (visual_param_container_get (pcontw, "source and place"));
	
	visual_param_entry_set_integer (visual_param_container_get (pcont, "place"), sourceplace >> 4);
	visual_param_entry_set_integer (visual_param_container_get (pcont, "source"), (sourceplace & 0x0f) / 4);

	element = lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_ring");
	element->pcont = pcont;

	return element;
}

