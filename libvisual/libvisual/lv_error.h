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
	VISUAL_ERROR_NULL,				/**< Something is NULL that shouldn't be. */
	VISUAL_ERROR_IMPOSSIBLE,			/**< The impossible happened, this should never happen. */

	/* Error entries for the VisActor system */
	VISUAL_ERROR_ACTOR_NULL,			/**< The VisActor is NULL. */
	VISUAL_ERROR_ACTOR_VIDEO_NULL,			/**< The VisVideo target member in the VisActor is NULL. */
	VISUAL_ERROR_ACTOR_PLUGIN_NULL,			/**< The VisActor plugin in this context is NULL. */
	VISUAL_ERROR_ACTOR_GL_NEGOTIATE,		/**< Tried depth forcing a GL VisACtor. */

	/* Error entries for the VisAudio system */
	VISUAL_ERROR_AUDIO_NULL,			/**< The VisAudio is NULL. */
	
	/* Error entries for the VisBMP system */
	VISUAL_ERROR_BMP_NO_BMP,			/**< Not a bitmap file. */
	VISUAL_ERROR_BMP_NOT_FOUND,			/**< File not found. */
	VISUAL_ERROR_BMP_NOT_SUPPORTED,			/**< File format not supported. */
	VISUAL_ERROR_BMP_CORRUPTED,			/**< Bitmap file is corrupted. */
	
	/* Error entries for the VisColor system */
	VISUAL_ERROR_COLOR_NULL,			/**< The VisColor is NULL. */
	
	/* Error entries for the VisError system */
	VISUAL_ERROR_ERROR_HANDLER_NULL,		/**< Error handler is NULL. */
	
	/* Error entries for the VisEvent system */
	VISUAL_ERROR_EVENT_NULL,			/**< The VisEvent is NULL. */
	VISUAL_ERROR_EVENT_QUEUE_NULL,			/**< The VisEventQueue is nULL. */
	
	/* Error entries for the VisInput system */
	VISUAL_ERROR_INPUT_NULL,			/**< The VisInput is NULL. */
	VISUAL_ERROR_INPUT_PLUGIN_NULL,			/**< The VisInputPlugin is NULL. */
	
	/* Error entries for the VisLibvisual system */
	VISUAL_ERROR_LIBVISUAL_NO_PATHS,		/**< Paths can be added to plugin dir list. */
	VISUAL_ERROR_LIBVISUAL_ALREADY_INITIALIZED,	/**< Libvisual is already initialized. */
	VISUAL_ERROR_LIBVISUAL_NOT_INITIALIZED,		/**< Libvisual is not initialized. */
	VISUAL_ERROR_LIBVISUAL_NO_REGISTRY,		/**< No internal plugin registry is set up. */
	
	/* Error entries for the VisList system */
	VISUAL_ERROR_LIST_NULL,				/**< The VisList is NULL. */
	VISUAL_ERROR_LIST_ENTRY_NULL,			/**< The VisListEntry is NULL. */
	VISUAL_ERROR_LIST_ENTRY_INVALID,		/**< The VisListEntry is invalid. */
	
	/* Error entries for the VisMem system */
	VISUAL_ERROR_MEM_NULL,				/**< The memory pointer given is NULL. */
	
	/* Error entries for the VisMorph system */
	VISUAL_ERROR_MORPH_NULL,			/**< The VisMorph is NULL. */
	VISUAL_ERROR_MORPH_PLUGIN_NULL,			/**< The VisMorphPlugin is NULL. */
	
	/* Error entries for the VisPalette system */
	VISUAL_ERROR_PALETTE_NULL,			/**< The VisPalette is NULL. */
	VISUAL_ERROR_PALETTE_SIZE,			/**< Given VisPalette entries are not of the same size. */
	
	/* Error entries for the VisParam system */
	VISUAL_ERROR_PARAM_NULL,			/**< The VisParamEntry is NULL. */
	VISUAL_ERROR_PARAM_CONTAINER_NULL,		/**< The VisParamContainer is NULL. */
	VISUAL_ERROR_PARAM_NOT_FOUND,			/**< The requested VisParamEntry is not found. */
	VISUAL_ERROR_PARAM_CALLBACK_NULL,		/**< The given param change callback is NULL. */
	VISUAL_ERROR_PARAM_INVALID_TYPE,		/**< The VisParamEntry is of invalid type. */
	
	/* Error entries for the VisPlugin system */
	VISUAL_ERROR_PLUGIN_NULL,			/**< The VisPluginData is NULL. */
	VISUAL_ERROR_PLUGIN_INFO_NULL,			/**< The VisPluginInfo is NULL. */
	VISUAL_ERROR_PLUGIN_REF_NULL,			/**< The VisPluginRef is NULL. */
	VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER,		/**< The plugin has no event handler registrated. */
	VISUAL_ERROR_PLUGIN_HANDLE_NULL,		/**< The dlopen handle of the plugin is NULL. */
	VISUAL_ERROR_PLUGIN_ALREADY_REALIZED,		/**< The plugin is already realized. */
	
	/* Error entries for the VisRandom system */
	VISUAL_ERROR_RANDOM_CONTEXT_NULL,		/**< The VisRandomContext is NULL. */
	
	/* Error entries for the VisSonginfo system */
	VISUAL_ERROR_SONGINFO_NULL,			/**< The VisSongInfo is NULL. */
	
	/* Error entries for the VisTime system */
	VISUAL_ERROR_TIME_NULL,				/**< The VisTime is NULL. */
	VISUAL_ERROR_TIMER_NULL,			/**< The VisTimer is NULL. */
	
	/* Error entries for the VisUI system */
	VISUAL_ERROR_UI_WIDGET_NULL,			/**< The VisUIWidget is NULL. */
	VISUAL_ERROR_UI_CONTAINER_NULL,			/**< The VisUIContainer is NULL. */
	VISUAL_ERROR_UI_BOX_NULL,			/**< The VisUIBox is NULL. */
	VISUAL_ERROR_UI_TABLE_NULL,			/**< The VisUITable is NULL. */
	VISUAL_ERROR_UI_FRAME_NULL,			/**< The VisUIFrame is NULL. */
	VISUAL_ERROR_UI_LABEL_NULL,			/**< The VisUILabel is NULL. */
	VISUAL_ERROR_UI_IMAGE_NULL,			/**< The VisUIImage is NULL. */
	VISUAL_ERROR_UI_SEPARATOR_NULL,			/**< The VisUISperator is NULL. */
	VISUAL_ERROR_UI_MUTATOR_NULL,			/**< The VisUIMutator is NULL. */
	VISUAL_ERROR_UI_RANGE_NULL,			/**< The VisUIRange is NULL. */
	VISUAL_ERROR_UI_ENTRY_NULL,			/**< The VisUIEntry is NULL. */
	VISUAL_ERROR_UI_SLIDER_NULL,			/**< The VisUISlider is NULL. */
	VISUAL_ERROR_UI_NUMERIC_NULL,			/**< The VisUINumeric is NULL. */
	VISUAL_ERROR_UI_COLOR_NULL,			/**< The VisUIColor is NULL. */
	VISUAL_ERROR_UI_CHOICE_NULL,			/**< The VisUIChoice is NULL. */
	VISUAL_ERROR_UI_POPUP_NULL,			/**< The VisUIPopup is NULL. */
	VISUAL_ERROR_UI_LIST_NULL,			/**< The VisUIList is NULL. */
	VISUAL_ERROR_UI_RADIO_NULL,			/**< The VisUIRadio is NULL. */
	VISUAL_ERROR_UI_CHECKBOX_NULL,			/**< The VisUICheckbox is NULL. */
	VISUAL_ERROR_UI_CHOICE_ENTRY_NULL,		/**< The VisUIChoiceEntry is NULL. */
	VISUAL_ERROR_UI_CHOICE_NONE_ACTIVE,		/**< there is no VisUIChoiceEntry active. */
	VISUAL_ERROR_UI_INVALID_TYPE,			/**< The VisUIWidget is of invalid type. */
	VISUAL_ERROR_UI_NO_BOX,				/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_BOX. */
	VISUAL_ERROR_UI_NO_TABLE,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_TABLE. */
	VISUAL_ERROR_UI_NO_FRAME,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_FRAME. */
	VISUAL_ERROR_UI_NO_LABEL,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_LABEL. */
	VISUAL_ERROR_UI_NO_IMAGE,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_IMAGE. */
	VISUAL_ERROR_UI_NO_SEPARATOR,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_SEPARATOR. */
	VISUAL_ERROR_UI_NO_ENTRY,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_ENTRY. */
	VISUAL_ERROR_UI_NO_SLIDER,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_SLIDER. */
	VISUAL_ERROR_UI_NO_NUMERIC,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_NUMERIC. */
	VISUAL_ERROR_UI_NO_COLOR,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_COLOR. */
	VISUAL_ERROR_UI_NO_POPUP,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_POPUP. */
	VISUAL_ERROR_UI_NO_LIST,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_LIST. */
	VISUAL_ERROR_UI_NO_RADIO,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_RADIO. */
	VISUAL_ERROR_UI_NO_CHECKBOX,			/**< The VisUIWidget is not of type VISUAL_WIDGET_TYPE_CHECKBOX. */

	/* Error entries for the VisVideo system */
	VISUAL_ERROR_VIDEO_NULL,			/**< The VisVideo is NULL. */
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
