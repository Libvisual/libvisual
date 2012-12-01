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
//#include <libxml/parser.h>
//#include <libxml/xmlmemory.h>

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
	[AVS_ELEMENT_TYPE_RENDER_SVP]			= "error",
	[AVS_ELEMENT_TYPE_TRANS_COLORFADE]		= "avs_colorfade",
	[AVS_ELEMENT_TYPE_TRANS_CONTRASTENHANCE]	= "avs_contrastenhance",
	[AVS_ELEMENT_TYPE_RENDER_ROTSTAR]		= "avs_rotstar",
	[AVS_ELEMENT_TYPE_RENDER_RING]			= "avs_ring",
	[AVS_ELEMENT_TYPE_TRANS_MOVEMENT]		= "avs_movement",
	[AVS_ELEMENT_TYPE_TRANS_SCATTER]		= "avs_scatter",
	[AVS_ELEMENT_TYPE_RENDER_DOTGRID]		= "avs_dotgrid",
	[AVS_ELEMENT_TYPE_MISC_STACK]		= "error",
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
	[AVS_ELEMENT_TYPE_MISC_BPM]			= "error",
	[AVS_ELEMENT_TYPE_RENDER_PICTURE]		= "avs_picture",
	[AVS_ELEMENT_TYPE_UNKNOWN_DDM]			= "error",
	[AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE]		= "avs_superscope",
	[AVS_ELEMENT_TYPE_TRANS_INVERT]			= "avs_invert",
	[AVS_ELEMENT_TYPE_TRANS_ONETONE]		= "avs_onetone",
	[AVS_ELEMENT_TYPE_RENDER_TIMESCOPE]		= "avs_timescope",
	[AVS_ELEMENT_TYPE_MISC_RENDERSTATE]		= "error",
	[AVS_ELEMENT_TYPE_TRANS_INTERFERENCES]		= "avs_interferences",
	[AVS_ELEMENT_TYPE_TRANS_CHANNELSHIFT]		= "avs_channelshift",
	[AVS_ELEMENT_TYPE_TRANS_DMOVE]		= "avs_dmovement",
	[AVS_ELEMENT_TYPE_TRANS_FASTBRIGHT]		= "avs_fastbright",
	[AVS_ELEMENT_TYPE_UNKNOWN_DCOLORMODE]		= "error",

	[AVS_ELEMENT_TYPE_MAIN]				= "error",
	[AVS_ELEMENT_TYPE_APE]				= "error",
	[AVS_ELEMENT_TYPE_TRANS_MULTIPLIER]		= "avs_multiplier",
	NULL
};

static VisParamEntry container_params[] = {
	VISUAL_PARAM_LIST_ENTRY_INTEGER("clearscreen", 1),
	VISUAL_PARAM_LIST_END
};

/* Prototypes */
static int lvavs_preset_dtor (VisObject *object);
static int lvavs_preset_element_dtor (VisObject *object);
static int lvavs_preset_container_dtor (VisObject *object);

//LVAVSPresetContainer *lvavs_preset_container_from_xml_node(LVAVSPresetContainer *cont, xmlNodePtr node);

/*static int preset_convert_from_wavs (LVAVSPresetContainer *presetcont, AVSContainer *cont);

LVAVSPresetElement *wavs_convert_main_new (AVSElement *avselem);
LVAVSPresetElement *wavs_convert_ring_new (AVSElement *avselem);
LVAVSPresetElement *wavs_convert_channelshift_new (AVSElement *avselem);

LVAVSPresetElement *wavs_convert_remap (AVSElement *avselem, const char *plugname);
*/

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

	if (element->element_name != NULL)
		visual_mem_free((void *)element->element_name);

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

/* cur->name should be the element name */
/*
static int parse_element (xmlNodePtr cur, LVAVSPresetElement *element)
{
  char *content;
  LVAVSPresetElement *child;
  VisParamEntry *param;

  for (cur = cur->xmlChildrenNode; cur; cur = cur->next)
  {
      if (xmlIsBlankNode (cur) || cur->type != XML_ELEMENT_NODE)
	continue;

	LVAVSPresetElementType type;
	xmlChar *prop = xmlGetProp(cur, (xmlChar *)"type");
	content = (char*)xmlNodeGetContent (cur);

	param = visual_param_entry_new((char *)cur->name);

	if(strcmp((char *)prop, "string") == 0) {
		visual_param_entry_set_string(param, content);
	} else if( strcmp((char *)prop, "float") == 0) {
		visual_param_entry_set_double(param, strtod(content, NULL));
	} else if( strcmp((char *)prop, "integer") == 0) {
		visual_param_entry_set_integer(param, (int)strtol(content, NULL, 0));
	} else if( strcmp((char *)prop, "color") == 0) {
		int r,g,b;
		char *s = content+1;
		r = strtoul (s, &s, 0);
		if (r > 255 || ! (s = strchr (s, ',')))
		  continue;
		g = strtoul (s+1, &s, 0);
		if (g > 255 || ! (s = strchr (s, ',')))
		  continue;
		b = strtoul (s+1, NULL, 0);
		if (b > 255)
		  continue;
		visual_param_entry_set_color(param, r, g, b);
	} else if( strcmp((char *)prop, "bool") == 0) {
		char *c, *d;
		int val;

#define isspace(c) (c == ' ' || c == '\t' || c == '\n')

		for (c=content; isspace (*c); c++);
		for (d=c; !isspace(*d); d++);
		*d = '\0';
		if (g_strcasecmp (c, "true") == 0)
		  val = TRUE;
		else if (g_strcasecmp (c, "false") == 0)
		  val = FALSE;
		else
			continue;
		visual_param_entry_set_integer(param, val);
	}

	visual_param_container_add(element->pcont, param);	

	xmlFree ((xmlChar*)content);
    }
    return TRUE;
}
*/

