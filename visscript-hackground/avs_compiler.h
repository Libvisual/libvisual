#ifndef _AVS_COMPILER_H
#define _AVS_COMPILER_H 1


#define AVS_COMPILER_INITIAL_STACKSIZE 512
#define AVS_COMPILER_MAXIMUM_STACKSIZE 1024*1024

enum _AvsCompilerInstruction;
typedef enum _AvsCompilerInstruction AvsCompilerInstruction;
enum _AvsCompilerArgumentType;
typedef enum _AvsCompilerArgumentType AvsCompilerArgumentType;
enum _AvsCompilerMarkerType;
typedef enum _AvsCompilerMarkerType AvsCompilerMarkerType;
struct _AvsCompilerArgument;
typedef struct _AvsCompilerArgument AvsCompilerArgument;
struct _AvsCompilerStack;
typedef struct _AvsCompilerStack AvsCompilerStack;
struct _AvsCompilerContext;
typedef struct _AvsCompilerContext AvsCompilerContext;

enum _AvsCompilerInstruction {
	AvsCompilerInstructionNop,
	AvsCompilerInstructionCall,
	AvsCompilerInstructionAssign,
	AvsCompilerInstructionNegate,
	AvsCompilerInstructionAdd,
	AvsCompilerInstructionSub,
	AvsCompilerInstructionMul,
	AvsCompilerInstructionDiv,
	AvsCompilerInstructionMod,
	AvsCompilerInstructionAnd,
	AvsCompilerInstructionOr,
	AvsCompilerInstructionCount,
};

enum _AvsCompilerArgumentType {
	AvsCompilerArgumentInvalid,
	AvsCompilerArgumentConstant,
	AvsCompilerArgumentIdentifier,
	AvsCompilerArgumentMarker,
	AvsCompilerArgumentPrivate,
};

enum _AvsCompilerMarkerType {
	AvsCompilerMarkerInvalid,
	AvsCompilerMarkerFunction,
	AvsCompilerMarkerArgument,
	AvsCompilerMarkerSequencePoint,
	AvsCompilerMarkerRollback,
};

struct _AvsCompilerArgument {
	AvsCompilerArgumentType		type;
	union {
		AvsNumber		constant;
		char			*identifier;
		void			*ptr;
		AvsCompilerMarkerType	marker;
	} value;
};

struct _AvsCompilerStack {
	AvsCompilerArgument	*base;
	AvsCompilerArgument	*end;
	AvsCompilerArgument	*ptr;
	int		   	length;
};

#include "avs_il_assembler.h"

struct _AvsCompilerContext {
	AvsStack		*stack;
	AvsILAssemblerContext	*assembler;
};

/* prototypes */
int avs_compile_reset_stack(AvsCompilerContext *ctx);
int avs_compile_push_identifier(AvsCompilerContext *ctx, AvsRunnable *obj, char *identifier);
int avs_compile_push_constant(AvsCompilerContext *ctx, AvsRunnable *obj, AvsNumber constant);
int avs_compile_marker(AvsCompilerContext *ctx, AvsRunnable *obj, AvsCompilerMarkerType type, char *name);
int avs_compile_arithop(AvsCompilerContext *ctx, AvsRunnable *obj, AvsCompilerInstruction insn);
void avs_compiler_cleanup(AvsCompilerContext *cx);
int avs_compiler_init(AvsCompilerContext *cx, AvsILAssemblerContext *ax);

#endif /* !_AVS_COMPILER_H */
