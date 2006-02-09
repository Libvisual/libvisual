#ifndef _AVS_IX_MACHINE_H
#define _AVS_IX_MACHINE_H 1

typedef void (*IXOpcodeHandler)(IXOpcode *);

#define IX_INSTRUCTION(fn) \
	static void fn (IXOpcode *op)

/* prototypes */
int avs_ix_machine_run(AvsRunnable *obj);

#endif /* !_AVS_IX_MACHINE_H */
