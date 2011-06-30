#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"

#define check_str(str1, str2) if(strlen(str1) + strlen(str2) >= sizeof(str1)) break;

static inline int dump_stack(AvsCompilerContext *ctx)
{
	AvsCompilerArgument *pa;
	char output[2048];
    char tmp[2048];

	snprintf(output, 2047, "\ncompiler: stackdump: Stack dump\n");
	for (pa=(AvsCompilerArgument *)ctx->stack->base; pa < (AvsCompilerArgument *)ctx->stack->pointer; pa++) {
		snprintf(tmp, 2047, "compiler: stackdump: [%2d] = ", (pa - (AvsCompilerArgument *)ctx->stack->base));
        check_str(output, tmp);
        strcat(output, tmp);
		switch (pa->type) {
			case AvsCompilerArgumentInvalid:
                snprintf(tmp, 2047, "invalid");
                check_str(output, tmp);
                strcat(output, tmp);
				break;

			case AvsCompilerArgumentConstant:
				snprintf(tmp, 2047, "%.2f", pa->value.constant);
                check_str(output, tmp);
                strcat(output, tmp);
				break;

			case AvsCompilerArgumentIdentifier:
				snprintf(tmp, 2047, "%s", pa->value.identifier);
                check_str(output, tmp);
                strcat(output, tmp);
				break;
				
			case AvsCompilerArgumentMarker: {
				char *markers[] = { "invalid", "function", "argument", NULL };
				snprintf(tmp, 2047, "--- %s marker ---", markers[pa->value.marker]);
                check_str(output, tmp);
                strcat(output, tmp);
				break;
			}
							
			case AvsCompilerArgumentPrivate:
                snprintf(tmp, 2047, "private");
                check_str(output, tmp);
                strcat(output, tmp);
				break;

		}
        snprintf(tmp, 2047, "\n");
        check_str(output, tmp);
        strcat(output, tmp);
	}

    avs_debug(print(output));
	return VISUAL_OK;
}

static inline int push_item(AvsCompilerContext *ctx, int count)
{
	if (avs_stack_move(ctx->stack, count) != VISUAL_OK) {
		/* Stack overflow */
		visual_log(VISUAL_LOG_CRITICAL, "compiler: Compiler stack overflow");
		exit(1);
	}

	return VISUAL_OK;
}

static inline int pop_item(AvsCompilerContext *ctx, int count)
{
	if (avs_stack_multipop(ctx->stack, count) == NULL) {
		/* Stack underflow */
		visual_log(VISUAL_LOG_CRITICAL, "compiler: Stack underflow");
		exit(1);
	}

	return VISUAL_OK;
}

#define TOP(x) \
	(&((AvsCompilerArgument *)ctx->stack->pointer)[-(x)])
#define UP(x) \
	(&((AvsCompilerArgument *)ctx->stack->pointer)[(x) - 1])

static inline int push_identifier(AvsCompilerContext *ctx, AvsRunnable *obj, char *identifier)
{
	char *blob;
	visual_size_t length = strlen(identifier) + 1;
	
	/* Duplicate identifier */
	if ((blob = avs_blob_new(&obj->bm, length)) == NULL)
		return VISUAL_ERROR_GENERAL;
	
	strcpy(blob, identifier);
	
	push_item(ctx, 1);
	TOP(1)->type = AvsCompilerArgumentIdentifier;
	TOP(1)->value.identifier = blob;
	return VISUAL_OK;
}

static inline int push_constant(AvsCompilerContext *ctx, AvsRunnable *obj, AvsNumber constant)
{
	push_item(ctx, 1);
	TOP(1)->type = AvsCompilerArgumentConstant;
	TOP(1)->value.constant = constant;
	return VISUAL_OK;
}

static inline int push_marker(AvsCompilerContext *ctx, AvsRunnable *obj, AvsCompilerMarkerType type)
{
	push_item(ctx, 1);
	TOP(1)->type = AvsCompilerArgumentMarker;
	TOP(1)->value.marker = type;
	return VISUAL_OK;
}

static inline int rollback_marker(AvsCompilerContext *ctx, AvsRunnable *obj)
{
	AvsCompilerArgument *sp;

	avs_debug(print("compiler: Rolling back marker!"));
	for (sp=(AvsCompilerArgument *)ctx->stack->pointer - 1; sp >= (AvsCompilerArgument *)ctx->stack->base; sp--) {
		if (sp->type != AvsCompilerArgumentMarker)
			continue;

		return (AvsCompilerArgument *)ctx->stack->pointer - sp;
	}

	return -1;
}

/**
 * Reset stack
 *
 * @param ctx Compiler context
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_compile_reset_stack(AvsCompilerContext *ctx)
{
	avs_stack_reset(ctx->stack);
	return VISUAL_OK;
}

/**
 * Push an identifer onto the compiler stack.
 * Identifier will be duplicated.
 *
 * @param ctx Compiler context
 * @param constant Identifier to push.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_compile_push_identifier(AvsCompilerContext *ctx, AvsRunnable *obj, char *identifier)
{
	return push_identifier(ctx, obj, identifier);
}

/**
 * Push a constant onto the compiler stack.
 *
 * @param ctx Compiler context
 * @param constant Constant to push.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_compile_push_constant(AvsCompilerContext *ctx, AvsRunnable *obj, AvsNumber constant)
{
	return push_constant(ctx, obj, constant);
}

/**
 * Push a stack marker onto the compiler stack.
 *
 * @param ctx Compiler context
 * @param obj Runnable object
 * @param type Marker type
 * @param name Name accompanying marker (only used for function markers, to pass along the function name).
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_compile_marker(AvsCompilerContext *ctx, AvsRunnable *obj, AvsCompilerMarkerType type, char *name)
{
	/* Only Function Markers will be pushed onto the stack,
	 * other markers will be silently discarded.
	 */
	if (type == AvsCompilerMarkerFunction)
		push_marker(ctx, obj, type);

	/* Push name identifier onto the stack, if available */
	if (name)
		push_identifier(ctx, obj, name);

	/* Notify assembler about all markers. */
	avs_il_emit_marker(ctx->assembler, obj, type, name);
	return VISUAL_OK;
}

