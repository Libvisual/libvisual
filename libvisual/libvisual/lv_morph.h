#ifndef _LV_MORPH_H
#define _LV_MORPH_H

#include <libvisual/lv_palette.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_time.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Morph morphing methods.
 */
typedef enum {
	VISUAL_MORPH_MODE_SET,		/**< Morphing is done by a rate set,
					  * nothing is automated here. */
	VISUAL_MORPH_MODE_STEPS,	/**< Morphing is done by setting a number of steps,
					  * the morph will be automated. */
	VISUAL_MORPH_MODE_TIME		/**< Morphing is done by setting a target time when the morph should be done,
					  * This is as well automated. */
} VisMorphMode;

typedef struct _VisMorph VisMorph;

/**
 * The VisMorph structure encapsulates the morph plugin and provides 
 * abstract interfaces for morphing between actors, or rather between
 * two video sources.
 *
 * Members in the structure shouldn't be accessed directly but instead
 * it's adviced to use the methods provided.
 *
 * @see visual_morph_new
 */
struct _VisMorph {
	VisPluginData	*plugin;	/**< Pointer to the plugin itself. */
	VisVideo	*dest;		/**< Destination video, this is where
					 * the result of the morph gets drawn. */
	float		 rate;		/**< The rate of morph, 0 draws the first video source
					 * 1 the second video source, 0.5 is a 50/50, final
					 * content depends on the plugin being used. */
	VisPalette	 morphpal;	/**< Morph plugins can also set a palette for indexed
					 * color depths. */
	VisTime		 morphtime;	/**< Amount of time which the morphing should take. */
	VisTimer	 timer;		/**< Private entry that holds the time elapsed from 
					 * the beginning of the switch. */
	int		 steps;		/**< Private entry that contains the number of steps
					 * a morph suppose to take. */
	int		 stepsdone;	/**< Private entry that contains the number of steps done. */

	VisMorphMode	 mode;		/**< Private entry that holds the mode of morphing. */
};

VisPluginData *visual_morph_get_plugin (VisMorph *morph);

VisList *visual_morph_get_list (void);
char *visual_morph_get_next_by_name (char *name);
char *visual_morph_get_prev_by_name (char *name);
int visual_morph_valid_by_name (char *name);

VisMorph *visual_morph_new (char *morphname);

int visual_morph_realize (VisMorph *morph);
int visual_morph_destroy (VisMorph *morph);
int visual_morph_free (VisMorph *morph);

int visual_morph_get_supported_depth (VisMorph *morph);

int visual_morph_set_video (VisMorph *morph, VisVideo *video);
int visual_morph_set_time (VisMorph *morph, VisTime *time);
int visual_morph_set_rate (VisMorph *morph, float rate);
int visual_morph_set_steps (VisMorph *morph, int steps);
int visual_morph_set_mode (VisMorph *morph, VisMorphMode mode);

VisPalette *visual_morph_get_palette (VisMorph *morph);

int visual_morph_is_done (VisMorph *morph);

int visual_morph_requests_audio (VisMorph *morph);

int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_MORPH_H */
