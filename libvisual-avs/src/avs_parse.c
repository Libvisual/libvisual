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

// ADD A LOT OF SANITY CHECKING TO THE COMPLETE PARSER!!

/*
 * FIXME: Find more patterns between different elements.
 * FIXME: Hex values replacen met defines.
 * FIXME: Next section on a global layer, not per element
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libvisual/libvisual.h>

#include "avs_parse.h"

/* Prototypes */
static int avs_tree_dtor (VisObject *object);
static int avs_element_dtor (VisObject *object);
static int avs_main_dtor (VisObject *object);
static int avs_render_superscope_dtor (VisObject *object);

static void show_options (AVSElement *element);

/* Object destructors */
static int avs_tree_dtor (VisObject *object)
{
	AVSTree *avstree = AVS_TREE (object);

	if (avstree->data != NULL)
		visual_mem_free (avstree->data);
	
	if (avstree->main != NULL)
		visual_object_unref (VISUAL_OBJECT (avstree->main));

	avstree->origfile = NULL;
	avstree->data = NULL;
	avstree->cur = NULL;
	avstree->main = NULL;
	
	return VISUAL_OK;
}

/* Every object that derives from AVSElement needs to pass on to this dtor when setting a custom dtor */
static int avs_element_dtor (VisObject *object)
{
	AVSElement *element = AVS_ELEMENT (object);

	if (element->serialize != NULL)
		visual_object_unref (VISUAL_OBJECT (element->serialize));

	if (element->pcont != NULL)
		visual_object_unref (VISUAL_OBJECT (element->pcont));
	
	element->serialize = NULL;
	element->pcont = NULL;

	return VISUAL_OK;
}

static int avs_container_dtor (VisObject *object)
{
	AVSContainer *container = AVS_CONTAINER (object);

	if (container->members != NULL)
		visual_object_unref (VISUAL_OBJECT (container->members));

	container->members = NULL;

	avs_element_dtor (object);

	return VISUAL_OK;
}



static void show_options (AVSElement *element)
{
	VisListEntry *le = NULL;
	VisParamEntry *param;

	if (element == NULL)
		return;

	printf ("Element options of element type: %d\n", element->type);

	while ((param = visual_list_next (&element->pcont->entries, &le)) != NULL) {

		switch (param->type) {
			case VISUAL_PARAM_ENTRY_TYPE_NULL:
				printf ("\t%s: Type NULL\n", visual_param_entry_get_name (param));

				break;

			case VISUAL_PARAM_ENTRY_TYPE_STRING:
				printf ("\t%s: Type STRING: %s\n", visual_param_entry_get_name (param),
						visual_param_entry_get_string (param));

				break;

			case VISUAL_PARAM_ENTRY_TYPE_INTEGER:
				printf ("\t%s: Type INTEGER: %d\n", visual_param_entry_get_name (param),
						visual_param_entry_get_integer (param));
				
				break;

			case VISUAL_PARAM_ENTRY_TYPE_FLOAT:
				printf ("\t%s: Type FLOAT: %f\n", visual_param_entry_get_name (param),
						visual_param_entry_get_float (param));
				
				break;

			case VISUAL_PARAM_ENTRY_TYPE_DOUBLE:
				printf ("\t%s: Type DOUBLE: %f\n", visual_param_entry_get_name (param),
						visual_param_entry_get_double (param));
				
				break;

			case VISUAL_PARAM_ENTRY_TYPE_COLOR:
				printf ("\t%s: Type COLOR: %d %d %d\n", visual_param_entry_get_name (param),
						param->color.r, param->color.g, param->color.b);
				
				break;
	
			case VISUAL_PARAM_ENTRY_TYPE_PALETTE:
				{
					int i;
				
					printf ("\t%s: Type PALETTE:\n", visual_param_entry_get_name (param));
					
					for (i = 0; i < param->pal.ncolors; i++) {
						printf ("\t\tcolor[%d] %d %d %d\n", i,
								param->pal.colors[i].r,
								param->pal.colors[i].g,
								param->pal.colors[i].b);
					}
				}
				break;
		}
	}

	printf ("\n");
}



