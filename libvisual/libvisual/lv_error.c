#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <signal.h>

#include "lv_log.h"
#include "lv_error.h"


static visual_error_handler_func_t error_handler = NULL;
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
 * @return 0 on succes -1 on error.
 */
int visual_error_set_handler (visual_error_handler_func_t handler, void *priv)
{
	visual_log_return_val_if_fail (handler != NULL, -VISUAL_ERROR_ERROR_HANDLER_NULL);

	error_handler = handler;
	error_handler_priv = priv;

	return VISUAL_OK;
}

/**
 * Translates an error into a human readable string.
 *
 * @param err Numeric error value.
 * 
 * @return Human readable string, or NULL on failure.
 */
const char *visual_error_to_string (int err)
{

}

/**
 * @}
 */

