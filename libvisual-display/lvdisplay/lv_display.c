#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>

#include "lv_display.h"

static VisList *__lv_plugins_display_class;
static VisList *__lv_plugins_display_type;

LvdDriver *lvdisplay_driver_create(const char *nclass, const char* ntype)
{
	VisList *lv_plugins;
	LvdDriver *drv;
	VisPluginRef *rclass, *rtype;
	VisPluginData *plclass, *pltype;
	int r1, r2;

	lv_plugins = visual_plugin_get_registry();

	if (!visual_is_initialized()){
		fprintf(stderr, "Oops! Libvisual is not initialized!\n");
		return NULL;
	}

	__lv_plugins_display_class = visual_plugin_registry_filter (lv_plugins, VISUAL_PLUGIN_TYPE_DISPLAY_CLASS);
	__lv_plugins_display_type = visual_plugin_registry_filter (lv_plugins, VISUAL_PLUGIN_TYPE_DISPLAY_TYPE);

	rclass = visual_plugin_find(__lv_plugins_display_class, (char*)nclass);
	rtype = visual_plugin_find(__lv_plugins_display_type, (char*)ntype);

	if ((rclass == NULL) || (rtype == NULL)){
		return NULL;
	}

	plclass = visual_plugin_load(rclass);
	if (plclass == NULL){
		return NULL;
	}

	pltype = visual_plugin_load(rtype);
	if (pltype == NULL){
		visual_plugin_unload(plclass);
		return NULL;
	}

	drv = malloc(sizeof(LvdDriver));
	if (drv == NULL){
		visual_plugin_unload(plclass);
		visual_plugin_unload(pltype);
		return NULL;
	}

	r1 = visual_plugin_realize(plclass);
	r2 = visual_plugin_realize(pltype);

	if (r1 || r2){
		visual_plugin_unload(plclass);
		visual_plugin_unload(pltype);
		free(drv);
	}

	memset(drv, 0, sizeof(LvdDriver));

	drv->pclass = plclass;
	drv->ptype = pltype;
	drv->video = visual_video_new();

	return drv;
}

void lvdisplay_driver_delete(LvdDriver *drv)
{
	assert(drv);

	if (drv->pclass)
		visual_plugin_unload(drv->pclass);

	if (drv->ptype)
		visual_plugin_unload(drv->ptype);

	if (drv->params)
		free(drv->params);

	if (visual_video_have_allocated_buffer(drv->video))
		visual_video_free_with_buffer(drv->video);
	else
		visual_video_free(drv->video);

	free(drv);
}


int lvdisplay_driver_set_opts(LvdDriver *drv, int *params)
{
	int i, res;

	assert(drv);
	/*
	 * send params to type
	 * and store result in vodriver->params.
	 * also check class ant type compat.
	 *
	 * return 0 if params are accepted -1 otherwise
	 */

	if (params){
		for (i=0;params[i] != LVD_SET_DONE;i++);
		drv->params_cnt = i;
		i *= sizeof(int);
		drv->params = malloc(i);
		memcpy(drv->params, params, i);
	} else {
		drv->params = NULL;
		drv->params_cnt = 0;
	}

	res = ((LvdFrontendDescription*)drv->ptype->info->plugin)->
			create(drv->ptype, &drv->params, &drv->params_cnt, drv->video);

	if (res){
		return -1;
	}

	drv->compat_data = ((LvdFrontendDescription*)drv->ptype->info->plugin)->
			get_compat_data(drv->ptype);

	drv->prepared = 1;
	return 0;
}




/***********************************************************************/


Lvd* lvdisplay_initialize(LvdDriver *drv)
{
	int res;
	Lvd *v;

	v = malloc(sizeof(Lvd));
	if (v == NULL){
		return NULL;
	}
	memset(v, 0, sizeof(Lvd));

	v->drv = drv;

	if (!drv->prepared){
		if ((res = lvdisplay_driver_set_opts(drv, NULL)) != 0)
			return NULL;
	}

	/* init drv's class */
	res = ((LvdBackendDescription*)drv->pclass->info->plugin)->
		setup(drv->pclass, drv->compat_data,drv->params,
		drv->params_cnt);

	if (res)
		return NULL;

	v->bin = visual_bin_new();

	return v;
}

VisVideo *lvdisplay_visual_get_video(Lvd *v)
{
	return v->drv->video;
}

int lvdisplay_realize(Lvd *v)
{
	VisActor *actor;
	VisVideoDepth adepth;

	actor = visual_bin_get_actor(v->bin);
	if (actor == NULL){
		return 1;
	}

	adepth = visual_actor_get_supported_depth(actor);

	// XXX setup video for actor
	if (adepth & VISUAL_VIDEO_DEPTH_GL){
		visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_GL);
	} else
	if (adepth & VISUAL_VIDEO_DEPTH_32BIT) {
		visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_32BIT);
		visual_video_set_dimension(v->drv->video, v->drv->video->width, v->drv->video->height);
		visual_video_allocate_buffer(v->drv->video);
	} else
	if (adepth & VISUAL_VIDEO_DEPTH_24BIT) {
		visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_24BIT);
		visual_video_set_dimension(v->drv->video, v->drv->video->width, v->drv->video->height);
		visual_video_allocate_buffer(v->drv->video);
	} else
	if (adepth & VISUAL_VIDEO_DEPTH_16BIT) {
		visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_16BIT);
		visual_video_set_dimension(v->drv->video, v->drv->video->width, v->drv->video->height);
		visual_video_allocate_buffer(v->drv->video);
	} else
	if (adepth & VISUAL_VIDEO_DEPTH_8BIT) {
		visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_8BIT);
		visual_video_set_dimension(v->drv->video, v->drv->video->width, v->drv->video->height);
		visual_video_allocate_buffer(v->drv->video);
	} else {
		return 1;
	}

	v->ctx = ((LvdBackendDescription*)v->drv->pclass->info->plugin)->
		context_create(v->drv->pclass, v->drv->video);

	((LvdBackendDescription*)v->drv->pclass->info->plugin)->
		context_activate(v->drv->pclass, v->ctx);

	visual_bin_realize(v->bin);
	visual_actor_set_video(actor, v->drv->video);
	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	return 0;
}


void lvdisplay_finalize(Lvd *v)
{
	LvdDriver *drv = v->drv;
	assert(drv);
	assert(drv->pclass);

	if (v->ctx){
		((LvdBackendDescription*)drv->pclass->info->plugin)->context_delete(drv->pclass, v->ctx);
	}

	visual_bin_destroy(v->bin);
}

VisBin *lvdisplay_visual_get_bin(Lvd *v)
{
	return v->bin;
}

int lvdisplay_run(Lvd *v)
{
	LvdDriver *drv = v->drv;
	assert(drv);
	assert(drv->pclass);
	assert(drv->ptype);

	drv->ptype->info->events(drv->ptype, &drv->ptype->eventqueue);

	visual_bin_realize(v->bin);
	visual_bin_run(v->bin);

	((LvdBackendDescription*)drv->pclass->info->plugin)->draw(drv->pclass);

	return 0;
}

VisEventQueue *lvdisplay_get_eventqueue(Lvd *v)
{
	LvdDriver *drv = v->drv;
	assert(drv);
	assert(drv->ptype);

	return &drv->ptype->eventqueue;
}

int lvdisplay_poll_event(Lvd *v, VisEvent *event)
{
	return visual_event_queue_poll(lvdisplay_get_eventqueue(v), event);
}
