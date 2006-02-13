#ifndef _AVS_IL_INSTRUCTION_H
#define _AVS_IL_INSTRUCTION_H 1

struct _AvsILInstruction;
typedef struct _AvsILInstruction ILInstruction;

typedef enum _AvsILInstructionType {
	ILInstructionNop,
	ILInstructionCall,
	ILInstructionNegate,
	ILInstructionAssign,
	ILInstructionAdd,
	ILInstructionSub,
	ILInstructionMul,
	ILInstructionDiv,
	ILInstructionMod,
	ILInstructionAnd,
	ILInstructionOr,
	ILInstructionLoopInit,
	ILInstructionLoop,
	ILInstructionJump,
	ILInstructionJumpTrue,
	ILInstructionMergeMarker,
	ILInstructionLoadReference,
	ILInstructionStoreReference,
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
		struct AvsILInstructionCallArguments {
			AvsRunnableFunction		*call;
			unsigned int			argc;
			ILRegister			**argv;
		} call;
		struct AvsILInstructionMergeArguments {
			enum AvsILinstructionMergeType {
				AvsILInstructionMergeTypeJumpConditional,
				AvsILInstructionMergeTypeLoop,
			} type;
			ILInstruction			*from[3];
		} merge;
	} ex;

	/* Private pointer for compilers */
	void			*private;

	/* Linked list pointers */
	ILInstruction		*prev;
	ILInstruction		*next;	
};

#endif /* !_AVS_IL_INSTRUCTION_H */