/* AVS parser */
AVSTree *avs_tree_new_from_preset (char *filename)
{
	AVSTree *avstree;

	avstree = visual_mem_new0 (AVSTree, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (avstree), TRUE, avs_tree_dtor);

	avs_parse_data (avstree, filename);

	return avstree;
}

int avs_check_version (AVSTree *avstree)
{
	if (avstree->datasize <= strlen ("Nullsoft AVS Preset 0.2")) {
		avstree->version = AVS_VERSION_UNKNOWN;

		printf ("VERSION PARSE: unknown\n");

		return -1;
	}

	if (!strncmp(avstree->data, "Nullsoft AVS Preset 0.2", strlen ("Nullsoft AVS Preset 0.2"))) {
		avstree->version = AVS_VERSION_2;

		printf ("VERSION PARSE: Nullsoft AVS Preset 0.2\n");
		
		avstree->cur = avstree->data + strlen ("Nullsoft AVS Preset 0.2");
		
		return 0;
	}

	avstree->version = AVS_VERSION_UNKNOWN;

	printf ("VERSION PARSE: unknown\n");

	return -1;
}

int avs_parse_tree (AVSTree *avstree, AVSContainer *curcontainer)
{
	AVSElement *element = NULL;
	char namedelem[30];
	int isnamed;
	char *next_section = NULL;
	int marker;

	if (curcontainer == NULL)
		return -1;
	
	while (avstree->cur < (avstree->data + avstree->datasize)) {
		
		marker = AVS_SERIALIZE_GET_INT (AVS_TREE_GET_CURRENT_POINTER (avstree));
		AVS_SERIALIZE_SKIP_INT (AVS_TREE_GET_CURRENT_POINTER (avstree));

		/* Named preset section */
		isnamed = FALSE;
		if (marker > 0xff) {
			printf ("Marker > 0xff, named APE %x\n", marker);

			strncpy (namedelem, AVS_TREE_GET_CURRENT_POINTER (avstree), 30);

			AVS_SERIALIZE_SKIP_LENGTH (avstree->cur, 32);
			isnamed = TRUE;
			marker = 0xffff;
		}
		
		next_section = AVS_SERIALIZE_GET_NEXT_SECTION (AVS_TREE_GET_CURRENT_POINTER (avstree));
		AVS_SERIALIZE_SKIP_INT (AVS_TREE_GET_CURRENT_POINTER (avstree));
		
		/* FIXME: Use a table lookup here instead of giant function */
		switch (marker) {
			case 0x15:	// comment
				element = AVS_ELEMENT (avs_parse_misc_comment (avstree));

				visual_list_add (curcontainer->members, element); 

				break;

			case 0x0e:	// ring
				element = AVS_ELEMENT (avs_parse_render_ring (avstree));

				visual_list_add (curcontainer->members, element);

				break;

			case 0x24:	// super scope
				element = AVS_ELEMENT (avs_parse_render_superscope (avstree));

				visual_list_add (curcontainer->members, element);

				break;

			case 0x19:	// clearscreen
				element = AVS_ELEMENT (avs_parse_render_clearscreen (avstree));

				visual_list_add (curcontainer->members, element);

				break;

			case 0x07:	// bass spin
				element = AVS_ELEMENT (avs_parse_render_bassspin (avstree));

				visual_list_add (curcontainer->members, element);

				break;

			case 0x2c:	// fast brightness
				element = AVS_ELEMENT (avs_parse_trans_fastbrightness (avstree));

				visual_list_add (curcontainer->members, element);

				break;

			case 0x25:	// invert
				element = AVS_ELEMENT (avs_parse_trans_invert (avstree));

				visual_list_add (curcontainer->members, element);

				break;

			case 0x06:	// blur
				element = AVS_ELEMENT (avs_parse_trans_blur (avstree));

				visual_list_add (curcontainer->members, element);

				break;
			
			case 0xffff:

				printf ("APE NAME: %s\n", namedelem);
				
				element = NULL;
				if (strcmp (namedelem, "Multiplier") == 0)
					element = AVS_ELEMENT (avs_parse_trans_multiplier (avstree));
				else if (strcmp (namedelem, "Channel Shift") == 0)
					element = AVS_ELEMENT (avs_parse_trans_channelshift (avstree));
				else
					printf ("Unhandled named entry: %s position: %x\n", namedelem, avstree->cur - avstree->data);

				if (element != NULL)
					visual_list_add (curcontainer->members, element);

				break;
			
			default:
				printf ("Unhandled type: %x position: %x\n", *avstree->cur, avstree->cur - avstree->data);

				return -1;
				
				break;
		}

		show_options (element);

		avstree->cur = next_section;
	}

	return 0;
}

