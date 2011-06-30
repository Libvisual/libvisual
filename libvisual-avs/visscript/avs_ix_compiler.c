#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_ix.h"

static IXExInfo *xinfo_create(ILInstruction *insn, IXExInfoType type)
{
	IXExInfo *xi = malloc(sizeof(IXExInfo));
	memset(xi, 0, sizeof(IXExInfo));
	xi->type = type;

	if (insn)
		insn->private = xi;

	return xi;
}

static void loop_done(IXGlobalData *gd)
{
	if (!gd->loop.end)
		return;

	gd->loop.end = gd->loop.end->prev;
	if (!gd->loop.end)
		gd->loop.base = NULL;
}

static IXExLoop * loop_get(IXGlobalData *gd)
{
	return gd->loop.end;
}

static IXExLoop * loop_new(IXGlobalData *gd)
{
	IXExLoop *l = malloc(sizeof(IXExLoop));
	memset(l, 0, sizeof(IXExLoop));
	
	if (!gd->loop.end) {
		gd->loop.base = l;
		gd->loop.end = l;
	} else {
		l->prev = gd->loop.end;
		gd->loop.end->next = l;
		gd->loop.end = l;
	}
	
	return l;
}

static IXOpcode * opcode_new(IXOpcodeType type)
{
	IXOpcode *op = malloc(sizeof(IXOpcode));
	memset(op, 0, sizeof(IXOpcode));
	op->opcode = type;
	
	return op;
}

static void bottom_halve(IXGlobalData *gd, IXOpcode *op)
{
	IXExLoop *lx = loop_get(gd);

	if (!lx || !lx->finished)
		return;

	lx->jpt0->ex.jmp.dest = op;
	loop_done(gd);
}

static void bottom_halve_context(IXGlobalData *gd, ILInstruction *ctx, IXOpcode *op)
{
	IXExInfo *xi;

	for (xi=gd->bh.queued; xi != NULL; xi = xi->prev) {
		switch (xi->type) {
			case IXExInfoTypeLinkJumpNext:
				xi->ex.jmp.from->ex.jmp.dest = op;

			default:
				free(xi);
		}
	}
	
	gd->bh.queued = NULL;

	xi = IX_EXINFO(ctx);
	if (!xi)
		return;

	switch (xi->type) {
		case IXExInfoTypeLinkJump:
			xi->ex.jmp.from->ex.jmp.dest = op;

		default: /* fallthrough */
			free(xi);
			ctx->private = NULL;
			break;
	}
}

static void check_link_jump(IXGlobalData *gd, ILInstruction *ctx)
{
	IXExInfo *xi;
	xi = IX_EXINFO(ctx);
	
	if (!xi || xi->type != IXExInfoTypeLinkJumpNext)
		return;

	ctx->private = NULL;
	xi->prev = gd->bh.queued;
	gd->bh.queued = xi;
}

static IXOpcode * opcode_add(ILInstruction *ctx, IXGlobalData *gd, IXRunnableData *rd, IXOpcodeType type)
{
	IXOpcode *op = opcode_new(type);

	if (!rd->end) {
		rd->base = op;
		rd->end = op;
	} else {
		op->prev = rd->end;
		op->next = NULL;

		rd->end->next = op;
		rd->end = op;
	}

	bottom_halve(gd, op);

	if (ctx)
		bottom_halve_context(gd, ctx, op);
	return op;
}

