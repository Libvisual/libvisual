#ifndef _AVS_X86_ASSEMBLER_H
#define _AVS_X86_ASSEMBLER_H 1

#include "avs_x86_opcode.h"

#define X86_GLOBALDATA(x) ((X86GlobalData *) (x))
#define X86_RUNNABLEDATA(x) ((X86RunnableData *) (x))
#define X86_VARIABLE(x) ((X86Variable *) (x))

struct _X86GlobalData;
typedef struct _X86GlobalData X86GlobalData;
struct _X86RunnableData;
typedef struct _X86RunnableData X86RunnableData;
enum _X86VariableType;
typedef enum _X86VariableType X86VariableType;
struct _X86Variable;
typedef struct _X86Variable X86Variable;

struct _X86GlobalData {
	X86Context		ctx;
	AvsRunnableVariable	*constant;
	AvsRunnableVariable	*result;
	AvsBlobPool		*xvmgr;
	AvsBlobPool		*rvmgr;
	X86Variable		*fss[8];
	int			fssused;
};

struct _X86RunnableData {
	X86GlobalData	*gd;
};

enum _X86VariableType {
	X86VariableTypeEmpty,
	X86VariableTypeRegister,
	X86VariableTypeMemory,
};

struct _X86Variable {
	AvsRunnableVariable	*rv;
	X86VariableType		type;
	union {
		int		reg;
		void		*ptr;
	} x;
};

/* prototypes */
AvsAssembler *avs_assembler_x86(void);

#endif /* !_AVS_X86_ASSEMBLER_H */