int avs_element_connect_serialize_container (AVSElement *element, AVSSerializeContainer *scont)
{
	element->serialize = scont;

	return 0;
}

int avs_element_deserialize (AVSElement *element, AVSTree *avstree)
{
	AVS_TREE_GET_CURRENT_POINTER (avstree) =
		avs_serialize_container_deserialize (element->serialize, AVS_TREE_GET_CURRENT_POINTER (avstree));

	return 0;
}

AVSContainer *avs_parse_main (AVSTree *avstree)
{
	AVSContainer *avsmain;
	AVSSerializeContainer *scont;
	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("clear screen"),
		VISUAL_PARAM_LIST_END
	};
	
	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);
	
	avsmain = visual_mem_new0 (AVSContainer, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (avsmain), TRUE, avs_container_dtor);

	AVS_ELEMENT (avsmain)->pcont = pcont;
	AVS_ELEMENT (avsmain)->type = AVS_ELEMENT_TYPE_MAIN;

	AVS_CONTAINER (avsmain)->members = visual_list_new (visual_object_list_destroyer);

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte (scont, visual_param_container_get (pcont, "clear screen"));

	avs_element_connect_serialize_container (AVS_ELEMENT (avsmain), scont);

	avs_element_deserialize (AVS_ELEMENT (avsmain), avstree);

	avstree->main = avsmain;

	return avsmain;
}

AVSElement *avs_parse_misc_comment (AVSTree *avstree)
{
	AVSElement *comment;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("text"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);
	
	comment = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (comment), TRUE, avs_element_dtor);

	AVS_ELEMENT (comment)->pcont = pcont;
	AVS_ELEMENT (comment)->type = AVS_ELEMENT_TYPE_MISC_COMMENT;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_string (scont, visual_param_container_get (pcont, "text"));

	avs_element_connect_serialize_container (AVS_ELEMENT (comment), scont);

	avs_element_deserialize (AVS_ELEMENT (comment), avstree);

	return comment;
}

AVSElement *avs_parse_render_superscope (AVSTree *avstree)
{
	AVSElement *superscope;
	AVSSerializeContainer *scont;
	int len;
	int i;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("point"),
		VISUAL_PARAM_LIST_ENTRY ("frame"),
		VISUAL_PARAM_LIST_ENTRY ("beat"),
		VISUAL_PARAM_LIST_ENTRY ("init"),
		VISUAL_PARAM_LIST_ENTRY ("channel source"),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY ("draw type"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	superscope = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (superscope), TRUE, avs_element_dtor);

	AVS_ELEMENT (superscope)->pcont = pcont;
	AVS_ELEMENT (superscope)->type = AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE;
	
	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte (scont, NULL); // not sure what kind of bit this is
	avs_serialize_container_add_string (scont, visual_param_container_get (pcont, "point"));
	avs_serialize_container_add_string (scont, visual_param_container_get (pcont, "frame"));
	avs_serialize_container_add_string (scont, visual_param_container_get (pcont, "beat"));
	avs_serialize_container_add_string (scont, visual_param_container_get (pcont, "init"));
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "channel source"), 0x06);	
	avs_serialize_container_add_palette (scont, visual_param_container_get (pcont, "palette"));
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "draw type"), 0x01);
	
	avs_element_connect_serialize_container (AVS_ELEMENT (superscope), scont);

	avs_element_deserialize (AVS_ELEMENT (superscope), avstree);

	return superscope;
}