/**
 * Compile a single arithmetic instruction into an execution tree.
 *
 * @param ctx Compiler context
 * @param insn Instruction to compile.
 *
 * @return VISUAL_OK on success. VISUAL_ERROR_GENERAL on failure.
 */
int avs_compile_arithop(AvsCompilerContext *ctx, AvsRunnable *obj, AvsCompilerInstruction insn)
{
	int marker;
	
	#define CompileArithOp(insn, op, cast) \
		pop_item(ctx, 2); \
		if (UP(2)->type == AvsCompilerArgumentConstant && \
		    UP(1)->type == AvsCompilerArgumentConstant) { \
			push_constant(ctx, obj, (AvsNumber) (cast UP(1)->value.constant op cast UP(2)->value.constant)); \
		} else { \
			avs_il_emit_instruction(ctx->assembler, \
					 obj, \
					 insn, \
					 UP(1), \
					 UP(1), \
					 2); \
			push_item(ctx, 1); \
		} \
		break;
	
	avs_debug(print("compiler: stackdump: Start of compilation... dumping stack"));
	avs_debug(call(dump_stack(ctx)));
	switch (insn) {
		case AvsCompilerInstructionCall:
			if ((marker = rollback_marker(ctx, obj)) == -1) {
				visual_log(VISUAL_LOG_CRITICAL, "compiler: Unable to find stack marker for function call!");
				exit(1);
			}
			
			/* Do builtin function constant elimination */
			avs_debug(print("compiler: Stack marker found at %d buckets up", marker));
			pop_item(ctx, marker);
			avs_il_emit_instruction(ctx->assembler, obj, AvsCompilerInstructionCall, UP(1), UP(2), marker-1);
			push_item(ctx, 1);
			break;
			
		case AvsCompilerInstructionAssign:
			pop_item(ctx, 2);
			avs_debug(print("compiler: Emitting Assign"));
			avs_debug(call(dump_stack(ctx)));
			avs_il_emit_instruction(ctx->assembler,
					 obj,
					 AvsCompilerInstructionAssign,
					 UP(1),
					 UP(1),
					 2);
			push_item(ctx, 1);
			break;
			
		case AvsCompilerInstructionNegate:
			pop_item(ctx, 1);
			if (UP(1)->type == AvsCompilerArgumentConstant)
				push_constant(ctx, obj, -UP(1)->value.constant);
			else {
				avs_il_emit_instruction(ctx->assembler,
						 obj, 
						 AvsCompilerInstructionNegate,
						 UP(1),
						 UP(1), 
						 1);
				push_item(ctx, 1);
			}
			break;

		case AvsCompilerInstructionAdd:
			CompileArithOp(insn, +,);
		case AvsCompilerInstructionSub:
			CompileArithOp(insn, -,);
		case AvsCompilerInstructionMul:
			CompileArithOp(insn, *,);
		case AvsCompilerInstructionDiv:
			pop_item(ctx, 2);
			if (UP(2)->type == AvsCompilerArgumentConstant &&
			    UP(1)->type == AvsCompilerArgumentConstant) { 
				push_constant(ctx, obj, UP(1)->value.constant / UP(2)->value.constant); 
			} else if (UP(2)->type == AvsCompilerArgumentConstant) {
				/* Convert constant divisor divisions into multiply instructions */
				UP(2)->value.constant = 1 / UP(2)->value.constant;
				avs_il_emit_instruction(ctx->assembler,
						 obj,
						 AvsCompilerInstructionMul,
						 UP(1),
						 UP(1),
						 2);
				push_item(ctx, 1);
			} else {
				avs_il_emit_instruction(ctx->assembler,
						 obj,
						 insn,
						 UP(1),
						 UP(1),
						 2);

				push_item(ctx, 1);
			}
			break;
		case AvsCompilerInstructionMod:
			CompileArithOp(insn, %, (int));
		case AvsCompilerInstructionAnd:
			CompileArithOp(insn, &, (int));
		case AvsCompilerInstructionOr:
			CompileArithOp(insn, |, (int));
		default:
			break;
	}
	avs_debug(print("compiler: stackdump: Compilation of instruction done... dumping stack"));
	avs_debug(call(dump_stack(ctx)));

	return VISUAL_OK;
}

/**
 * Cleanup a compiler context.
 *
 * @param cx Compiler context to cleanup.
 */
void avs_compiler_cleanup(AvsCompilerContext *cx)
{
	/* Cleanup compiler stack */
	visual_object_unref(VISUAL_OBJECT(cx->stack));
}


/**
 * Initialize a compiler context.
 *
 * @param cx Compiler context to initialize
 * @param ax Assembler context to associate with compiler context.
 *
 * @return VISUAL_OK on succes, VISUAL_ERROR_GENERAL on failure.
 */
int avs_compiler_init(AvsCompilerContext *cx, AvsILAssemblerContext *ax)
{
	memset(cx, 0, sizeof(AvsCompilerContext));

	/* Associate assembler with compiler */
	cx->assembler = ax;

	/* Allocate compiler stack */
	cx->stack = avs_stack_new0(AvsCompilerArgument, AVS_COMPILER_INITIAL_STACKSIZE, 
							AVS_COMPILER_MAXIMUM_STACKSIZE);

	return VISUAL_OK; 
}
