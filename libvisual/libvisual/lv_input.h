#ifndef _LV_INPUT_H
#define _LV_INPUT_H

#include <libvisual/lv_audio.h>
#include <libvisual/lv_plugin.h>
#include <libvisual/lv_common.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define VISUAL_INPUT(obj)				(VISUAL_CHECK_CAST ((obj), 0, VisInput))

typedef struct _VisInput VisInput;

/**
 * Callback function that is set using visual_input_set_callback should use this signature.
 *
 * @see visual_input_set_callback
 *
 * @arg input Pointer to the VisInput structure.
 *
 * @arg audio Pointer to the VisAudio containing all the audio information, and in which
 * data needs to be set within the callback.
 *
 * @arg priv Private field to be used by the client. The library will never touch this.
 */
typedef int (*VisInputUploadCallbackFunc)(VisInput *input, VisAudio *audio, void *priv);

/**
 * The VisInput structure encapsulates the input plugin and provides
 * abstract interfaces to the input. The VisInput system provides
 * PCM data to the visualisation elements of libvisual. This can be done
 * through both plugins and callback functions.
 *
 * Members in the structure shouldn't be accessed directly but instead
 * it's adviced to use the methods provided.
 *
 * @see visual_input_new
 */ 
struct _VisInput {
	VisObject			 object;	/**< The VisObject data. */

	VisPluginData			*plugin;	/**< Pointer to the plugin itself. */
	VisAudio			*audio;		/**< Pointer to the VisAudio structure
							  * that contains the audio analyse
							  * results.
							  * @see visual_audio_analyse */
	VisInputUploadCallbackFunc	 callback;	/**< Callback function when a callback
							  * is used instead of a plugin. */
};

/* prototypes */
VisPluginData *visual_input_get_plugin (VisInput *input);

const VisList *visual_input_get_list (void);
const char *visual_input_get_next_by_name (const char *name);
const char *visual_input_get_prev_by_name (const char *name);
int visual_input_valid_by_name (const char *name);

VisInput *visual_input_new (const char *inputname);

int visual_input_realize (VisInput *input);

int visual_input_set_callback (VisInput *input, VisInputUploadCallbackFunc callback, void *priv);

int visual_input_run (VisInput *input);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_INPUT_H */
