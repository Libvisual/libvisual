#ifndef _AVS_IX_MACHINE_H
#define _AVS_IX_MACHINE_H 1

#define IXMachineStateFlagZero	1

typedef struct _AvsIXMachineState {
	AvsRunnable	*runnable;
	unsigned int	flags;
	IXOpcode	*ip;
} IXMachineState;

typedef void (*IXOpcodeHandler)(IXMachineState *, IXOpcode *);

#define IX_INSTRUCTION(fn) \
	static void fn (IXMachineState *state, IXOpcode *op)

/* prototypes */
int avs_ix_machine_run(AvsRunnable *obj);

#endif /* !_AVS_IX_MACHINE_H */
