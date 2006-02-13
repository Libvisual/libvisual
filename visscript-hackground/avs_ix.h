#ifndef _AVS_IX_H
#define _AVS_IX_H 1

struct _AvsIXOpcode;
typedef struct _AvsIXOpcode IXOpcode;
struct _AvsIXExLoop;
typedef struct _AvsIXExLoop IXExLoop;
struct _AvsIXExInfo;
typedef struct _AvsIXExInfo IXExInfo;
struct _AvsIXRegisterReference;
typedef struct _AvsIXRegisterReference IXRegisterReference;

#include "avs.h"
#include "avs_ix_compiler.h"
#include "avs_ix_machine.h"


typedef enum _AvsIXOpcodeType {
	IXOpcodeNop,
	IXOpcodeCall,
	IXOpcodeNegate,
	IXOpcodeAssign,
	IXOpcodeAdd,
	IXOpcodeSub,
	IXOpcodeMul,
	IXOpcodeDiv,
	IXOpcodeMod,
	IXOpcodeAnd,
	IXOpcodeOr,
	IXOpcodeCmp,
	IXOpcodeJmp,
	IXOpcodeJmpNz,
	IXOpcodeJmpZ,
	IXOpcodeLoadRef,
	IXOpcodeStoreRef,
	IXOpcodeInvalid
} IXOpcodeType;

struct _AvsIXRegisterReference {
	AvsNumber	*ref;
};

struct _AvsIXOpcode {
	IXOpcodeType	opcode;
	IXOpcode	*next, *prev;
	AvsNumber	*reg[3];
	union {
		struct {
			IXOpcode		 *dest;
		} jmp;
		struct {
			AvsRunnableFunction	*call;
			int			argc;
			AvsNumber		**argv;
		} call;
		IXRegisterReference		*ref;
	} ex;
};

typedef struct _AvsIXRunnableData {
	IXOpcode	*base, *end;
} IXRunnableData;

struct _AvsIXExLoop {
	AvsNumber	*counter;
	int		finished;
	IXOpcode	*jpt0, *jpt1;
	IXExLoop	*prev, *next;
};

typedef enum _AvsIXExInfoType {
	IXExInfoTypeNull,
	IXExInfoTypeLinkJump,
	IXExInfoTypeLinkJumpNext,
	IXExInfoTypeInvalid,
} IXExInfoType;

struct _AvsIXExInfo {
	IXExInfoType	type;
	union {
		struct AvsIXExInfoJump {
			IXOpcode	*from;
		} jmp;
	} ex;

	IXExInfo	*prev;
};

typedef struct _AvsIXGlobalData {
	struct {
		IXExLoop	*base, *end;
	} loop;
	struct {
		IXExInfo	*queued;
	} bh;
} IXGlobalData;

#define IX_RUNNABLE_DATA(robj) \
	((IXRunnableData *) (robj)->pcore)

#define IX_GLOBAL_DATA(gobj) \
	((IXGlobalData *) (gobj)->ctx)

#define IX_EXINFO(insn) \
	((IXExInfo *) (insn)->private)

#endif /* !_AVS_IX_H */
