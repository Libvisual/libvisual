#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "lvconfig.h"
#include "lv_log.h"
#include "lv_list.h"
#include "lv_actor.h"

extern VisList *__lv_plugins_actor;

static VisActorPlugin *get_actor_plugin (VisActor *actor);

static VisActorPlugin *get_actor_plugin (VisActor *actor)
{
	VisActorPlugin *actplugin;

	visual_log_return_val_if_fail (actor != NULL, NULL);
	visual_log_return_val_if_fail (actor->plugin != NULL, NULL);

	actplugin = actor->plugin->plugin.actorplugin;

	return actplugin;
}

/**
 * @defgroup VisActor VisActor
 * @{
 */

/**
 * Gives a list of VisActors in the current plugin registry.
 *
 * @return An VisList containing the VisActors in the plugin registry.
 */
VisList *visual_actor_get_list ()
{
	return __lv_plugins_actor;
}

/**
 * Gives the next actor plugin based on the name of a plugin.
 *
 * @see visual_actor_get_prev_by_name
 * 
 * @param name The name of the current plugin, or NULL to get the first.
 *
 * @return The name of the next plugin within the list.
 */
char *visual_actor_get_next_by_name (char *name)
{
	return visual_plugin_get_next_by_name (visual_actor_get_list (), name);
}

/**
 * Gives the previous actor plugin based on the name of a plugin.
 *
 * @see visual_actor_get_next_by_name
 * 
 * @param name The name of the current plugin. or NULL to get the last.
 *
 * @return The name of the previous plugin within the list.
 */
char *visual_actor_get_prev_by_name (char *name)
{
	return visual_plugin_get_prev_by_name (visual_actor_get_list (), name);
}

/**
 * Checks if the actor plugin is in the registry, based on it's name.
 *
 * @param name The name of the plugin that needs to be checked.
 *
 * @return TRUE if found, else FALSE.
 */
int visual_actor_valid_by_name (char *name)
{
	if (_lv_plugin_find (visual_actor_get_list (), name) == NULL)
		return FALSE;
	else
		return TRUE;
}

/**
 * Creates a new actor from name, the plugin will be loaded but won't be realized.
 *
 * @param actorname
 * 	The name of the plugin to load, or NULL to simply allocate a new
 * 	actor. 
 *
 * @return A newly allocated VisActor, optionally containing a loaded plugin. Or NULL on error.
 */
VisActor *visual_actor_new (char *actorname)
{
	VisActor *actor;
	VisPluginRef *ref;

	if (__lv_plugins_actor == NULL && actorname != NULL) {
		visual_log (VISUAL_LOG_CRITICAL, "the plugin list is NULL");
		return NULL;
	}
	
	actor = malloc (sizeof (VisActor));
	memset (actor, 0, sizeof (VisActor));

	if (actor == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"Could not get memory for a new VisActor struct");
		return NULL;
	}

	if (actorname == NULL)
		return actor;

	ref = _lv_plugin_find (__lv_plugins_actor, actorname);

	actor->plugin = _lv_plugin_load (ref);

	return actor;
}

/**
 * Realize the VisActor. This also calls the plugin init function.
 *
 * @param actor Pointer to a VisActor that needs to be realized.
 *
 * @return 0 on succes -1 on error.
 */
int visual_actor_realize (VisActor *actor)
{
	visual_log_return_val_if_fail (actor != NULL, -1);
	visual_log_return_val_if_fail (actor->plugin != NULL, -1);

	_lv_plugin_realize (actor->plugin);

	return 0;
}

/**
 * Destroy the VisActor. This unloads the plugin when it's loaded, and also frees the actor itself including
 * all it's members.
 *
 * @param actor Pointer to a VisActor that needs to be destroyed.
 *
 * @return 0 on succes -1 on error.
 */ 
int visual_actor_destroy (VisActor *actor)
{
	visual_log_return_val_if_fail (actor != NULL, -1);

	if (actor->plugin != NULL)
		_lv_plugin_unload (actor->plugin);

	return visual_actor_free (actor);
}

