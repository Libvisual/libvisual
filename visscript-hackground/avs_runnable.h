#ifndef _AVS_RUNNABLE_H
#define _AVS_RUNNABLE_H 1

#define AVS_RUNNABLE_CONTEXT(obj)	(VISUAL_CHECK_CAST ((obj), AvsRunnableContext))
#define AVS_RUNNABLE(obj)		(VISUAL_CHECK_CAST ((obj), AvsRunnable))

enum _AvsRunnableVariableFlag;
typedef enum _AvsRunnableVariableFlag AvsRunnableVariableFlag;
struct _AvsRunnableFunction;
typedef struct _AvsRunnableFunction AvsRunnableFunction;

struct _AvsRunnableContext {
	VisObject		object;
	AvsLexerContext		lexer;
	AvsParserContext	parser;
	AvsCompilerContext	compiler;
	AvsILAssemblerContext	assembler;
	ILCoreContext		core;
};

enum _AvsRunnableVariableFlag {
	AvsRunnableVariableNull		= 0,
	AvsRunnableVariableConstant	= 1,
	AvsRunnableVariableAnonymous	= 2, 
	AvsRunnableVariablePrivateBase	= (1<<16),
	AvsRunnableVariablePrivateEnd	= (1<<31),
};

struct _AvsRunnableVariable {
	char			*name;
	AvsNumber		local_value;
	AvsNumber		*value;
	AvsRunnableVariableFlag	flags;
	AvsRunnableVariable	*next;
	void			*private;
};

#define AVS_RUNNABLE_FUNCTION(x) \
	static void x (AvsRunnable *obj, AvsNumber *retval, AvsNumber **args, int count)

typedef void (*AvsRunnableFunctionCall)(AvsRunnable *, AvsNumber *, AvsNumber **, int count);

struct _AvsRunnableFunction {
	char			*name;
	AvsRunnableFunctionCall	run;
	int			param_count;
	void			*private;
};

typedef int (*AvsRunnableExecuteCall)(AvsRunnable *);

struct _AvsRunnable {
	VisObject		object;
	AvsBlobManager		bm;
	AvsRunnableContext	*ctx;
	AvsRunnableVariable	*variables;
	void			*pasm;
	void			*pcore;

	AvsRunnableExecuteCall	run;
};

/* prototypes */
AvsRunnableVariable *avs_runnable_variable_create(AvsRunnable *obj, char *name, AvsNumber value);
AvsRunnableVariable *avs_runnable_variable_find(AvsRunnable *obj, char *name);
int avs_runnable_variable_post_bind(AvsRunnable *obj, char *name, AvsNumber **retval);
int avs_runnable_variable_bind(AvsRunnable *obj, char *name, AvsNumber *value);
int avs_runnable_execute(AvsRunnable *obj);
int avs_runnable_compile(AvsRunnable *obj, unsigned char *data, unsigned int length);
int avs_runnable_init(AvsRunnableContext *ctx, AvsRunnable *obj);
AvsRunnable *avs_runnable_new(AvsRunnableContext *ctx);
int avs_runnable_context_init(AvsRunnableContext *ctx);
AvsRunnableContext *avs_runnable_context_new(void);

#endif /* !_AVS_RUNNABLE_H */
