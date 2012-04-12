#ifndef _LV_CHECKS_H
#define _LV_CHECKS_H

#include <libvisual/lvconfig.h>

#ifdef NDEBUG

#define visual_return_if_fail(expr)
#define visual_return_val_if_fail(expr, val)

#else /* NDEBUG */

#include <libvisual/lv_log.h>

#ifdef VISUAL_PEDANTIC_CHECKS

#include <libvisual/lv_error.h>

#define visual_return_if_fail(expr) \
	if (!(expr)) {                                                        \
		visual_log (VISUAL_LOG_CRITICAL, "Assertion '%s' failed", #expr); \
		visual_error_raise (VISUAL_ERROR_FAILED_CHECK);                   \
	}

#define visual_return_val_if_fail(expr, val) \
	if (!(expr)) {                                                        \
		visual_log (VISUAL_LOG_CRITICAL, "Assertion '%s' failed", #expr); \
		visual_error_raise (VISUAL_ERROR_FAILED_CHECK);                   \
		return (val);                                                     \
	}

#else /* VISUAL_PEDANTIC_CHECKS */

/**
 * Return if @a expr is FALSE, showing a critical message with
 * useful information.
 */
#define visual_return_if_fail(expr)	\
	if (!(expr)) {                              \
		visual_log (VISUAL_LOG_WARNING,         \
			"Assertion `%s' failed", #expr);    \
		return;                                 \
	}

/**
 * Return if @a val if @a expr is FALSE, showing a critical message
 * with useful information.
 */
#define visual_return_val_if_fail(expr, val) \
	if (!(expr)) {                                  \
		visual_log (VISUAL_LOG_WARNING,             \
			"Assertion `%s' failed", #expr);        \
		return (val);                               \
	}

#endif /* VISUAL_PEDANTIC_CHECKS */

#endif /* NDEBUG */

#endif /* _LV_CHECKS_H */
