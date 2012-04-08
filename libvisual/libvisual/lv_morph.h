/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_morph.h,v 1.17 2006/01/27 20:18:26 synap Exp $
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

#ifndef _LV_MORPH_H
#define _LV_MORPH_H

#include <libvisual/lv_audio.h>
#include <libvisual/lv_palette.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_list.h>
#include <libvisual/lv_video.h>
#include <libvisual/lv_time.h>

/**
 * @defgroup VisMorph VisMorph
 * @{
 */

VISUAL_BEGIN_DECLS

#define VISUAL_MORPH(obj)               (VISUAL_CHECK_CAST ((obj), VisMorph))
#define VISUAL_MORPH_PLUGIN(obj)            (VISUAL_CHECK_CAST ((obj), VisMorphPlugin))

/**
 * Morph morphing methods.
 */
typedef enum {
    VISUAL_MORPH_MODE_SET,      /**< Morphing is done by a rate set,
                      * nothing is automated here. */
    VISUAL_MORPH_MODE_STEPS,    /**< Morphing is done by setting a number of steps,
                      * the morph will be automated. */
    VISUAL_MORPH_MODE_TIME      /**< Morphing is done by setting a target time when the morph should be done,
                      * This is as well automated. */
} VisMorphMode;

typedef struct _VisMorph VisMorph;
typedef struct _VisMorphPlugin VisMorphPlugin;

/* Morph plugin methods */

/**
 * A morph plugin needs this signature for the palette function. The palette function
 * is used to give a palette for the morph. The palette function isn't mandatory and the
 * VisMorph system will interpolate between the two palettes in VISUAL_VIDEO_DEPTH_8BIT when
 * a palette function isn't set.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 * @arg rate A float between 0.0 and 1.0 that tells how far the morph has proceeded.
 * @arg audio Pointer to the VisAudio containing all the data regarding the current audio sample.
 * @arg pal A pointer to the target VisPalette in which the morph between the two palettes is saved. Should have
 *  256 VisColor entries.
 * @arg src1 A pointer to the first VisVideo source.
 * @arg src2 A pointer to the second VisVideo source.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginMorphPaletteFunc)(VisPluginData *plugin, float rate, VisAudio *audio, VisPalette *pal,
        VisVideo *src1, VisVideo *src2);

/**
 * A morph plugin needs this signature for the apply function. The apply function
 * is used to execute a morph between two VisVideo sources. It's the 'render' function of
 * the morph plugin and here is the morphing done.
 *
 * @arg plugin Pointer to the VisPluginData instance structure.
 * @arg rate A float between 0.0 and 1.0 that tells how far the morph has proceeded.
 * @arg audio Pointer to the VisAudio containing all the data regarding the current audio sample.
 * @arg src1 A pointer to the first VisVideo source.
 * @arg src2 A pointer to the second VisVideo source.
 *
 * @return 0 on succes -1 on error.
 */
typedef int (*VisPluginMorphApplyFunc)(VisPluginData *plugin, float rate, VisAudio *audio, VisVideo *dest,
        VisVideo *src1, VisVideo *src2);

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
    VisObject    object;    /**< The VisObject data. */

    VisPluginData   *plugin;    /**< Pointer to the plugin itself. */
    VisVideo    *dest;      /**< Destination video, this is where
                     * the result of the morph gets drawn. */
    float        rate;      /**< The rate of morph, 0 draws the first video source
                     * 1 the second video source, 0.5 is a 50/50, final
                     * content depends on the plugin being used. */
    VisPalette  *morphpal;  /**< Morph plugins can also set a palette for indexed
                     * color depths. */
    VisTime     *morphtime; /**< Amount of time which the morphing should take. */
    VisTimer    *timer;     /**< Private entry that holds the time elapsed from 
                     * the beginning of the switch. */
    int      steps;     /**< Private entry that contains the number of steps
                     * a morph suppose to take. */
    int      stepsdone; /**< Private entry that contains the number of steps done. */

    VisMorphMode     mode;      /**< Private entry that holds the mode of morphing. */
};

/**
 * The VisMorphPlugin structure is the main data structure
 * for the morph plugin.
 *
 * The morph plugin is capable of morphing between two VisVideo
 * sources, and thus is capable of morphing between two
 * VisActors.
 */
struct _VisMorphPlugin {
    VisObject            object;    /**< The VisObject data. */
    VisPluginMorphPaletteFunc    palette;   /**< The plugin's palette function. This can be used
                              * to obtain a palette for VISUAL_VIDEO_DEPTH_8BIT surfaces.
                              * However the function may be set to NULL. In this case the
                              * VisMorph system morphs between palettes itself. */
    VisPluginMorphApplyFunc      apply;     /**< The plugin it's main function. This is used to morph
                              * between two VisVideo sources. */
    int              requests_audio;/**< When set on TRUE this will indicate that the Morph plugin
                              * requires an VisAudio context in order to render properly. */
    VisVideoAttributeOptions     vidoptions;
};


/**
 * Gives the encapsulated VisPluginData from a VisMorph.
 *
 * @param morph Pointer of a VisMorph of which the VisPluginData needs to be returned.
 *
 * @return VisPluginData that is encapsulated in the VisMorph, possibly NULL.
 */
VisPluginData *visual_morph_get_plugin (VisMorph *morph);

/**
 * Gives the next morph plugin based on the name of a plugin.
 *
 * @see visual_morph_get_prev_by_name
 *
 * @param name The name of the current plugin, or NULL to get the first.
 *
 * @return The name of the next plugin within the list.
 */
const char *visual_morph_get_next_by_name (const char *name);