AVSElement *avs_parse_render_clearscreen (AVSTree *avstree)
{
	AVSElement *clearscreen;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("enabled"),
		VISUAL_PARAM_LIST_ENTRY ("color"),
		VISUAL_PARAM_LIST_ENTRY ("clear type"),
		VISUAL_PARAM_LIST_ENTRY ("clearr 5050"),
		VISUAL_PARAM_LIST_ENTRY ("first"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	clearscreen = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (clearscreen), TRUE, avs_element_dtor);

	AVS_ELEMENT (clearscreen)->pcont = pcont;
	AVS_ELEMENT (clearscreen)->type = AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "enabled"), 0x01);
	avs_serialize_container_add_color (scont, visual_param_container_get (pcont, "color"));
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "clear type"), 0x02);
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "clear 5050"), 0x01);
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "first"), 0x01);

	avs_element_connect_serialize_container (AVS_ELEMENT (clearscreen), scont);

	avs_element_deserialize (AVS_ELEMENT (clearscreen), avstree);
	
	return clearscreen;
}

AVSElement *avs_parse_render_ring (AVSTree *avstree)
{
	AVSElement *ring;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("source and place"),
		VISUAL_PARAM_LIST_ENTRY ("palette"),
		VISUAL_PARAM_LIST_ENTRY ("size"),
		VISUAL_PARAM_LIST_ENTRY ("type"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	ring = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (ring), TRUE, avs_element_dtor);

	AVS_ELEMENT (ring)->pcont = pcont;
	AVS_ELEMENT (ring)->type = AVS_ELEMENT_TYPE_RENDER_RING;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_int_skip (scont, visual_param_container_get (pcont, "source and place"));
	avs_serialize_container_add_palette (scont, visual_param_container_get (pcont, "palette"));
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "size"), 0x40);
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "type"), 0x01);

	avs_element_connect_serialize_container (AVS_ELEMENT (ring), scont);

	avs_element_deserialize (AVS_ELEMENT (ring), avstree);

	return ring;
}

AVSElement *avs_parse_render_bassspin (AVSTree *avstree)
{
	AVSElement *bassspin;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("chan enabled"),
		VISUAL_PARAM_LIST_ENTRY ("left color"),
		VISUAL_PARAM_LIST_ENTRY ("right color"),
		VISUAL_PARAM_LIST_ENTRY ("draw type"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	bassspin = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (bassspin), TRUE, avs_element_dtor);

	AVS_ELEMENT (bassspin)->pcont = pcont;
	AVS_ELEMENT (bassspin)->type = AVS_ELEMENT_TYPE_RENDER_BASSSPIN;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "chan enabled"), 0x03);
	avs_serialize_container_add_color (scont, visual_param_container_get (pcont, "left color"));
	avs_serialize_container_add_color (scont, visual_param_container_get (pcont, "right color"));
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "draw type"), 0x01);

	avs_element_connect_serialize_container (AVS_ELEMENT (bassspin), scont);

	avs_element_deserialize (AVS_ELEMENT (bassspin), avstree);

	return bassspin;
}

AVSElement *avs_parse_trans_fastbrightness (AVSTree *avstree)
{
	AVSElement *fbrightness;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("brightness type"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	fbrightness = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (fbrightness), TRUE, avs_element_dtor);

	AVS_ELEMENT (fbrightness)->pcont = pcont;
	AVS_ELEMENT (fbrightness)->type = AVS_ELEMENT_TYPE_TRANS_FASTBRIGHTNESS;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "brightness type"), 0x02);

	avs_element_connect_serialize_container (AVS_ELEMENT (fbrightness), scont);

	avs_element_deserialize (AVS_ELEMENT (fbrightness), avstree);

	return fbrightness;
}

AVSElement *avs_parse_trans_invert (AVSTree *avstree)
{
	AVSElement *invert;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("enabled"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	invert = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (invert), TRUE, avs_element_dtor);

	AVS_ELEMENT (invert)->pcont = pcont;
	AVS_ELEMENT (invert)->type = AVS_ELEMENT_TYPE_TRANS_INVERT;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "enabled"), 0x01);

	avs_element_connect_serialize_container (AVS_ELEMENT (invert), scont);

	avs_element_deserialize (AVS_ELEMENT (invert), avstree);
	
	return invert;
}

