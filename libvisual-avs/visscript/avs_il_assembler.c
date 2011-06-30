#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"

static int emit_assign(
		AvsILAssemblerContext *ctx,
		AvsRunnable *obj,
		AvsCompilerInstruction insn,
		AvsCompilerArgument *retval, 
		AvsCompilerArgument *args,
		int count)
{
	ILRegister *reg[3];
	ILInstruction *is;
	
	reg[1] = avs_il_register_load_from_argument(ctx, obj, &args[0]);
	reg[2] = avs_il_register_load_from_argument(ctx, obj, &args[1]);
	reg[0] = avs_il_register_load_worker(ctx, obj, retval);

	if (reg[1]->flags & ILRegisterPointer)
		is = avs_il_instruction_emit_triplet(ctx, obj, ILInstructionStoreReference, reg[0], reg[1], reg[2]);
	else
		is = avs_il_instruction_emit_triplet(ctx, obj, ILInstructionAssign, reg[0], reg[1], reg[2]);
	
	avs_il_tree_add(&ctx->tree, is);
	return VISUAL_OK;
}

static int emit_loop(
		AvsILAssemblerContext *ctx,
		AvsRunnable *obj,
		AvsCompilerInstruction insn,
		AvsCompilerArgument *retval, 
		AvsCompilerArgument *args,
		int count)
{
	AvsILTreeNodeIterator iter;
	AvsILTreeNode *base;
	ILRegister *pds, *lhs, *rhs;
	ILInstruction *is, *iloop, *mm;
	int argc;
	
	
	argc = avs_il_tree_node_level_count(&ctx->tree);
	avs_debug(print("il: Loop function, dumping argument context (count: %d)", argc));

	if(argc < 2) {
		avs_debug(print("il: Need more parameters for 'loop' function"));
		exit(1);
	}

	/* Level down */
	avs_il_tree_node_level_down(&ctx->tree, &iter);
	
	/* Retrieve stack variables, order IS important */
	lhs = avs_il_register_load_from_argument(ctx, obj, &args[1]);
	rhs = avs_il_register_load_from_argument(ctx, obj, &args[2]);
	pds = avs_il_register_load_worker(ctx, obj, retval);
			
	/* Load counter */
	avs_il_tree_merge(&ctx->tree, avs_il_tree_node_iterator_next(&iter));
	iloop = avs_il_instruction_emit_twin(ctx, obj, ILInstructionLoopInit, pds, lhs);
	avs_il_tree_add(&ctx->tree, iloop);
	
	/* Merge loop statement */
	base = avs_il_tree_node_iterator_next(&iter);
	if (!base) {
		fprintf(stderr, "No loop statement to execute!\n");
		exit(1);
	}
	avs_il_tree_merge(&ctx->tree, base);

	/* Emit loop instruction */
	is = avs_il_instruction_emit(ctx, obj, ILInstructionLoop);
	is->ex.jmp.pointer = base->insn.base;
	avs_il_tree_add(&ctx->tree, is);
	
	/* Emit merge marker */
	mm = avs_il_instruction_emit(ctx, obj, ILInstructionMergeMarker);
	mm->ex.merge.type = AvsILInstructionMergeTypeLoop;
	mm->ex.merge.from[0] = iloop;
	mm->ex.merge.from[1] = base->insn.base;
	mm->ex.merge.from[2] = is;
	avs_il_tree_add(&ctx->tree, mm);

	return VISUAL_OK;
}

