#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include "lv_log.h"
#include "lv_error.h"

static const char *__lv_error_human_readable[] = {
	"", /* VISUAL_OK */

	"", /* VISUAL_ERROR_GENERAL */
	"", /* VISUAL_ERROR_NULL */
	"", /* VISUAL_ERROR_IMPOSSIBLE */

	"", /* VISUAL_ERROR_ACTOR_NULL */
	"", /* VISUAL_ERROR_ACTOR_VIDEO_NULL */
	"", /* VISUAL_ERROR_ACTOR_PLUGIN_NULL */
	"", /* VISUAL_ERROR_ACTOR_GL_NEGOTIATE */

	"", /* VISUAL_ERROR_AUDIO_NULL */
	
	"", /* VISUAL_ERROR_BMP_NO_BMP */
	"", /* VISUAL_ERROR_BMP_NOT_FOUND */
	"", /* VISUAL_ERROR_BMP_NOT_SUPPORTED */
	"", /* VISUAL_ERROR_BMP_CORRUPTED */
	
	"", /* VISUAL_ERROR_COLOR_NULL */
	
	"", /* VISUAL_ERROR_ERROR_HANDLER_NULL */
	
	"", /* VISUAL_ERROR_EVENT_NULL */
	"", /* VISUAL_ERROR_EVENT_QUEUE_NULL */
	
	"", /* VISUAL_ERROR_INPUT_NULL */
	"", /* VISUAL_ERROR_INPUT_PLUGIN_NULL */
	
	"", /* VISUAL_ERROR_LIBVISUAL_NO_PATHS */
	"", /* VISUAL_ERROR_LIBVISUAL_ALREADY_INITIALIZED */
	"", /* VISUAL_ERROR_LIBVISUAL_NOT_INITIALIZED */
	"", /* VISUAL_ERROR_LIBVISUAL_NO_REGISTRY */
	
	"", /* VISUAL_ERROR_LIST_NULL */
	"", /* VISUAL_ERROR_LIST_ENTRY_NULL */
	"", /* VISUAL_ERROR_LIST_ENTRY_INVALID */
	
	"", /* VISUAL_ERROR_MEM_NULL */
	
	"", /* VISUAL_ERROR_MORPH_NULL */
	"", /* VISUAL_ERROR_MORPH_PLUGIN_NULL */
	
	"", /* VISUAL_ERROR_PALETTE_NULL */
	"", /* VISUAL_ERROR_PALETTE_SIZE */
	
	"", /* VISUAL_ERROR_PARAM_NULL */
	"", /* VISUAL_ERROR_PARAM_CONTAINER_NULL */
	"", /* VISUAL_ERROR_PARAM_NOT_FOUND */
	"", /* VISUAL_ERROR_PARAM_CALLBACK_NULL */
	"", /* VISUAL_ERROR_PARAM_CALLBACK_TOO_MANY */
	"", /* VISUAL_ERROR_PARAM_INVALID_TYPE */
	
	"", /* VISUAL_ERROR_PLUGIN_NULL */
	"", /* VISUAL_ERROR_PLUGIN_INFO_NULL */
	"", /* VISUAL_ERROR_PLUGIN_REF_NULL */
	"", /* VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER */
	"", /* VISUAL_ERROR_PLUGIN_HANDLE_NULL */
	"", /* VISUAL_ERROR_PLUGIN_ALREADY_REALIZED */
	
	"", /* VISUAL_ERROR_RANDOM_CONTEXT_NULL */
	
	"", /* VISUAL_ERROR_SONGINFO_NULL */

	"", /* VISUAL_ERROR_THREAD_NULL */
	"", /* VISUAL_ERROR_THREAD_NO_THREADING */
	"", /* VISUAL_ERROR_MUTEX_NULL */
	"", /* VISUAL_ERROR_MUTEX_LOCK_FAILURE */
	"", /* VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE */
	"", /* VISUAL_ERROR_MUTEX_UNLOCK_FAILURE */
	
	"", /* VISUAL_ERROR_OBJECT_DTOR_FAILED */
	"", /* VISUAL_ERROR_OBJECT_NULL */
	"", /* VISUAL_ERROR_OBJECT_NOT_ALLOCATED */
	
	"", /* VISUAL_ERROR_TIME_NULL */
	"", /* VISUAL_ERROR_TIME_NO_USLEEP */
	"", /* VISUAL_ERROR_TIMER_NULL */
	
	"", /* VISUAL_ERROR_UI_WIDGET_NULL */
	"", /* VISUAL_ERROR_UI_CONTAINER_NULL */
	"", /* VISUAL_ERROR_UI_BOX_NULL */
	"", /* VISUAL_ERROR_UI_TABLE_NULL */
	"", /* VISUAL_ERROR_UI_FRAME_NULL */
	"", /* VISUAL_ERROR_UI_LABEL_NULL */
	"", /* VISUAL_ERROR_UI_IMAGE_NULL */
	"", /* VISUAL_ERROR_UI_SEPARATOR_NULL */
	"", /* VISUAL_ERROR_UI_MUTATOR_NULL */
	"", /* VISUAL_ERROR_UI_RANGE_NULL */
	"", /* VISUAL_ERROR_UI_ENTRY_NULL */
	"", /* VISUAL_ERROR_UI_SLIDER_NULL */
	"", /* VISUAL_ERROR_UI_NUMERIC_NULL */
	"", /* VISUAL_ERROR_UI_COLOR_NULL */
	"", /* VISUAL_ERROR_UI_CHOICE_NULL */
	"", /* VISUAL_ERROR_UI_POPUP_NULL */
	"", /* VISUAL_ERROR_UI_LIST_NULL */
	"", /* VISUAL_ERROR_UI_RADIO_NULL */
	"", /* VISUAL_ERROR_UI_CHECKBOX_NULL */
	"", /* VISUAL_ERROR_UI_CHOICE_ENTRY_NULL */
	"", /* VISUAL_ERROR_UI_CHOICE_NONE_ACTIVE */
	"", /* VISUAL_ERROR_UI_INVALID_TYPE */

	"", /* VISUAL_ERROR_VIDEO_NULL */
	"", /* VISUAL_ERROR_VIDEO_HAS_ALLOCATED */
	"", /* VISUAL_ERROR_VIDEO_PIXELS_NULL */
	"", /* VISUAL_ERROR_VIDEO_NO_ALLOCATED */
	"", /* VISUAL_ERROR_VIDEO_HAS_PIXELS */
	"", /* VISUAL_ERROR_VIDEO_INVALID_BPP */
	"", /* VISUAL_ERROR_VIDEO_INVALID_DEPTH */
	"", /* VISUAL_ERROR_VIDEO_INVALID_SCALE_METHOD */
	"", /* VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS */
	"", /* VISUAL_ERROR_VIDEO_NOT_INDENTICAL */
	"" /* VISUAL_ERROR_VIDEO_NOT_TRANSFORMED */
};

