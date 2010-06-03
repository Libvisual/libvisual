
#include <libvisual/libvisual.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "jstypes.h"
#include "jsapi.h"

typedef struct {
	JSRuntime *rt;
} VisscriptPrivate;

/* The class of the global object. */
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *ctx, const char *message, JSErrorReport *report)
{
    fprintf(stderr, "%s:%u:%s\n",
            report->filename ? report->filename : "<no filename>",
            (unsigned int) report->lineno,
            message);
}

int script_visscript_init (VisPluginData *plugin);
int script_visscript_cleanup (VisPluginData *plugin);

double function_log(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_sin(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_cos(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_tan(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_asin(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_acos(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_atan(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_if(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_div(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double function_rand(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec global_functions[] = {
	JS_FS("log", function_log, 1, 0, 0),
	JS_FS("cos", function_cos, 1, 0, 0),
	JS_FS("tan", function_tan, 1, 0, 0),
	JS_FS("asin", function_asin, 1, 0, 0),
	JS_FS("acos", function_acos, 1, 0, 0),
	JS_FS("atan", function_atan, 1, 0, 0),
	JS_FS("if", function_if, 3, 0, 0),
	JS_FS("div", function_div, 1, 0, 0),
	JS_FS("rand", function_rand, 2, 0, 0),
	JS_FS_END
};

void * get_context(VisPluginData *plugin)
{
	VisscriptPrivate *priv = visual_object_get_private(VISUAL_OBJECT(plugin));
	JSObject *global;

	JSContext ctx = JS_NewContext(priv->rt, 8192);

	JS_SetOPtions(ctx, JSOPTION_VAROBJFIX);
	JS_SetVersion(ctx, JSVERSION_LATEST);
	//JS_SetError(ctx, reportError);

	global = JS_NewObject(ctx, &global_class, NULL, NULL);

	JS_DefineFunctions(ctx, global, global_functions)

	return ctx;
}

const VisPluginInfo *get_plugin_info(int *count)
{

	static VisScriptPlugin script[] = {{
		.get_context = get_context
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_SCRIPT,
		.name = "VisScript plugin",
		.author = "Scott Sibley <starlon@sf.net>",
		.version = "0.1",
		.about = "Libvisual javascript plugin",
		.license = VISUAL_PLUGIN_LICENSE_GPL,
		.init = script_visscript_init,
		.cleanup = script_visscript_cleanup,
		.plugin = VISUAL_OBJECT (&script[0])
	}};

	*count = sizeof(info) / sizeof(*info);

	return info;
}

int script_visscript_init (VisPluginData *plugin)
{
	VisscriptPrivate *priv = visual_mem_new0(VisscriptPrivate, 1);

	visual_object_set_private(VISUAL_OBJECT(plugin), priv);

	priv->rt = JS_NewRuntime(8L * 1024L * 1024L);
	visual_log_return_val_if_fail(priv->rt != NULL, -VISUAL_ERROR_GENERAL);

	return VISUAL_OK;
}

int script_visscript_cleanup (VisPluginData *plugin)
{
	VisscriptPrivate *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	JS_DestroyRuntime(priv->rt);
	JS_ShutDown();

	if( priv ) 
		visual_mem_free(priv);

	return VISUAL_OK;
}

double function_log(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return log(val);
}

double function_sin(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return sin(val);
}

double function_cos(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return cos(val);
}

double function_tan(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return tan(val);
}

double function_asin(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return asin(val);
}

double function_acos(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return acos(val);
}

double function_atan(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "u", &val) )
		return JS_FALSE;

	return atan(val);
}

double function_if(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t a, b, c;

	if (argc != 3) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "uuu", &a, &b, &c) )
		return JS_FALSE;

	return (c != 0.0) ? a : b;
}

double function_div(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t a, b;

	if (argc != 2) 
		return JS_FALSE;

	if (!JS_ConvertArguments(ctx, argc, argv, "uu", &a, &b) )
		return JS_FALSE;

	return (a == 0) ? 0 : (b / a);
}

double function_rand(JSContext *ctx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t a, b, seed;
	
	if (!JS_ConvertArguments(ctx, argc, argv, "uu", &a, &b))
		return JS_FALSE;

	seed = time(NULL);

	srand(seed);

	return (rand()%(b-a))+a;
}
