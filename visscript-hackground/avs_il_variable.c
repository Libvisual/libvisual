#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"


static inline AvsILVariable * find_constant(AvsILVariableContext *ctx, AvsNumber constant)
{

}

static inline AvsILVariable * find_variable(AvsILVariableContext *ctx, char *identifier)
{
	AvsRunnableVariable *rv;
	
	/* Look at runnable variable list for matching variable */
	avs_debug(print("ILV: Searching for variable: %s", arg->value.identifier));
	if ((rv=avs_runnable_variable_find(obj, arg->value.identifier)) == NULL) {
		avs_debug(print("ILV: Not found, creating new one"));
		rv = avs_runnable_variable_create(obj, arg->value.identifier, 0);
	}

	/* Create IL Variable for RV, if needed */
	if (!rv->private) {
		iv_create();
	}
	
	return AVS_IL_VARIABLE(rv->private);
}


static inline void convert_argument(AvsCompilerArgument *ca, AvsRunnableVarialbe *var)
{
	ca->type = AvsCompilerArgumentPrivate;
	ca->value.ptr = var;
}

AvsILVariable * avs_il_variable_retrieve(AvsILVariableContext *ctx,
				 	 AvsRunnable *obj,
					 AvsCompilerArgument *arg)
{
	switch (arg->type) {
		case AvsCompilerArgumentConstant:
			break;

		case AvsCompilerArgumentIdentifier:
			break;

		case AvsCompilerArgumentPrivate:
			break;

		default:
			return NULL;
	}

	return NULL;
}

AvsILVariable * avs_il_variable_store(AvsILVariableContext *ctx,
				      AvsRunnable *obj,
				      AvsCompilerArgument *arg)
{
	return NULL;
}


/**
 *
 *	Reset an IL Variable context.
 *
 *	@param ctx IL Variable context to reset.
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_variable_reset(AvsILVariableContext *ctx)
{
	/* Reset IL Variable stack */
	avs_stack_Reset(ctx->vstack);
	return VISUAL_OK;
}

/** 
 *	Cleanup an IL Variable context.
 *
 *	@param ctx IL Variable context to cleanup.
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_tree_cleanup(AvsILVariableContext *ctx)
{
	/* Cleanup IL Variable stack */
	visual_object_unref(VISUAL_OBJECT(ctx->vstack));
	return VISUAL_OK;
}

/**
 *	Initialize an IL Variable context.
 * 
 * 	@param ctx IL Variable context to initialize.
 *
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_variable_init(AvsILVariableContext *ctx)
{
	memset(ctx, 0, sizeof(AvsILVariableContext *ctx));

	/* Allocate Variable Stack */
	ctx->vstack = avs_stack_new0(AvsILVariable, AVS_ILVARIABLE_INITIAL_STACKSIZE,
						    AVS_ILVARIABLE_MAXIMUM_STACKSIZE);

	return VISUAL_OK;
}

