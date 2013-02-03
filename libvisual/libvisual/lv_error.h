/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
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

#ifndef _LV_ERROR_H
#define _LV_ERROR_H

#include <libvisual/lv_defines.h>

/**
 * @defgroup VisError VisError
 * @{
 */

/* WARNING when you add an new error to this list, make sure that you update lv_error.c it's
 * human readable string list as well!!! */
/**
 * Enumerate of all possible numeric error values.
 */
enum {
	/* Ok! */
	VISUAL_OK,                          /**< No error. */

	/* Standard error entries */
	VISUAL_ERROR_GENERAL,               /**< General error. */
	VISUAL_ERROR_NULL,                  /**< Something is NULL that shouldn't be. */
	VISUAL_ERROR_IMPOSSIBLE,            /**< The impossible happened, this should never happen. */
    VISUAL_ERROR_FAILED_CHECK,          /**< Failed an assertion check */

	/* Error entries for the VisActor system */
	VISUAL_ERROR_ACTOR_NULL,            /**< The VisActor is NULL. */
	VISUAL_ERROR_ACTOR_VIDEO_NULL,      /**< The VisVideo target member in the VisActor is NULL. */
	VISUAL_ERROR_ACTOR_PLUGIN_NULL,     /**< The VisActor plugin in this context is NULL. */
	VISUAL_ERROR_ACTOR_GL_NEGOTIATE,    /**< Tried depth forcing a GL VisACtor. */

	/* Error entries for the VisCollection system */
	VISUAL_ERROR_COLLECTION_NULL,			/**< The VisCollection is NULL. */
	VISUAL_ERROR_COLLECTION_ITER_NULL,		/**< The VisCollectionIter is NULL. */

	/* Error entries for the VisError system */
	VISUAL_ERROR_ERROR_HANDLER_NULL,		/**< Error handler is NULL. */

	/* Error entries for the VisInput system */
	VISUAL_ERROR_INPUT_NULL,			/**< The VisInput is NULL. */
	VISUAL_ERROR_INPUT_PLUGIN_NULL,			/**< The VisInputPlugin is NULL. */

	/* Error entries for the VisList system */
	VISUAL_ERROR_LIST_NULL,				/**< The VisList is NULL. */
	VISUAL_ERROR_LIST_ENTRY_NULL,			/**< The VisListEntry is NULL. */
	VISUAL_ERROR_LIST_ENTRY_INVALID,		/**< The VisListEntry is invalid. */

	/* Error entries for the VisMorph system */
	VISUAL_ERROR_MORPH_NULL,			/**< The VisMorph is NULL. */
	VISUAL_ERROR_MORPH_PLUGIN_NULL,			/**< The VisMorphPlugin is NULL. */

	/* Error entries for the VisOS system */
	VISUAL_ERROR_OS_SCHED,				/**< The scheduler related call wasn't succesful. */
	VISUAL_ERROR_OS_SCHED_NOT_SUPPORTED,		/**< Scheduler operations are not supported on the platform. */

	/* Error entries for the VisPlugin system */
	VISUAL_ERROR_PLUGIN_NULL,			/**< The VisPluginData is NULL. */
	VISUAL_ERROR_PLUGIN_INFO_NULL,			/**< The VisPluginInfo is NULL. */
	VISUAL_ERROR_PLUGIN_REF_NULL,			/**< The VisPluginRef is NULL. */
	VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER,		/**< The plugin has no event handler registrated. */
	VISUAL_ERROR_PLUGIN_HANDLE_NULL,		/**< The dlopen handle of the plugin is NULL. */
	VISUAL_ERROR_PLUGIN_ALREADY_REALIZED,		/**< The plugin is already realized. */
	VISUAL_ERROR_PLUGIN_NO_LIST,			/**< The plugin list can't be found. */
	VISUAL_ERROR_PLUGIN_NOT_FOUND,          /**< The plugin can't be found */

	/* Error entries for the VisVideo system */
	VISUAL_ERROR_VIDEO_INVALID_DEPTH,       /**< Depth is invalid */

	/* Error entries for the VisObject system */
	VISUAL_ERROR_OBJECT_DTOR_FAILED,		/**< The destructor assigned to a VisObject failed destroying the VisObject. */
	VISUAL_ERROR_OBJECT_NULL,			/**< The VisObject is NULL. */
	VISUAL_ERROR_OBJECT_NOT_ALLOCATED,		/**< The VisObject is not allocated. */

	VISUAL_ERROR_LIST_END				/**< Last entry, to check against for the number of errors. */
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
typedef int (*VisErrorHandlerFunc) (int error, void *priv);

LV_BEGIN_DECLS

/**
 * Raise a libvisual error. With the standard error handler this will
 * do a raise(SIGTRAP). You can set your own error handler function using the
 * visual_error_set_handler.
 *
 * @see visual_error_set_handler
 *
 * @return Returns the return value from the handler that is set.
 */
LV_API int visual_error_raise (int error);

/**
 * Sets the error handler callback. By using this function you
 * can override libvisual it's default error handler.
 *
 * @param handler The error handler which you want to use
 *      to handle libvisual errors.
 * @param priv Optional private data which could be needed in the
 *      error handler that has been set.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_ERROR_HANDLER_NULL on failure.
 */
LV_API void visual_error_set_handler (VisErrorHandlerFunc handler, void *priv);

/**
 * Translates an error into a human readable string, the returned string should not be freed.
 *
 * @param err Numeric error value.
 *
 * @return Human readable string, or NULL on failure.
 */
LV_API const char *visual_error_to_string (int err);

LV_END_DECLS

#if __cplusplus

#include <string>
#include <stdexcept>

namespace LV {

  class LV_API Error
    : public std::runtime_error
  {
  public:

    explicit Error (int code)
        : std::runtime_error (visual_error_to_string (code))
    {}

    Error (int code, std::string const& reason)
        : std::runtime_error (std::string (visual_error_to_string (code)) + ": " + reason)
    {}

    explicit Error (std::string const& msg)
        : std::runtime_error (msg)
    {}
  };

} // LV namespace

#endif // __cplusplus


/**
 * @}
 */

#endif /* _LV_ERROR_H */
