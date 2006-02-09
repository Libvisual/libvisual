#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"
#include "avs_ix.h"

IX_INSTRUCTION(ix_nop)
{

}

IX_INSTRUCTION(ix_assign)
{
	*op->reg[0] = *op->reg[1];
}

IX_INSTRUCTION(ix_mul)
{
	*op->reg[0] = *op->reg[1] * *op->reg[2];
}

static IXOpcodeHandler opcode_handler[] =
{
	ix_nop,
	ix_assign,
	ix_mul,
};

int avs_ix_machine_run(AvsRunnable *obj)
{
	IXRunnableData *rd = IX_RUNNABLE_DATA(obj);
	IXOpcode *op;
	
	avs_debug(print("IX: Runnable data: %p", obj));
	avs_debug(print("IX: Running..."));

	for (op = rd->base; op != NULL; op = op->next) 
		opcode_handler[op->opcode](op);
	
	avs_debug(print("IX: Halting..."));
	return VISUAL_OK;
}