static int emit_if(
		AvsILAssemblerContext *ctx,
		AvsRunnable *obj,
		AvsCompilerInstruction insn,
		AvsCompilerArgument *retval, 
		AvsCompilerArgument *args,
		int count)
{
	AvsILTreeNodeIterator iter;
	AvsILTreeNode *base[3];
	ILRegister *reg[4];
	ILInstruction *jpt0, *jpt1, *mm;
	int argc;

	argc = avs_il_tree_node_level_count(&ctx->tree);
	avs_debug(print("il: If function, dumpring argument context (count: %d)", argc));

	if (argc < 3) {
		avs_debug(print("il: Need more parameters for 'if' function"));
		exit(1);
	}

	/* Level down */
	avs_il_tree_node_level_down(&ctx->tree, &iter);

	/* Retrieve stack variables, order IS important */
	reg[1] = avs_il_register_load_from_argument(ctx, obj, &args[1]);
	reg[2] = avs_il_register_load_from_argument(ctx, obj, &args[2]);
	reg[3] = avs_il_register_load_from_argument(ctx, obj, &args[3]);
	reg[0] = avs_il_register_load_worker(ctx, obj, retval);

	/* Load instruction contexts */
	base[0] = avs_il_tree_node_iterator_next(&iter);
	base[1] = avs_il_tree_node_iterator_next(&iter);
	base[2] = avs_il_tree_node_iterator_next(&iter);
	
	/* Merge condition statement */
	avs_il_tree_merge(&ctx->tree, base[0]);

	/* Jump to valtrue or valfalse statement */
	jpt0 = avs_il_instruction_emit_single(ctx, obj, ILInstructionJumpTrue, reg[1]);
	avs_il_tree_add(&ctx->tree, jpt0);

	/* Handle special case of assign(if(..., x, y), ...) */
	if (!base[1]->insn.base && !base[2]->insn.base) {
		reg[0]->flags |= ILRegisterPointer;

		/* Emit valfalse statement */
		avs_il_tree_add(&ctx->tree, avs_il_instruction_emit_twin(ctx, obj, ILInstructionLoadReference, reg[0], reg[3]));
		jpt1 = avs_il_instruction_emit(ctx, obj, ILInstructionJump);
		avs_il_tree_add(&ctx->tree, jpt1);
		
		/* Emit valtrue statement */
		avs_il_tree_add(&ctx->tree, avs_il_instruction_emit_twin(ctx, obj, ILInstructionLoadReference, reg[0], reg[2]));
		jpt0->ex.jmp.pointer = ctx->tree.current->insn.end;

		/* Emit merge marker */
		mm = avs_il_instruction_emit(ctx, obj, ILInstructionMergeMarker);
		avs_il_tree_add(&ctx->tree, mm);
		jpt1->ex.jmp.pointer = mm;
		mm->ex.merge.type = AvsILInstructionMergeTypeJumpConditional;
		mm->ex.merge.from[0] = jpt1;
		return VISUAL_OK;
	}
		
	/* Emit valfalse statement */
	avs_il_tree_merge(&ctx->tree, base[2]);
	avs_il_tree_add(&ctx->tree, avs_il_instruction_emit_triplet(ctx, obj, ILInstructionAssign, reg[0], reg[3], reg[3]));
	jpt1 = avs_il_instruction_emit(ctx, obj, ILInstructionJump);
	avs_il_tree_add(&ctx->tree, jpt1);

	/* Emit valtrue statement */
	avs_il_tree_merge(&ctx->tree, base[1]);
	avs_il_tree_add(&ctx->tree, avs_il_instruction_emit_triplet(ctx, obj, ILInstructionAssign, reg[0], reg[2], reg[2]));
	jpt0->ex.jmp.pointer = base[1]->insn.base;

	/* Emit merge marker */
	mm = avs_il_instruction_emit(ctx, obj, ILInstructionMergeMarker);
	avs_il_tree_add(&ctx->tree, mm);
	jpt1->ex.jmp.pointer = mm;
	mm->ex.merge.type = AvsILInstructionMergeTypeJumpConditional;
	mm->ex.merge.from[0] = jpt1;

	return VISUAL_OK;
}

static void emit_trampoline(
        AvsILAssemblerContext *ctx,
        AvsRunnable *obj,
        AvsCompilerArgument *retval,
        AvsCompilerArgument *args,
        int count,
        AvsRunnableFunction *fn)
{
    ILInstruction *insn;
    ILRegister *reg;
    int i;

    /* Note: retrieving the result value before retrieving the function arguments
     * is normally dangerous, because it'll overwrite the first argument.
     * but since the first stack value is equal to the function name 
     * it isn't a problem in this instance. */
    reg = avs_il_register_load_worker(ctx, obj, retval);
    insn = avs_il_instruction_emit_single(ctx, obj, ILInstructionCall, reg);

    insn->ex.call.call = fn;
    insn->ex.call.argc = count - 1;
    insn->ex.call.argv = malloc(sizeof(ILRegister **) * insn->ex.call.argc);

    /* Load function arguments */
    for (i=1; i < count; i++)
        insn->ex.call.argv[i-1] = avs_il_register_load_from_argument(ctx, obj, &args[i]);

    avs_il_tree_add(&ctx->tree, insn);
    return;
}

