/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Chong Kai Xiong <kaixiong@codeleft.sg>
 *          Dennis Smit <ds@nerds-incorporated.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_LOG_H
#define _LV_LOG_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>

/**
 * @defgroup VisLog VisLog
 * @{
 */

LV_BEGIN_DECLS

/**
 * Message severity levels.
 *
 * @see visual_log()
 */
typedef enum {
	VISUAL_LOG_DEBUG,     /**< Debugging information */
	VISUAL_LOG_INFO,      /**< Status and general information */
	VISUAL_LOG_WARNING,   /**< Warnings */
	VISUAL_LOG_ERROR,     /**< Errors */
	VISUAL_LOG_CRITICAL,  /**< Critical errors */

	VISUAL_LOG_NUM_LEVELS /**< Number of log severity levels in total */
} VisLogSeverity;

/**
 * Contains information on the source of a log message.
 *
 * @see VisLogHandlerFunc
 */
typedef struct {
	const char   *file;  /**< Name of source file */
	const char   *func;  /**< Name of function */
	unsigned int  line;  /**< Line number in source file */
} VisLogSource;

/**
 * Function signature for log message handlers.
 *
 * @see visual_log()
 *
 * @param severity  Message severity level
 * @param message   Message to log
 * @param source    Information on the message origin
 * @param user_data User data passed to message handler
 */
typedef void (*VisLogHandlerFunc) (VisLogSeverity severity, const char *message,
	const VisLogSource *source, void *user_data);

/**
 * Sets the log verbosity level. Any message of a lower severity level will be dropped.
 *
 * @param level Minimum severity level
 */
LV_API void visual_log_set_verbosity (VisLogSeverity level);

/**
 * Returns the current log verbosity level.
 *
 * @return level Current severity level threshold
 */
LV_API VisLogSeverity visual_log_get_verbosity (void);

/**
 * Registers a log message handler for a severity level.
 *
 * @param severity  Message severity level
 * @param handler   Message handler callback
 * @param user_data User data passed to the handler during invocation
 */
LV_API void visual_log_set_handler (VisLogSeverity severity, VisLogHandlerFunc handler, void *user_data);

/**
 * Logs a message.
 *
 * @param severity  Message severity level
 * @param format    printf format string of the log message
 * @param ...       Format string arguments
 */
#define visual_log(severity,...)  \
	_lv_log (severity,            \
	         __FILE__,            \
	         __LINE__,            \
	         __PRETTY_FUNCTION__, \
	         __VA_ARGS__)

LV_API void _lv_log (VisLogSeverity severity, const char *file, int line, const char *funcname,
	const char *fmt, ...) LV_CHECK_PRINTF_FORMAT(5, 6);

LV_END_DECLS

/**
 * @}
 */

#endif /* _LV_LOG_H */
