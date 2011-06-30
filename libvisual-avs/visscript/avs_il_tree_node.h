#ifndef _AVS_IL_TREE_NODE_H
#define _AVS_IL_TREE_NODE_H 1

struct _AvsILTreeNode;
typedef struct _AvsILTreeNode AvsILTreeNode;
enum _AvsILTreeNodeType;
typedef enum _AvsILTreeNodeType AvsILTreeNodeType;
struct _AvsILTreeNodeIterator;
typedef struct _AvsILTreeNodeIterator AvsILTreeNodeIterator;

enum _AvsILTreeNodeType {
	AvsILTreeNodeTypeNull,
	AvsILTreeNodeTypeBase,
	AvsILTreeNodeTypeFunction,
	AvsILTreeNodeTypeFunctionArgument,
	AvsILTreeNodeTypeInvalid
};

struct _AvsILTreeNode {
	AvsILTreeNodeType	type;

	unsigned int		mark;
	int			depth; /* depth level */
	AvsILTreeNode		*parentlevel; /* parent of current level */
	AvsILTreeNode		*parent;
	AvsILTreeNode		*base, *end; /* next level */
	AvsILTreeNode		*prev, *next; /* neighbours of this node at the same level */

	struct {
		ILInstruction		*base;
		ILInstruction		*end;
	} insn;
};

struct _AvsILTreeNodeIterator {
	AvsILTreeNode		*base;
	AvsILTreeNode		*current;
};

#endif /* !_AVS_IL_TREE_NODE_H */