/**
 * Free the VisActor. This frees the VisActor data structure, but does not destroy the plugin within. If this is desired
 * use visual_actor_destroy.
 *
 * @see visual_actor_destroy
 * 
 * @param actor Pointer to a VisActor that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_actor_free (VisActor *actor)
{
	visual_log_return_val_if_fail (actor != NULL, -1);

	if (actor->transform != NULL)
		visual_video_free_with_buffer (actor->transform);

	if (actor->fitting != NULL)
		visual_video_free_with_buffer (actor->fitting);
		
	if (actor->songinfo != NULL)
		visual_songinfo_free (actor->songinfo);

	if (actor->songcompare != NULL)
		visual_songinfo_free (actor->songcompare);

	free (actor);
	
	return 0;
}

/**
 * Pumps events from the VisActor into the plugin event handler. When visual_actor_run
 * is used to run the actor, which is highly adviced. Events get pumped automatically
 * and there is no need to call this function manually.
 *
 * @param actor Pointer to a VisActor of which the events needs to be pumped.
 *
 * @return 0 on succes -1 on error.
 */
int visual_actor_events_pump (VisActor *actor)
{
	VisActorPlugin *actplugin;

	visual_log_return_val_if_fail (actor != NULL, -1);

	actplugin = get_actor_plugin (actor);

	if (actplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"The given actor does not reference any actor plugin");
		return -1;
	}

	if (actplugin->events != NULL)
		actplugin->events (actplugin, &actor->events);

	return 0;
}

/**
 * Gives a pointer to the event queue within the VisActor. This event queue can be used to
 * queue user events like mouse and keyboard that need to be handled by the plugin.
 * 
 * @see VisEventQueue
 * @see VisEvent
 * 
 * @param actor Pointer to a VisActor of which the event queue is needed.
 *
 * @return Pointer to the event queue structure on succes or NULL on error.
 */
VisEventQueue *visual_actor_get_eventqueue (VisActor *actor)
{
	visual_log_return_val_if_fail (actor != NULL, NULL);

	return &actor->events;
}

/**
 * Gives a pointer to the song info data within the VisActor. This song info data can be used
 * to set name, artist and even coverart which can be used by the plugins and the framework itself.
 *
 * @see VisSongInfo
 *
 * @param actor Pointer to a VisActor of which the song info is needed.
 *
 * @return Pointer to the song info structure on succes or NULL on error.
 */
VisSongInfo *visual_actor_get_songinfo (VisActor *actor)
{
	visual_log_return_val_if_fail (actor != NULL, NULL);

	if (actor->songinfo == NULL)
		actor->songinfo = visual_songinfo_new (VISUAL_SONGINFO_TYPE_NULL);
	
	return actor->songinfo;
}

/**
 * Gives a pointer to the palette within the VisActor. This can be needed to set a palette on the target
 * display when it's in index mode.
 *
 * @see VisPalette
 *
 * @param actor Pointer to a VisActor of which the palette is needed.
 *
 * @return Pointer to the palette structure on succes or NULL on error. Also it's possible that NULL
 * is returned when the plugin is running in a full color mode or openGL. The returned palette is
 * read only.
 */
VisPalette *visual_actor_get_palette (VisActor *actor)
{
	VisActorPlugin *actplugin;

	visual_log_return_val_if_fail (actor != NULL, NULL);

	actplugin = get_actor_plugin (actor);
	
	if (actplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"The given actor does not reference any actor plugin");
		return NULL;
	}

	if (actor->transform != NULL &&
		actor->video->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		
		return actor->ditherpal;

	} else {
		return actplugin->palette (actplugin);
	}

	return NULL;
}

/**
 * This function negotiates the VisActor with it's target video that is set by visual_actor_set_video.
 * When needed it also sets up size fitting environment and depth transformation environment.
 *
 * The function has a few extra arguments that are mainly to be used from within internal code.
 *
 * This function needs to be called everytime there is a change within either the size or depth of
 * the target video. 
 *
 * The main method of calling this function is: "visual_actor_video_negotiate (actor, 0, FALSE, FALSE)"
 * 
 * @see visual_actor_set_video
 *
 * @param actor Pointer to a VisActor that needs negotiation.
 * @param rundepth An depth in the form of the VISUAL_VIDEO_DEPTH_* style when a depth is forced.
 * 	  This could be needed when for example a plugin has both a 8 bits and a 32 bits display method
 * 	  but while the target video is in 32 bits you still want to run the plugin in 8 bits. If this
 * 	  is desired the "forced" argument also needs to be set on TRUE.
 * @param noevent When set on TRUE this does only renegotiate depth transformation environments. For example
 * 	  when the target display was running in 32 bits and switched to 8 bits while the plugin was already
 * 	  in 8 bits it doesn't need an events, which possibly reinitializes the plugin.
 * @param forced This should be set if the rundepth argument is set, so it forces the plugin in a certain
 * 	  depth.
 *
 * @return 0 on succes -1 on error. 
 */ 
