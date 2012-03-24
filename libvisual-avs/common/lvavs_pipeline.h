/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lvavs_pipeline.h,v 1.6 2006-09-19 19:05:47 synap Exp $
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

#ifndef _LV_LVAVS_PIPELINE_H
#define _LV_LVAVS_PIPELINE_H

#include <libvisual/libvisual.h>

#include "lvavs_preset.h"

#include "avs_globals.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LVAVS_PIPELINE(obj)				(VISUAL_CHECK_CAST ((obj), LVAVSPipeline))
#define LVAVS_PIPELINE_RENDERSTATE(obj)			(VISUAL_CHECK_CAST ((obj), LVAVSPipelineRenderState))
#define LVAVS_PIPELINE_ELEMENT(obj)			(VISUAL_CHECK_CAST ((obj), LVAVSPipelineElement))
#define LVAVS_PIPELINE_CONTAINER(obj)			(VISUAL_CHECK_CAST ((obj), LVAVSPipelineContainer))

#define LVAVS_MAX_BUFFERS 16

typedef struct _LVAVSPipeline LVAVSPipeline;
typedef struct _LVAVSPipelineRenderState LVAVSPipelineRenderState;
typedef struct _LVAVSPipelineElement LVAVSPipelineElement;
typedef struct _LVAVSPipelineContainer LVAVSPipelineContainer;


typedef enum {
	LVAVS_PIPELINE_ELEMENT_TYPE_NULL,
	LVAVS_PIPELINE_ELEMENT_TYPE_ACTOR,
	LVAVS_PIPELINE_ELEMENT_TYPE_TRANSFORM,
	LVAVS_PIPELINE_ELEMENT_TYPE_MORPH,
	LVAVS_PIPELINE_ELEMENT_TYPE_RENDERSTATE,
	LVAVS_PIPELINE_ELEMENT_TYPE_CONTAINER
} LVAVSPipelineElementType;

typedef enum {
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_REPLACE,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_ADDITIVE,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_MAXIMUM,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_5050,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_SUBSTRACT1,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_SUBSTRACT2,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_MULTIPLY,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_ADJUSTABLE,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_XOR,
	LVAVS_PIPELINE_RENDER_STATE_BLEND_TYPE_MINIMUM
} LVAVSPipelineRenderStateBlendMode;

/* The AVS data structure */
struct _LVAVSPipeline {
	VisObject			 object;

	int				 clearframe;

	LVAVSPipelineRenderState	*renderstate;

	VisVideo			*target;

	VisVideo			*buffers[LVAVS_MAX_BUFFERS];

	VisVideo *dummy_vid;
        VisVideo *last_vid;

	float audiodata[2][2][1024];

	unsigned char blendtable[256][256];

	int enabled;

	unsigned char blendadjust;

	LVAVSPipelineRenderStateBlendMode	blendmode;

	int linewidth;

	int *fbout;

	int *framebuffer;

	int swap; // whether to swap buffers -- fbout<->framebuffer

	int isBeat;

        int mode;
        int inblendval;
	int outblendval;
	int bufferin;
	int bufferout;
	int ininvert;
	int outinvert;
	int beat_render;
	int beat_render_frames;
        int use_inblendval;

	LVAVSPipelineContainer		*container;
};

// For removal. I don't see any reason to separatet these fields from the VisPipeline. Maybe there's a reason to exhcange RenderStates between pipelines? Leaving this here for now.
struct _LVAVSPipelineRenderState {
	VisObject				object;

	int					enabled;

	unsigned char				blendadjust;

	LVAVSPipelineRenderStateBlendMode	blendmode;

	int					linewidth;
};

struct _LVAVSPipelineElement {
	VisObject			 object;

	LVAVSPipelineElementType	 type;

	LVAVSPipeline			*pipeline;

	VisParamContainer		*params;

	union {
		VisActor			*actor;
		VisMorph			*morph;
		VisTransform			*transform;
		LVAVSPipelineRenderState	*renderstate;
		LVAVSPipelineContainer		*container;
	} data;
};

struct _LVAVSPipelineContainer {
	LVAVSPipelineElement		 element;

	VisList				*members;
};


/* Prototypes */
LVAVSPipeline *lvavs_pipeline_new (void);
LVAVSPipelineElement *lvavs_pipeline_element_new (LVAVSPipelineElementType type);
LVAVSPipelineContainer *lvavs_pipeline_container_new (void);

LVAVSPipeline *lvavs_pipeline_new_from_preset (LVAVSPreset *preset);
int lvavs_pipeline_realize (LVAVSPipeline *pipeline);
int lvavs_pipeline_negotiate (LVAVSPipeline *pipeline, VisVideo *video);
int lvavs_pipeline_propagate_event (LVAVSPipeline *pipeline, VisEvent *event);
int lvavs_pipeline_run (LVAVSPipeline *pipeline, VisVideo *video, VisAudio *audio);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LVAVS_PIPELINE_H */
