#ifndef _AVS_X86_H
#define _AVS_X86_H 1

#include "avs_x86_opcode.h"
#include "avs_x86_compiler.h"

typedef enum _AvsX86ExInfoType {
	X86ExInfoTypeNull,
	X86ExInfoTypeLinkJump,
	X86ExInfoTypeLinkJumpNext,
	X86ExInfoTypeInvalid,
} X86ExInfoType;

typedef struct _X86ExInfo {
	X86ExInfoType		type;
	union {
		struct AvsX86ExInfoJump {
			unsigned char	*offset;
			unsigned char	*next;
		} jmp;
	} ex;

	struct _X86ExInfo	*prev;
} X86ExInfo;

typedef struct _X86ExLoop {
	unsigned char 		*jpt_start;
	unsigned char		*rel_init;
	struct _X86ExLoop	*prev, *next;
} X86ExLoop;

typedef struct _X86GlobalData {
	X86Context	ctx;
	X86ExLoop	*loop;
} X86GlobalData;

#define X86_GLOBALDATA(ictx) \
	((X86GlobalData *) (ictx)->ctx)

#define X86_EXINFO(insn) \
	((X86ExInfo *) (insn)->private)

#endif /* !_AVS_X86_H */

