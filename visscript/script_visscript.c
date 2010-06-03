
#include <libvisual/libvisual.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "jstypes.h"
#include "jsapi.h"

typedef struct {
	JSRuntime *rt;
	JSContext *ctx;
	JSObject *global;
} VisscriptPrivate;

/* The class of the global object. */
static JSClass global_class = {
    "global", JSCLASS_GLOBAL_FLAGS,
    JS_PropertyStub, JS_PropertyStub, JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub, JS_ConvertStub, JS_FinalizeStub,
    JSCLASS_NO_OPTIONAL_MEMBERS
};

/* The error reporter callback. */
void reportError(JSContext *cx, const char *message, JSErrorReport *report)
{
    fprintf(stderr, "%s:%u:%s\n",
            report->filename ? report->filename : "<no filename>",
            (unsigned int) report->lineno,
            message);
}

int script_visscript_init (VisPluginData *plugin);
int script_visscript_cleanup (VisPluginData *plugin);

double command_log(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_sin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_cos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_tan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_asin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_acos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_atan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_if(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
double command_rand(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

static JSFunctionSpec global_functions[] = {
	JS_FS("log", command_log, 1, 0, 0),
	JS_FS("cos", command_cos, 1, 0, 0),
	JS_FS("tan", command_tan, 1, 0, 0),
	JS_FS("asin", command_asin, 1, 0, 0),
	JS_FS("acos", command_acos, 1, 0, 0),
	JS_FS("atan", command_atan, 1, 0, 0),
	JS_FS("if", command_if, 3, 0, 0),
	JS_FS("div", command_div, 1, 0, 0),
	JS_FS("rand", command_rand, 2, 0, 0)
};

/*
void * get_global(VisPluginData *plugin)
{
	VisscriptPrivate *priv = (VisscriptPrivate *)visual_object_get_private(VISUAL_OBJECT(plugin));

	return priv->global;
}

void * get_context(VisPluginData *plugin)
{
	VisscriptPrivate *priv = (VisscriptPrivate *)visual_object_get_private(VISUAL_OBJECT(plugin));
	return Context::New(NULL, priv->global);
}
*/

const VisPluginInfo *get_plugin_info(int *count)
{

	static VisScriptPlugin script[] = {{

	}};

	//script[0].get_global = get_global;
	//script[0].get_context = get_context;

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

	priv->ctx = JS_NewContext(priv->rt, 8192);
	visual_log_return_val_if_fail(priv->ctx != NULL, -VISUAL_ERROR_GENERAL);

	JS_SetOptions(priv->ctx, JSOPTION_VAROBJFIX);
	JS_SetVersion(priv->ctx, JSVERSION_LATEST);
	JS_SetError(priv->ctx, reportError);

	priv->global = JS_NewObject(priv->ctx, &global_class, NULL, NULL);
	visual_log_return_val_if_fail(priv->global != NULL, -VISUAL_ERROR_GENERAL);

	if (!JS_DefineFunctions(priv->ctx, priv->global, global_functions))
		return -VISUAL_ERROR_GENERAL;

	return VISUAL_OK;
}

int script_visscript_cleanup (VisPluginData *plugin)
{
	VisscriptPrivate *priv = visual_object_get_private(VISUAL_OBJECT(plugin));

	JS_DestroyContext(priv->ctx);
	JS_DestroyRuntime(priv->rt);
	JS_ShutDown();

	if( priv ) 
		visual_mem_free(priv);

	return VISUAL_OK;
}

double command_log(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return log(val);
}

double command_sin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return sin(val);
}

double command_cos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return cos(val);
}

double command_tan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return tan(val);
}

double command_asin(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return asin(val);
}

double command_acos(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return acos(val);
}

double command_atan(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t val;

	if (argc != 1) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "u", &val) )
		return JS_FALSE;

	return atan(val);
}

double command_if(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t a, b, c;

	if (argc != 3) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "uuu", &a, &b, &c) )
		return JS_FALSE;

	return (c != 0.0) ? a : b;
}

double command_div(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t a, b;

	if (argc != 2) 
		return JS_FALSE;

	if (!JS_ConvertArguments(cx, argc, argv, "uu", &a, &b) )
		return JS_FALSE;

	return (a == 0) ? 0 : (b / a);
}

double command_rand(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	uint32_t a, b, seed;
	
	if (!JS_ConvertArguments(cx, argc, argv, "uu", &a, &b))
		return JS_FALSE;

	seed = time(NULL);

	srand(seed);

	return (rand()%(b-a))+a;
}
