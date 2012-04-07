/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_log.h,v 1.19.2.1 2006/03/04 12:32:47 descender Exp $
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

VISUAL_BEGIN_DECLS

/* This is read-only */
extern char *__lv_progname;

/**
 * Used to determine the severity of the log message using the visual_log
 * define.
 *
 * @see visual_log
 */
typedef enum {
	VISUAL_LOG_DEBUG,	 /**< Debug message, to use for debug messages. */
	VISUAL_LOG_INFO,	 /**< Informative message, can be used for general info. */
	VISUAL_LOG_WARNING,	 /**< Warning message, use to warn the user. */
	VISUAL_LOG_ERROR,	 /**< Error message, use to notify the user of fatals. */
	VISUAL_LOG_CRITICAL  /**< Critical message, when a critical situation happens. */
} VisLogSeverity;

typedef struct {
	const char	 *file;
	const char	 *func;
	unsigned int  line;
} VisLogSource;

/**
 * Functions that want to handle messages must match this signature.
 *
 * @arg message The message that will be shown, exactly the same as that was passed
 * to visual_log(), but after formatting.
 *
 * @arg funcname The name of the function that invokes visual_log(). On non-GNU systems
 * this will probably be NULL.
 *
 * @arg priv Private field to be used by the client. The library will never touch this.
 */
typedef void (*VisLogHandlerFunc) (VisLogSeverity severity, const char *message,
	const VisLogSource *source, void *priv);

/**
 * Set the log verbosity level. Any message of a lower severity then
 * the given level will be dropped.
 *
 * @param level The verbosity level
 */
void visual_log_set_verbosity (VisLogSeverity level);

/**
 * Get the current library it's verbosity level.
 *
 * @return The verboseness level as a VisLogVerboseness enumerate value.
 */
VisLogSeverity visual_log_get_verbosity (void);

/**
 * Set the callback function that handles info messages.
 *
 * @param handler The custom message handler callback.
 * @param priv Optional private data to pass on to the handler.
 */
void visual_log_set_handler (VisLogSeverity severity, VisLogHandlerFunc handler, void *priv);

/**
 * Used for log messages, this is brought under a define so
 * that the __FILE__ and __LINE__ macros (and probably __PRETTY_FUNC__) work,
 * and thus provide better information.
 *
 * @see VisLogSeverity
 *
 * @param severity Determines the severity of the message using VisLogSeverity.
 * @param format The format string of the log message.
 */
#ifdef LV_HAVE_ISO_C_VARARGS

#define _LV_LOG_HAVE_SOURCE
#define visual_log(severity,...)		\
	_lv_log (severity,					\
		__FILE__,						\
		__LINE__,						\
		__PRETTY_FUNCTION__,			\
		__VA_ARGS__)

#elif defined(LV_HAVE_GNU_C_VARARGS)

#define _LV_LOG_HAVE_SOURCE
#define visual_log(severity,format...)	\
	_lv_log (severity,					\
		__FILE__,						\
		__LINE__,						\
		__PRETTY_FUNCTION__,			\
		format)
#else

#define visual_log _lv_log

#endif /* LV_HAVE_ISO_C_VARARGS */

#if defined(_LV_LOG_HAVE_SOURCE)
void _lv_log (VisLogSeverity severity, const char *file, int line, const char *funcname,
	const char *fmt, ...) VIS_CHECK_PRINTF_FORMAT(5, 6);
#else
void _lv_log (VisLogSeverity severity, const char *fmt, ...)
	VIS_CHECK_PRINTF_FORMAT(2, 3);
#endif

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_LOG_H */