static AvsNumber * get_number(AvsNumber defvalue)
{
	AvsNumber * num = malloc(sizeof(AvsNumber));
	*num = defvalue;
	return num;
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

static IXRegisterReference * get_reference(ILRegister *reg)
{
	IXRegisterReference *ref;

	/* Register already referenced ? */
	if (reg->private)
		ref = (IXRegisterReference *) reg->private;
	else {
		ref = malloc(sizeof(IXRegisterReference));
		ref->ref = NULL;
		reg->private = ref;
	}
	
	return ref;
}

static int compile_opcode(IXGlobalData *gd, IXRunnableData *rd, ILInstruction *insn)
{
	IXOpcode *op;
	IXExInfo *xi;
	int i;
	

	check_link_jump(gd, insn);
	switch (insn->type) {
		case ILInstructionNegate:
			op = opcode_add(insn, gd, rd, IXOpcodeNegate);
			op->reg[0] = get_operand(insn->reg[0]);
			op->reg[1] = get_operand(insn->reg[1]);
			break;

		case ILInstructionCall: 
			op = opcode_add(insn, gd, rd, IXOpcodeCall);
			op->reg[0] = get_operand(insn->reg[0]);
			op->ex.call.call = insn->ex.call.call;
			op->ex.call.argc = insn->ex.call.argc;
			op->ex.call.argv = malloc(sizeof(AvsNumber **) * insn->ex.call.argc);
			for (i=0; i < insn->ex.call.argc; i++) 
				op->ex.call.argv[i] = get_operand(insn->ex.call.argv[i]);
			break;
			
		case ILInstructionAssign:
		case ILInstructionAdd:
		case ILInstructionSub:
		case ILInstructionMul:
		case ILInstructionDiv:
		case ILInstructionMod:
		case ILInstructionAnd:
		case ILInstructionOr: {
			#define _L(x) ILInstruction##x
			#define _R(x) IXOpcode##x
			static unsigned char insn2insn[ILInstructionCount] = {
				[_L(Assign)]	= _R(Assign),	[_L(Add)] = _R(Add),
				[_L(Sub)]	= _R(Sub),	[_L(Mul)] = _R(Mul),
				[_L(Div)]	= _R(Div),	[_L(Mod)] = _R(Mod),
				[_L(And)]	= _R(And),	[_L(Or)]  = _R(Or),
			};
			#undef _L
			#undef _R
			
			op = opcode_add(insn, gd, rd, insn2insn[insn->type]);
			op->reg[0] = get_operand(insn->reg[0]);
			op->reg[1] = get_operand(insn->reg[1]);
			op->reg[2] = get_operand(insn->reg[2]);
			break;
		}
		
		case ILInstructionLoopInit: {
			AvsNumber *counter = get_operand(insn->reg[1]);
			IXExLoop *l;
			
			op = opcode_add(insn, gd, rd, IXOpcodeCmp);
			op->reg[0] = counter;
			op->reg[1] = get_number(0);
			
			op = opcode_add(insn, gd, rd, IXOpcodeJmpZ);
			op->ex.jmp.dest = NULL;

			l = loop_new(gd);
			l->counter = counter;
			l->jpt0 = op;
			break;
		}
					    
		case ILInstructionLoop: {
			IXExLoop *l = loop_get(gd);

			op = opcode_add(insn, gd, rd, IXOpcodeSub);
			op->reg[0] = l->counter;
			op->reg[1] = l->counter;
			op->reg[2] = get_number(1);

			op = opcode_add(insn, gd, rd, IXOpcodeCmp);
			op->reg[0] = l->counter;
			op->reg[1] = get_number(0);

			op = opcode_add(insn, gd, rd, IXOpcodeJmpNz);
			op->ex.jmp.dest = l->jpt0->next;

			l->jpt1 = op;
			l->finished = 1;
			break;
		}
					
		case ILInstructionJump: {
			IXExInfoType type;
			
			op = opcode_add(insn, gd, rd, IXOpcodeJmp);

			if (insn->ex.jmp.pointer->type == ILInstructionMergeMarker)
				type = IXExInfoTypeLinkJumpNext;
			else
				type = IXExInfoTypeLinkJump;
			
			xi = xinfo_create(insn->ex.jmp.pointer, type);
			xi->ex.jmp.from = op;
			break;
		}
					
		case ILInstructionJumpTrue:
			op = opcode_add(insn, gd, rd, IXOpcodeCmp);
			op->reg[0] = get_operand(insn->reg[0]);
			op->reg[1] = get_number(1.0);

			op = opcode_add(insn, gd, rd, IXOpcodeJmpZ);
			xi = xinfo_create(insn->ex.jmp.pointer, IXExInfoTypeLinkJump);
			xi->ex.jmp.from = op;
			break;
			
		case ILInstructionMergeMarker:
			break;

		case ILInstructionLoadReference:
			op = opcode_add(insn, gd, rd, IXOpcodeLoadRef);
			op->reg[0] = get_operand(insn->reg[0]);
			op->reg[1] = get_operand(insn->reg[1]);
			op->ex.ref = get_reference(insn->reg[0]);
			break;

		case ILInstructionStoreReference:
			op = opcode_add(insn, gd, rd, IXOpcodeStoreRef);
			op->reg[0] = get_operand(insn->reg[0]);
			op->reg[1] = get_operand(insn->reg[1]);
			op->reg[2] = get_operand(insn->reg[2]);
			op->ex.ref = get_reference(insn->reg[1]);
			break;
			
		default:
			break;
	}

	return 0;
}

static IL_CORE_COMPILE(avs_ix_compiler_compile)
{
	IXGlobalData *gd;
	IXRunnableData *rd;
	ILInstruction *insn;
	
	gd = IX_GLOBAL_DATA(ctx);
	avs_debug(print("IX: Compiling started..."));
		
	rd = malloc(sizeof(IXRunnableData));
	memset(rd, 0, sizeof(IXRunnableData));
	obj->pcore = rd;
	
	for (insn=avs_il_tree_base(tree); insn != NULL; insn = insn->next) {
		avs_debug(print("IX: Compiling instruction: %p", insn));
		compile_opcode(gd, rd, insn);
	}

	/* Link machine */
	obj->run = avs_ix_machine_run;

	avs_debug(print("IX: Compiling finished..."));
	return 0;
}

static IL_CORE_INIT(avs_ix_compiler_init)
{
	IXGlobalData *gd = malloc(sizeof(IXGlobalData));
	
	memset(gd, 0, sizeof(IXGlobalData));
	ctx->ctx = gd;

	return 0;
}

ILCore il_core_ix =
{
	.name		=	"IX Virtual Machine",
	.init		=	avs_ix_compiler_init,
	.compile	=	avs_ix_compiler_compile,
};