AVSElement *avs_parse_trans_blur (AVSTree *avstree)
{
	AVSElement *blur;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("blur type"),
		VISUAL_PARAM_LIST_ENTRY ("roundup"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	blur = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (blur), TRUE, avs_element_dtor);

	AVS_ELEMENT (blur)->pcont = pcont;
	AVS_ELEMENT (blur)->type = AVS_ELEMENT_TYPE_TRANS_BLUR;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "blur type"), 0x03);
	avs_serialize_container_add_byte_with_boundry (scont, visual_param_container_get (pcont, "roundup"), 0x01);

	avs_element_connect_serialize_container (AVS_ELEMENT (blur), scont);

	avs_element_deserialize (AVS_ELEMENT (blur), avstree);

	return blur;
}

AVSElement *avs_parse_trans_multiplier (AVSTree *avstree)
{
	AVSElement *multiplier;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("multiply"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	multiplier = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (multiplier), TRUE, avs_element_dtor);

	AVS_ELEMENT (multiplier)->pcont = pcont;
	AVS_ELEMENT (multiplier)->type = AVS_ELEMENT_TYPE_TRANS_MULTIPLIER;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "multiply"), 0x07);

	avs_element_connect_serialize_container (AVS_ELEMENT (multiplier), scont);

	avs_element_deserialize (AVS_ELEMENT (multiplier), avstree);

	return multiplier;
}

AVSElement *avs_parse_trans_channelshift (AVSTree *avstree)
{
	AVSElement *shift;
	AVSSerializeContainer *scont;

	VisParamContainer *pcont;

	static VisParamEntry params[] = {
		VISUAL_PARAM_LIST_ENTRY ("shift"),
		VISUAL_PARAM_LIST_ENTRY ("onbeat"),
		VISUAL_PARAM_LIST_END
	};

	pcont = visual_param_container_new ();

	visual_param_container_add_many (pcont, params);

	shift = visual_mem_new0 (AVSElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (shift), TRUE, avs_element_dtor);

	AVS_ELEMENT (shift)->pcont = pcont;
	AVS_ELEMENT (shift)->type = AVS_ELEMENT_TYPE_TRANS_CHANNELSHIFT;

	scont = avs_serialize_container_new ();
	avs_serialize_container_add_byte_int_skip (scont, visual_param_container_get (pcont, "shift"));
	avs_serialize_container_add_byte_int_skip_with_boundry (scont, visual_param_container_get (pcont, "onbeat"), 0x01);

	avs_element_connect_serialize_container (AVS_ELEMENT (shift), scont);

	avs_element_deserialize (AVS_ELEMENT (shift), avstree);

	return shift;
}

int avs_parse_data (AVSTree *avstree, char *filename)
{
	int fd;
	int size;

	char *buf;

	if (avstree == NULL || filename == NULL)
		return -1;

	/* Open file, and read it into a memory buffer, a bit rough edged, but who cares! */
	fd = open (filename, O_RDONLY);

	if (fd < 0) {
		printf ("FILE NOT FOUND BLAH\n");
		
		exit (-1);
	}

	/* So we know how big this file is, nice for allocating our buffer */
	size = lseek (fd, 0, SEEK_END);
	lseek (fd, 0, SEEK_SET);

	/* Read the data into the buffer */
	avstree->data = malloc (size);
	read (fd, avstree->data , size);
	avstree->datasize = size;

	/* Close the file since it's no longer needed */
	close (fd);

	avstree->cur = avstree->data;
	avs_check_version (avstree);

	if (avstree->version != AVS_VERSION_2) {
		printf ("Not supported AVS version, or wrong filetype\n");

		return -1;
	}

	/* Skip over the main section marker, on to it's flags */
	AVS_SERIALIZE_SKIP_BYTE (AVS_TREE_GET_CURRENT_POINTER (avstree));

	avstree->main = avs_parse_main (avstree);

	avs_parse_tree (avstree, AVS_CONTAINER (avstree->main));

	return 0;
}

