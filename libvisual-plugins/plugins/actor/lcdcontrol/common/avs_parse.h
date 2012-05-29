/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_parse.h,v 1.9 2006-09-19 19:05:47 synap Exp $
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

#ifndef _LV_AVS_PARSE_H
#define _LV_AVS_PARSE_H

#include <libvisual/libvisual.h>

#include "avs_serialize.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define AVS_TREE(obj)					(VISUAL_CHECK_CAST ((obj), AVSTree))

#define AVS_ELEMENT(obj)				(VISUAL_CHECK_CAST ((obj), AVSElement))
#define AVS_CONTAINER(obj)				(VISUAL_CHECK_CAST ((obj), AVSContainer))

#define AVS_TREE_GET_CURRENT_POINTER(f)	((f)->cur)


typedef struct _AVSTree AVSTree;
typedef struct _AVSElement AVSElement;
typedef struct _AVSContainer AVSContainer;

typedef enum {
	AVS_VERSION_UNKNOWN,
	AVS_VERSION_1,
	AVS_VERSION_2
} AVSVersion;

typedef enum {
	AVS_ELEMENT_TYPE_RENDER_SIMPLESPECTRUM	= 0,
	AVS_ELEMENT_TYPE_RENDER_DOTPLANE	= 1,
	AVS_ELEMENT_TYPE_RENDER_OSCSTARS	= 2,
	AVS_ELEMENT_TYPE_TRANS_FADEOUT		= 3,
	AVS_ELEMENT_TYPE_TRANS_BLITTERFB	= 4,
	AVS_ELEMENT_TYPE_TRANS_NFRAMECLEAR	= 5,
	AVS_ELEMENT_TYPE_TRANS_BLUR		= 6,
	AVS_ELEMENT_TYPE_RENDER_BASSSPIN	= 7,
	AVS_ELEMENT_TYPE_RENDER_PARTICLE	= 8,
	AVS_ELEMENT_TYPE_RENDER_ROTBLIT		= 9,
	AVS_ELEMENT_TYPE_RENDER_SVP		= 10,
	AVS_ELEMENT_TYPE_TRANS_COLORFADE	= 11,
	AVS_ELEMENT_TYPE_TRANS_CONTRASTENHANCE	= 12,
	AVS_ELEMENT_TYPE_RENDER_ROTSTAR		= 13,
	AVS_ELEMENT_TYPE_RENDER_RING		= 14,
	AVS_ELEMENT_TYPE_TRANS_MOVEMENT		= 15,
	AVS_ELEMENT_TYPE_TRANS_SCATTER		= 16,
	AVS_ELEMENT_TYPE_RENDER_DOTGRID		= 17,
	AVS_ELEMENT_TYPE_MISC_STACK		= 18,
	AVS_ELEMENT_TYPE_RENDER_DOTFOUNTAIN	= 19,
	AVS_ELEMENT_TYPE_TRANS_WATER		= 20,
	AVS_ELEMENT_TYPE_MISC_COMMENT		= 21,
	AVS_ELEMENT_TYPE_TRANS_BRIGHTNESS	= 22,
	AVS_ELEMENT_TYPE_TRANS_INTERLEAVE	= 23,
	AVS_ELEMENT_TYPE_TRANS_GRAIN		= 24,
	AVS_ELEMENT_TYPE_RENDER_CLEARSCREEN	= 25,
	AVS_ELEMENT_TYPE_TRANS_MIRROR		= 26,
	AVS_ELEMENT_TYPE_RENDER_STARFIELD	= 27,
	AVS_ELEMENT_TYPE_RENDER_TEXT		= 28,
	AVS_ELEMENT_TYPE_TRANS_BUMPMAP		= 29,
	AVS_ELEMENT_TYPE_TRANS_MOSAIC		= 30,
	AVS_ELEMENT_TYPE_TRANS_WATERBUMP	= 31,
	AVS_ELEMENT_TYPE_RENDER_AVI		= 32,
	AVS_ELEMENT_TYPE_MISC_BPM		= 33,
	AVS_ELEMENT_TYPE_RENDER_PICTURE		= 34,
	AVS_ELEMENT_TYPE_UNKNOWN_DDM		= 35,
	AVS_ELEMENT_TYPE_RENDER_SUPERSCOPE	= 36,
	AVS_ELEMENT_TYPE_TRANS_INVERT		= 37,
	AVS_ELEMENT_TYPE_TRANS_ONETONE		= 38,
	AVS_ELEMENT_TYPE_RENDER_TIMESCOPE	= 39,
	AVS_ELEMENT_TYPE_MISC_RENDERSTATE		= 40,
	AVS_ELEMENT_TYPE_TRANS_INTERFERENCES	= 41,
	AVS_ELEMENT_TYPE_TRANS_CHANNELSHIFT	= 42,
	AVS_ELEMENT_TYPE_TRANS_DMOVE		= 43,
	AVS_ELEMENT_TYPE_TRANS_FASTBRIGHT	= 44,
	AVS_ELEMENT_TYPE_UNKNOWN_DCOLORMODE	= 45,

	AVS_ELEMENT_TYPE_MAIN			= 0x1000,
	AVS_ELEMENT_TYPE_APE			= 0xffff,
	AVS_ELEMENT_TYPE_TRANS_MULTIPLIER,

	AVS_ELEMENT_TYPE_LAST
} AVSElementType;

/* The AVS data structure */
struct _AVSTree {
	VisObject object;

	char *origfile;

	int datasize;
	char *data;

	char *cur;
	int cur_section_length;

	AVSVersion version;

	AVSContainer *main;
};

struct _AVSElement {
	VisObject object;

	AVSElementType type;

	AVSSerializeContainer *serialize;
	VisParamContainer *pcont;
};

struct _AVSContainer {
	AVSElement element;

	VisList *members;
};


/* Prototypes */
AVSTree *avs_tree_new_from_preset (char *filename);
int avs_check_version (AVSTree *avstree);

int avs_parse_tree (AVSTree *avstree, AVSContainer *curcontainer);

int avs_element_connect_serialize_container (AVSElement *element, AVSSerializeContainer *scont);
int avs_element_deserialize (AVSElement *element, AVSTree *avstree);
int avs_element_deserialize_many_new_params (AVSElement *element, AVSTree *avstree, ...);

AVSContainer *avs_parse_main (AVSTree *avstree);

AVSElement *avs_parse_trans_movement (AVSTree *avstree);
AVSElement *avs_parse_element_non_complex (AVSTree *avstree, AVSElementType type, ...);

int avs_parse_data (AVSTree *avstree, char *filename);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_AVS_PARSE_H */
