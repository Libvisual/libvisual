/* Libvisual-display - The display library for libvisual.
 * 
 * Copyright (C) 2004, 2005 Vitaly V. Bursov <vitalyvb@ukr.net> 
 *
 * Authors: Vitaly V. Bursov <vitalyvb@ukr.net>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

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
	VisList *lv_plugins;

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

	visual_object_initialize(VISUAL_OBJECT(drv), TRUE, lvdisplay_driver_dtor);


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

	drv->be = (void*)beplug->info->plugin;
	drv->fe = (void*)feplug->info->plugin;

	return drv;
}

int lvdisplay_driver_dtor(VisObject *drv_obj)
{
	LvdDriver *drv = (LvdDriver*)drv_obj;

	if (drv->beplug)
		visual_plugin_unload(drv->beplug);

	if (drv->feplug)
		visual_plugin_unload(drv->feplug);

	visual_object_unref (VISUAL_OBJECT (drv->video));

	drv->beplug = NULL;
	drv->feplug = NULL;
	drv->be = NULL;
	drv->fe = NULL;
	drv->video = NULL;

	return VISUAL_OK;
}


int lvdisplay_driver_realize(LvdDriver *drv)
{
	int res;

	visual_log_return_val_if_fail (drv != NULL, -1);

	if (drv->realized)
		return 0;

	res = drv->fe->create(drv->feplug, drv->video);

	if (res){
		visual_log(VISUAL_LOG_DEBUG, "Frontend's create() failed %d\n", res);
		return res;
	}

	drv->compat_data = drv->fe->get_compat_data(drv->feplug);

	/* init drv's class */
	res = drv->be->setup(drv->beplug, drv->compat_data);

	if (res) {
		visual_log(VISUAL_LOG_DEBUG, "Backend's setup() failed %d\n", res);
		return res;
	}

	drv->realized = 1;

	return 0;
}


int lvdisplay_driver_set_opts(LvdDriver *drv, int *params, int count)
{
	int i, res;

	visual_log_return_val_if_fail (drv != NULL, -1);

	if (!drv->realized){
		visual_log(VISUAL_LOG_DEBUG, "Driver is not realized\n");
		return -1;
	}

	/*
	 * send params to type
	 * and store result in vodriver->params.
	 * also check class and type compat.
	 *
	 * return 0 if params are accepted !0 otherwise
	 */

	if (params){
		// XXX avoid pointer arith
		while ((*params != LVD_SET_DONE) && (count > 0)){
			if (*params >= LVD_PARAM_LAST){ /* invalid parameter */
				visual_log(VISUAL_LOG_ERROR, "invalid parameter\n");
				break;
			}

			res = drv->fe->set_param(drv->feplug,
				*params, params+1, count-1);

			if (res <= 0) { /* unhandled parameter */
				visual_log(VISUAL_LOG_WARNING, "unhandled parameter. aborted\n");
				break;
			}

			params+=res+1;
			count-=res+1;
		}
	}

	return 0;
}

int lvdisplay_driver_set_visible(LvdDriver *drv, int is_visible)
{
	int param[2] = {LVD_SET_VISIBLE, is_visible};
	return lvdisplay_driver_set_opts(drv, param, 2);
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

/***********************************************************************/

Lvd* lvdisplay_initialize()
{
	int res;
	Lvd *v;

	v = visual_mem_new0(Lvd, 1);
	if (v == NULL)
		return NULL;

	visual_object_initialize(VISUAL_OBJECT(v), TRUE, lvdisplay_dtor);

	v->bin = visual_bin_new();
	if (v->bin == NULL) {
		visual_object_unref(VISUAL_OBJECT(v));
		return NULL;
	}

	return v;
}


int lvdisplay_set_driver(Lvd *v, LvdDriver *drv)
{
	int res;

	visual_log_return_val_if_fail (v != NULL, -1);
	visual_log_return_val_if_fail (drv != NULL, -1);

	if (v->drv){
		visual_object_unref(VISUAL_OBJECT(v->drv));
	}

	visual_object_ref(VISUAL_OBJECT(drv));

	v->drv = drv;
	/* shortcuts */
	v->be = drv->be;
	v->fe = drv->fe;
	v->beplug = drv->beplug;
	v->feplug = drv->feplug;

	if (!drv->realized){
		if ((res = lvdisplay_driver_realize(drv)) != 0){
			visual_object_unref(VISUAL_OBJECT(drv));
			v->drv = NULL;
			v->be = NULL;
			v->fe = NULL;
			v->beplug = NULL;
			v->feplug = NULL;
			return res;
		}
	}

	return 0;
}


VisVideo *lvdisplay_visual_get_video(Lvd *v)
{
	visual_log_return_val_if_fail (v != NULL, NULL);
	visual_object_ref(VISUAL_OBJECT(v->drv->video));
	return v->drv->video;
}

int lvdisplay_realize(Lvd *v)
{
	int res;
	VisActor *actor;
	VisVideoDepth adepth, vdepth;

	visual_log_return_val_if_fail (v != NULL, -1);

	actor = visual_bin_get_actor(v->bin);
	if (actor == NULL){
		visual_log(VISUAL_LOG_ERROR, "Bin contains no actor\n");
		return 1;
	}

	adepth = visual_actor_get_supported_depth(actor);
	vdepth = v->drv->be->get_supported_depths(v->drv->beplug);

	// XXX setup video for actor
	if (vdepth & adepth & VISUAL_VIDEO_DEPTH_GL){
		int depth = VISUAL_VIDEO_DEPTH_GL;
		res = v->drv->fe->set_param(v->drv->feplug, LVD_SET_DEPTH, &depth, 1);
	} else {
		int depth = vdepth & adepth;

		if (depth & VISUAL_VIDEO_DEPTH_32BIT) {
			depth = VISUAL_VIDEO_DEPTH_32BIT;
		} else
		if (depth & VISUAL_VIDEO_DEPTH_24BIT) {
			depth = VISUAL_VIDEO_DEPTH_24BIT;
		} else
		if (depth & VISUAL_VIDEO_DEPTH_16BIT) {
			depth = VISUAL_VIDEO_DEPTH_16BIT;
		} else
		if (depth & VISUAL_VIDEO_DEPTH_8BIT) {
			depth = VISUAL_VIDEO_DEPTH_8BIT;
		} else {
			// XXX choose best transformation

			if (adepth & vdepth){
				visual_log(VISUAL_LOG_ERROR, "WTF?\n");
				return 1;
			}

//			visual_log(VISUAL_LOG_WARNING, "Hm-m..... Can't display this actor with this video backend\n");
//			return 1;
			depth = VISUAL_VIDEO_DEPTH_32BIT;
		}

		res = v->drv->fe->set_param(v->drv->feplug, LVD_SET_DEPTH, &depth, 1);

		visual_video_allocate_buffer(v->drv->video);
	}

	active_context_release();

	if (v->ctx)
		v->be->context_delete(v->beplug, v->ctx);

	v->ctx = v->be->context_create(v->beplug, v->drv->video);
	if (v->ctx == NULL){
		visual_log(VISUAL_LOG_ERROR, "Failed to create context\n");
		return 2;
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
