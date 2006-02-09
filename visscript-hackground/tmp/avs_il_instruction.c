#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_vm.h"

/**
 * 	Pop all instruction contexts until a marker context is found.
 *
 * 	@param gd Global Data context
 *
 * 	@return Pointer to the top most marker context on success, NULL on failure.
 */
VmInstructionContext * avs_vm_instruction_context_pop(VmGlobalData *gd)
{
	VmInstructionContext *cx;
	
	if (!gd->ixmarker->next)
		return NULL;
	
	/* Pop all items above the marker from the stack.
	 * Care should be taken that those items won't be used anymore
	 * after an instruction add. */
	avs_stack_multipop(gd->ixstack, avs_vm_instruction_context_count(gd));

	/* Pop maker */
	cx = gd->ixmarker;
	gd->ixmarker = gd->ixmarker->next;
	gd->ix = cx - 1;
	avs_debug(print("vm instruction: context POP gd->ix %p, cx %p", gd->ix, cx));
	return cx;
}

/**
 *	Mark instruction context.
 *
 *	@param Global Data context
 *	@param nestlevel Nesting level of marked instruction context
 *
 *	@returns VISUAL_OK on sucess, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_context_mark(VmGlobalData *gd, int nestlevel)
{
#if 0
	/* Check if Instruction Context is already marked.
	 * Otherwise an infinite loop could ensue. */
	if (gd->ix == gd->ixmarker)
		return VISUAL_ERROR_GENERAL;
#endif

	gd->ix->next = gd->ixmarker;
	gd->ixmarker = gd->ix;
	gd->ixmarker->nestlevel = nestlevel;

	return VISUAL_OK;
}

/**
 *	Add/create a new instruction context, on top of the previous one.
 *
 *	@param gd Global Data context
 *
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_context_add(VmGlobalData *gd)
{
	gd->ix = avs_stack_push(gd->ixstack);
	return (gd->ix ? VISUAL_OK : VISUAL_ERROR_GENERAL);	
}

/**
 *	Merge an instruction context with the active instruction context.
 *	
 *	@param gd Global Data context.
 *	@param ctx Instruction context to merge
 *
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_merge(VmGlobalData *gd, VmInstructionContext *ctx)
{
	VmInstructionContext *gdx = gd->ix;
	
	if (!ctx->base)
		return VISUAL_ERROR_GENERAL;
			
	if (!gdx->end) {
		avs_debug(print("vm instruction: Merging tree %p with %p", gdx, ctx->base));
		gdx->base = ctx->base;
		gdx->end = ctx->end;
	} else {
		gdx->end->next = ctx->base;
		gdx->end = ctx->end;
	}

	return VISUAL_OK;
}

/**
 *	Link an instruction to the proper instruction context.
 *
 *	@param gd Global Data context.
 *	@param insn Instruction to link.
 *
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_link(VmGlobalData *gd, VmInstruction *insn)
{
	VmInstructionContext *gdx = gd->ix;
	
	if (!insn)
		return VISUAL_ERROR_GENERAL;

	if (!gdx->end) {
		avs_debug(print("vm instruction: Assigning new vm instruction %p", gdx));
		gdx->base = gdx->end = insn;
	} else {
		gdx->end->next = insn;
		gdx->end = insn;
	}

	return VISUAL_OK;
}


/**
 *	Destroy a instruction and free associated memory.
 *	
 *	@param vm Global data context.
 *	@param insn Instruction to destroy.
 *
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_pop(VmGlobalData *vm, VmInstruction *insn)
{
	return avs_blob_pool_pop(vm->vmpool, insn);
}

/**
 *	Create a new instruction.
 *	
 *	@param vm Global Data context.
 *	@param insn Instruction type to create.
 *	@param retval Return value for instruction.
 *	@param arg0 First argument passed to instruction.
 *	@param arg1 Second argument passed to instruction.
 *
 *	@returns Newly created instruction on success, NULL on failure.
 *	
 */
VmInstruction * avs_vm_instruction_new(VmGlobalData *vm, 
				       AvsInstruction insn,
				       AvsNumber *retval, 
				       AvsNumber *arg0, 
				       AvsNumber *arg1)
{
	VmInstruction *v = avs_blob_pool_push(vm->vmpool);

	v->run = vm_instruction_table[insn];
	v->result = retval;
	v->arg[0] = arg0;
	v->arg[1] = arg1;

	return v;
}

/**
 *	Create and add a single instruction to the proper instruction context.
 *
 *	@param gd Global Data context.
 *	@param insn Instruction to add.
 *	@param retval Return value for instruction.
 *	@param arg0 First argument passed to instruction.
 *	@param arg1 Second argument passed to instruction.
 *
 *	@returns Newly created instruction on success, NULL on failure.
 */
VmInstruction * avs_vm_instruction_add(VmGlobalData *gd, 
				       AvsInstruction insn,
			               AvsNumber *retval,
				       AvsNumber *arg0, 
				       AvsNumber *arg1)
{ 
	VmInstruction *v;

	/* Intercept assignments, assign(if(..., var, var),...) fixup cases */
#if 0
	if (gd->loadptr) {
		if (insn == AvsInsnAssign)
			insn = AvsInsnAssignPtr;
		gd->loadptr = 0;
	}
#endif

	/* Remove nops when possible */
	if (gd->ix->end && gd->ix->end->run == vm_instruction_table[AvsInsnNop])
		v = gd->ix->end;
	else {
		v = avs_blob_pool_push(gd->vmpool);
		avs_vm_instruction_link(gd, v);
	}

	v->run = vm_instruction_table[insn];
	v->result = retval;
	v->arg[0] = arg0;
	v->arg[1] = arg1;

	return v;
}

/**
 *	Reset instruction unit.
 *	This should be called before compilation of all runnable objects
 *	associated with this context.
 *
 *	@param gd Global Data context.
 *
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_reset(VmGlobalData *gd)
{
	avs_stack_reset(gd->ixstack);
	gd->ix = avs_stack_push(gd->ixstack);
	gd->ixmarker = gd->ix;
	return VISUAL_OK;
}

/**
 *	Cleanup instruction unit
 *
 *	@param gd Global Data context.
 *
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_cleanup(VmGlobalData *gd)
{
	avs_debug(print("vm instruction: Cleanup instruction stack and vmpool"));
	visual_object_unref(VISUAL_OBJECT(gd->ixstack));
	visual_object_unref(VISUAL_OBJECT(gd->vmpool));
	return VISUAL_OK;
}

/**
 *	Initialize instruction unit
 *
 * 	@param gd Global Data context.
 * 	
 *	@returns VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_vm_instruction_init(VmGlobalData *gd)
{
	/* Allocate Instruction Context stack and VM Instruction pool manager */
	gd->ixstack = avs_stack_new0(VmInstructionContext, 32, 4096);
	gd->vmpool = avs_blob_pool_new0(VmInstruction);

	return VISUAL_OK;
}