static VisErrorHandlerFunc error_handler = NULL;
static void *error_handler_priv = NULL;

/**
 * @defgroup VisError VisError
 * @{
 */

/**
 * Raise a libvisual error. With the standard error handler this will
 * do a raise(SIGTRAP). You can set your own error handler function using the
 * visual_error_set_handler.
 *
 * @see visual_error_set_handler
 *
 * @return Returns the return value from the handler that is set.
 */
int visual_error_raise ()
{
	if (error_handler == NULL) {
		raise (SIGTRAP);
		exit (1);
	}
	
	return error_handler (error_handler_priv);
}

/**
 * Sets the error handler callback. By using this function you
 * can override libvisual it's default error handler.
 *
 * @param handler The error handler which you want to use
 *      to handle libvisual errors.
 * @param priv Optional private data which could be needed in the
 *      error handler that has been set.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_ERROR_HANDLER_NULL on failure.
 */
int visual_error_set_handler (VisErrorHandlerFunc handler, void *priv)
{
	visual_log_return_val_if_fail (handler != NULL, -VISUAL_ERROR_ERROR_HANDLER_NULL);

	error_handler = handler;
	error_handler_priv = priv;

	return VISUAL_OK;
}

/**
 * Translates an error into a human readable string, the returned string should not be freed.
 *
 * @param err Numeric error value.
 * 
 * @return Human readable string, or NULL on failure.
 */
const char *visual_error_to_string (int err)
{
	if (abs (err) > VISUAL_ERROR_LIST_END)
		return "The error value given to visual_error_to_string() is invalid";

	return __lv_error_human_readable[abs (err)];
}

/**
 * @}
 */

