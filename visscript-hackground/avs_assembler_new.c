#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_vm.h"

AVS_STACK_MANAGER_TEMPLATE_LOCAL(AvsRunnableVariable, rvmgr, 64, 1024*1024)

static inline AvsRunnableVariable * create_result(VmGlobalData *vm)
{
	AvsRunnableVariable *var = rvmgr_push(vm->rvmgr);

	var->name = "#retval#";
	var->local_value = 0.0;
	var->value = &var->local_value;
	var->flags = AvsRunnableVariableAnonymous;
	var->next = vm->result;
	vm->result = var;

	return var;
}

static inline AvsRunnableVariable * create_constant(VmGlobalData *vm, AvsNumber value)
{
	AvsRunnableVariable *var = rvmgr_push(vm->rvmgr);

	var->name = "#anonymous constant#";
	var->local_value = value;
	var->value = &var->local_value;
	var->flags = AvsRunnableVariableConstant | AvsRunnableVariableAnonymous;
	var->next = vm->constant;
	vm->constant = var;

	return var;
}

static inline AvsRunnableVariable * find_constant(VmGlobalData *vm, AvsNumber constant)
{
	AvsRunnableVariable *var;

	for (var=vm->constant; var != NULL; var=var->next) {
		if (*var->value != constant)
			continue;

		return var;
	}

	return NULL;
}

static inline void convert_argument(AvsCompilerArgument *ca, AvsRunnableVariable *var)
{
	ca->type = AvsCompilerArgumentPrivate;
	ca->value.ptr = var;
}

static inline AvsRunnableVariable * retrieve_variable(AvsAssemblerContext *ctx, 
						      AvsRunnable *obj,
						      AvsCompilerArgument *arg)
{
	AvsRunnableVariable *av;
	
	switch (arg->type) {
		case AvsCompilerArgumentConstant:
			if ((av=find_constant(VM_GLOBALDATA(ctx->pdata), arg->value.constant)) == NULL)
				av=create_constant(VM_GLOBALDATA(ctx->pdata), arg->value.constant);
			
			convert_argument(arg, av);
			break;
			
		case AvsCompilerArgumentIdentifier:
			avs_debug(print("vm assembler: Searching for variable: %s", arg->value.identifier));
			if ((av=avs_runnable_variable_find(obj, arg->value.identifier)) == NULL) {
				avs_debug(print("vm assembler: Not found, creating new one"));
				av = avs_runnable_variable_create(obj, arg->value.identifier, 0);
			}
			convert_argument(arg, av);
			break;
			
		case AvsCompilerArgumentPrivate:
			break;

		default:
			return NULL;
	}

	return VM_VARIABLE(arg->value.ptr);
}

static inline AvsNumber * retrieve_data(AvsAssemblerContext *ctx, 
					AvsRunnable *obj, 
					AvsCompilerArgument *arg)
{
	AvsRunnableVariable *av = retrieve_variable(ctx, obj, arg);

	if (av)
		return av->value;
	else
		return NULL;
}

static inline AvsRunnableVariable * store_variable(AvsAssemblerContext *ctx, 
						   AvsRunnable *obj, 
						   AvsCompilerArgument *arg)
{
	AvsRunnableVariable *av;

	av = create_result(VM_GLOBALDATA(ctx->pdata));
	convert_argument(arg, av);
	return VM_VARIABLE(arg->value.ptr);
}

static inline AvsNumber * store_data(AvsAssemblerContext *ctx, AvsRunnable *obj, AvsCompilerArgument *arg)
{
	AvsRunnableVariable *av = store_variable(ctx, obj, arg);

	if (av)
		return av->value;
	else
		return NULL;
}

