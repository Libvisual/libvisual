#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_vm.h"
#include "avs_x86.h"

AVS_STACK_MANAGER_TEMPLATE_LOCAL(X86Variable, xvmgr, 64, 1024*1024)
AVS_STACK_MANAGER_TEMPLATE_LOCAL(AvsRunnableVariable, rvmgr, 64, 1024*1024)

	
static inline X86Variable * create_variable(X86GlobalData *gd, AvsRunnableVariable *rv)
{
	X86Variable *var = xvmgr_push(gd->xvmgr);

	var->rv = rv;
	var->type = X86VariableTypeEmpty;
	return var;
}

static inline AvsRunnableVariable * create_result(X86GlobalData *gd)
{
	AvsRunnableVariable *var = rvmgr_push(gd->rvmgr);

	var->name = "#retval#";
	var->local_value = 0.0;
	var->value = &var->local_value;
	var->flags = AvsRunnableVariableAnonymous;
	var->next = gd->result;
	gd->result = var;

	return var;
}

static inline AvsRunnableVariable * create_constant(X86GlobalData *gd, AvsNumber value)
{
	AvsRunnableVariable *var = rvmgr_push(gd->rvmgr);

	var->name = "#anonymous constant#";
	var->local_value = value;
	var->value = &var->local_value;
	var->flags = AvsRunnableVariableConstant | AvsRunnableVariableAnonymous;
	var->next = gd->constant;
	gd->constant = var;

	return var;
}

static inline AvsRunnableVariable * find_constant(X86GlobalData *gd, AvsNumber constant)
{
	AvsRunnableVariable *var;

	for (var=gd->constant; var != NULL; var=var->next) {
		if (*var->value != constant)
			continue;

		return var;
	}

	return NULL;
}

static inline void convert_argument(AvsCompilerArgument *ca, X86Variable *var)
{
	ca->type = AvsCompilerArgumentPrivate;
	ca->value.ptr = var;
}

static inline X86Variable * retrieve_variable(AvsAssemblerContext *ctx, 
					      AvsRunnable *obj,
					      AvsCompilerArgument *arg)
{
	X86Variable *xv;
	AvsRunnableVariable *av;
	
	switch (arg->type) {
		case AvsCompilerArgumentConstant:
			if ((av=find_constant(X86_GLOBALDATA(ctx->pdata), arg->value.constant)) == NULL)
				av=create_constant(X86_GLOBALDATA(ctx->pdata), arg->value.constant);
		
			xv = create_variable(X86_GLOBALDATA(ctx->pdata), av);
			convert_argument(arg, xv);
			break;
			
		case AvsCompilerArgumentIdentifier:
			avs_debug(print("vm assembler: Searching for variable: %s", arg->value.identifier));
			if ((av=avs_runnable_variable_find(obj, arg->value.identifier)) == NULL) {
				avs_debug(print("vm assembler: Not found, creating new one"));
				av = avs_runnable_variable_create(obj, arg->value.identifier, 0);
			}

			xv = create_variable(X86_GLOBALDATA(ctx->pdata), av);
			convert_argument(arg, xv);
			break;
			
		case AvsCompilerArgumentPrivate:
			break;

		default:
			return NULL;
	}

	return X86_VARIABLE(arg->value.ptr);
}

static inline AvsNumber * retrieve_data(AvsAssemblerContext *ctx, 
					AvsRunnable *obj, 
					AvsCompilerArgument *arg)
{
	X86Variable *xv = retrieve_variable(ctx, obj, arg);

	if (xv)
		return xv->rv->value;
	else
		return NULL;
}

static inline X86Variable * store_variable(AvsAssemblerContext *ctx, 
					   AvsRunnable *obj, 
					   AvsCompilerArgument *arg)
{
	X86Variable *xv;
	AvsRunnableVariable *av;

	av = create_result(X86_GLOBALDATA(ctx->pdata));
	xv = create_variable(X86_GLOBALDATA(ctx->pdata), av);
	convert_argument(arg, xv);
	return X86_VARIABLE(arg->value.ptr);
}

static inline AvsNumber * store_data(AvsAssemblerContext *ctx, AvsRunnable *obj, AvsCompilerArgument *arg)
{
	X86Variable *xv = store_variable(ctx, obj, arg);

	if (xv)
		return xv->rv->value;
	else
		return NULL;
}


