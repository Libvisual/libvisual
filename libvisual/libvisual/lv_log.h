#ifndef _LV_LOG_H
#define _LV_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

#ifdef __GNUC__
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
#define visual_log(severity, message)		\
	_lv_log (severity,			\
	"%s:%d, %s (): %s",			\
	__FILE__,				\
	__LINE__,				\
	__PRETTY_FUNCTION__,			\
	message)

#else /* not __GNUC__ */
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
#define visual_log(severity, message)		\
	_lv_log (severity,			\
	"%s:%d: %s",				\
	__FILE__,				\
	__LINE__,				\
	message)
#endif

int _lv_log (VisLogSeverity severity, const char *fmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LOG_H */
