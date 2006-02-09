#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_ix.h"

static IXOpcode * opcode_new(IXOpcodeType type)
{
	IXOpcode *op = malloc(sizeof(IXOpcode));
	memset(op, 0, sizeof(IXOpcode));
	op->opcode = type;
	
	return op;
}

static IXOpcode * opcode_add(IXRunnableData *rd, IXOpcodeType type)
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

	return op;
}

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

static int compile_opcode(IXRunnableData *rd, ILInstruction *insn)
{
	IXOpcode *op;
	
	switch (insn->type) {
		case ILInstructionAssign:
			op = opcode_add(rd, IXOpcodeAssign);
			op->reg[0] = get_operand(insn->reg[0]);
			op->reg[1] = get_operand(insn->reg[1]);
			break;

		case ILInstructionMul:
			op = opcode_add(rd, IXOpcodeMul);
			op->reg[0] = get_operand(insn->reg[0]); /* Dest */
			op->reg[1] = get_operand(insn->reg[1]); /* Source0 */
			op->reg[2] = get_operand(insn->reg[2]); /* Source1 */
			break;

		default:
			break;
	}

	return 0;
}

IL_CORE_COMPILE(avs_ix_compiler_compile)
{
	IXRunnableData *rd;
	ILInstruction *insn;
	
	avs_debug(print("IX: Compiling started..."));
		
	rd = malloc(sizeof(IXRunnableData));
	memset(rd, 0, sizeof(IXRunnableData));
	obj->pcore = rd;
	
	for (insn=tree->base; insn != NULL; insn = insn->next) {
		avs_debug(print("IX: Compiling instruction: %p", insn));
		compile_opcode(rd, insn);
	}

	/* Link machine */
	obj->run = avs_ix_machine_run;

	avs_debug(print("IX: Compiling finished..."));
	return 0;
}

IL_CORE_INIT(avs_ix_compiler_init)
{
	return 0;
}

ILCore il_core_ix =
{
	.name		=	"IX Virtual Machine",
	.init		=	avs_ix_compiler_init,
	.compile	=	avs_ix_compiler_compile,
};