static int emit_call(
		AvsILAssemblerContext *ctx,
		AvsRunnable *obj,
		AvsCompilerInstruction insn,
		AvsCompilerArgument *retval, 
		AvsCompilerArgument *args,
		int count)
{
	AvsBuiltinFunctionType type;
	char *name;

	/* Retrieve function name from compiler argument stack */
	name = args[0].value.identifier;
	type = avs_builtin_function_type(name);

	switch (type) {
		case AVS_BUILTIN_FUNCTION_ASSIGN:
			emit_assign(ctx, obj, insn, retval, args + 1, count - 1);
			break;

		case AVS_BUILTIN_FUNCTION_EXEC2:
			*retval = args[2];
			break;
			
		case AVS_BUILTIN_FUNCTION_EXEC3:
			*retval = args[3];
			break;
			
		case AVS_BUILTIN_FUNCTION_LOOP:
			emit_loop(ctx, obj, insn, retval, args, count);
			break;
			
		case AVS_BUILTIN_FUNCTION_IF:
			emit_if(ctx, obj, insn, retval, args, count);
			break;

		default: {
            AvsRunnableFunction *fn = avs_builtin_function_lookup(type);
			if (!fn) 
				avs_debug(print("il: Unable to find builtin function: %s", name));

			if (fn->param_count && fn->param_count != count - 1) {
				avs_debug(print("il: Incorrect parameter count for function: %s, needed: %d parameters, received: %d parameters",
							fn->name, fn->param_count, count - 1));
				exit(1);
			}
		
			avs_debug(print("il: Call to function: %s", fn->name));
			emit_trampoline(ctx, obj, retval, args, count, fn);
		}	
	}

	/* Descrese nesting level */
	ctx->nestlevel--;

	return VISUAL_OK;
}

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
			emit_call(ctx, obj, insn, retval, args, count);
			break;

		case AvsCompilerInstructionNegate: {
			ILRegister *pds, *rhs;

			rhs = avs_il_register_load_from_argument(ctx, obj, &args[0]);
			pds = avs_il_register_load_worker(ctx, obj, retval);

			avs_il_tree_add(&ctx->tree, 
					avs_il_instruction_emit_twin(ctx, obj, ILInstructionNegate,
								     pds, rhs));
			break;
		}
						   
		case AvsCompilerInstructionAssign:
			emit_assign(ctx, obj, insn, retval, args, count);
			break;
			
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
			/* Function marker: notifies start of function (and thus function argumentation) */
			avs_debug(print("il: Function marker for function: %s", name));
			ctx->nestlevel++;
			
			switch (avs_builtin_function_type(name)) {
				case AVS_BUILTIN_FUNCTION_IF:
				case AVS_BUILTIN_FUNCTION_LOOP:
					avs_debug(print("il: Function marker: leveling-up!"));
					avs_il_tree_node_level_up(&ctx->tree, AvsILTreeNodeTypeFunction);
					avs_il_tree_node_level_mark(&ctx->tree, ctx->nestlevel);
					break;
					
				default:
					break;
			}
			break;

		case AvsCompilerMarkerArgument:
			avs_debug(print("il: Argument marker: nestlevel = %d, level depth = %d, mark = %d",
					ctx->nestlevel, avs_il_tree_node_level_depth(&ctx->tree),
					avs_il_tree_node_level_get_mark(&ctx->tree)));
			if (!ctx->nestlevel || ctx->nestlevel != avs_il_tree_node_level_get_mark(&ctx->tree))
				break;
			
			avs_debug(print("il: Argument marker!"));
			avs_il_tree_node_up(&ctx->tree, AvsILTreeNodeTypeFunctionArgument);
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
/*
    AvsILTreeNode *node = NULL;

    for(node = ctx->tree.base->base; node != ctx->tree.base->end; node = node->next)
    {
        visual_mem_free(node);
    }

    if(node)
        visual_mem_free(node);
*/
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
		"nop", "call", "negate", "assign", 
		"add", "sub", "mul", "div", "mod", "and", "or", 
		"loopinit", "loop", "jump", "jumptrue", "mergemarker",
		"loadreference", "storereference", "count"
	};
	
	avs_print("il: finished! (count: %d)", (ILInstruction*)avs_stack_end(ctx->tree.ixstack) - (ILInstruction*)avs_stack_begin(ctx->tree.ixstack));
	
	for (insn=avs_il_tree_base(&ctx->tree); insn != NULL; insn = insn->next) {
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
