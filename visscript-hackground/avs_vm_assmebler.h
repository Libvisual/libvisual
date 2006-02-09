#ifndef _AVS_ASSEMBLER_VM_H
#define _AVS_ASSEMBLER_VM_H 1

struct _VmGlobalData;
typedef struct _VmGlobalData VmGlobalData;
struct _VmRunnableData;
typedef struct _VmRunnableData VmRunnableData;
struct _VmFunctionContext;
typedef struct _VmFunctionContext VmFunctionContext;
struct _VmArgumentContext;
typedef struct _VmArgumentContext VmArgumentContext;
struct _VmInstructionContext;
typedef struct _VmInstructionContext VmInstructionContext;

#define VM_VARIABLE(x) ((AvsRunnableVariable *)(x))

struct _VmInstructionContext {
	VmInstruction		*base;
	VmInstruction		*end;
	int			nestlevel; /**< Nest level */
	VmInstructionContext	*next;	/**< Next instruction context, NOT the next instruction */
};

struct _VmGlobalData {
	AvsRunnableVariable	*constant;
	AvsRunnableVariable	*result;
	AvsStack		*ixstack;
	VmInstructionContext	*ix;
	VmInstructionContext	*ixmarker;
	VmInstruction		*seqpoint;
	AvsBlobPool		*rvpool;
	AvsBlobPool		*vmpool;
	int			nestlevel;
	int			loadmarker;
};

struct _VmRunnableData {
	VmGlobalData		*gd;
	VmInstructionContext	ix;
	AvsNumber		*pointer;
};

#define VM_GLOBALDATA(x) ((VmGlobalData *) (x))
#define VM_RUNNABLEDATA(x) ((VmRunnableData *) (x))

/* prototypes */
AvsAssembler *avs_assembler_vm(void);

#endif /* !_AVS_ASSEMBLER_VM_H */
