#ifndef _LV_ERROR_H
#define _LV_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Enumerate of all possible numeric error values. 
 */
enum {
	VISUAL_OK = 0,			/**< No error. */
	VISUAL_ERROR_GENERIC = -1	/**< Generic error. */
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
