#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_x86.h"


static X86ExInfo * xinfo_create(ILInstruction *insn, X86ExInfoType type)
{
	X86ExInfo *xi = malloc(sizeof(X86ExInfo));
	memset(xi, 0, sizeof(X86ExInfo));
	xi->type = type;

	if (insn)
		insn->private = xi;

	return xi;
}

static X86ExLoop * loop_new(X86GlobalData *gd)
{
	X86ExLoop *xl = malloc(sizeof(X86ExLoop));
	memset(xl, 0, sizeof(X86ExLoop));

	xl->prev = gd->loop;
	if (gd->loop)
		gd->loop->next = xl;
	gd->loop = xl;

	return xl;
}

static X86ExLoop * loop_finish(X86GlobalData *gd)
{
	X86ExLoop *xl = gd->loop;

	if (xl)
		gd->loop = xl->prev;

	return xl;
}

/* XXX: Duplicate / Reference */
static AvsNumber * get_operand(ILRegister *reg)
{
	switch (reg->type) {
		case ILRegisterTypeConstant:
			return &reg->value.constant;

		case ILRegisterTypeVariable:
			return reg->value.variable->value;

        default:
            break;
	}
	
	return NULL;
}

static void * get_reference(ILRegister *reg)
{
	void *ref;
	
	if (reg->private)
		return reg->private;
	
	ref = malloc(sizeof(AvsNumber **));
	reg->private = ref;
	return ref;
}

static void check_link_jump(X86GlobalData *gd, ILInstruction *ctx)
{
	X86ExInfo *xi;

	xi = X86_EXINFO(ctx);
	if (!xi)
		return;

	switch (xi->type) {
		case X86ExInfoTypeLinkJump:
			*(unsigned int *) xi->ex.jmp.offset = (int)(x86_next_offset(&gd->ctx) - xi->ex.jmp.next);

		default:
			free(xi);
			ctx->private = NULL;
			break;
	}
}
static void compile_function(X86GlobalData *gd, AvsRunnable *obj, ILInstruction *insn)
{
	AvsBuiltinFunctionType type = avs_builtin_function_type(insn->ex.call.call->name);

	switch(type) {
		case AVS_BUILTIN_FUNCTION_SIN:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit0(&gd->ctx, fsin);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;
			
		case AVS_BUILTIN_FUNCTION_COS:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit0(&gd->ctx, fcos);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

		case AVS_BUILTIN_FUNCTION_SQRT:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit0(&gd->ctx, fsqrt);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

		case AVS_BUILTIN_FUNCTION_ABS:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit0(&gd->ctx, fabs);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;
			
		case AVS_BUILTIN_FUNCTION_TAN:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit0(&gd->ctx, fptan);
			x86_emit0(&gd->ctx, fincstp);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;
			
		case AVS_BUILTIN_FUNCTION_ATAN:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit0(&gd->ctx, fld1);
			x86_emit0(&gd->ctx, fpatan);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

		case AVS_BUILTIN_FUNCTION_ATAN2:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[0])));
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->ex.call.argv[1])));
			x86_emit0(&gd->ctx, fpatan);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

		default: {
			int i;

			x86_emit2(&gd->ctx, subl, imm(insn->ex.call.argc * 4), esp);
			for (i=0; i < insn->ex.call.argc; i++)
				x86_emit2(&gd->ctx, movl, imm(get_operand(insn->ex.call.argv[i])), disp(i * 4, esp));
			
			x86_emit1(&gd->ctx, pushl, imm(insn->ex.call.argc));
			x86_emit2(&gd->ctx, leal, disp(4, esp), eax);
			x86_emit1(&gd->ctx, pushl, eax);
			x86_emit1(&gd->ctx, pushl, imm(get_operand(insn->reg[0])));
			x86_emit1(&gd->ctx, pushl, imm(obj));
			x86_emit1(&gd->ctx, calll, relative(insn->ex.call.call->run));
			x86_emit2(&gd->ctx, addl, imm(4*4 + insn->ex.call.argc * 4), esp);
			break;
		}
	}
}

