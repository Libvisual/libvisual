#ifndef _AVS_VM_H 
#define _AVS_VM_H 1

struct _VmInstruction;
typedef struct _VmInstruction VmInstruction;

#define VM_INSTRUCTION(x) \
	static void x (AvsRunnable *obj, VmInstruction *vmi, VmInstruction **next)

typedef void (*VmInstructionCall)(AvsRunnable *, VmInstruction *, VmInstruction **);

#include "avs_vm_assembler.h"
#include "avs_vm_instruction.h"

struct _VmInstruction {
	VmInstructionCall	run;
	VmInstruction		*next;
	VmInstruction		*jump[2];
	AvsNumber		*result;
	AvsNumber		*arg[2];
	AvsRunnableFunctionCall	fn;
	AvsNumber		**fnarg;
	int 			fncount;
	void			*private;
};

enum _VmPrivateInstruction {
	AvsInsnLoadCounter	= AvsInstructionCount + 0,
	AvsInsnLoop		= AvsInstructionCount + 1,
	AvsInsnCondJump		= AvsInstructionCount + 2,
	AvsInsnJump		= AvsInstructionCount + 3,
	AvsInsnLoadPtr		= AvsInstructionCount + 4,
	AvsInsnAssignToPtr	= AvsInstructionCount + 5,
	AvsInsnAssignFromPtr	= AvsInstructionCount + 6,
};

enum _VmPrivateVariableFlag {
	AvsRunnableVariableIfValue	= AvsRunnableVariablePrivateBase,
};

extern VmInstructionCall vm_instruction_table[];

/* prototypes */
int vm_run(AvsRunnable *obj);

#endif /* _AVS_VM_H */
