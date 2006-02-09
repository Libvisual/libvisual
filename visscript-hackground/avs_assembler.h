#ifndef _AVS_ASSEMBLER_H
#define _AVS_ASSEMBLER_H 1

enum _AvsArithOperator;
typedef enum _AvsArithOperator AvsArithOperator;
enum _AvsInstruction;
typedef enum _AvsInstruction AvsInstruction;
struct _AvsAssembler;
typedef struct _AvsAssembler AvsAssembler;
struct _AvsAssemblerContext;
typedef struct _AvsAssemblerContext AvsAssemblerContext;

enum _AvsArithOperator {
	AvsArithOpNull,
	AvsArithOpNegate,
	AvsArithOpAdd,
	AvsArithOpSub,
	AvsArithOpMul,
	AvsArithOpDiv,
	AvsArithOpMod,
	AvsArithOpAnd,
	AvsArithOpOr,
	AvsArithOpCount,
};

enum _AvsInstruction {
	AvsInsnNop,
	AvsInsnCall,
	AvsInsnAssign,
	AvsInsnNegate,
	AvsInsnAdd,
	AvsInsnSub,
	AvsInsnMul,
	AvsInsnDiv,
	AvsInsnMod,
	AvsInsnAnd,
	AvsInsnOr,
	AvsInstructionCount,
};


#define ASSEMBLER_EMIT_FUNCTION(x) \
	static void x (AvsAssemblerContext *ctx, \
		      AvsRunnable *obj, \
		      AvsInstruction insn, \
		      AvsCompilerArgument *retval, \
		      AvsCompilerArgument *args, \
		      int count)

#define ASSEMBLER_INIT(x) \
	static int x (AvsAssemblerContext *ctx)
#define ASSEMBLER_CLEANUP(x) \
	static int x (AvsAssemblerContext *ctx)
#define ASSEMBLER_RUNNABLE_INIT(x) \
	static int x (AvsAssemblerContext *ctx, AvsRunnable *obj)
#define ASSEMBLER_RUNNABLE_FINISH(x) \
	static int x (AvsAssemblerContext *ctx, AvsRunnable *obj)
#define ASSEMBLER_RUNNABLE_CLEANUP(x) \
	static int x (AvsAssemblerContext *ctx, AvsRunnable *obj)
#define ASSEMBLER_MARKER(x) \
	static int x (AvsAssemblerContext *ctx, \
		      AvsRunnable *obj, \
		      AvsCompilerMarkerType marker, \
		      char *name)

typedef void (*AvsAssemblerCall)(AvsAssemblerContext *, 
				AvsRunnable *, 
				AvsInstruction, 
				AvsCompilerArgument *, 
				AvsCompilerArgument *,
				int);
typedef int (*AvsAssemblerInit)(AvsAssemblerContext *);
typedef int (*AvsAssemblerCleanup)(AvsAssemblerContext *);
typedef int (*AvsAssemblerRunnableInit)(AvsAssemblerContext *, AvsRunnable *);
typedef int (*AvsAssemblerRunnableFinish)(AvsAssemblerContext *, AvsRunnable *);
typedef int (*AvsAssemblerRunnableCleanup)(AvsAssemblerContext *, AvsRunnable *);
typedef int (*AvsAssemblerMarker)(AvsAssemblerContext *, 
				  AvsRunnable *, 
				  AvsCompilerMarkerType,
				  char *name);

struct _AvsAssembler {
	char		 		*name;
	AvsAssemblerInit		init;
	AvsAssemblerCleanup		cleanup;
	AvsAssemblerRunnableInit	object_init;
	AvsAssemblerRunnableFinish	object_finish;
	AvsAssemblerRunnableCleanup	object_cleanup;
	AvsAssemblerMarker		marker;
	AvsAssemblerCall		emit_insn[AvsInstructionCount];
};

struct _AvsAssemblerContext {
	AvsAssembler		*pasm;
	void			*pdata;
};

/* prototypes */
int avs_assembler_emit_insn(AvsAssemblerContext *ctx, AvsRunnable *obj, AvsInstruction insn, AvsCompilerArgument *retval, AvsCompilerArgument *args, int count);
int avs_assembler_marker(AvsAssemblerContext *ctx, AvsRunnable *obj, AvsCompilerMarkerType marker, char *name);
int avs_assembler_runnable_cleanup(AvsAssemblerContext *ctx, AvsRunnable *obj);
int avs_assembler_runnable_finish(AvsAssemblerContext *ctx, AvsRunnable *obj);
int avs_assembler_runnable_init(AvsAssemblerContext *ctx, AvsRunnable *obj);
int avs_assembler_cleanup(AvsAssemblerContext *ctx);
int avs_assembler_init(AvsAssemblerContext *ctx);

#endif /* !_AVS_ASSEMBLER_H */
