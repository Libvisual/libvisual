#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <sys/types.h>

#include "lv_log.h"

static const char *log_severity_to_string (VisLogSeverity severity);

static const char *log_severity_to_string (VisLogSeverity severity)
{
	switch (severity) {
		case VISUAL_LOG_DEBUG:
			return "DEBUG";

		case VISUAL_LOG_INFO:
			return "INFO";

		case VISUAL_LOG_WARNING:
			return "WARNING";

		case VISUAL_LOG_CRITICAL:
			return "CRITICAL";

		case VISUAL_LOG_ERROR:
			return "ERROR";

		default:
			/*
			 * FIXME This cannot happen! We must abort
			 */
			return "INVALID LOG LOG";
	}

	assert (0);
}

/**
 * @defgroup VisLog VisLog
 * @{
 */


/**
 * Private library call used by the visual_log define to display debug,
 * warning and error messages.
 *
 * @see visual_log
 * 
 * @param severity Severity of the log message.
 * @param fmt Format string to display the log message.
 *
 * @return 0 on succes -1 on error.
 */
void _lv_log (VisLogSeverity severity, const char *file,
			int line, const char *funcname, const char *fmt, ...)
{
	char str[1024];
	va_list va;
	
	assert (fmt != NULL);

	va_start (va, fmt);
	vsnprintf (str, 1023, fmt, va);
	va_end (va);

	if (funcname == NULL) { /* The system is not GNUC */
		if (severity == VISUAL_LOG_INFO) {
			printf ("libvisual %s: %s: %s\n",
					log_severity_to_string (severity),
					__lv_progname, str);
		} else {
			fprintf (stderr, "libvisual %s: %s [(%s,%d)]: %s\n",
					log_severity_to_string (severity), __lv_progname,
					file, line, str);
		}
	} else {
		if (severity == VISUAL_LOG_INFO) {
			printf ("libvisual %s: %s: %s\n",
					log_severity_to_string (severity),
					__lv_progname, str);
		} else {
			fprintf (stderr, "libvisual %s: %s: %s [(%s,%d)]: %s\n",
					log_severity_to_string (severity), __lv_progname,
					funcname, file, line, str);
		}
	}

	if (severity == VISUAL_LOG_ERROR)
		exit (1);
}

/**
 * @}
 */