/**
 * Gives the previous morph plugin based on the name of a plugin.
 *
 * @see visual_morph_get_next_by_name
 *
 * @param name The name of the current plugin. or NULL to get the last.
 *
 * @return The name of the previous plugin within the list.
 */
const char *visual_morph_get_prev_by_name (const char *name);

/**
 * Creates a new VisMorph from name, the plugin will be loaded but won't be realized.
 *
 * @param morphname
 *  The name of the plugin to load, or NULL to simply allocate a new
 *  morph.
 *
 * @return A newly allocated VisMorph, optionally containing a loaded plugin. Or NULL on failure.
 */
VisMorph *visual_morph_new (const char *morphname);

/**
 * Initializes a VisMorph, this will set the allocated flag for the object to FALSE. Should not
 * be used to reset a VisMorph, or on a VisMorph created by visual_morph_new().
 *
 * @see visual_morph_new
 *
 * @param morph Pointer to the VisMorph that is initialized.
 * @param morphname
 *  The name of the plugin to load, or NULL to simply initialize a new morph.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL or -VISUAL_ERROR_PLUGIN_NO_LIST on failure.
 */
int visual_morph_init (VisMorph *morph, const char *morphname);

/**
 * Realize the VisMorph. This also calls the plugin init function.
 *
 * @param morph Pointer to a VisMorph that needs to be realized.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL, -VISUAL_ERROR_PLUGIN_NULL or error values
 *  returned by visual_plugin_realize () on failure.
 */
int visual_morph_realize (VisMorph *morph);

/**
 * Gives the by the plugin natively supported depths
 *
 * @param morph Pointer to a VisMorph of which the supported depth of it's
 *    encapsulated plugin is requested.
 *
 * @return an OR value of the VISUAL_VIDEO_CONTEXT_* values which can be checked against using AND on success, -1 on failure
 */
int visual_morph_get_supported_depth (VisMorph *morph);

VisVideoAttributeOptions *visual_morph_get_video_attribute_options (VisMorph *morph);

/**
 * Used to connect the target display, or a buffer it's VisVideo to the VisMorph plugin.
 *
 * @see visual_video_new
 *
 * @param morph Pointer to a VisMorph to which the VisVideo needs to be set.
 * @param video Pointer to a VisVideo which contains information about the target display and the pointer
 *    to it's screenbuffer.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL or -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_morph_set_video (VisMorph *morph, VisVideo *video);

/**
 * Set the time when the morph should be finished morphing.
 * The VisMorph keeps a local copy of the given time.
 *
 * @param morph Pointer to the VisMorph to which finish time is set.
 * @param time Pointer to the VisTime that contains the finish time.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL, -VISUAL_ERROR_TIME_NULL or error values returned by
 *  visual_time_copy () on failure.
 */
int visual_morph_set_time (VisMorph *morph, VisTime *time);

/**
 * Used to set the rate of the VisMmorph. The rate ranges from 0 to 1
 * and the content of the result depends on the morph plugin being used.
 *
 * @param morph Pointer to a VisMorph to which the rate needs to be set.
 * @param rate Value that sets the rate of the current morph. The rate
 *    contains the amount that is currently being morphed and needs to be
 *    manually adjust. The morph system doesn't increase the rate itself.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_set_rate (VisMorph *morph, float rate);

/**
 * Used to set the number of steps that a morph will take to finish.
 *
 * @param morph Pointer to a VisMorph to which the number of morph steps is set.
 * @param steps The number of steps that a morph should take.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_set_steps (VisMorph *morph, int steps);

/**
 * Used to set the method of morphing.
 *
 * @param morph Pointer to a VisMorph to which the method of morphing is set.
 * @param mode Method of morphing that is of type VisMorphMode.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_set_mode (VisMorph *morph, VisMorphMode mode);

/**
 * Some morph plugins can give a custom palette while morphing two 8 bits plugins.
 *
 * @param morph Pointer to a VisMorph of which the palette needs to be retrieved.
 *
 * @return The pointer to the custom palette on succes or NULL on failure.
 */
VisPalette *visual_morph_get_palette (VisMorph *morph);

/**
 * Function that helps to check if a morph is done with it's morphing.
 *
 * @param morph Pointer to a VisMorph of which we want to know if it's done yet.
 *
 * @return TRUE or FALSE, -VISUAL_ERROR_MORPH_NULL on failure.
 */
int visual_morph_is_done (VisMorph *morph);

/**
 * Some morph plugins request an VisAudio context to draw properly. Using this function
 * you can check if the VisMorphPlugin being used in the VisMorph requests this.
 *
 * @param morph Pointer to a VisMorph of which we want to know if it wants a VisAudio.
 *
 * @return TRUE or FALSE, -VISUAL_ERROR_MORPH_NULL or -VISUAL_ERROR_MORPH_PLUGIN_NULL on failure.
 */
int visual_morph_requests_audio (VisMorph *morph);

/**
 * This is called to run the VisMorph. It will put the result in the buffer that is previously
 * set by visual_morph_set_video and also when the morph is being runned in 8 bits mode
 * it will automaticly interpolate between the two palettes if the plugin doesn't have
 * a method for adjusting the palette.
 *
 * Note that all the VisVideo structures being used need to be clones.
 *
 * @param morph Pointer to a VisMorph that needs to be runned.
 * @param audio Pointer to a VisAudio which a morph could use for extra effects
 * @param src1 Pointer to a VisVideo that acts as the first source for the morph.
 * @param src2 Pointer to a VisVideo that acts as the second source for the morph.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_MORPH_NULL, -VISUAL_ERROR_AUDIO_NULL,
 *  -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_morph_run (VisMorph *morph, VisAudio *audio, VisVideo *src1, VisVideo *src2);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_MORPH_H */
