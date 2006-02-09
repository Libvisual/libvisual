#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <math.h>

#include "avs.h"
#include "avs_functions.perf.h"

#define AVS_BUILTIN_FUNCTION(x, expr) \
	AVS_RUNNABLE_FUNCTION(avs_builtin_function_##x) { *retval = (expr); }

AVS_BUILTIN_FUNCTION(abs,	fabs(*args[0]));
AVS_BUILTIN_FUNCTION(sin,	sin(*args[0]));
AVS_BUILTIN_FUNCTION(cos,	cos(*args[0]));
AVS_BUILTIN_FUNCTION(tan,	tan(*args[0]));
AVS_BUILTIN_FUNCTION(asin,	asin(*args[0]));
AVS_BUILTIN_FUNCTION(acos,	acos(*args[0]));
AVS_BUILTIN_FUNCTION(atan,	atan(*args[0]));
AVS_BUILTIN_FUNCTION(atan2,	atan2(*args[0], *args[1]));
AVS_BUILTIN_FUNCTION(sqr,	*args[0] * *args[0]);
AVS_BUILTIN_FUNCTION(sqrt,	sqrt(*args[0]));
AVS_BUILTIN_FUNCTION(invsqrt,	1/sqrt(*args[0]));
AVS_BUILTIN_FUNCTION(pow,	pow(*args[0], *args[1]));
AVS_BUILTIN_FUNCTION(exp,	exp(*args[0]));
AVS_BUILTIN_FUNCTION(log,	log(*args[0]));
AVS_BUILTIN_FUNCTION(log10,	log10(*args[0]));
AVS_BUILTIN_FUNCTION(floor,	floor(*args[0]));
AVS_BUILTIN_FUNCTION(ceil,	ceil(*args[0]));
AVS_BUILTIN_FUNCTION(sign,	copysign(*args[0]!=0.0?1.0:0.0, *args[0]));
AVS_BUILTIN_FUNCTION(min,	*args[0] < *args[1] ? *args[0] : *args[1]);
AVS_BUILTIN_FUNCTION(max,	*args[0] > *args[1] ? *args[0] : *args[1]);
AVS_BUILTIN_FUNCTION(sigmoid,	1.0/(1.0 + exp(-*args[0])));
AVS_BUILTIN_FUNCTION(rand,	0xdeadbeaf * *args[0]);
AVS_BUILTIN_FUNCTION(band,	AVS_VALUEBOOL(*args[0]) && AVS_VALUEBOOL(*args[1]));
AVS_BUILTIN_FUNCTION(bor,	AVS_VALUEBOOL(*args[0]) || AVS_VALUEBOOL(*args[1]));
AVS_BUILTIN_FUNCTION(bnot,	!AVS_VALUEBOOL(*args[0]));
AVS_BUILTIN_FUNCTION(equal,	AVS_VALUEBOOL(*args[0] - *args[1]) ? 1.0 : 0.0)
AVS_BUILTIN_FUNCTION(above,	*args[0] > *args[1] ? 1.0 : 0.0)
AVS_BUILTIN_FUNCTION(below,	*args[0] < *args[1] ? 1.0 : 0.0)

AvsRunnableFunction avs_builtin_functions[] =
{
	{ "abs",	avs_builtin_function_abs, 	1, },
	{ "sin",	avs_builtin_function_sin, 	1, },
	{ "cos",	avs_builtin_function_cos, 	1, },
	{ "tan",	avs_builtin_function_tan, 	1, },
	{ "asin",	avs_builtin_function_asin, 	1, },
	{ "acos",	avs_builtin_function_acos, 	1, },
	{ "atan",	avs_builtin_function_atan,	1, },
	{ "atan2",	avs_builtin_function_atan2,	2, },
	{ "sqr",	avs_builtin_function_sqr,	1, },
	{ "sqrt",	avs_builtin_function_sqrt,	1, },
	{ "invsqrt",	avs_builtin_function_invsqrt,	1, },
	{ "pow",	avs_builtin_function_pow,	2, },
	{ "exp",	avs_builtin_function_exp,	1, },
	{ "log",	avs_builtin_function_log,	1, },
	{ "log10",	avs_builtin_function_log10,	1, },
	{ "floor",	avs_builtin_function_floor,	1, },
	{ "ceil",	avs_builtin_function_ceil,	1, },
	{ "sign",	avs_builtin_function_sign,	1, },
	{ "min",	avs_builtin_function_min,	2, },
	{ "max",	avs_builtin_function_max,	2, },
	{ "sigmoid",	avs_builtin_function_sigmoid,	1, },
	{ "rand",	avs_builtin_function_rand,	1, },
	{ "band",	avs_builtin_function_band,	2, },
	{ "bor",	avs_builtin_function_bor,	2, },
	{ "bnot",	avs_builtin_function_bnot,	2, },
	{ "equal",	avs_builtin_function_equal,	2, },
	{ "above",	avs_builtin_function_above,	2, },
	{ "below",	avs_builtin_function_below,	2, },
	{ NULL, },
};

/**
 *	Translate a function name into an avs_builtin_functions table index number.
 *
 *	@param name Name to lookup.
 *
 *	@return AvsBuiltinFunctionType index number matching 'name'.
 */
AvsBuiltinFunctionType avs_builtin_function_type(char *name)
{
	AvsBuiltinFunctionToken *tok = in_word_set(name, strlen(name));

	if (!tok)
		return -1;
	
	return tok->lookup;
}

/**
 *	Retrieve AvsRunnableFunction structure by index.
 *
 *	@param lookup Index into avs_builtin_function table.
 *
 * 	@see AvsBuiltinFunctionType
 *	@return AvsRunnableFunction structure matching 'lookup' on success, NULL on failure.
 */
AvsRunnableFunction * avs_builtin_function_lookup(AvsBuiltinFunctionType lookup)
{
	if (lookup >= sizeof(avs_builtin_functions) / sizeof(AvsRunnableFunction))
		return NULL;

	return &avs_builtin_functions[lookup];
}

/**
 *	Search builtin function
 *
 *	@param name Builtin function to find.
 *
 *	@return AvsRunnableFunction structure matching 'name' on success, NULL on failure.
 */
AvsRunnableFunction * avs_builtin_function_find(char *name)
{
	AvsBuiltinFunctionToken *tok;

	tok = in_word_set(name, strlen(name));
	if (tok == NULL)
		return NULL;

	if (tok->lookup >= sizeof(avs_builtin_functions) / sizeof(AvsRunnableFunction))
		return NULL;

	return &avs_builtin_functions[tok->lookup];
}