int visual_actor_video_negotiate (VisActor *actor, int rundepth, int noevent, int forced)
{
	VisActorPlugin *actplugin = get_actor_plugin (actor);
	int tmpwidth, tmpheight, tmppitch;
	int depthflag;

	/*
	 * Uhau, we really check the structure sanity.
	 */
	visual_log_return_val_if_fail (actor != NULL, -1);
	visual_log_return_val_if_fail (actor->plugin != NULL, -1);
	visual_log_return_val_if_fail (actor->plugin->ref != NULL, -1);
	visual_log_return_val_if_fail (actor->video != NULL, -1);

	if (actor->transform != NULL) {
		visual_video_free_with_buffer (actor->transform);
		actor->transform = NULL;
	}
	
	if (actor->fitting != NULL) {
		visual_video_free_with_buffer (actor->fitting);
		actor->fitting = NULL;
	}

	if (actor->ditherpal != NULL) {
		visual_palette_free (actor->ditherpal);
		actor->ditherpal = NULL;
	}

	depthflag = visual_actor_get_supported_depth (actor);

	visual_log (VISUAL_LOG_INFO, "negotiating plugin %s", actor->plugin->ref->name);
	/* Set up depth transformation enviroment */
	if (visual_video_depth_is_supported (depthflag, actor->video->depth) != TRUE ||
			(forced == TRUE && actor->video->depth != rundepth)) {

		/* Depth transform enviroment, it automaticly
		 * fits size because it can use the pitch from
		 * the dest video context */
		actor->transform = visual_video_new ();

		if (forced == TRUE)
			visual_video_set_depth (actor->transform, rundepth);
		else
			visual_video_set_depth (actor->transform,
					visual_video_depth_get_highest_nogl (depthflag));

		visual_log (VISUAL_LOG_INFO, "transpitch1 %d", actor->transform->pitch);
		/* If there is only GL (which gets returned by highest nogl if
		 * nothing else is there, stop here */
		if (actor->transform->depth == VISUAL_VIDEO_DEPTH_GL)
			return -1;

		visual_video_set_dimension (actor->transform, actor->video->width, actor->video->height);
		visual_log (VISUAL_LOG_INFO, "transpitch2 %d %d", actor->transform->width, actor->transform->pitch);
		
		actplugin->requisition (actplugin, &actor->transform->width, &actor->transform->height);
		visual_log (VISUAL_LOG_INFO, "transpitch3 %d", actor->transform->pitch);
		
		if (noevent == FALSE) {
			visual_event_queue_add_resize (&actor->events, actor->transform,
					actor->transform->width, actor->transform->height);
			visual_actor_events_pump (actor);
		} else {
			/* Normally a visual_video_set_dimension get's called within the
			 * event handler, but we won't come there right now so we've
			 * got to set the pitch ourself */
			visual_video_set_dimension (actor->transform,
					actor->transform->width, actor->transform->height);
		}
		
		visual_log (VISUAL_LOG_INFO, "rundepth: %d transpitch %d\n", rundepth, actor->transform->pitch);
		visual_video_allocate_buffer (actor->transform);

		if (actor->video->depth == VISUAL_VIDEO_DEPTH_8BIT)
			actor->ditherpal = visual_palette_new ();

	} else {
		tmpwidth = actor->video->width;
		tmpheight = actor->video->height;
		tmppitch = actor->video->pitch;

		/* Pump the resize events and handle all the pending events */
		actplugin->requisition (actplugin, &actor->video->width, &actor->video->height);
		
		if (noevent == FALSE) {
			visual_event_queue_add_resize (&actor->events, actor->video,
					actor->video->width, actor->video->height);
			visual_actor_events_pump (actor);
		}

		/* Size fitting enviroment */
		if (tmpwidth != actor->video->width || tmpheight != actor->video->height) {
			actor->fitting = visual_video_new_with_buffer (actor->video->width,
					actor->video->height, actor->video->depth);

			visual_video_set_dimension (actor->video, tmpwidth, tmpheight);
		}

		/* Set the pitch seen this is the framebuffer context */
		visual_video_set_pitch (actor->video, tmppitch);
	}

	return 0;
}

