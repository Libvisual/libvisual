#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_x86.h"

/* XXX: Duplicate / Reference */
static AvsNumber * get_operand(ILRegister *reg)
{
	switch (reg->type) {
		case ILRegisterTypeConstant:
			return &reg->value.constant;

		case ILRegisterTypeVariable:
			return reg->value.variable->value;
	}
	
	return NULL;
}

static int compile_opcode(X86GlobalData *gd, ILInstruction *insn)
{
	switch (insn->type) {
		case ILInstructionAssign:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fsts, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));	
			break;

		case ILInstructionAdd:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fadds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;
			
		case ILInstructionSub:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fsubs, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

		case ILInstructionMul:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fmuls, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;
			
		case ILInstructionDiv:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fdivs, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

		case ILInstructionAnd:
#if 0
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fistp, eax);
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fistp, ebx);
			x86_emit2(&gd->ctx, and, eax, ebx);
			x86_emit1(&gd->ctx, filds, eax);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
#endif
			break;

		default:
			break;
	}

	return 0;
}

IL_CORE_COMPILE(avs_x86_compiler_compile)
{
	X86GlobalData *gd = X86_GLOBALDATA(ctx);
	ILInstruction *insn;
	
	avs_debug(print("X86: Compiling started..."));
	x86_context_reset(&gd->ctx);

	/* Compile function entrance, setup stack frame*/
	x86_emit1(&gd->ctx, pushl, ebp);
	x86_emit2(&gd->ctx, movl, esp, ebp);

	for (insn=tree->base; insn != NULL; insn = insn->next) {
		avs_debug(print("X86: Compiling instruction: %p", insn));
		compile_opcode(gd, insn);
	}

	/* Cleanup stack frame */
	x86_emit0(&gd->ctx, emms);
	x86_emit0(&gd->ctx, leave);
	x86_emit0(&gd->ctx, ret);

	/* Link machine */
	obj->run = (AvsRunnableExecuteCall) gd->ctx.buf;
	avs_debug(print("X86: Compiling finished..."));
	return 0;
}

IL_CORE_INIT(avs_x86_compiler_init)
{
	X86GlobalData *gd = visual_mem_malloc0(sizeof(X86GlobalData));
	memset(gd, 0, sizeof(X86GlobalData));
	ctx->ctx = gd;

	/* Initialize X86 Assembler opcode context */
	x86_context_init(&gd->ctx, 4096, 1024*1024);
	
	return 0;
}

ILCore il_core_x86 =
{
	.name		=	"X86 Bytecode",
	.init		=	avs_x86_compiler_init,
	.compile	=	avs_x86_compiler_compile,
};