ASSEMBLER_EMIT_FUNCTION(emit_call_if)
{
	VmRunnableData *rd = VM_RUNNABLEDATA(obj->pasm);
	VmInstructionContext *ix;
	VmInstruction *v, *vx;
	AvsRunnableVariable *arg[4];
	int cxcount;

	cxcount = avs_vm_instruction_context_count(rd->gd);
	avs_debug(print("vm assembler: IF function, dumping argument context (count: %d)", count));
	avs_debug(print("vm assembler: Argument context count: %d, gd->ix %p, gd->ixmarker %p", cxcount,
			rd->gd->ix, rd->gd->ixmarker));

	if (count < 4 || cxcount < 3) {
		avs_debug(print("vm assembler: Need more parameters for '%s' function", "if"));
		exit(1);
	}

	/* Retrieve marker instruction contexts */
	ix = avs_vm_instruction_context_pop(rd->gd);
	
	/* Retrieve stack variables, order IS important */
	arg[1] = retrieve_variable(ctx, obj, &args[1]);
	arg[2] = retrieve_variable(ctx, obj, &args[2]);
	arg[3] = retrieve_variable(ctx, obj, &args[3]);
	arg[0] = store_variable(ctx, obj, retval);

	/* If one of the valtrue or valfalse argument is constant or an anonymous variable
	 * propagate it further. By flagging the IF Return Value as an anonymous variable also. */
	arg[0]->flags &= ~AvsRunnableVariableAnonymous;
	if ((arg[2]->flags & AvsRunnableVariableAnonymous) ||
	    (arg[3]->flags & AvsRunnableVariableAnonymous))
		arg[0]->flags |= AvsRunnableVariableAnonymous;

	/* Emit conditional jump statement */
	avs_vm_instruction_merge(rd->gd, &ix[0]);
	v = avs_vm_instruction_add(rd->gd, 
				AvsInsnCondJump,
				NULL,
				arg[1]->value,
				NULL);

	/* Handle special case of assign(if(..., x, y), ...) */
	if (!ix[1].base && !ix[2].base) {
		/* Make sure the assign instruction knows about this special 
		 * assignment case */
		arg[0]->flags |= AvsRunnableVariableIfValue;

		/* valtrue and valfalse are constants or variables */
		avs_debug(print("vm assembler: BUG() if(...,var,var), base = , base = "));
		v->jump[0] = avs_vm_instruction_add(rd->gd,
						AvsInsnLoadPtr,
						NULL,
						arg[2]->value,
						NULL);

		vx = avs_vm_instruction_add(rd->gd, AvsInsnJump, NULL, NULL, NULL);

		v->jump[1] = avs_vm_instruction_add(rd->gd,
						AvsInsnLoadPtr,
						NULL,
						arg[3]->value,
						NULL);
		
		vx->jump[0] = avs_vm_instruction_add(rd->gd, AvsInsnNop, NULL, NULL, NULL);
		return;
	} 

	avs_debug(print("vm assembler: ix[1] = %p, ix[2] = %p", ix[1].base, ix[2].base));

	/* Handle normal if() statements */
	/* Link cases into condjmp statement */
	v->jump[0] = ix[1].base;
	v->jump[1] = ix[2].base;

	/* Emit valtrue statement */
	avs_vm_instruction_merge(rd->gd, &ix[1]);
	avs_vm_instruction_add(rd->gd, AvsInsnAssign, arg[0]->value, arg[2]->value, arg[2]->value);
	v = avs_vm_instruction_add(rd->gd, AvsInsnJump, NULL, NULL, NULL);

	/* Emit valfalse statement */
	avs_vm_instruction_merge(rd->gd, &ix[2]);
	avs_vm_instruction_add(rd->gd, AvsInsnAssign, arg[0]->value, arg[3]->value, arg[3]->value);

	/* Emit nop instruction, link valtrue statement to this instruction */
	v->jump[0] = avs_vm_instruction_add(rd->gd, AvsInsnNop, NULL, NULL, NULL);
}

ASSEMBLER_EMIT_FUNCTION(emit_call_loop)
{
	VmRunnableData *rd = VM_RUNNABLEDATA(obj->pasm);
	VmInstructionContext *ix;
	VmInstruction *v;
	AvsNumber *arg[3];
	int cxcount;

	cxcount = avs_vm_instruction_context_count(rd->gd);
	avs_debug(print("vm assembler: Loop function, dumping argument context (count: %d)", count));
	avs_debug(print("vm assembler: Argument context count: %d", cxcount));

	if (count < 3 || cxcount < 2) {
		avs_debug(print("vm assembler: Need more parameters for '%s' function", "loop"));
		exit(1);
	}

	/* Retrieve marker instruction contexts */
	ix = avs_vm_instruction_context_pop(rd->gd);
	
	/* Retrieve stack variables, order IS important */
	arg[1] = retrieve_data(ctx, obj, &args[1]);
	arg[2] = retrieve_data(ctx, obj, &args[2]);
	arg[0] = store_data(ctx, obj, retval);
	
	/* Emit LoadCounter instruction  */
	avs_vm_instruction_merge(rd->gd, &ix[0]);
	avs_vm_instruction_add(rd->gd, AvsInsnLoadCounter, arg[0], arg[1], NULL);

	/* Emit loop statement */
	avs_vm_instruction_merge(rd->gd, &ix[1]);

	/* Emit loop instruction */
	v = avs_vm_instruction_add(rd->gd, AvsInsnLoop, arg[0], NULL, NULL);
	v->jump[0] = ix[1].base;
}

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


