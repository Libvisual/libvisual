#ifndef _LV_INPUT_H
#define _LV_INPUT_H

#include <libvisual/lv_audio.h>
#include <libvisual/lv_plugin.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _VisInput VisInput;

typedef int (*input_upload_callback_func_t)(VisInput *, VisAudio *, void *);

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
	LVPlugin			*plugin;	/**< Pointer to the plugin itself. */
	VisAudio			*audio;		/**< Pointer to the VisAudio structure
							  * that contains the audio analyse
							  * results.
							  * @see visual_audio_analyse */
	input_upload_callback_func_t	 callback;	/**< Callback function when a callback
							  * is used instead of a plugin. */
	void				*private;	/**< Private which can pass on data
							  * to the callback function. */
};

/* prototypes */
VisList *visual_input_get_list ();
char *visual_input_get_next_by_name (char *name);
char *visual_input_get_prev_by_name (char *name);
int visual_morph_valid_by_name (char *name);

VisInput *visual_input_new (char *inputname);

int visual_input_realize (VisInput *input);
int visual_input_destroy (VisInput *input);
int visual_input_free (VisInput *input);

int visual_input_set_callback (VisInput *input, input_upload_callback_func_t callback, void *private);

int visual_input_run (VisInput *input);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_INPUT_H */
