#ifndef _AVS_H
#define _AVS_H 1

#include <libvisual/libvisual.h>

#define AVS_DEBUG 1
#ifdef AVS_DEBUG
	#define avs_log(args...)    visual_log(VISUAL_LOG_DEBUG, args)
    #define avs_info(args...)   message(VISUAL_LOG_VERBOSENESS_HIGH, args)
	#define avs_print(args...)	message(VISUAL_LOG_VERBOSENESS_MEDIUM, args)
    #define avs_error(args...)  message(VISUAL_LOG_VERBOSENESS_LOW, args)
	#define avs_call(args...)	args
	#define avs_debug(x) avs_##x 
#else
	#define avs_debug(x)
#endif

void message(const VisLogVerboseness level, const char *format, ...) __attribute__ ((format(__printf__, 2, 3)));

struct _AvsRunnableContext;
typedef struct _AvsRunnableContext AvsRunnableContext;
struct _AvsRunnable;
typedef struct _AvsRunnable AvsRunnable;
struct _AvsRunnableVariable;
typedef struct _AvsRunnableVariable AvsRunnableVariable;
struct _AvsRunnableVariableManager;
typedef struct _AvsRunnableVariableManager AvsRunnableVariableManager;
struct _AvsRunnableFunction;
typedef struct _AvsRunnableFunction AvsRunnableFunction;

/* Internal numeric representation */
//typedef float AvsNumber;
typedef float AvsNumber;

/* Value used for comparison, close enough to consider it to be zero */
#define AVS_EPSILON 0.00001
#define AVS_VALUEBOOL(x) (abs(x) > AVS_EPSILON)

#include <libvisual/libvisual.h>
#include "avs_stack.h"
#include "avs_blob.h"
#include "avs_blob_pool.h"
#include "avs_lexer.h"
#include "avs_parser.h"
#include "avs_compiler.h"
#include "avs_il_assembler.h"
#include "avs_runnable.h"
#include "avs_functions.h"

#endif /* !_AVS_H */
