#ifndef _LV_MORPH_H
#define _LV_MORPH_H

#include <libvisual/lv_palette.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_video.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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
	LVPlugin	*plugin;	/**< Pointer to the plugin itself. */
	VisVideo	*dest;		/**< Destination video, this is where
					 * the result of the morph gets drawn. */
	float		 rate;		/**< The rate of morph, 0 draws the first video source
					 * 1 the second video source, 0.5 is a 50/50, final
					 * content depends on the plugin being used. */
	VisPalette	 morphpal;	/**< Morph plugins can also set a palette for indexed
					 * color depths. */
};

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
int visual_morph_set_rate (VisMorph *morph, float rate);

VisPalette *visual_morph_get_palette (VisMorph *morph);

int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_MORPH_H */
