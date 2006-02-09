#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"

/**
 *	Emit a single compiler instruction to IL Assembler.
 *
 *	@param ctx IL Assembler context, previously intialized with avs_assembler_init()
 *	@param insn Instruction to emit
 *	@param retval Compiler argument to save the instruction product into.
 *	@param args Compiler arguments available for instruction.
 *
 *	@see avs_assembler_init
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_il_emit_instruction(
			   AvsILAssemblerContext *ctx,
			   AvsRunnable *obj,
			   AvsCompilerInstruction insn,
			   AvsCompilerArgument *retval, 
			   AvsCompilerArgument *args,
			   int count)
{
	
	switch (insn) {
		case AvsCompilerInstructionNop:
			break;
			
		case AvsCompilerInstructionCall:
			break;

		case AvsCompilerInstructionNegate:
			break;
			
		case AvsCompilerInstructionAssign:
		case AvsCompilerInstructionAdd:
		case AvsCompilerInstructionSub:
		case AvsCompilerInstructionMul:
		case AvsCompilerInstructionDiv:
		case AvsCompilerInstructionMod:
		case AvsCompilerInstructionAnd:
		case AvsCompilerInstructionOr: {
			#define _L(x) AvsCompilerInstruction##x
			#define _R(x) ILInstruction##x
			static unsigned char insn2insn[AvsCompilerInstructionCount] = {
				[_L(Assign)]	= _R(Assign),	[_L(Add)] = _R(Add),
				[_L(Sub)]	= _R(Sub),	[_L(Mul)] = _R(Mul),
				[_L(Div)]	= _R(Div),	[_L(Mod)] = _R(Mod),
				[_L(And)]	= _R(And),	[_L(Or)]  = _R(Or),
			};
			#undef _L
			#undef _R
			ILRegister *pds, *lhs, *rhs;
			
			lhs = avs_il_register_load_from_argument(ctx, obj, &args[0]);
			rhs = avs_il_register_load_from_argument(ctx, obj, &args[1]);
			pds = avs_il_register_load_worker(ctx, obj, retval);
			
			avs_debug(print("ilregister: lhs %p rhs %p", lhs, rhs));
			avs_il_tree_add(&ctx->tree, 
					avs_il_instruction_emit_triplet(ctx, obj, insn2insn[insn], 
									pds, lhs, rhs));
			break;
		}
					       
		default:
			break;
	}

	return VISUAL_OK;
}

/**
 *	Notify IL Assembler about stack markers (function markers, function argument markers).
 *
 *	@param ctx IL Assembler context.
 *	@param obj Runnable output object.
 *	@param marker Stack marker.
 *	@param args Stack pointer
 *	@param count Stack item count
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_il_emit_marker(
		 AvsILAssemblerContext *ctx, 
		 AvsRunnable *obj, 
		 AvsCompilerMarkerType marker,
		 char *name)
{
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
	
	return VISUAL_OK;
}

/**
 *	Cleanup runnable output object
 *
 * 	@param ctx IL Assembler context.
 *	@param obj Runnable object to cleanup
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_runnable_cleanup(AvsILAssemblerContext *ctx, AvsRunnable *obj)
{
	return VISUAL_OK;
}

/**
 *	Compiling is finished, notify assembler.
 *
 * 	@param ctx IL Assembler context.
 *	@param obj Runnable object to notify.
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_runnable_finish(AvsILAssemblerContext *ctx, AvsRunnable *obj)
{
	ILInstruction *insn;
	static char *insnname[] = {
		"nop", "load", "loadconstant", "negate", "assign", 
		"add", "sub", "mul", "div", "mod", "and", "or", "count"
	};
	
	avs_print("il: finished! (count: %d)", (ILInstruction*)avs_stack_end(ctx->tree.ixstack) - (ILInstruction*)avs_stack_begin(ctx->tree.ixstack));
	
	for (insn=ctx->tree.base; insn != NULL; insn = insn->next) {
		avs_print("il: instruction %p %s %p, %p, %p", insn, insnname[insn->type], insn->reg[0], insn->reg[1], insn->reg[2]);

	}
	
	/* Level up */
	avs_il_core_compile(ctx->core, &ctx->tree, obj);
	return VISUAL_OK;
}

/**
 *	Initialize runnable output object
 *
 * 	@param ctx IL Assembler Context
 *	@param obj Runnable object to initialize
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_runnable_init(AvsILAssemblerContext *ctx, AvsRunnable *obj)
{
	/* Reset IL Tree context */
	avs_il_tree_reset(&ctx->tree);

	return VISUAL_OK;
}

/**
 * 	Cleanup an IL Assembler context.
 *
 * 	@param ctx IL Assembler context to cleanup.
 *
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_cleanup(AvsILAssemblerContext *ctx)
{
	/* Cleanup IL tree */
	avs_il_tree_cleanup(&ctx->tree);

	return VISUAL_OK;
}

/**
 *	Initialize an IL assembler context.
 *
 *	@param ctx IL Assembler context to initialize
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_init(AvsILAssemblerContext *ctx, ILCoreContext *core)
{
	memset(ctx, 0, sizeof(AvsILAssemblerContext));
	ctx->core = core;
	avs_il_core_init(core);
	return avs_il_tree_init(&ctx->tree);
}
