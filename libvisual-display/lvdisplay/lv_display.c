#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>

#include "lv_display.h"

static int lvdisplay_driver_dtor(VisObject *drv_obj);
static int lvdisplay_dtor(VisObject *v_obj);

// visual_log_return_val_if_fail (dest != NULL, -1);  

static VisList *__lv_plugins_display_be = NULL;
static VisList *__lv_plugins_display_fe = NULL;

static void reread_plugin_lists()
{
	const VisList *lv_plugins;

	lv_plugins = visual_plugin_get_registry();
	/*
	 * XXX actually lists should be freed(?) and reloaded.
	 */
	if ((!__lv_plugins_display_be) || (!__lv_plugins_display_fe)){
		__lv_plugins_display_be = visual_plugin_registry_filter (lv_plugins, VISUAL_PLUGIN_TYPE_DISPLAY_BACKEND);
		__lv_plugins_display_fe = visual_plugin_registry_filter (lv_plugins, VISUAL_PLUGIN_TYPE_DISPLAY_FRONTEND);
	}
}

LvdDriver *lvdisplay_driver_create(const char *bename, const char *fename)
{
	LvdDriver *drv;
	VisPluginRef *beref, *feref;
	VisPluginData *beplug, *feplug;
	int r1, r2;

	visual_log_return_val_if_fail (bename != NULL, NULL);
	visual_log_return_val_if_fail (fename != NULL, NULL);

	if (!visual_is_initialized()){
		visual_log(VISUAL_LOG_CRITICAL, "Libvisual is not initialized!\n");
		// Hm... should Libvisual be initialized here?
		return NULL;
	}

	reread_plugin_lists();

	beref = visual_plugin_find(__lv_plugins_display_be, bename);
	feref = visual_plugin_find(__lv_plugins_display_fe, fename);

	if ((beref == NULL) || (feref == NULL)){
		visual_log(VISUAL_LOG_DEBUG, "Failed to get plugin references %p %p\n", beref, feref);
		return NULL;
	}

	beplug = visual_plugin_load(beref);
	if (beplug == NULL){
		visual_log(VISUAL_LOG_DEBUG, "Failed to load backend plugin\n");
		return NULL;
	}

	feplug = visual_plugin_load(feref);
	if (feplug == NULL){
		visual_log(VISUAL_LOG_DEBUG, "Failed to load frontend plugin\n");
		visual_plugin_unload(beplug);
		return NULL;
	}

	drv = visual_mem_new0(LvdDriver, 1);
	if (drv == NULL){
		visual_plugin_unload(beplug);
		visual_plugin_unload(feplug);
		return NULL;
	}

	VISUAL_OBJECT(drv)->allocated = TRUE;
	VISUAL_OBJECT(drv)->dtor = lvdisplay_driver_dtor;
	visual_object_ref(VISUAL_OBJECT(drv));


	r1 = visual_plugin_realize(beplug);
	r2 = visual_plugin_realize(feplug);

	if (r1 || r2){
		visual_log(VISUAL_LOG_DEBUG, "Failed to realize plugins %d %d\n", r1, r2);
		visual_plugin_unload(beplug);
		visual_plugin_unload(feplug);
		visual_object_unref(VISUAL_OBJECT(drv));
		return NULL;
	}

	drv->beplug = beplug;
	drv->feplug = feplug;

	drv->video = visual_video_new();
	if (drv->video == NULL){
		visual_object_unref(VISUAL_OBJECT(drv));
		return NULL;
	}

	drv->be = beplug->info->plugin;
	drv->fe = feplug->info->plugin;

	return drv;
}