static int compile_opcode(X86GlobalData *gd, AvsRunnable *obj, ILInstruction *insn)
{
	X86ExInfo *xi;

	check_link_jump(gd, insn);
	switch (insn->type) {
		case ILInstructionNop:
			break;
			
		case ILInstructionCall:
			compile_function(gd, obj, insn);
			break;

		case ILInstructionNegate:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit0(&gd->ctx, fchs);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			break;

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

		case ILInstructionMod:
			/* Real floating point modulus */
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit0(&gd->ctx, frndint);
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit0(&gd->ctx, fprem);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
#if 0
			x86_emit2(&gd->ctx, subl, imm(4), esp);
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fistpl, disp(0, esp));
			x86_emit1(&gd->ctx, filds, disp(0, esp));
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit0(&gd->ctx, fprem1);
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			x86_emit2(&gd->ctx, addl, imm(4), esp);
#endif
			break;
			
		case ILInstructionAnd:
			x86_emit1(&gd->ctx, pushl, ebx);
			x86_emit2(&gd->ctx, subl, imm(8), esp);
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fistpl, disp(4, esp));
			x86_emit1(&gd->ctx, fistpl, disp(0, esp));
			x86_emit2(&gd->ctx, movl, disp(4, esp), eax);
			x86_emit2(&gd->ctx, andl, eax, disp(0, esp));
			x86_emit1(&gd->ctx, filds, disp(0, esp));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			x86_emit2(&gd->ctx, addl, imm(8), esp);
			x86_emit1(&gd->ctx, popl, ebx);
			break;
			
		case ILInstructionOr:
			x86_emit1(&gd->ctx, pushl, ebx);
			x86_emit2(&gd->ctx, subl, imm(8), esp);
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fistpl, disp(4, esp));
			x86_emit1(&gd->ctx, fistpl, disp(0, esp));
			x86_emit2(&gd->ctx, movl, disp(4, esp), eax);
			x86_emit2(&gd->ctx, orl, eax, disp(0, esp));
			x86_emit1(&gd->ctx, filds, disp(0, esp));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			x86_emit2(&gd->ctx, addl, imm(8), esp);
			x86_emit1(&gd->ctx, popl, ebx);
			break;
			
		case ILInstructionLoopInit: {
			X86ExLoop *xl = loop_new(gd);
			x86_emit1(&gd->ctx, pushl, ebx);
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit2(&gd->ctx, subl, imm(4), esp);
			x86_emit1(&gd->ctx, fistpl, disp(0, esp));
			x86_emit1(&gd->ctx, popl, ebx);
			x86_emit2(&gd->ctx, orl, ebx, ebx);
			x86_emit1(&gd->ctx, jel, relative(0));
			xl->rel_init = x86_argument_offset(&gd->ctx, 0);
			xl->jpt_start = x86_next_offset(&gd->ctx);
			break;
		}
					    
		case ILInstructionLoop: {
			X86ExLoop *xl = loop_finish(gd);
			x86_emit1(&gd->ctx, decl, ebx);
			x86_emit1(&gd->ctx, jnel, relative(xl->jpt_start));
			
			/* Encode end of loop offset into zero init */
			*(unsigned int *) xl->rel_init = x86_next_offset(&gd->ctx) - xl->jpt_start;
			
			/* Cleanup stack */
			x86_emit1(&gd->ctx, popl, ebx);
			free(xl);	
			break;
		}

		case ILInstructionJump: 
			x86_emit1(&gd->ctx, jmpl, relative(0));
			xi = xinfo_create(insn->ex.jmp.pointer, X86ExInfoTypeLinkJump);
			xi->ex.jmp.offset = x86_argument_offset(&gd->ctx, 0);
			xi->ex.jmp.next = x86_next_offset(&gd->ctx);
			break;
					
		case ILInstructionJumpTrue:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[0])));
			x86_emit0(&gd->ctx, fld1);
			x86_emit0(&gd->ctx, fucompp);
			x86_emit1(&gd->ctx, fstsw, ax);
			x86_emit0(&gd->ctx, sahf);
			x86_emit1(&gd->ctx, jel, relative(0));
			xi = xinfo_create(insn->ex.jmp.pointer, X86ExInfoTypeLinkJump);
			xi->ex.jmp.offset = x86_argument_offset(&gd->ctx, 0);
			xi->ex.jmp.next = x86_next_offset(&gd->ctx);
			break;

		case ILInstructionMergeMarker:
			break;
			
		case ILInstructionLoadReference:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fstps, offset(get_operand(insn->reg[0])));
			x86_emit2(&gd->ctx, leal, offset(get_operand(insn->reg[1])), eax);
			x86_emit2(&gd->ctx, movl, eax, offset(get_reference(insn->reg[0])));
			break;
			
		case ILInstructionStoreReference:
			x86_emit1(&gd->ctx, flds, offset(get_operand(insn->reg[2])));
			x86_emit1(&gd->ctx, fsts, offset(get_operand(insn->reg[1])));
			x86_emit1(&gd->ctx, fsts, offset(get_operand(insn->reg[0])));	
			x86_emit2(&gd->ctx, movl, offset(get_reference(insn->reg[1])), eax);
			x86_emit1(&gd->ctx, fstps, disp(0, eax));
			break;
			
		default:
			break;
	}

	return 0;
}

