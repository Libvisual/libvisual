#ifndef _AVS_IL_CORE_H
#define _AVS_IL_CORE_H 1

struct _AvsILCoreContext;
typedef struct _AvsILCoreContext ILCoreContext;

struct _AvsILCore;
typedef struct _AvsILCore ILCore;

typedef int (*ILCoreVirtualInit)(ILCoreContext *);
typedef int (*ILCoreVirtualCompile)(ILCoreContext *, AvsILTreeContext *, AvsRunnable *);
typedef int (*ILCoreVirtualCleanup)(ILCoreContext *);
	
struct _AvsILCoreContext {
	ILCore		*core;
	void		*ctx;
};

struct _AvsILCore {
	char			*name;	
	ILCoreVirtualInit	init;
	ILCoreVirtualCompile	compile;
    ILCoreVirtualCleanup    cleanup;    
};

#define IL_CORE_INIT(fn) \
	int fn (ILCoreContext *ctx)

#define IL_CORE_COMPILE(fn) \
	int fn (ILCoreContext *ctx, AvsILTreeContext *tree, AvsRunnable *obj)

/* prototypes */
int avs_il_core_compile(ILCoreContext *ctx, AvsILTreeContext *tree, AvsRunnable *obj);
int avs_il_core_init(ILCoreContext *ctx);
int avs_il_core_context_init(ILCoreContext *ctx);
int avs_il_core_context_cleanup(ILCoreContext *ctx);
ILCoreContext *avs_il_core_context_create(void);


#endif /* !_AVS_IL_CORE_H */
