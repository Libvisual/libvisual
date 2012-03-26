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
#include "lv_log.h"
#include "lv_common.h"
#include "gettext.h"
#include <stdio.h>
#include <stdarg.h>

#define LV_LOG_MAX_MESSAGE_SIZE 1024

typedef struct {
	VisLogHandlerFunc  func;
	void			  *priv;
} LogHandler;

static VisLogSeverity verbosity = VISUAL_LOG_WARNING;

static const char *log_prefixes[VISUAL_LOG_CRITICAL+1] = {
	"DEBUG    ",
	"INFO     ",
	"WARNING  ",
	"ERROR    ",
	"CRITICAL "
};

static LogHandler log_handlers[VISUAL_LOG_CRITICAL+1];

static void output_to_stderr (VisLogSeverity severity, const char *msg,
    VisLogSource const *source);

static int is_valid_severity (VisLogSeverity severity)
{
    return (severity >= VISUAL_LOG_DEBUG && severity <= VISUAL_LOG_CRITICAL);
}

void visual_log_set_verbosity (VisLogSeverity level)
{
	verbosity = level;
}

VisLogSeverity visual_log_get_verbosity ()
{
	return verbosity;
}

void visual_log_set_handler (VisLogSeverity severity, VisLogHandlerFunc func, void *priv)
{
	LogHandler *handler;

	visual_return_if_fail (is_valid_severity (severity));

	handler = &log_handlers[severity];
	handler->func = func;
	handler->priv = priv;
}

#if defined(LV_HAVE_ISO_C_VARARGS) || defined(LV_HAVE_GNU_C_VARARGS)

void _lv_log (VisLogSeverity severity, const char *file,
	int line, const char *funcname, const char *fmt, ...)
{
	VisLogSource source;
	LogHandler *handler;
	char message[LV_LOG_MAX_MESSAGE_SIZE];
	va_list va;

	if (!is_valid_severity (severity) || fmt == NULL) {
		visual_log (VISUAL_LOG_ERROR, "(malformed message)");
		if (fmt == NULL) {
		  printf ("wtf null\n");
		}
		if (!is_valid_severity (severity)) {
		  printf ("Wtf invalid severity\n");
		}
		return;
	}

	if (verbosity > severity)
		 return;

	va_start (va, fmt);
	vsnprintf (message, LV_LOG_MAX_MESSAGE_SIZE-1, fmt, va);
	va_end (va);

	source.file = file;
	source.func = funcname;
	source.line = line;

	handler = &log_handlers[severity];

	if (handler->func != NULL) {
		(*handler->func) (severity, message, &source, handler->priv);
	} else {
		output_to_stderr (severity, message, &source);
	}
}

#else /* LV_HAVE_ISO_C_VARARGS && LV_HAVE_GNU_C_VARARGS */

void visual_log (VisLogSeverity severity, const char *fmt, ...)
{
	va_list vargs;
	LogHandler *handler;
	char str[LV_LOG_MAX_MESSAGE_SIZE];

	if (!is_valid_severity (severity) || fmt == NULL) {
		visual_log (VISUAL_LOG_ERROR, "(malformed message)");
		return;
	}

	if (verbosity > severity)
		 return;

	va_start (vargs, fmt);
	vsnprintf (str, LV_LOG_MAX_MESSAGE_SIZE-1, fmt, vargs);
	va_end (vargs);

	handler = &message_handlers[severity];

	if (handler->func != NULL) {
		handler->func (severity, str, NULL, handler->priv);
	} else {
		output_to_stderr (severity, str, NULL);
	}
}

#endif /* LV_HAVE_ISO_C_VARARGS && LV_HAVE_GNU_C_VARARGS */

static void output_to_stderr (VisLogSeverity severity, const char *msg,
	VisLogSource const *source)
{
	if (source != NULL) {
		fprintf (stderr, "%s %s:%d:%s: %s\n", log_prefixes[severity],
			 source->file, source->line, source->func, msg);
	} else {
		fprintf (stderr, "%s %s\n", log_prefixes[severity], msg);
	}
}
