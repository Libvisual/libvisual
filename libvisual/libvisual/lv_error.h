#ifndef _LV_ERROR_H
#define _LV_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Enumerate of all possible numeric error values. 
 */
enum {
	/* Ok! */
	VISUAL_OK,					/**< No error. */

	/* Standard error entries */
	VISUAL_ERROR_GENERIC,				/**< Generic error. */
	VISUAL_ERROR_NULL,				/**< One of the parameters is NULL. */
	VISUAL_ERROR_IMPOSSIBLE,

	/* Error entries for the VisActor system */
	VISUAL_ERROR_ACTOR_NULL,			/**< The actor is NULL. */
	VISUAL_ERROR_ACTOR_VIDEO_NULL,			/**< The video target member in the actor is NULL. */
	VISUAL_ERROR_ACTOR_PLUGIN_NULL,			/**< The actor plugin in this context is NULL. */
	VISUAL_ERROR_ACTOR_GL_NEGOTIATE,		/**< Tried depth forcing a GL actor. */

	/* Error entries for the VisAudio system */
	VISUAL_ERROR_AUDIO_NULL,
	
	/* Error entries for the VisBMP system */
	VISUAL_ERROR_BMP_NO_BMP,
	VISUAL_ERROR_BMP_NOT_FOUND,
	VISUAL_ERROR_BMP_NOT_SUPPORTED,
	VISUAL_ERROR_BMP_CORRUPTED,
	
	/* Error entries for the VisColor system */
	VISUAL_ERROR_COLOR_NULL,
	
	/* Error entries for the VisError system */
	VISUAL_ERROR_ERROR_HANDLER_NULL,
	
	/* Error entries for the VisEvent system */
	VISUAL_ERROR_EVENT_NULL,
	VISUAL_ERROR_EVENT_QUEUE_NULL,
	
	/* Error entries for the VisInput system */
	VISUAL_ERROR_INPUT_NULL,
	VISUAL_ERROR_INPUT_PLUGIN_NULL,
	
	/* Error entries for the VisLibvisual system */
	VISUAL_ERROR_LIBVISUAL_NO_PATHS,
	VISUAL_ERROR_LIBVISUAL_ALREADY_INITIALIZED,
	VISUAL_ERROR_LIBVISUAL_NOT_INITIALIZED,
	VISUAL_ERROR_LIBVISUAL_NO_REGISTRY,
	
	/* Error entries for the VisList system */
	VISUAL_ERROR_LIST_NULL,
	VISUAL_ERROR_LIST_ENTRY_NULL,
	VISUAL_ERROR_LIST_ENTRY_INVALID,
	
	/* Error entries for the VisMem system */
	VISUAL_ERROR_MEM_NULL,
	
	/* Error entries for the VisMorph system */
	VISUAL_ERROR_MORPH_NULL,
	VISUAL_ERROR_MORPH_PLUGIN_NULL,
	
	/* Error entries for the VisPalette system */
	VISUAL_ERROR_PALETTE_NULL,
	VISUAL_ERROR_PALETTE_SIZE,
	
	/* Error entries for the VisParam system */
	VISUAL_ERROR_PARAM_NULL,
	VISUAL_ERROR_PARAM_CONTAINER_NULL,
	VISUAL_ERROR_PARAM_NOT_FOUND,
	VISUAL_ERROR_PARAM_CALLBACK_NULL,
	VISUAL_ERROR_PARAM_INVALID_TYPE,
	
	/* Error entries for the VisPlugin system */
	VISUAL_ERROR_PLUGIN_NULL,			/**< The plugin is NULL. */
	VISUAL_ERROR_PLUGIN_INFO_NULL,
	VISUAL_ERROR_PLUGIN_REF_NULL,			/**< The reference in the plugin is NULL. */
	VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER,
	VISUAL_ERROR_PLUGIN_HANDLE_NULL,
	VISUAL_ERROR_PLUGIN_ALREADY_REALIZED,
	
	/* Error entries for the VisRandom system */
	VISUAL_ERROR_RANDOM_CONTEXT_NULL,
	
	/* Error entries for the VisSonginfo system */
	VISUAL_ERROR_SONGINFO_NULL,
	
	/* Error entries for the VisTime system */
	VISUAL_ERROR_TIME_NULL,
	VISUAL_ERROR_TIMER_NULL,
	
	/* Error entries for the VisUI system */
	VISUAL_ERROR_UI_WIDGET_NULL,
	VISUAL_ERROR_UI_CONTAINER_NULL,
	VISUAL_ERROR_UI_BOX_NULL,
	VISUAL_ERROR_UI_TABLE_NULL,
	VISUAL_ERROR_UI_FRAME_NULL,
	VISUAL_ERROR_UI_LABEL_NULL,
	VISUAL_ERROR_UI_IMAGE_NULL,
	VISUAL_ERROR_UI_SEPARATOR_NULL,
	VISUAL_ERROR_UI_MUTATOR_NULL,
	VISUAL_ERROR_UI_RANGE_NULL,
	VISUAL_ERROR_UI_ENTRY_NULL,
	VISUAL_ERROR_UI_SLIDER_NULL,
	VISUAL_ERROR_UI_NUMERIC_NULL,
	VISUAL_ERROR_UI_COLOR_NULL,
	VISUAL_ERROR_UI_CHOICE_NULL,
	VISUAL_ERROR_UI_POPUP_NULL,
	VISUAL_ERROR_UI_LIST_NULL,
	VISUAL_ERROR_UI_RADIO_NULL,
	VISUAL_ERROR_UI_CHECKBOX_NULL,
	VISUAL_ERROR_UI_CHOICE_ENTRY_NULL,
	VISUAL_ERROR_UI_CHOICE_NONE_ACTIVE,
	VISUAL_ERROR_UI_NO_BOX,
	VISUAL_ERROR_UI_NO_TABLE,
	VISUAL_ERROR_UI_NO_FRAME,
	VISUAL_ERROR_UI_NO_LABEL,
	VISUAL_ERROR_UI_NO_IMAGE,
	VISUAL_ERROR_UI_NO_SEPARATOR,
	VISUAL_ERROR_UI_NO_ENTRY,
	VISUAL_ERROR_UI_NO_SLIDER,
	VISUAL_ERROR_UI_NO_NUMERIC,
	VISUAL_ERROR_UI_NO_COLOR,
	VISUAL_ERROR_UI_NO_POPUP,
	VISUAL_ERROR_UI_NO_LIST,
	VISUAL_ERROR_UI_NO_RADIO,
	VISUAL_ERROR_UI_NO_CHECKBOX,

	/* Error entries for the VisVideo system */
	VISUAL_ERROR_VIDEO_NULL,
};

/**
 * Functions that want to handle libvisual errors must match this signature. The standard
 * libvisual error handler aborts the program after an error by raise(SIGTRAP). If it's
 * desired to override this use visual_set_error_handler to set your own error handler.
 *
 * @see visual_set_error_handler
 *
 * @arg priv Private field to be used by the client. The library will never touch this.
 */
typedef int (*visual_error_handler_func_t) (void *priv);

int visual_error_raise (void);
int visual_error_set_handler (visual_error_handler_func_t handler, void *priv);

const char *visual_error_to_string (int err);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_ERROR_H */