static IL_CORE_COMPILE(avs_x86_compiler_compile)
{
	X86GlobalData *gd = X86_GLOBALDATA(ctx);
	ILInstruction *insn;
	
	avs_debug(print("X86: Compiling started..."));
	/* Initialize X86 Assembler opcode context */
	x86_context_init(&gd->ctx);

	/* Compile function entrance, setup stack frame*/
	x86_emit1(&gd->ctx, pushl, ebp);
	x86_emit2(&gd->ctx, movl, esp, ebp);

	/* Setup floating point rounding mode to integer truncation */
	x86_emit2(&gd->ctx, subl, imm(8), esp);
	x86_emit1(&gd->ctx, fstcw, disp(0, esp));
	x86_emit2(&gd->ctx, movl, disp(0, esp), eax);
	x86_emit2(&gd->ctx, orl, imm(0xc00), eax);
	x86_emit2(&gd->ctx, movl, eax, disp(4, esp));
	x86_emit1(&gd->ctx, fldcw, disp(4, esp));

	for (insn=avs_il_tree_base(tree); insn != NULL; insn = insn->next) {
		avs_debug(print("X86: Compiling instruction: %p", insn));
		compile_opcode(gd, obj, insn);
	}

	/* Restore floating point rounding mode */
	x86_emit1(&gd->ctx, fldcw, disp(0, esp));
	x86_emit2(&gd->ctx, addl, imm(8), esp);

	/* Cleanup stack frame */
	x86_emit0(&gd->ctx, emms);
	x86_emit0(&gd->ctx, leave);
	x86_emit0(&gd->ctx, ret);

	/* Link machine */
	obj->run = (AvsRunnableExecuteCall) gd->ctx.buf;
	avs_debug(print("X86: Compiling finished..."));
	avs_debug(print("X86: Function: %p", obj->run));
	return 0;
}

static int avs_x86_compiler_cleanup(ILCoreContext *core)
{
    visual_mem_free(core->ctx);
    return 0;
}

static IL_CORE_INIT(avs_x86_compiler_init)
{
	X86GlobalData *gd = visual_mem_malloc0(sizeof(X86GlobalData));
	memset(gd, 0, sizeof(X86GlobalData));
	ctx->ctx = gd;

	/* Initialize X86 Assembler opcode context */
	x86_context_init(&gd->ctx);
	
	return 0;
}

ILCore il_core_x86 =
{
	.name		=	"X86 Bytecode",
	.init		=	avs_x86_compiler_init,
	.compile	=	avs_x86_compiler_compile,
    .cleanup    =   avs_x86_compiler_cleanup,
};

