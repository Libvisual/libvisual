#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>

#include "lv_log.h"

extern char *__lv_progname;

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
			return "INVALID LOG LOG";
	}

	return "ERROR WITHIN LOG SYSTEM";
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
int _lv_log (VisLogSeverity severity, const char *fmt, ...)
{
	char str[1024];
	va_list va;
	
	if (fmt == NULL)
		return -1;

	va_start (va, fmt);
	vsnprintf (str, 1023, fmt, va);
	va_end (va);

	fprintf (stderr, "(%s:%d): libvisual-%s **: %s\n",
			__lv_progname, getpid (), log_severity_to_string (severity),
			str);

	return 0;
}

/**
 * @}
 */

