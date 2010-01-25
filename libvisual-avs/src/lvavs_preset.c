/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lvavs_preset.c,v 1.9 2006-09-19 19:05:47 synap Exp $
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

static const char *id_to_name_map[] = {
	[AVS_ELEMENT_TYPE_RENDER_SIMPLESPECTRUM]	= "avs_superscope",
	[AVS_ELEMENT_TYPE_RENDER_DOTPLANE]		= "avs_dotplane",
	[AVS_ELEMENT_TYPE_RENDER_OSCSTARS]		= "avs_stars",
	[AVS_ELEMENT_TYPE_TRANS_FADEOUT]		= "avs_fadeout",
	[AVS_ELEMENT_TYPE_TRANS_BLITTERFB]		= "avs_blitterfb",
	[AVS_ELEMENT_TYPE_TRANS_NFRAMECLEAR]		= "avs_nframeclear",
	[AVS_ELEMENT_TYPE_TRANS_BLUR]			= "avs_blur",
	[AVS_ELEMENT_TYPE_RENDER_BASSSPIN]		= "avs_bassspin",
	[AVS_ELEMENT_TYPE_RENDER_PARTICLE]		= "avs_particle",
	[AVS_ELEMENT_TYPE_RENDER_ROTBLIT]		= "avs_rotblitter",
	[AVS_ELEMENT_TYPE_UNKNOWN_SVP]			= "error",
	[AVS_ELEMENT_TYPE_TRANS_COLORFADE]		= "avs_colorfade",
	[AVS_ELEMENT_TYPE_TRANS_CONTRASTENHANCE]	= "avs_contrastenhance",
	[AVS_ELEMENT_TYPE_RENDER_ROTSTAR]		= "avs_rotstar",
	[AVS_ELEMENT_TYPE_RENDER_RING]			= "avs_ring",
	[AVS_ELEMENT_TYPE_TRANS_MOVEMENT]		= "avs_movement",
	[AVS_ELEMENT_TYPE_TRANS_SCATTER]		= "avs_scatter",
	[AVS_ELEMENT_TYPE_RENDER_DOTGRID]		= "avs_dotgrid",
	[AVS_ELEMENT_TYPE_UNKNOWN_STACK]		= "error",
	[AVS_ELEMENT_TYPE_RENDER_DOTFOUNTAIN]		= "avs_dotfountain",
	[AVS_ELEMENT_TYPE_TRANS_WATER]			= "avs_water",
	[AVS_ELEMENT_TYPE_MISC_COMMENT]			= "error",
	[AVS_ELEMENT_TYPE_TRANS_BRIGHTNESS]		= "avs_brightness",
	[AVS_ELEMENT_TYPE_TRANS_INTERLEAVE]		= "avs_interleave",
	[AVS_ELEMENT_TYPE_TRANS_GRAIN]			= "avs_grain",
	[AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN]		= "avs_clearscreen",
	[AVS_ELEMENT_TYPE_TRANS_MIRROR]			= "avs_mirror",
	[AVS_ELEMENT_TYPE_RENDER_STARFIELD]		= "avs_starfield",
	[AVS_ELEMENT_TYPE_RENDER_TEXT]			= "avs_text",
	[AVS_ELEMENT_TYPE_TRANS_BUMPMAP]		= "avs_bumpmap",
	[AVS_ELEMENT_TYPE_TRANS_MOSAIC]			= "avs_mosaic",
	[AVS_ELEMENT_TYPE_TRANS_WATERBUMP]		= "avs_waterbump",
	[AVS_ELEMENT_TYPE_RENDER_AVI]			= "avs_avi",
	[AVS_ELEMENT_TYPE_UNKNOWN_BPM]			= "error",
	[AVS_ELEMENT_TYPE_RENDER_PICTURE]		= "avs_picture",
	[AVS_ELEMENT_TYPE_UNKNOWN_DDM]			= "error",
	[AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE]		= "avs_superscope",
	[AVS_ELEMENT_TYPE_TRANS_INVERT]			= "avs_invert",
	[AVS_ELEMENT_TYPE_TRANS_ONETONE]		= "avs_onetone",
	[AVS_ELEMENT_TYPE_RENDER_TIMESCOPE]		= "avs_timescope",
	[AVS_ELEMENT_TYPE_MISC_LINEMODE]		= "error",
	[AVS_ELEMENT_TYPE_TRANS_INTERFERENCES]		= "avs_interferences",
	[AVS_ELEMENT_TYPE_TRANS_CHANNELSHIFT]		= "avs_channelshift",
	[AVS_ELEMENT_TYPE_TRANS_DMOVE]		= "avs_dmovement",
	[AVS_ELEMENT_TYPE_TRANS_FASTBRIGHT]		= "avs_fastbright",
	[AVS_ELEMENT_TYPE_UNKNOWN_DCOLORMODE]		= "error",

	[AVS_ELEMENT_TYPE_MAIN]				= "error",
	[AVS_ELEMENT_TYPE_APE]				= "error",
	[AVS_ELEMENT_TYPE_TRANS_MULTIPLIER]		= "avs_multiplier"
};