ASSEMBLER_EMIT_FUNCTION(emit_call)
{
	VmRunnableData *rd = VM_RUNNABLEDATA(obj->pasm);
	VmInstruction *vmi;
	AvsBuiltinFunctionType type;
	AvsRunnableFunction *fn;
	char *name = args[0].value.identifier;
	int i;

	type = avs_builtin_function_type(name);
	switch (type) {
		case AVS_BUILTIN_FUNCTION_ASSIGN:
			emit_assign(ctx, obj, insn, retval, &args[1], count - 1);
			break;

		case AVS_BUILTIN_FUNCTION_EXEC2:
			*retval = args[2];
			break;

		case AVS_BUILTIN_FUNCTION_EXEC3:
			*retval = args[3];	
			break;

		case AVS_BUILTIN_FUNCTION_LOOP:
			emit_call_loop(ctx, obj, insn, retval, args, count);
			break;
			
		case AVS_BUILTIN_FUNCTION_IF:
			emit_call_if(ctx, obj, insn, retval, args, count);
			break;

		default:
			fn = avs_builtin_function_lookup(type);
			if (!fn) {
				avs_debug(print("vm assembler: Unable to find builtin function: %s", name));
			}

			if (fn->param_count && fn->param_count != count - 1) {
				avs_debug(print("vm assembler: Incorrect parameter count for function: %s, needed: %d parameters, received: %d parameters", fn->name, fn->param_count, count - 1));
				exit(1);
			}

			avs_debug(print("vm assembler: Call to function: %s", fn->name));
			/* Note: retrieving the result value before retrieving the function arguments
			 * is normally dangerious, because it'll overwrite the first argument.
			 * but since the first stack value is equal to the function name 
			 * it isn't a problem in this instance. */
			vmi = avs_vm_instruction_add(rd->gd, 
					insn,
					store_data(ctx, obj, retval),
					NULL,
					NULL);

			vmi->fn = fn->run;
			vmi->fncount = count - 1;
			vmi->fnarg = visual_mem_malloc0(sizeof(AvsNumber *) * vmi->fncount);

			for (i=1; i < count; i++)
				vmi->fnarg[i-1] = retrieve_data(ctx, obj, &args[i]);
	}

	avs_debug(print("vm assembler: Adding new instruction..."));
	/* Decrease nesting level */
	rd->gd->nestlevel--;
}

ASSEMBLER_EMIT_FUNCTION(emit_nop)
{
	return;
}

ASSEMBLER_EMIT_FUNCTION(emit_arithop)
{
	AvsNumber *arg[3];
	static char *name[] = { "nop", "call", "assign", "negate", "add", "sub",
			        "mul", "div", "mod", "and", "or", };
	
	avs_debug(print("vm assembler: Arg0 = %d %s Arg1 = %d", args[0].type, name[insn], args[1].type));
	/* Note: order is important! store_data will overwrite the first stack value */
	arg[1] = retrieve_data(ctx, obj, &args[0]);
	arg[2] = retrieve_data(ctx, obj, &args[1]);
	arg[0] = store_data(ctx, obj, retval);

	avs_vm_instruction_add(VM_GLOBALDATA(ctx->pdata), insn, arg[0], arg[1], arg[2]);
	avs_debug(print("vm assembler: Adding new instruction..."));
}

static inline void free_instruction(VmGlobalData *gd, VmInstruction *insn)
{
	/* Free instruction */
	if (insn->fncount)
		visual_mem_free(insn->fnarg);

	/* Free instruction bucket */
	avs_vm_instruction_pop(gd, insn);
}

static inline void free_instruction_tree(VmGlobalData *gd, VmInstruction *insn)
{
	VmInstruction *next;
	
	/* Cleanup instruction tree */
	for (; insn != NULL; insn = next) {
		next = insn->next;
		free_instruction(gd, insn);
	}
}

static void free_context(VmGlobalData *gd, VmInstructionContext *ix)
{
	free_instruction_tree(gd, ix->base);
}

static void rollback(VmGlobalData *gd)
{
	VmInstructionContext *ix;
	int context, count;
	
	/* Destroy all function & argument instruction contexts.
	 * Except main instruction context (nestlevel > 0) */
	/* FIXME: Used result & constant variables won't bee freed until
	 * a object / global cleanup */
	while (gd->ixmarker->nestlevel) {
		count = avs_vm_instruction_context_count(gd);
		ix = avs_vm_instruction_context_pop(gd);

		for (context=0; context < count; context++)
			free_context(gd, ix + context);
	}

	/* Destroy all instructions after sequence point */
	free_instruction_tree(gd, gd->seqpoint->next);
	gd->ixmarker->end = gd->seqpoint;
	gd->seqpoint->next = NULL;
}

