#ifndef _LV_ACTOR_H
#define _LV_ACTOR_H

#include <libvisual/lv_audio.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_songinfo.h>
#include <libvisual/lv_event.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _VisActor VisActor;

/**
 * The VisActor structure encapsulates the actor plugin and provides
 * abstract interfaces to the actor. The VisActor system 
 * it's methods are also capable of doing automatic size fitting 
 * and depth transformations, and it keeps track on songinfo and events.
 *
 * Members in the structure shouldn't be accessed directly but instead
 * it's adviced to use the methods provided.
 *
 * @see visual_actor_new
 */
struct _VisActor {
	LVPlugin	*plugin;	/**< Pointer to the plugin itself. */

	VisVideo	*video;		/**< Pointer to the target display video. 
					 * @see visual_actor_set_video */
	VisVideo	*transform;	/**< Private member which is used for depth transformation. */
	VisVideo	*fitting;	/**< Private member which is used to fit the plugin. */
	VisPalette	*ditherpal;	/**< Private member in which a palette is set when transforming
					 * depth from true color to indexed.
					 * @see visual_actor_get_palette */
	VisSongInfo	*songinfo;	/**< Pointer to the songinfo structure which is used to set
					 * informaton about the currently playing music.
					 * @see visual_actor_get_songinfo */
	VisSongInfo	*songcompare;	/**< Private member which is used to compare with new songinfo
					  * to check if a new song event should be emitted. */
	VisEventQueue	events;		/**< The event queue which is used to queue new events.
					  * @see visual_actor_get_eventqueue */
	void		*private;	/**< Private member for internal usage, currently unused. */
};

/* prototypes */
VisList *visual_actor_get_list (void);
char *visual_actor_get_next_by_name_gl (char *name);
char *visual_actor_get_prev_by_name_gl (char *name);
char *visual_actor_get_next_by_name_nogl (char *name);
char *visual_actor_get_prev_by_name_nogl (char *name);
char *visual_actor_get_next_by_name (char *name);
char *visual_actor_get_prev_by_name (char *name);
int visual_actor_valid_by_name (char *name);

VisActor *visual_actor_new (char *actorname);

int visual_actor_realize (VisActor *actor);
int visual_actor_destroy (VisActor *actor);
int visual_actor_free (VisActor *actor);

int visual_actor_events_pump (VisActor *actor);

VisEventQueue *visual_actor_get_eventqueue (VisActor *actor);
VisSongInfo *visual_actor_get_songinfo (VisActor *actor);
VisPalette *visual_actor_get_palette (VisActor *actor);

int visual_actor_video_negotiate (VisActor *actor, int rundepth, int noevent, int forced);
int visual_actor_get_supported_depth (VisActor *actor);

int visual_actor_set_video (VisActor *actor, VisVideo *video);

int visual_actor_run (VisActor *actor, VisAudio *audio);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_ACTOR_H */
