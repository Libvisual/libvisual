#ifndef _LV_LOG_H
#define _LV_LOG_H

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* This is read-only */
extern char *__lv_progname;

/**
 * Used to determine the severity of the log message using the visual_log
 * define.
 *
 * @see visual_log
 */
typedef enum {
	VISUAL_LOG_DEBUG,	/**< Debug message, to use for debug messages. */
	VISUAL_LOG_INFO,	/**< Informative message, can be used for general info. */
	VISUAL_LOG_WARNING,	/**< Warning message, use to warn the user. */
	VISUAL_LOG_CRITICAL,	/**< Critical message, when a critical situation happens.
				 * Like a NULL pointer passed to a method. */
	VISUAL_LOG_ERROR	/**< Error message, use to notify the user of fatals. */
} VisLogSeverity;

/*#ifdef __GNUC__*/
/**
 * Used for log messages, this is brought under a define so
 * that the __FILE__ and __LINE__ macros work, and thus provide
 * better information.
 *
 * @see VisLogSeverity
 *
 * @param severity Determines the severity of the message using VisLogSeverity.
 * @param message The log message itself.
 */
/*#define visual_log(severity, message)		\
	_lv_log (severity,			\
	"%s:%d, %s (): %s",			\
	__FILE__,				\
	__LINE__,				\
	__PRETTY_FUNCTION__,			\
	message)

#else*/ /* not __GNUC__ */

/**
 * Used for log messages, this is brought under a define so
 * that the __FILE__ and __LINE__ macros work, and thus provide
 * better information.
 *
 * @see VisLogSeverity
 *
 * @param severity Determines the severity of the message using VisLogSeverity.
 * @param message The log message itself.
 */
#ifdef LV_HAVE_ISO_VARARGS
#define visual_log(severity,...)		\
		_lv_log (severity,		\
			__FILE__,		\
			__LINE__,		\
			__VA_ARGS__)
#elif defined(LV_HAVE_GNUC_VARARGS)
#define visual_log(severity,format...)		\
		_lv_log (severity,		\
			__FILE__,		\
			__LINE__,		\
			format)
#else
static void visual_log (VisLogSeverity severity, const char *fmt, ...)
{
	char str[1024];
	va_list va;
	char sever_msg[10];
	
	assert (fmt != NULL);

	va_start (va, fmt);
	vsnprintf (str, 1023, fmt, va);
	va_end (va);

	switch (severity) {
		case VISUAL_LOG_DEBUG:
			strncpy (sever_msg, "DEBUG", 9);
			break;
		case VISUAL_LOG_INFO:
			strncpy (sever_msg, "INFO", 9);
			break;
		case VISUAL_LOG_WARNING:
			strncpy (sever_msg, "WARNING", 9);
			break;
		case VISUAL_LOG_CRITICAL:
			strncpy (sever_msg, "CRITICAL", 9);
			break;
		case VISUAL_LOG_ERROR:
			strncpy (sever_msg, "ERROR", 9);
			break;
		default:
			assert (0);
	}
	/*
	 * Sorry, we doesn't have (file,line) information
	 */
	fprintf (stderr, "libvisual %s: %s: %s\n",
			sever_msg, __lv_progname, str);
}
#endif

/*#define visual_log(severity, message)		\
	_lv_log (severity,			\
	"%s:%d: %s",				\
	__FILE__,				\
	__LINE__,				\
	message)
#endif*/

void _lv_log (VisLogSeverity severity, const char *file, int line, const char *fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LOG_H */
