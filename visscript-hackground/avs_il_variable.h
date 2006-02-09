#ifndef _AVS_IL_VARIABLE_H
#define _AVS_IL_VARIABLE_H 1

#define AVS_ILVARIABLE_INITIAL_STACKSIZE 512
#define AVS_ILVARIABLE_MAXIMUM_STACKSIZE 1024*1024

struct _AvsILVariableContext;
typedef struct _AvsILVariableContext AvsILVariableContext;
struct _AvsILVariable;
typedef struct _AvsILVariable AvsILVariable;

struct _AvsILVariableContext {
	AvsStack		*vstack;
};

struct _AvsILVariable {
	int			refcount;
	AvsRunnableVariable	*rv;
};

#define AVS_IL_VARIABLE(x) ((AvsILVariable *) (x))

/* prototypes */
AvsILVariable *avs_il_variable_retrieve(AvsILVariableContext *ctx, AvsRunnable *obj, AvsCompilerArgument *arg);
AvsILVariable *avs_il_variable_store(AvsILVariableContext *ctx, AvsRunnable *obj, AvsCompilerArgument *arg);

#endif /* !_AVS_IL_VARIABLE */
