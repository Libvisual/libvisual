#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"


extern ILCore il_core_ix;
extern ILCore il_core_x86;

static ILCore * get_core(void)
{
	ILCore *cores[] =
	{
		&il_core_ix,
		&il_core_x86,
		NULL,
	};

	return cores[1];
}



static void context_ctor(ILCoreContext *ctx, ILCore *core)
{
	memset(ctx, 0, sizeof(ILCoreContext));
	ctx->core = core;
}

static ILCoreContext * create_context(ILCore *core)
{
	ILCoreContext *ctx = malloc(sizeof(ILCoreContext));
	context_ctor(ctx, core);
	return ctx;
}

int avs_il_core_compile(ILCoreContext *ctx, AvsILTreeContext *tree, AvsRunnable *obj)
{
	return ctx->core->compile(ctx, tree, obj);
}

int avs_il_core_init(ILCoreContext *ctx)
{
	return ctx->core->init(ctx);
}

int avs_il_core_context_init(ILCoreContext *ctx)
{
	context_ctor(ctx, get_core());
	return VISUAL_OK;
}

int avs_il_core_context_cleanup(ILCoreContext *ctx)
{
    ctx->core->cleanup(ctx);
    return VISUAL_OK;
}

ILCoreContext * avs_il_core_context_create(void)
{
	return create_context(get_core());
}