#if 0
ASSEMBLER_EMIT_FUNCTION(emit_assign)
{
	VmRunnableData *rd = VM_RUNNABLEDATA(obj->pasm);
	AvsRunnableVariable *arg[3];
	AvsInstruction vi;
	
	arg[1] = retrieve_variable(ctx, obj, &args[0]);
	arg[2] = retrieve_variable(ctx, obj, &args[1]);
	arg[0] = store_variable(ctx, obj, retval);
	
	/* Check if LHS value is constant */
	if (arg[1]->flags & AvsRunnableVariableConstant) {
		avs_debug(print("vm assembler: Trying to assign a value to a constant value, dickhead!"));
		exit(1);
	}

	/* Check if LHS value is anonymous */
	if (arg[1]->flags & AvsRunnableVariableAnonymous) {
		avs_debug(print("vm assembler: Trying to assign a value to a anonymous variable, dickweed!@"));
		exit(1);
	}
	
	/* Handle special fixup cases: assign(if(..., var, var), ...) */
	if (arg[1]->flags & AvsRunnableVariableIfValue) {
		/* assign(if(..., var, var), ...) */
		avs_debug(print("vm assembler: Assignment to if return value"));
		vi = AvsInsnAssignToPtr;
	} else if (arg[2]->flags & AvsRunnableVariableIfValue) {
		/* assign(..., if(..., var, var)) */
		avs_debug(print("vm assembler: Assignment from if return value"));
		vi = AvsInsnAssignFromPtr;
	} else {
		avs_debug(print("vm assembler: Assign %s = %s", arg[0]->name, arg[1]->name));
		vi = AvsInsnAssign;
	}
	
	avs_vm_instruction_add(rd->gd, vi, arg[0]->value, arg[1]->value, arg[2]->value);
}
#endif


ASSEMBLER_EMIT_FUNCTION(emit_call)
{
}

ASSEMBLER_EMIT_FUNCTION(emit_nop)
{
	return;
}

ASSEMBLER_EMIT_FUNCTION(emit_negate)
{

}

static void load_variable(X86RunnableData *rd, X86Variable *xv)
{
	switch (xv->type) {
		case X86VariableTypeMemory:
		case X86VariableTypeEmpty:
			/* Check if enough registers are available */
			x86_emit1(&rd->gd->ctx, flds, offset(xv->rv->value));
			xv->type = X86VariableTypeRegister;
			xv->x.reg = 0;
			break;

		case X86VariableTypeRegister:
			break;
	}	
}

ASSEMBLER_EMIT_FUNCTION(emit_assign)
{
	X86RunnableData *rd = X86_RUNNABLEDATA(obj->pasm);
	X86Variable *arg[3];
	
	arg[1] = retrieve_variable(ctx, obj, &args[0]);
	arg[2] = retrieve_variable(ctx, obj, &args[1]);
	arg[0] = store_variable(ctx, obj, retval);

	if (arg[1]->type != X86VariableTypeRegister &&
	    arg[2]->type != X86VariableTypeRegister) {
		x86_emit2(&rd->gd->ctx, movql, offset(arg[2]->rv->value), mm0);
		x86_emit2(&rd->gd->ctx, movql, mm0, offset(arg[1]->rv->value));
		arg[0]->type = X86VariableTypeMemory;
	}
	
	if (arg[1]->type != X86VariableTypeRegister &&
	    arg[2]->type == X86VariableTypeRegister) {
		x86_emit1(&rd->gd->ctx, fstps, offset(arg[1]->rv->value));
	}
}

ASSEMBLER_EMIT_FUNCTION(emit_add)
{
	X86RunnableData *rd = X86_RUNNABLEDATA(obj->pasm);
	X86Variable *arg[3];
	
	arg[1] = retrieve_variable(ctx, obj, &args[0]);
	arg[2] = retrieve_variable(ctx, obj, &args[1]);
	arg[0] = store_variable(ctx, obj, retval);

	if (arg[1]->type != X86VariableTypeRegister &&
	    arg[2]->type != X86VariableTypeRegister)
		load_variable(rd, arg[1]);

	if (arg[1]->type == X86VariableTypeRegister &&
	    arg[2]->type != X86VariableTypeRegister) {
		x86_emit1(&rd->gd->ctx, fadds, offset(arg[2]->rv->value));
		arg[0]->type = X86VariableTypeRegister;
		arg[0]->x.reg = 0;
	}
}

ASSEMBLER_EMIT_FUNCTION(emit_sub)
{

}

