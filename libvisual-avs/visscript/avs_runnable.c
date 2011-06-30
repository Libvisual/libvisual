#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <math.h>

#include "avs.h"

#define PI M_PI
#define E 2.71828
#define PHI 1.618033

#define AVS_VARIABLE_MANAGER(obj)   (VISUAL_CHECK_CAST((obj), AvsRunnableVariableManager))

static int vm_dtor(VisObject *object) {
    AvsRunnableVariableManager *manager = AVS_VARIABLE_MANAGER(object);
    AvsRunnableVariable *var = NULL;
    AvsRunnableVariable *tmp = NULL;
    for(var = tmp = manager->variables; var; var = tmp) {
        tmp = var->next;
        visual_object_unref(VISUAL_OBJECT(var));
    }

    return VISUAL_OK;
}

/**
 * 	Create a new variable manager, used for sharing variables between
 * 	runnable objects.
 *
 * 	@return Pointer to a newly created variable manager
 */
AvsRunnableVariableManager * avs_runnable_variable_manager_new()
{
	AvsRunnableVariableManager *manager = visual_mem_new0(AvsRunnableVariableManager, 1);//malloc(sizeof(AvsRunnableVariableManager));

    visual_object_initialize(VISUAL_OBJECT(manager), TRUE, vm_dtor);

    // Every manager should have these variables
    avs_runnable_variable_create(manager, "PI", PI);
    avs_runnable_variable_create(manager, "E", E);
    avs_runnable_variable_create(manager, "PHI", PHI);

	return manager;
}

static int runnable_variable_dtor (VisObject *obj)
{
    AvsRunnableVariable *var = VISUAL_CHECK_CAST(obj, AvsRunnableVariable);

    return VISUAL_OK;
}


/**
 * 	Create a new variable with name @a name and a 
 * 	default value of @a value.
 *
 * 	@param manager Runnable Variable Manager.
 * 	@param name Name of variable to create.
 * 	@param value Default value.
 *
 *	@note The name of the variable to be added shouldn't exist yet.
 *	      However, no check for duplicate variables will be done.
 *	      Use avs_runnable_variable_find, if you want to make sure that variable doesn't exist already.
 *
 *	@see avs_runnable_variable_find
 * 	@return Pointer to newly created variable on success, NULL on failure.
 */
AvsRunnableVariable * avs_runnable_variable_create(AvsRunnableVariableManager *manager, char *name, AvsNumber value)
{

	AvsRunnableVariable *var = visual_mem_new0(AvsRunnableVariable, 1);
    visual_object_initialize(VISUAL_OBJECT(var), TRUE, runnable_variable_dtor);

	var->name = name;
	var->local_value = value;
	var->value = &var->local_value;
	var->flags = 0;
	var->next = manager->variables;
	manager->variables = var;

	avs_debug(print("runnable: Creating variable: %s = %f", name, value));
	return var;
}

/**
 * 	Find variable matching @a name
 *
 * 	@param manager Runnable Variable Manager containing @a name.
 * 	@param name Variable name to search for.
 *
 * 	@return variable matching @a name on success, NULL on failure.
 */
AvsRunnableVariable * avs_runnable_variable_find(AvsRunnableVariableManager *manager, char *name)
{
	AvsRunnableVariable *var;

	for (var=manager->variables; var != NULL; var=var->next) {
		if (strcmp(var->name, name))
			continue;

		return var;
	}

	return NULL;
}

/**
 *	Bind a Runnable Variable Manager variable to a user supplied pointer.
 *	
 *	@param manager Runnable Variable Manager
 *	@param name Name of variable to bind
 *	@param retval AvsNumber pointer to store variable value pointer into.
 *
 * 	@note Can only be used AFTER compilation is done.
 *
 * 	@see avs_runnable_compile
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_runnable_variable_post_bind(AvsRunnableVariableManager *manager, char *name, AvsNumber **retval)
{
	AvsRunnableVariable *var = avs_runnable_variable_find(manager, name);
	
	if (!var)
		return VISUAL_ERROR_GENERAL;

	*retval = var->value;
	return VISUAL_OK;
}

/**
 *	Create a new Runnable Object variable and bind the variable value
 *	to a user supplied pointer.
 *
 *	@param manager Runnable Variable Manager
 *	@param name Name of variable to bind
 *	@param value AvsNumber to bind variable to.
 *	
 *	@note Can only be used BEFORE compilation is done.
 *
 *	@see avs_runnable_compile
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_runnable_variable_bind(AvsRunnableVariableManager *manager, char *name, AvsNumber *value)
{
	AvsRunnableVariable *var = avs_runnable_variable_find(manager, name);
	
	if (!var)
		var = avs_runnable_variable_create(manager, name, -1);
	
	if (!var)
		return VISUAL_ERROR_GENERAL;

	var->value = value;
	return VISUAL_OK;
}

/**
 * Execute a runnable object
 *
 * @param obj Runnable object to execute.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_runnable_execute(AvsRunnable *obj)
{
	if (!obj->run)
		return VISUAL_ERROR_GENERAL;

	return obj->run(obj);
}

/**
 * Parse and compile code buffer into runnable object code.
 *
 * @param obj Runnable Object, previously initialized or created with avs_runnable_new()
 * @param data Buffer containing runnable code to compile.
 * @param length Length of data buffer.
 * 
 * @see avs_runnable_context_new
 * @see avs_runnable_new
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_runnable_compile(AvsRunnable *obj, unsigned char *data, unsigned int length)
{
	/* Point lexer to new input data */
	avs_lexer_reset(&obj->ctx->lexer, data, length);

	/* Reset compiler */
	avs_compile_reset_stack(&obj->ctx->compiler);

	/* Initialize IL assembler for output object */
	avs_il_runnable_init(&obj->ctx->assembler, obj);

	/* Run parser (lexer -> parser -> compiler -> assembler -> runnable) */
	avs_parser_run(&obj->ctx->parser, obj);
	
	/* Finish output object */
	avs_il_runnable_finish(&obj->ctx->assembler, obj);

	return VISUAL_OK;
}