int lvdisplay_driver_dtor(VisObject *drv_obj)
{
	LvdDriver *drv = (LvdDriver*)drv_obj;

	if (drv->beplug)
		visual_plugin_unload(drv->beplug);

	if (drv->feplug)
		visual_plugin_unload(drv->feplug);

	if (drv->params)
		visual_mem_free(drv->params);

	visual_object_unref (VISUAL_OBJECT (drv->video));

	drv->beplug = NULL;
	drv->feplug = NULL;
	drv->be = NULL;
	drv->fe = NULL;
	drv->params = NULL;
	drv->video = NULL;

	return VISUAL_OK;
}


int lvdisplay_driver_set_opts(LvdDriver *drv, int *params)
{
	int i, res;

	visual_log_return_val_if_fail (drv != NULL, -1);

	/*
	 * send params to type
	 * and store result in vodriver->params.
	 * also check class and type compat.
	 *
	 * return 0 if params are accepted !0 otherwise
	 */

	if (params){
		for (i=0;params[i] != LVD_SET_DONE;i++);
		drv->params_cnt = i;
		i *= sizeof(int);
		drv->params = visual_mem_malloc0(i);
		memcpy(drv->params, params, i);
	} else {
		drv->params = NULL;
		drv->params_cnt = 0;
	}

	res = drv->fe->create(drv->feplug, &drv->params, &drv->params_cnt,
			drv->video);

	if (res){
		visual_log(VISUAL_LOG_DEBUG, "Frontend's create() failed %d\n", res);
		return res;
	}

	drv->compat_data = drv->fe->get_compat_data(drv->feplug);

	drv->prepared = 1;
	return 0;
}




/***********************************************************************/
/// XXX make it per-thread

static Lvd *actx_v = NULL;
static LvdDContext *actx = NULL;

static void set_active_context(Lvd *v, LvdDContext *ctx)
{
// XXX MUTEX IN
	if ((actx_v == v) && (actx == ctx))
		return;

	if ((actx_v != NULL) && (actx != NULL))
	actx_v->be->context_deactivate(actx_v->beplug, actx);
	v->be->context_activate(v->beplug, ctx);

	actx_v = v;
	actx = ctx;
// XXX MUTEX OUT
}

static void active_context_release()
{
	if ((actx_v == NULL) || (actx == NULL))
		return;

	actx_v->be->context_deactivate(actx_v->beplug, actx);

	actx_v = NULL;
	actx = NULL;
}



Lvd* lvdisplay_initialize(LvdDriver *drv)
{
	int res;
	Lvd *v;

	visual_log_return_val_if_fail (drv != NULL, NULL);

	v = visual_mem_new0(Lvd, 1);
	if (v == NULL)
		return NULL;

	VISUAL_OBJECT(v)->allocated = TRUE;
	VISUAL_OBJECT(v)->dtor = lvdisplay_dtor;
	visual_object_ref(VISUAL_OBJECT(v));

	visual_object_ref(VISUAL_OBJECT(drv));

	v->drv = drv;
	/* shortcuts */
	v->be = drv->be;
	v->fe = drv->fe;
	v->beplug = drv->beplug;
	v->feplug = drv->feplug;

	if (!drv->prepared){
		if ((res = lvdisplay_driver_set_opts(drv, NULL)) != 0){
			visual_object_unref(VISUAL_OBJECT(v));
			return NULL;
		}
	}

	/* init drv's class */
	res = drv->be->setup(drv->beplug, drv->compat_data,
			drv->params, drv->params_cnt);

	if (res) {
		visual_log(VISUAL_LOG_DEBUG, "Backend's setup() failed %d\n", res);
		visual_object_unref(VISUAL_OBJECT(v));
		return NULL;
	}

	v->bin = visual_bin_new();
	if (v->bin == NULL) {
		visual_object_unref(VISUAL_OBJECT(v));
		return NULL;
	}

	return v;
}

VisVideo *lvdisplay_visual_get_video(Lvd *v)
{
	visual_log_return_val_if_fail (v != NULL, NULL);
	visual_object_ref(VISUAL_OBJECT(v->drv->video));
	return v->drv->video;
}

