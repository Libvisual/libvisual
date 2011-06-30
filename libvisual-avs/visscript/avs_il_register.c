#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"

static void convert_argument(AvsCompilerArgument *arg, ILRegister *reg)
{
	arg->type = AvsCompilerArgumentPrivate;
	arg->value.ptr = reg;
}

static ILRegister * load_constant(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg)
{
	ILRegister *reg = avs_il_register_create();

	reg->type = ILRegisterTypeConstant;
	reg->value.constant = arg->value.constant;

	/* Convert argument */
	convert_argument(arg, reg);
	return reg;
}

static ILRegister * load_identifier(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg)
{
	ILRegister *reg = avs_il_register_create();
	AvsRunnableVariable *var;
	
	/* Lookup variable */
	var = avs_runnable_variable_find(robj->variable_manager, arg->value.identifier);
	if (var == NULL) {
		/* Variable not found, auto-create one */
		var = avs_runnable_variable_create(robj->variable_manager, arg->value.identifier, 0);
	}

	avs_debug(print("ILR: Variable name: %s", var->name));

	reg->type = ILRegisterTypeVariable;
	reg->value.variable = var;

	/* Convert argument */
	convert_argument(arg, reg);
	return reg;
}

static ILRegister * load_worker(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg)
{
	ILRegister *reg = avs_il_register_create();
	
	reg->type = ILRegisterTypeConstant;
	convert_argument(arg, reg);
	return reg;
}

ILRegister * avs_il_register_load_worker(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg)
{
	return load_worker(ctx, robj, arg);
}

ILRegister * avs_il_register_load_from_argument(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg)
{
	ILRegister *reg;
	
	switch (arg->type) {
		case AvsCompilerArgumentConstant:
			/* Generate load instruction */
			reg = load_constant(ctx, robj, arg);
			break;

		case AvsCompilerArgumentIdentifier:
			reg = load_identifier(ctx, robj, arg);
			break;
			
		case AvsCompilerArgumentPrivate:
			/* Data already loaded into a register */
			reg = (ILRegister *)arg->value.ptr;
			break;
			
		default:
			return NULL; /* Unsupported */
	}

	return reg;
}


void avs_il_register_dereference(ILRegister *reg)
{
	reg->ref--;
    if(reg->ref <= 0)
        free(reg);
}

void avs_il_register_reference(ILRegister *reg)
{
	reg->ref++;
}

ILRegister * avs_il_register_create(void)
{
	ILRegister *reg;

	reg = malloc(sizeof(ILRegister));
	memset(reg, 0, sizeof(ILRegister));

	return reg;
}