/* Prototypes */
static int lvavs_preset_dtor (VisObject *object);
static int lvavs_preset_element_dtor (VisObject *object);
static int lvavs_preset_container_dtor (VisObject *object);

static int preset_convert_from_wavs (LVAVSPresetContainer *presetcont, AVSContainer *cont);

LVAVSPresetElement *wavs_convert_main_new (AVSElement *avselem);
LVAVSPresetElement *wavs_convert_ring_new (AVSElement *avselem);
LVAVSPresetElement *wavs_convert_channelshift_new (AVSElement *avselem);

LVAVSPresetElement *wavs_convert_remap (AVSElement *avselem, const char *plugname);

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

	return TRUE;
}

static int lvavs_preset_element_dtor (VisObject *object)
{
	LVAVSPresetElement *element = LVAVS_PRESET_ELEMENT (object);

	if (element->pcont != NULL)
		visual_object_unref (VISUAL_OBJECT (element->pcont));

	element->pcont = NULL;

	return TRUE;
}

static int lvavs_preset_container_dtor (VisObject *object)
{
	LVAVSPresetContainer *container = LVAVS_PRESET_CONTAINER (object);

	if (container->members != NULL)
		visual_object_unref (VISUAL_OBJECT (container->members));

	container->members = NULL;

	lvavs_preset_element_dtor (object);

	return TRUE;
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

	container->members = visual_list_new (visual_object_collection_destroyer);

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

			case AVS_ELEMENT_TYPE_RENDER_RING:
				visual_list_add (presetcont->members, wavs_convert_ring_new (avselem));

				break;

			case AVS_ELEMENT_TYPE_TRANS_CHANNELSHIFT:
				visual_list_add (presetcont->members, wavs_convert_channelshift_new (avselem));

				break;

			default:
				if (avselem->type >= AVS_ELEMENT_TYPE_RENDER_SIMPLESPECTRUM &&
						avselem->type < AVS_ELEMENT_TYPE_LAST) {

					visual_list_add (presetcont->members, wavs_convert_remap (avselem,
								id_to_name_map[avselem->type]));

				}

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

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY ("clear screen"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();
	visual_param_container_add_many_proxy (pcont, params);

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

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY ("source"),
		VISUAL_PARAM_LIST_ENTRY ("place"),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY ("size"),
		VISUAL_PARAM_LIST_ENTRY ("type"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();
	visual_param_container_add_many_proxy (pcont, params);

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

LVAVSPresetElement *wavs_convert_channelshift_new (AVSElement *avselem)
{
	LVAVSPresetElement *element;
	VisParamContainer *pcont;
	VisParamContainer *pcontw;
	int shift;

	static VisParamEntryProxy params[] = {
		VISUAL_PARAM_LIST_ENTRY ("shift"),
		VISUAL_PARAM_LIST_ENTRY ("onbeat"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();
	visual_param_container_add_many_proxy (pcont, params);

	pcontw = avselem->pcont;

	/* Copy all the matching */
	visual_param_container_copy_match (pcont, pcontw);

	shift = visual_param_entry_get_integer (visual_param_container_get (pcontw, "shift"));

	/* Yes, the RGB and BRG entries have the same value here, I think it's a bug in winamp AVS */
	switch (shift & 0xff) {
		case 0xfb: /* BRG */
			shift = 2;

			break;

		case 0xfc: /* RBG */
			shift = 1;

			break;

		case 0xfd: /* BGR */
			shift = 3;

			break;

		case 0xfa: /* GBR */
			shift = 4;

			break;

		case 0xfe: /* GRB */
			shift = 5;

			break;

		default: /* Default to RGB */
			shift = 0;

			break;


	}

	visual_param_entry_set_integer (visual_param_container_get (pcont, "shift"), shift);

	element = lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_channelshift");
	element->pcont = pcont;

	return element;
}

LVAVSPresetElement *wavs_convert_remap (AVSElement *avselem, const char *plugname)
{
	LVAVSPresetElement *element;

	element = lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, plugname);

	visual_object_ref (VISUAL_OBJECT (avselem->pcont));
	element->pcont = avselem->pcont;

	return element;
}