int lvdisplay_realize(Lvd *v)
{
	VisActor *actor;
	VisVideoDepth adepth;

	visual_log_return_val_if_fail (v != NULL, -1);

	actor = visual_bin_get_actor(v->bin);
	if (actor == NULL){
		return 1;
	}

	adepth = visual_actor_get_supported_depth(actor);

	// XXX setup video for actor
	if (adepth & VISUAL_VIDEO_DEPTH_GL){
		visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_GL);
	} else {
		int pitch;

		if (adepth & VISUAL_VIDEO_DEPTH_32BIT) {
			visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_32BIT);
		} else
		if (adepth & VISUAL_VIDEO_DEPTH_24BIT) {
			visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_24BIT);
		} else
		if (adepth & VISUAL_VIDEO_DEPTH_16BIT) {
			visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_16BIT);
		} else
		if (adepth & VISUAL_VIDEO_DEPTH_8BIT) {
			visual_video_set_depth(v->drv->video, VISUAL_VIDEO_DEPTH_24BIT);
		} else {
			visual_log(VISUAL_LOG_DEBUG, "Unknown actor plugin depth %d\n", adepth);
			return 1;
		}

		visual_video_set_dimension(v->drv->video, v->drv->video->width, v->drv->video->height);

		pitch = v->drv->video->width * v->drv->video->bpp;
		if (pitch&3)
			pitch = (pitch|3) + 1;
		visual_video_set_pitch(v->drv->video, pitch);

		visual_video_allocate_buffer(v->drv->video);
	}

	active_context_release();

	if (v->ctx)
		v->be->context_delete(v->beplug, v->ctx);

	v->ctx = v->be->context_create(v->beplug, v->drv->video);
	if (v->ctx == NULL){
		visual_log(VISUAL_LOG_DEBUG, "Failed to create context\n");
		return 1;
	}

	set_active_context(v, v->ctx);

	visual_bin_realize(v->bin);
	visual_actor_set_video(actor, v->drv->video);
	visual_actor_video_negotiate (actor, 0, FALSE, FALSE);

	return 0;
}


int lvdisplay_dtor(VisObject *v_obj)
{
	Lvd *v = (Lvd*)v_obj;

	active_context_release();

	if (v->ctx)
		v->be->context_delete(v->beplug, v->ctx);

	if (v->bin)
		visual_object_unref(VISUAL_OBJECT(v->bin));

	if (v->drv)
		visual_object_unref(VISUAL_OBJECT(v->drv));

	v->ctx = NULL;
	v->bin = NULL;
	v->drv = NULL;

	return VISUAL_OK;
}

VisBin *lvdisplay_visual_get_bin(Lvd *v)
{
	visual_log_return_val_if_fail (v != NULL, NULL);
	visual_object_ref(VISUAL_OBJECT(v->bin));
	return v->bin;
}

int lvdisplay_run(Lvd *v)
{
	visual_log_return_val_if_fail (v != NULL, -1);

	set_active_context(v, v->ctx);

	visual_bin_realize(v->bin);
	visual_bin_run(v->bin);

	v->be->draw(v->beplug);

	return 0;
}

VisEventQueue *lvdisplay_get_eventqueue(Lvd *v)
{
	visual_log_return_val_if_fail (v != NULL, NULL);
	visual_object_ref(VISUAL_OBJECT(&v->feplug->eventqueue));
	return &v->feplug->eventqueue;
}

int lvdisplay_poll_event(Lvd *v, VisEvent *event)
{
	visual_log_return_val_if_fail (v != NULL, 0);

	set_active_context(v, v->ctx);

	v->feplug->info->events(v->feplug, &v->feplug->eventqueue);
	// pass events we got to backend.
	// one can handle resize event, for example.
	v->beplug->info->events(v->beplug, &v->feplug->eventqueue);

	return visual_event_queue_poll(lvdisplay_get_eventqueue(v), event);
}
