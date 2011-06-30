#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"

#if 0
ILInstruction *avs_il_instruction_emit_load(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILRegister *dest, AvsRunnableVariable *variable)
{
	ILInstruction *insn = avs_il_instruction_create(ctx, obj);

	/* Instantiate Load Constant instruction */
	insn->type = ILInstructionLoad;
	insn->reg[0] = dest;
	insn->ex.load.variable = variable;

	/* Update reference counts */
	avs_il_register_reference(dest);

	return insn;
}

ILInstruction *avs_il_instruction_emit_load_constant(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILRegister *dest, AvsNumber constant)
{
	ILInstruction *insn = avs_il_instruction_create(ctx, obj);

	/* Instantiate Load Constant instruction */
	insn->type = ILInstructionLoadConstant;
	insn->reg[0] = dest;
	insn->ex.loadconst.number = constant;

	/* Update reference counts */
	avs_il_register_reference(dest);

	return insn;
}
#endif 

ILInstruction *avs_il_instruction_emit(AvsILAssemblerContext *ctx, 
				       AvsRunnable *obj,
				       ILInstructionType insntype)
{
	ILInstruction *insn = avs_il_instruction_create(ctx, obj);

	insn->type = insntype;
	return insn;
}

ILInstruction *avs_il_instruction_emit_single(AvsILAssemblerContext *ctx, AvsRunnable *obj, 
					      ILInstructionType insntype,
					      ILRegister *reg0)
{
	ILInstruction *insn = avs_il_instruction_create(ctx, obj);

	insn->type = insntype;
	insn->reg[0] = reg0;

	/* Update reference counts */
	avs_il_register_reference(reg0);
	
	return insn;
}

ILInstruction *avs_il_instruction_emit_twin(AvsILAssemblerContext *ctx, AvsRunnable *obj, 
				      	    ILInstructionType insntype, 
				     	    ILRegister *lhs,
				            ILRegister *rhs)
{
	ILInstruction *insn = avs_il_instruction_create(ctx, obj);

	insn->type = insntype;
	insn->reg[0] = lhs;
	insn->reg[1] = rhs;

	/* Update reference counts */
	avs_il_register_reference(lhs);
	avs_il_register_reference(rhs);

	return insn;
}

ILInstruction *avs_il_instruction_emit_triplet(AvsILAssemblerContext *ctx, AvsRunnable *obj, 
				      	    ILInstructionType insntype, 
				     	    ILRegister *reg0,
				            ILRegister *reg1,
					    ILRegister *reg2)
{
	ILInstruction *insn = avs_il_instruction_create(ctx, obj);

	insn->type = insntype;
	insn->reg[0] = reg0;
	insn->reg[1] = reg1;
	insn->reg[2] = reg2;

	/* Update reference counts */
	avs_il_register_reference(reg0);
	avs_il_register_reference(reg1);
	avs_il_register_reference(reg2);

	return insn;
}

ILInstruction *avs_il_instruction_create(AvsILAssemblerContext *ctx, AvsRunnable *obj)
{
    static int count = 0;
	ILInstruction *insn;
	insn = malloc(sizeof(ILInstruction));
	memset(insn, 0, sizeof(ILInstruction));
	return insn;
}
