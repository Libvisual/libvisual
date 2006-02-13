#ifndef _AVS_IL_ASSEMBLER_H
#define _AVS_IL_ASSEMBLER_H 1

struct _AvsILAssemblerContext;
typedef struct _AvsILAssemblerContext AvsILAssemblerContext;
struct _AvsILRunnableData;
typedef struct _AvsILRunnableData AvsILRunnableData;

#include "avs_il_register.h"
#include "avs_il_instruction.h"
#include "avs_il_tree_node.h"
#include "avs_il_tree.h"
#include "avs_il_core.h"

struct _AvsILAssemblerContext {
	AvsILTreeContext	tree;
	ILCoreContext		*core;
	int			nestlevel;
};

struct _AvsILRunnableData {
	AvsILAssemblerContext	*ctx;
};

/* avs_il_assembler.c */
int avs_il_emit_instruction(AvsILAssemblerContext *ctx, AvsRunnable *obj, AvsCompilerInstruction insn, AvsCompilerArgument *retval, AvsCompilerArgument *args, int count);
int avs_il_emit_marker(AvsILAssemblerContext *ctx, AvsRunnable *obj, AvsCompilerMarkerType marker, char *name);
int avs_il_runnable_cleanup(AvsILAssemblerContext *ctx, AvsRunnable *obj);
int avs_il_runnable_finish(AvsILAssemblerContext *ctx, AvsRunnable *obj);
int avs_il_runnable_init(AvsILAssemblerContext *ctx, AvsRunnable *obj);
int avs_il_cleanup(AvsILAssemblerContext *ctx);
int avs_il_init(AvsILAssemblerContext *ctx, ILCoreContext *core);

/* avs_il_tree.c */
ILInstruction *avs_il_tree_base(AvsILTreeContext *ctx);
void avs_il_tree_merge(AvsILTreeContext *ctx, AvsILTreeNode *node);
void avs_il_tree_add(AvsILTreeContext *ctx, ILInstruction *insn);
int avs_il_tree_reset(AvsILTreeContext *ctx);
int avs_il_tree_cleanup(AvsILTreeContext *ctx);
int avs_il_tree_init(AvsILTreeContext *ctx);

/* avs_il_tree_node.c */
AvsILTreeNode *avs_il_tree_node_iterator_next(AvsILTreeNodeIterator *iter);
AvsILTreeNode *avs_il_tree_node_iterator_prev(AvsILTreeNodeIterator *iter);
int avs_il_tree_node_level_down(AvsILTreeContext *ctx, AvsILTreeNodeIterator *iter);
int avs_il_tree_node_level_up(AvsILTreeContext *ctx, AvsILTreeNodeType type);
int avs_il_tree_node_level_count(AvsILTreeContext *ctx);
AvsILTreeNodeType avs_il_tree_node_level_type(AvsILTreeContext *ctx);
int avs_il_tree_node_level_depth(AvsILTreeContext *ctx);
void avs_il_tree_node_level_mark(AvsILTreeContext *ctx, unsigned int mark);
int avs_il_tree_node_level_get_mark(AvsILTreeContext *ctx);
int avs_il_tree_node_up(AvsILTreeContext *ctx, AvsILTreeNodeType type);
int avs_il_tree_node_init(AvsILTreeContext *ctx);

/* avs_il_register.c */
ILRegister *avs_il_register_load_worker(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg);
ILRegister *avs_il_register_load_from_argument(AvsILAssemblerContext *ctx, AvsRunnable *robj, AvsCompilerArgument *arg);
void avs_il_register_dereference(ILRegister *reg);
void avs_il_register_reference(ILRegister *reg);
ILRegister *avs_il_register_create(void);

/* avs_il_instruction.c */
ILInstruction *avs_il_instruction_emit_load(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILRegister *dest, AvsRunnableVariable *variable);
ILInstruction *avs_il_instruction_emit_load_constant(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILRegister *dest, AvsNumber constant);
ILInstruction *avs_il_instruction_emit(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILInstructionType insntype);
ILInstruction *avs_il_instruction_emit_single(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILInstructionType insntype, ILRegister *reg0);
ILInstruction *avs_il_instruction_emit_twin(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILInstructionType insntype, ILRegister *lhs, ILRegister *rhs);
ILInstruction *avs_il_instruction_emit_triplet(AvsILAssemblerContext *ctx, AvsRunnable *obj, ILInstructionType insntype, ILRegister *reg0, ILRegister *reg1, ILRegister *reg2);
ILInstruction *avs_il_instruction_create(AvsILAssemblerContext *ctx, AvsRunnable *obj);

#endif /* !_AVS_IL_ASSEMBLER_H */
