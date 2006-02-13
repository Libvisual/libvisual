#ifndef _AVS_IL_TREE_H
#define _AVS_IL_TREE_H 1

#define AVS_ILTREE_INITIAL_STACKSIZE	128
#define AVS_ILTREE_MAXIMUM_STACKSIZE	1024*1024

struct _AvsILTreeContext;
typedef struct _AvsILTreeContext AvsILTreeContext;

struct _AvsILTreeContext {
	AvsILTreeNode		*base, *currentlevel, *current;
	AvsStack		*ixstack;
};

#endif /* !_AVS_IL_TREE_H */