ASSEMBLER_MARKER(vm_marker)
{
	VmGlobalData *vm = VM_GLOBALDATA(ctx->pdata);
	
	switch (marker) {
		case AvsCompilerMarkerFunction:
			avs_debug(print("vm assembler: Function marker for function: %s", name));
			vm->nestlevel++;

			switch (avs_builtin_function_type(name)) {
				case AVS_BUILTIN_FUNCTION_IF:
				case AVS_BUILTIN_FUNCTION_LOOP:
					avs_debug(print("vm assembler: Adding function marker nestlevel: %d!", vm->nestlevel));
					vm->loadmarker = 1;
					avs_vm_instruction_context_add(vm);
					avs_vm_instruction_context_mark(vm, vm->nestlevel);
					avs_debug(print("vm assembler: MARK NESTLEVEL: %d", vm->ixmarker->nestlevel));
					break;

				default:
					break;
			}
			break;

		case AvsCompilerMarkerArgument:
			avs_debug(print("vm assembler: Nestlevel: %d, vm->ixmarker->nestlevle: %d",vm->nestlevel, vm->ixmarker->nestlevel));
			if (!vm->nestlevel || vm->ixmarker->nestlevel != vm->nestlevel)
				break;

			/* Skip first marker argument */
			if (vm->loadmarker)
				vm->loadmarker--;
			else
				avs_vm_instruction_context_add(vm);
			break;

		case AvsCompilerMarkerSequencePoint:
			/* Save current instruction context state */
			vm->seqpoint = vm->ixmarker->end;
			avs_debug(print("vm assembler: Sequence point, ix: %p nestlevel: %d marker: %d",
					vm->ix, vm->nestlevel, vm->ixmarker->nestlevel));
			break;
			
		case AvsCompilerMarkerRollback:
			/* Rollback to last saved sequence point */
			rollback(vm);
			break;

		default:
			break;
	}
	return 0;
}

ASSEMBLER_RUNNABLE_INIT(vm_object_init)
{
	avs_debug(print("vm assembler: Object initialization"));

	/* Allocate assembler data */
	VmRunnableData *vm = visual_mem_malloc0(sizeof(VmRunnableData));
	memset(vm, 0, sizeof(VmRunnableData));
	obj->pasm = vm;

	/* Assocate global data with vm runnable context */
	vm->gd = VM_GLOBALDATA(ctx->pdata);

	/* Bind execution handler */
	obj->run = vm_run;

	/* Reset instruction context */
	avs_vm_instruction_reset(vm->gd);

	return 0;
}

ASSEMBLER_RUNNABLE_FINISH(vm_object_finish)
{
	VmRunnableData *vm = VM_RUNNABLEDATA(obj->pasm);
	
	/* Assembling is finished, link instruction tree into runnable context */
	vm->ix.base = vm->gd->ix->base;
	vm->ix.end = vm->gd->ix->end;

	return 0;
}

ASSEMBLER_RUNNABLE_CLEANUP(vm_object_cleanup)
{
	VmRunnableData *rd = VM_RUNNABLEDATA(obj->pasm);
	
	avs_debug(print("vm assembler: Object cleanup..."));

	/* Cleanup instruction tree */
	free_context(rd->gd, &rd->ix);

#if 0
	for (insn=rd->ix.base; insn != NULL; insn = next) {
		next = insn->next;
		
		/* Free instruction */
		if (insn->fncount)
			visual_mem_free(insn->fnarg);

		/* Free instruction bucket */
		avs_vm_instruction_pop(rd->gd, insn);
	}
#endif

	/* Free runnable data */
	visual_mem_free(rd);

	return 0;
}

/* Assembler context destructor */
ASSEMBLER_CLEANUP(vm_cleanup)
{
	VmGlobalData *vm = VM_GLOBALDATA(ctx->pdata);

	/* Cleanup instruction stacks */
	avs_vm_instruction_cleanup(vm);

	/* Free Result Value stack */
	visual_object_unref(VISUAL_OBJECT(vm->rvmgr));

	/* Free global data structure */
	visual_mem_free(vm);

	avs_debug(print("vm assembler: Cleanup..."));
	return 0;
}

/* Assembler context initializer */
ASSEMBLER_INIT(vm_init)
{
	avs_debug(print("vm assembler: Initialize..."));
	VmGlobalData *vm = visual_mem_malloc0(sizeof(VmGlobalData));
	memset(vm, 0, sizeof(VmGlobalData));
	ctx->pdata = vm;

	/* Initialize instruction contexts */
	avs_vm_instruction_init(vm);

	/* Allocate stack managers */
	vm->rvmgr = rvmgr_new();
	return 0;
}

static AvsAssembler assembler = {
	.name			= "Virtual Machine Assembler",
	.init 			= vm_init,
	.cleanup		= vm_cleanup,
	.object_init		= vm_object_init,
	.object_finish		= vm_object_finish,
	.object_cleanup		= vm_object_cleanup,
	.marker			= vm_marker,
	.emit_insn = {
		emit_nop,
		emit_call,
		emit_assign,
		emit_arithop,
		emit_arithop,
		emit_arithop,
		emit_arithop,
		emit_arithop,
		emit_arithop,
		emit_arithop,
		emit_arithop,
	},
};

AvsAssembler * avs_assembler_vm(void)
{
	return &assembler;
}

