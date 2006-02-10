#ifndef _AVS_X86_H
#define _AVS_X86_H 1

#include "avs_x86_opcode.h"
#include "avs_x86_compiler.h"

typedef struct _X86GlobalData {
	X86Context	ctx;
} X86GlobalData;

#define X86_GLOBALDATA(ictx) \
	((X86GlobalData *) (ictx)->ctx)

#endif /* !_AVS_X86_H */

