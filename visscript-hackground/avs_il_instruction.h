#ifndef _AVS_IL_INSTRUCTION_H
#define _AVS_IL_INSTRUCTION_H 1

struct _AvsILInstruction;
typedef struct _AvsILInstruction ILInstruction;

typedef enum _AvsILInstructionType {
	ILInstructionNop,
	ILInstructionLoad,
	ILInstructionLoadConstant,
	ILInstructionNegate,
	ILInstructionAssign,
	ILInstructionAdd,
	ILInstructionSub,
	ILInstructionMul,
	ILInstructionDiv,
	ILInstructionMod,
	ILInstructionAnd,
	ILInstructionOr,
	ILInstructionCount
} ILInstructionType;

struct _AvsILInstruction {
	ILInstructionType	type;
	ILRegister		*reg[3];
	
	/* Additional arguments */
	union {
		struct AvsILInstructionLoadArguments {
			AvsRunnableVariable		*variable;
		} load;
		struct AvsILInstructionLoadConstantArguments {
			AvsNumber			number;
		} loadconst;
		struct AvsILInstructionJumpArguments {
			ILInstruction			*pointer;
		} jmp;
	} ex;

	/* Linked list pointers */
	ILInstruction		*prev;
	ILInstruction		*next;	
};

#endif /* !_AVS_IL_INSTRUCTION_H */