/**
 * Gives the by the plugin natively supported depths
 *
 * @param actor Pointer to a VisActor of which the supported depth of it's
 * 	  encapsulated plugin is requested.
 *
 * @return an OR value of the VISUAL_VIDEO_DEPTH_* values which can be checked against using AND on succes, -1 on error
 */
int visual_actor_get_supported_depth (VisActor *actor)
{
	VisActorPlugin *actplugin;

	visual_log_return_val_if_fail (actor != NULL, -1);

	actplugin = get_actor_plugin (actor);

	if (actplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"The given actor does not reference any actor plugin");
		return -1;
	}

	return actplugin->depth;
}

/**
 * Used to connect the target display it's VisVideo structure to the VisActor.
 *
 * Using the visual_video methods the screenbuffer, it's depth and dimension and optionally it's pitch
 * can be set so the actor plugins know about their graphical environment and have a place to draw.
 *
 * After this function it's most likely that visual_actor_video_negotiate needs to be called.
 *
 * @see visual_video_new
 * @see visual_actor_video_negotiate
 * 
 * @param actor Pointer to a VisActor to which the VisVideo needs to be set.
 * @param video Pointer to a VisVideo which contains information about the target display and the pointer
 * 	  to it's screenbuffer.
 *
 * @return 0 on succes -1 on error.
 */
int visual_actor_set_video (VisActor *actor, VisVideo *video)
{
	visual_log_return_val_if_fail (actor != NULL, -1);

	actor->video = video;

	return 0;
}

/**
 * This is called to run an VisActor. It also pump it's events when needed, checks for new song events and also does the fitting 
 * and depth transformation actions when needed.
 *
 * Every run cycle one frame is created, so this function needs to be used in the main draw loop of the application.
 *
 * @param actor Pointer to a VisActor that needs to be runned.
 * @param audio Pointer to a VisAudio that contains all the audio data.
 */
int visual_actor_run (VisActor *actor, VisAudio *audio)
{
	VisActorPlugin *actplugin;
	VisVideo *video;
	VisVideo *transform;
	VisVideo *fitting;

	/* We don't check for video, because we don't always need a video */
	/*
	 * Really? take a look at visual_video_set_palette bellow
	 */
	visual_log_return_val_if_fail (actor != NULL, -1);
	visual_log_return_val_if_fail (actor->video != NULL, -1);
	visual_log_return_val_if_fail (audio != NULL, -1);

	actplugin = get_actor_plugin (actor);

	if (actplugin == NULL) {
		visual_log (VISUAL_LOG_CRITICAL,
			"The given actor does not reference any actor plugin");
		return -1;
	}

	/* Songinfo handling */
	if (actor->songinfo != NULL) {
		if (actor->songcompare == NULL) {
			visual_songinfo_mark (actor->songinfo);
		
			if (actplugin->events != NULL)
				visual_event_queue_add_newsong (&actor->events, actor->songinfo);

			actor->songcompare = visual_songinfo_new (VISUAL_SONGINFO_TYPE_NULL);
		}

		if (visual_songinfo_compare (actor->songinfo, actor->songcompare) != 0) {
			visual_songinfo_mark (actor->songinfo);

			if (actplugin->events != NULL)
				visual_event_queue_add_newsong (&actor->events, actor->songinfo);
		}
		/** @todo should be freeing and copying everytime, should done more effecient */
		visual_songinfo_free_strings (actor->songcompare);
		visual_songinfo_copy (actor->songcompare, actor->songinfo);
	}
	
	actplugin->songinfo = actor->songinfo;

	video = actor->video;
	transform = actor->transform;
	fitting = actor->fitting;

	visual_video_set_palette (video, visual_actor_get_palette (actor));
	
	visual_actor_events_pump (actor);

	/* Yeah some transformation magic is going on here when needed */
	if (transform != NULL && (transform->depth != video->depth)) {
		actplugin->render (actplugin, transform, audio);

		if (transform->depth == VISUAL_VIDEO_DEPTH_8BIT) {
			visual_video_set_palette (transform, visual_actor_get_palette (actor));
			visual_video_depth_transform (video, transform);
		} else {
			visual_video_set_palette (transform, actor->ditherpal);
			visual_video_depth_transform (video, transform);
		}
	} else {
		if (fitting != NULL && (fitting->width != video->width || fitting->height != video->height)) {
			actplugin->render (actplugin, fitting, audio);
			visual_video_blit_overlay (video, fitting, 0, 0, FALSE);
		} else {
			actplugin->render (actplugin, video, audio);
		}
	}

	return 0;
}

/**
 * @}
 */