/*
LVAVSPreset *lvavs_preset_new_from_preset (char *filename)
{
	LVAVSPreset *preset = NULL;
	LVAVSPresetElement *element;
	LVAVSPresetContainer *cont;
	xmlDocPtr doc;
	xmlNodePtr cur;
	int i;

	doc = xmlParseFile(filename);
	if(doc == NULL) { 
		return NULL;
	}
	cur = xmlDocGetRootElement (doc);
	if(!cur) {
		xmlFreeDoc(doc);
		fprintf(stderr, "No root element\n");
		return NULL;
	}

	if(xmlStrcmp(cur->name, (const xmlChar *) "pipeline_preset") != 0)
	{	
		xmlFreeDoc(doc);
		fprintf(stderr, "Bad opening node in preset file %s. Looking for 'pipeline_preset' but got '%s'.", filename, cur->name);
		return NULL;
	}

	preset = lvavs_preset_new ();
	preset->main = lvavs_preset_container_new();

        VisParamContainer *pcont = visual_param_container_new();
	visual_param_container_add_many(pcont, container_params);
	LVAVS_PRESET_ELEMENT(preset->main)->pcont = pcont;

	for (cur = cur->children; cur; cur = cur->next)
	{
		if(xmlStrcmp(cur->name, (const xmlChar *)"container_main") == 0)
		{
			xmlNodePtr child;
			LVAVSPresetContainer *cont = lvavs_preset_container_new();
			LVAVS_PRESET_ELEMENT(cont)->pcont = pcont;
			visual_list_add(preset->main->members, cont);
			cont = lvavs_preset_container_from_xml_node(cont, cur);
		}
		if(xmlStrcmp(cur->name, (const xmlChar *)"params") == 0)
		{
			parse_element(cur, LVAVS_PRESET_ELEMENT(preset->main));		
		}
	}
	xmlFreeDoc (doc);

  return preset;
}
*/

/*
LVAVSPresetContainer *lvavs_preset_container_from_xml_node(LVAVSPresetContainer *cont, xmlNodePtr node) {
	xmlNodePtr child;
	LVAVSPresetElement *element;
	int i;
	for(child = node->children; child; child = child->next) 
	{
		if (xmlIsBlankNode (child) || child->type != XML_ELEMENT_NODE)
			continue;
		if(xmlStrcmp(child->name, (xmlChar *)"container_child") == 0) {
			LVAVSPresetContainer *cont2 = lvavs_preset_container_new();
			VisParamContainer *pcont = visual_param_container_new();
			LVAVS_PRESET_ELEMENT(cont2)->pcont = pcont;
			visual_param_container_add_many(pcont, container_params);
			visual_list_add(cont->members, cont2);
			lvavs_preset_container_from_xml_node(cont2, child);
		}
		for(i = 0; id_to_name_map[i] != NULL; i++)
			if (xmlStrcmp (child->name,
				(const xmlChar *) id_to_name_map[i]) == 0) 
				break;
		if(id_to_name_map[i] == NULL) 
			continue;

		element = lvavs_preset_element_new(LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, (char*)child->name);
		if(parse_element(child, element)) 
			visual_list_add(cont->members, element);
	}
	return cont;

}
*/

LVAVSPresetElement *lvavs_preset_element_new (LVAVSPresetElementType type, const char *name)
{
	LVAVSPresetElement *element;

	element = visual_mem_new0 (LVAVSPresetElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (element), TRUE, lvavs_preset_element_dtor);

	element->type = type;
	element->element_name = strdup(name);
	element->pcont = visual_param_container_new();

	return element;
}

LVAVSPresetContainer *lvavs_preset_container_new ()
{
	LVAVSPresetContainer *container;
        LVAVSPresetElement *el;

	container = visual_mem_new0 (LVAVSPresetContainer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (container), TRUE, lvavs_preset_container_dtor);

	container->members = visual_list_new (visual_object_collection_destroyer);
	container->element.element_name = strdup("new container");
	container->element.type = LVAVS_PRESET_ELEMENT_TYPE_CONTAINER;

	return container;
}


/* Internal functions */
/*
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
*/
/*
LVAVSPresetElement *wavs_convert_main_new (AVSElement *avselem)
{
	LVAVSPresetContainer *container;
	VisParamContainer *pcont;
	VisParamContainer *pcontw;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("clearscreen"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();
	visual_param_container_add_many (pcont, params);

	pcontw = avselem->pcont;

	// Copy all the matching
	visual_param_container_copy_match (pcont, pcontw);

	container = lvavs_preset_container_new ();
	LVAVS_PRESET_ELEMENT (container)->pcont = pcont;

	return LVAVS_PRESET_ELEMENT (container);
}
*/
/*
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

	// Copy all the matching 
	visual_param_container_copy_match (pcont, pcontw);

	sourceplace = visual_param_entry_get_integer (visual_param_container_get (pcontw, "source and place"));

	visual_param_entry_set_integer (visual_param_container_get (pcont, "place"), sourceplace >> 4);
	visual_param_entry_set_integer (visual_param_container_get (pcont, "source"), (sourceplace & 0x0f) / 4);

	element = lvavs_preset_element_new (LVAVS_PRESET_ELEMENT_TYPE_PLUGIN, "avs_ring");
	element->pcont = pcont;

	return element;
}
*/