ASSEMBLER_EMIT_FUNCTION(emit_mul)
{

}

ASSEMBLER_EMIT_FUNCTION(emit_div)
{

}

ASSEMBLER_EMIT_FUNCTION(emit_mod)
{

}

ASSEMBLER_EMIT_FUNCTION(emit_and)
{

}

ASSEMBLER_EMIT_FUNCTION(emit_or)
{

}

ASSEMBLER_MARKER(vm_marker)
{
	X86GlobalData *gd = X86_GLOBALDATA(ctx->pdata);
	
	switch (marker) {
		case AvsCompilerMarkerFunction:
			break;

		case AvsCompilerMarkerArgument:
			break;

		case AvsCompilerMarkerSequencePoint:
			break;
			
		case AvsCompilerMarkerRollback:
			break;

		default:
			break;
	}
	return 0;
}

ASSEMBLER_RUNNABLE_INIT(x86_object_init)
{
	avs_debug(print("x86 assembler: Object initialization"));

	/* Allocate assembler data */
	X86RunnableData *rd = visual_mem_malloc0(sizeof(X86RunnableData));
	memset(rd, 0, sizeof(X86RunnableData));
	obj->pasm = rd;

	/* Associate global data with x86 runnable context */
	rd->gd = X86_GLOBALDATA(ctx->pdata);

	/* Reset X86 Opcode context */
	x86_context_reset(&rd->gd->ctx);
	
	/* Setup stack frame */
	x86_emit1(&rd->gd->ctx, pushl, ebp);
	x86_emit2(&rd->gd->ctx, movl, esp, ebp);
	
	return 0;
}

ASSEMBLER_RUNNABLE_FINISH(x86_object_finish)
{
	X86RunnableData *rd = X86_RUNNABLEDATA(obj->pasm);

	/* Finish stack frame */
	x86_emit0(&rd->gd->ctx, emms);
	x86_emit0(&rd->gd->ctx, leave);
	x86_emit0(&rd->gd->ctx, ret);
	
	/* Bind execution handler */
	obj->run = (AvsRunnableExecuteCall) rd->gd->ctx.buf;
	write(1, rd->gd->ctx.buf, rd->gd->ctx.position);
	return 0;
}

ASSEMBLER_RUNNABLE_CLEANUP(x86_object_cleanup)
{
	X86RunnableData *rd = X86_RUNNABLEDATA(obj->pasm);
	
	avs_debug(print("x86 assembler: Object cleanup..."));
	
	/* Free runnable data */
	visual_mem_free(rd);

	return 0;
}

/* Assembler context destructor */
ASSEMBLER_CLEANUP(x86_cleanup)
{
	X86GlobalData *gd = X86_GLOBALDATA(ctx->pdata);
	
	/* Destruct X86 opcode context */
	visual_object_unref(VISUAL_OBJECT(&gd->ctx));

	/* Free stack managers */
	visual_object_unref(VISUAL_OBJECT(&gd->rvmgr));
	visual_object_unref(VISUAL_OBJECT(&gd->xvmgr));

	/* Free global data structure */
	visual_mem_free(gd);

	avs_debug(print("x86 assembler: Cleanup..."));
	return 0;
}

/* Assembler context initializer */
ASSEMBLER_INIT(x86_init)
{
	avs_debug(print("x86 assembler: Initialize..."));
	X86GlobalData *gd = visual_mem_malloc0(sizeof(X86GlobalData));
	memset(gd, 0, sizeof(X86GlobalData));
	ctx->pdata = gd;

	/* Initialize X86 Assembler opcode context */
	x86_context_init(&gd->ctx, 4096, 1024*1024);

	/* Allocate stack managers */
	gd->rvmgr = rvmgr_new();
	gd->xvmgr = xvmgr_new();
	return 0;
}

static AvsAssembler assembler = {
	.name			= "X86 FPU Assembler",
	.init 			= x86_init,
	.cleanup		= x86_cleanup,
	.object_init		= x86_object_init,
	.object_finish		= x86_object_finish,
	.object_cleanup		= x86_object_cleanup,
	.marker			= vm_marker,
	.emit_insn = {
		emit_nop,
		emit_call,
		emit_assign,
		emit_negate,
		emit_add,
		emit_sub,
		emit_mul,
		emit_div,
		emit_mod,
		emit_and,
		emit_or,
	},
};

AvsAssembler * avs_assembler_x86(void)
{
	return &assembler;
}

