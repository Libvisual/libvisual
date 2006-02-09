#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_vm.h"
#include "avs_x86_assembler.h"

/**
 *	Emit a single compiler instruction to the assembler.
 *
 *	@param ctx Assembler context, previously intialized with avs_assembler_init()
 *	@param insn Instruction to emit
 *	@param retval Compiler argument to save the instruction product into.
 *	@param args Compiler arguments available for instruction.
 *
 *	@see avs_assembler_init
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_assembler_emit_insn(AvsAssemblerContext *ctx,
				   AvsRunnable *obj,
				   AvsInstruction insn,
				   AvsCompilerArgument *retval, 
				   AvsCompilerArgument *args,
				   int count)
{
	ctx->pasm->emit_insn[insn](ctx, obj, insn, retval, args, count);
	return VISUAL_OK;
}

/**
 *	Notify assembler about stack markers (function marker, function argument marker).
 *
 *	@param ctx Assembler context.
 *	@param obj Runnable output object.
 *	@param marker Stack marker.
 *	@param args Stack pointer
 *	@param count Stack item count
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on failure.
 */
int avs_assembler_marker(AvsAssemblerContext *ctx, 
			 AvsRunnable *obj, 
			 AvsCompilerMarkerType marker,
			 char *name)
{
	if (!ctx->pasm->marker)
		return VISUAL_OK;
	
	return ctx->pasm->marker(ctx, obj, marker, name);
}

/**
 *	Cleanup runnable output object
 *
 *	@param obj Runnable object to cleanup
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_assembler_runnable_cleanup(AvsAssemblerContext *ctx, AvsRunnable *obj)
{
	return ctx->pasm->object_cleanup(ctx, obj);
}

/**
 *	Compiling is finished, notify assembler.
 *
 *	@param obj Runnable object to notify.
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_assembler_runnable_finish(AvsAssemblerContext *ctx, AvsRunnable *obj)
{
	return ctx->pasm->object_finish(ctx, obj);
}

/**
 *	Initialize runnable output object
 *
 *	@param obj Runnable object to initialize
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_assembler_runnable_init(AvsAssemblerContext *ctx, AvsRunnable *obj)
{
	return ctx->pasm->object_init(ctx, obj);
}

/**
 * 	Cleanup an assembler context.
 *
 * 	@param ctx Assembler context to cleanup.
 *
 * 	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_assembler_cleanup(AvsAssemblerContext *ctx)
{
	return ctx->pasm->cleanup(ctx);
}

/**
 *	Initialize an assembler context.
 *
 *	@param ctx Assembler context to initialize
 *
 *	@return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_assembler_init(AvsAssemblerContext *ctx)
{
	if (/*x86*/1==0)
		ctx->pasm = avs_assembler_x86();
	else
		ctx->pasm = avs_assembler_vm();

	return ctx->pasm->init(ctx);
}
