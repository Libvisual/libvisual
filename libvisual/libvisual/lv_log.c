/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_log.c,v 1.23 2006/01/22 13:23:37 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <gettext.h>

#include "lv_common.h"
#include "lv_error.h"
#include "lv_log.h"

#define LV_LOG_MAX_MESSAGE_SIZE 1024

typedef struct {
	VisLogMessageHandlerFunc  func;
	void					 *priv;
} MessageHandler;

static VisLogSeverity verboseness = VISUAL_LOG_WARNING;

static const char *severity_labels[] = {
	"DEBUG    ",
	"INFO     ",
	"WARNING  ",
	"CRITICAL ",
	"ERROR    "
};

static MessageHandler message_handlers[VISUAL_TABLESIZE(severity_labels)];

static void default_log_handler (VisLogSeverity severity,
	VisLogMessageSource const *source, const char *msg);

/**
 * @defgroup VisLog VisLog
 * @{
 */

/**
 * Set the library it's verbosity level.
 *
 * @param v The verbose level as a VisLogVerboseness enumerate value.
 */
void visual_log_set_verboseness (VisLogSeverity v)
{
	verboseness = v;
}

/**
 * Get the current library it's verbosity level.
 *
 * @return The verboseness level as a VisLogVerboseness enumerate value.
 */
VisLogSeverity visual_log_get_verboseness ()
{
	return verboseness;
}

static int is_valid_severity (VisLogSeverity severity)
{
	return (severity >= VISUAL_LOG_DEBUG && severity <= VISUAL_LOG_ERROR);
}

/**
 * Set the callback function that handles info messages.
 *
 * @param handler The custom message handler callback.
 * @param priv Optional private data to pass on to the handler.
 */
void visual_log_set_message_handler (VisLogSeverity severity, VisLogMessageHandlerFunc func, void *priv)
{
	MessageHandler *handler;

	visual_log_return_if_fail (is_valid_severity (severity));

	handler = &message_handlers[severity];
	handler->func = func;
	handler->priv = priv;
}

/**
 * Private library call used by the visual_log define to display debug,
 * warning and error messages.
 *
 * @see visual_log
 *
 * @param severity Severity of the log message.
 * @param file Char pointer to a string that contains the source filename.
 * @param line Line number for which the log message is.
 * @param funcname Function name in which the log message is called.
 * @param fmt Format string to display the log message.
 */
void _lv_log (VisLogSeverity severity, const char *file,
	int line, const char *funcname, const char *fmt, ...)
{
	VisLogMessageSource source;
	MessageHandler *handler;
	char str[LV_LOG_MAX_MESSAGE_SIZE];
	va_list va;

	visual_log_return_if_fail (is_valid_severity (severity));
	visual_log_return_if_fail (fmt != NULL);

	if (verboseness > severity)
		 return;

	va_start (va, fmt);
	vsnprintf (str, LV_LOG_MAX_MESSAGE_SIZE-1, fmt, va);
	va_end (va);

	source.file = file;
	source.func = funcname;
	source.line = line;

	handler = &message_handlers[severity];

	if (handler->func != NULL) {
		(*handler->func) (severity, str, &source, handler->priv);
	} else {
		default_log_handler (severity, &source, str);
	}
}

void _lv_log_bare (VisLogSeverity severity, const char *fmt, va_list va)
{
	VisLogMessageSource source;
	MessageHandler *handler;
	char str[LV_LOG_MAX_MESSAGE_SIZE];

	visual_log_return_if_fail (is_valid_severity (severity));
	visual_log_return_if_fail (fmt != NULL);

	if (verboseness > severity)
		 return;

	vsnprintf (str, LV_LOG_MAX_MESSAGE_SIZE-1, fmt, va);

	handler = &message_handlers[severity];

	if (handler->func != NULL) {
		(*handler->func) (severity, str, &source, handler->priv);
	} else {
		default_log_handler (severity, &source, str);
	}
}

static void default_log_handler (VisLogSeverity severity, VisLogMessageSource const *source, const char *msg)
{
	if (source != NULL) {
		fprintf (stderr, "%s %s:%d:%s: %s\n", severity_labels[severity],
			 source->file, source->line, source->func, msg);
	} else {
		fprintf (stderr, "%s %s", severity_labels[severity], msg);
	}
}

/**
 * @}
 */