static int runnable_dtor(VisObject *object)
{	
	AvsRunnable *obj = AVS_RUNNABLE(object);
	//AvsRunnableVariable *var, *next;
	
	/* Cleanup assembler for output object */
	avs_il_runnable_cleanup(&obj->ctx->assembler, obj);

	/* Cleanup variables */ // FIXME
//	for (var=obj->variables; var != NULL; var=next) {
//		next = var->next;
//		visual_mem_free(var);
//	}

	/* Cleanup blob manager */
	visual_object_unref(VISUAL_OBJECT(&obj->bm));

	return 0;
}

static int runnable_ctor(AvsRunnableContext *ctx, AvsRunnable *obj)
{
	/* Associate Runnable Context with Runnable Object */
	obj->ctx = ctx;

	/* Initialize blob manager */
	avs_blob_manager_init(&obj->bm);

	return VISUAL_OK;
}

AvsRunnableVariableManager * avs_runnable_get_variable_manager(AvsRunnable *obj)
{
	return obj->variable_manager;
}

void avs_runnable_set_variable_manager(AvsRunnable *obj, AvsRunnableVariableManager *manager)
{
	obj->variable_manager = manager;
}

/**
 * Initialize a static runnable object
 *
 * @param ctx Runnable Context, previously initialize or created with avs_runnable_context_new()
 * @param obj Runnable Object, statically allocated.
 * 
 * @see avs_runnable_context_new
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_runnable_init(AvsRunnableContext *ctx, AvsRunnable *obj)
{
	visual_object_initialize(VISUAL_OBJECT(obj), FALSE, runnable_dtor);
	return runnable_ctor(ctx, obj);
}

/**
 * Create a new runnable object
 *
 * @param ctx Runnable Context, previously initialized or created with avs_runnable_context_new()
 * 
 * @see avs_runnable_context_new
 * @return Newly created object on success, NULL on failure.
 */
AvsRunnable * avs_runnable_new(AvsRunnableContext *ctx)
{
	AvsRunnable *obj;

	/* Allocate memory for Runnable Object */
	if ((obj = visual_mem_new0(AvsRunnable, 1)) == NULL)
		return NULL;

	visual_object_initialize(VISUAL_OBJECT(obj), TRUE, runnable_dtor);

	/* Initialize Runnable Object */
	runnable_ctor(ctx, obj);
	
	return obj;
}

static int context_dtor(VisObject *object)
{	
	AvsRunnableContext *ctx = AVS_RUNNABLE_CONTEXT(object);

	/* Cleanup parser, compiler and assembler context */
	avs_parser_cleanup(&ctx->parser);
	avs_compiler_cleanup(&ctx->compiler);
	avs_il_cleanup(&ctx->assembler);
    avs_il_core_context_cleanup(&ctx->core);
	return VISUAL_OK;
}

static int context_ctor(AvsRunnableContext *ctx)
{
	/* Initialize avs system contexts */
	avs_il_core_context_init(&ctx->core);
	avs_il_init(&ctx->assembler, &ctx->core);
	avs_compiler_init(&ctx->compiler, &ctx->assembler);
	avs_lexer_init(&ctx->lexer);
	avs_parser_init(&ctx->parser, &ctx->lexer, &ctx->compiler);
	return VISUAL_OK;
}
		
/**
 * Initialize a static runnable context.
 * Can be used to create multiple runnable objects in succession.
 *
 * @param ctx Runnable Context to initialize, statically allocated.
 *
 * @see avs_runnable_new
 *
 * @returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_runnable_context_init(AvsRunnableContext *ctx)
{
	visual_object_initialize(VISUAL_OBJECT(ctx), TRUE, context_dtor);
	return context_ctor(ctx);
}

/**
 * Create a new runnable context.
 * Can be used to create multiple objects in succession.
 *
 * @see avs_runnable_new
 * @returns A newly created runnable context on success, NULL on failure.
 */
AvsRunnableContext * avs_runnable_context_new(void)
{
	AvsRunnableContext *ctx;

	/* Allocate memory for Runnable Context */
	ctx = visual_mem_new0(AvsRunnableContext, 1);
	visual_object_initialize(VISUAL_OBJECT(ctx), TRUE, context_dtor);

	/* Initialize Runnable Context */
	context_ctor(ctx);
	return ctx;
}
