#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_ix.h"

// FIXME see about getting rid of the warnings regarding %08x and the fact that we're passing (AvsNumber *) in that position, when it's expecting an unsigned integer.
static void dump_tree(IXOpcode *root)
{
	static char *names[] = {
		"nop", "call", "negate", "assign", "add", "sub", "mul", "div", "mod", "and", "or",
		"cmp", "jmp", "jmpnz", "jmpz", "loadref", "storeref", "invalid", NULL, 
	};
	IXOpcode *op;

	for (op=root; op != NULL; op = op->next) {
		fprintf(stderr, "disas: 0x%08x:\t%-12s", (int)op, names[op->opcode]);
		switch (op->opcode) {
			case IXOpcodeNop:
			case IXOpcodeInvalid:
				break;
			
			case IXOpcodeCall:
				fprintf(stderr, "%s", op->ex.call.call->name);
				break;

			case IXOpcodeNegate:
				fprintf(stderr, "Rd(0x%08x), Rs0(0x%08x, %.2f)",
						(int)op->reg[0], (int)op->reg[1], *op->reg[1]);
				break;

			case IXOpcodeAssign:
			case IXOpcodeAdd:
			case IXOpcodeSub:
			case IXOpcodeMul:
			case IXOpcodeDiv:
			case IXOpcodeMod:
			case IXOpcodeAnd:
			case IXOpcodeOr:
				fprintf(stderr, "Rd(0x%08x), Rs0(0x%08x, %.2f), Rs1(0x%08x, %.2f)", 
						(int)op->reg[0], (int)op->reg[1], *op->reg[1], (int)op->reg[2], *op->reg[2]);
				break;
			
			case IXOpcodeCmp:
				fprintf(stderr, "Rs0(0x%08x, %.2f), Rs1(0x%08x, %.2f)",
						(int)op->reg[0], *op->reg[0], (int)op->reg[1], *op->reg[1]);
				break;
				
			case IXOpcodeJmp:
			case IXOpcodeJmpNz:
			case IXOpcodeJmpZ:
				fprintf(stderr, "*0x%08x", (int)op->ex.jmp.dest);
				break;

			case IXOpcodeLoadRef:
				fprintf(stderr, "Rf(0x%08x), Rd(0x%08x, %.2f), Rs(0x%08x, %.2f)",
						(int)op->ex.ref, (int)op->reg[0], *op->reg[0], (int)op->reg[1], *op->reg[1]);
				break;	
				
			case IXOpcodeStoreRef:
				fprintf(stderr, "Rf(0x%08x, 0x%08x), Rd(0x%08x, %.2f), Rs(0x%08x, %.2f)",
						(int)op->ex.ref, (int)op->ex.ref->ref, (int)op->reg[0], *op->reg[0], (int)op->reg[1], *op->reg[1]);
				break;
				
		}
		fprintf(stderr, "\n");
	}
}

IX_INSTRUCTION(ix_nop)
{

}

IX_INSTRUCTION(ix_call)
{
	op->ex.call.call->run(state->runnable, op->reg[0], op->ex.call.argv, op->ex.call.argc);
}

IX_INSTRUCTION(ix_negate)
{
	*op->reg[0] = -*op->reg[1];
}

IX_INSTRUCTION(ix_assign)
{
	*op->reg[0] = *op->reg[1] = *op->reg[2];
}

IX_INSTRUCTION(ix_add)
{
	*op->reg[0] = *op->reg[1] + *op->reg[2];
}

IX_INSTRUCTION(ix_sub)
{
	*op->reg[0] = *op->reg[1] - *op->reg[2];
}

IX_INSTRUCTION(ix_mul)
{
	*op->reg[0] = *op->reg[1] * *op->reg[2];
}

IX_INSTRUCTION(ix_div)
{
	*op->reg[0] = *op->reg[1] / *op->reg[2];
}

IX_INSTRUCTION(ix_mod)
{
	*op->reg[0] = (unsigned int)*op->reg[1] % (unsigned int)*op->reg[2];
}

IX_INSTRUCTION(ix_and)
{
	*op->reg[0] = (unsigned int)*op->reg[1] & (unsigned int)*op->reg[2];
}

IX_INSTRUCTION(ix_or)
{
	*op->reg[0] = (unsigned int)*op->reg[1] | (unsigned int)*op->reg[2];
}

IX_INSTRUCTION(ix_cmp)
{
	unsigned int equals = (*op->reg[0] == *op->reg[1]);
	state->flags = (state->flags & ~IXMachineStateFlagZero) | (IXMachineStateFlagZero & -equals);
}

IX_INSTRUCTION(ix_jmp)
{
	state->ip = op->ex.jmp.dest;
}

IX_INSTRUCTION(ix_jmpnz)
{
	if ((state->flags & IXMachineStateFlagZero) == 0)
		state->ip = op->ex.jmp.dest;
}

IX_INSTRUCTION(ix_jmpz)
{
	if (state->flags & IXMachineStateFlagZero)
		state->ip = op->ex.jmp.dest;
}

IX_INSTRUCTION(ix_loadref)
{
	*op->reg[0] = *op->reg[1];
	op->ex.ref->ref = op->reg[1];
}

IX_INSTRUCTION(ix_storeref)
{
	*op->ex.ref->ref = *op->reg[0] = *op->reg[1] = *op->reg[2];	
}

static IXOpcodeHandler opcode_handler[] =
{
	ix_nop,
	ix_call,
	ix_negate,
	ix_assign,
	ix_add,
	ix_sub,
	ix_mul,
	ix_div,
	ix_mod,
	ix_and,
	ix_or,
	ix_cmp,
	ix_jmp,
	ix_jmpnz,
	ix_jmpz,
	ix_loadref,
	ix_storeref,
};

int avs_ix_machine_run(AvsRunnable *obj)
{
	IXRunnableData *rd = IX_RUNNABLE_DATA(obj);
	IXMachineState state;
	IXOpcode *ip;
	
	avs_debug(print("IX: Runnable data: %p", obj));
	dump_tree(rd->base);
	avs_debug(print("IX: Running..."));

	memset(&state, 0, sizeof(IXMachineState));
	state.runnable = obj;

	for (ip = rd->base; ip != NULL; ip = state.ip) {
		state.ip = ip->next;
		opcode_handler[ip->opcode](&state, ip);
	}
	
	avs_debug(print("IX: Halting..."));
	return VISUAL_OK;
}
