#ifndef _AVS_IX_H
#define _AVS_IX_H 1

struct _AvsIXOpcode;
typedef struct _AvsIXOpcode IXOpcode;

#include "avs.h"
#include "avs_ix_compiler.h"
#include "avs_ix_machine.h"


typedef enum _AvsIXOpcodeType {
	IXOpcodeNop,
	IXOpcodeAssign,
	IXOpcodeAdd,
	IXOpcodeSub,
	IXOpcodeMul,
	IXOpcodeDiv,
	IXOpcodeMod,
	IXOpcodeAnd,
	IXOpcodeOr,
	IXOpcodeInvalid
} IXOpcodeType;

struct _AvsIXOpcode {
	IXOpcodeType	opcode;
	IXOpcode	*next, *prev;
	AvsNumber	*reg[3];	
};

typedef struct _AvsIXRunnableData {
	IXOpcode	*base, *end;
} IXRunnableData;

#define IX_RUNNABLE_DATA(robj) \
	((IXRunnableData *) (robj)->pcore)

#endif /* !_AVS_IX_H */
