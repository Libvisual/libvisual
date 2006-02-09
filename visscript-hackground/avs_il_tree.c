#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"


/**
 * Add a instruction to the IL Tree.
 *
 * @param ctx IL Tree context.
 * @param type Instruction type
 *
 * @return Newly created instruction on success, NULL on failure
 */
void avs_il_tree_add(AvsILTreeContext *ctx, ILInstruction *insn)
{
	if (!ctx->end) {
		ctx->base = insn;
		ctx->end = insn;
	} else {
		insn->prev = ctx->end;
		insn->next = NULL;

		ctx->end->next = insn;
		ctx->end = insn;
	}
}

/**
 * Reset an IL Tree context.
 *
 * @param ctx IL Tree context to reset.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_tree_reset(AvsILTreeContext *ctx)
{
	/* Reset instruction stack */
	avs_stack_reset(ctx->ixstack);
	return VISUAL_OK;
}

/**
 * Cleanup an IL Tree context.
 *
 * @param ctx IL Tree context to cleanup.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_tree_cleanup(AvsILTreeContext *ctx)
{
	/* Cleanup instruction stack */
	visual_object_unref(VISUAL_OBJECT(ctx->ixstack));

	return VISUAL_OK;
}

/* Initialize an IL tree context.
 *
 * @param ctx IL tree context to initialize.
 *
 * @return VISUAL_OK on success, VISUAL_ERROR_GENERAL on error.
 */
int avs_il_tree_init(AvsILTreeContext *ctx)
{
	memset(ctx, 0, sizeof(AvsILTreeContext));

	/* Allocate Instruction Stack */
	ctx->ixstack = avs_stack_new0(ILInstruction, AVS_ILTREE_INITIAL_STACKSIZE,
						     AVS_ILTREE_MAXIMUM_STACKSIZE);

	return VISUAL_OK;
}

