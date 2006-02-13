#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "avs.h"


static AvsILTreeNode * create_node(AvsILTreeNodeType type)
{
	AvsILTreeNode *node = malloc(sizeof(AvsILTreeNode));
	memset(node, 0, sizeof(AvsILTreeNode));
	node->type = type;
	return node;
}

AvsILTreeNode * avs_il_tree_node_iterator_next(AvsILTreeNodeIterator *iter)
{
	AvsILTreeNode *node;
	
	if (!iter->current)
		node = iter->base->base;
	else
		node = iter->current->next;
	
	iter->current = node;
	return node;
}

AvsILTreeNode * avs_il_tree_node_iterator_prev(AvsILTreeNodeIterator *iter)
{
	AvsILTreeNode *node;

	if (!iter->current)
		node = iter->base->end;
	else
		node = iter->current->prev;

	iter->current = node;
	return node;
}

int avs_il_tree_node_level_down(AvsILTreeContext *ctx, AvsILTreeNodeIterator *iter)
{
	AvsILTreeNode *parent = ctx->currentlevel->parentlevel;

	if (!parent)
		return VISUAL_ERROR_GENERAL;

	/* Initialize iterator */
	if (iter) {
		iter->base = ctx->currentlevel;
		iter->current = NULL;
	}
	
	ctx->current = ctx->currentlevel->parent;
	ctx->currentlevel = parent;
	return VISUAL_OK;
}

int avs_il_tree_node_level_up(AvsILTreeContext *ctx, AvsILTreeNodeType type)
{
	AvsILTreeNode *node = create_node(type);
	AvsILTreeNode *current = ctx->currentlevel;

	/* Set depth level */
	node->depth = current->depth + 1;

	/* Link node into tree */
	node->parentlevel = current;
	node->parent = ctx->current;
	
	if (current->end) {
		node->prev = current->end;
		current->end->next = node;
		current->end = node;
	} else {
		current->base = node;
		current->end = node;
	}

	ctx->currentlevel = node;
	return VISUAL_OK;
}

int avs_il_tree_node_level_count(AvsILTreeContext *ctx)
{
	AvsILTreeNode *parent = ctx->currentlevel;
	AvsILTreeNode *node;
	int count = 0;

	if (!parent)
		return 0;
	
	for (node=parent->base; node != NULL; node = node->next) 
		count++;
	
	return count;
}

AvsILTreeNodeType avs_il_tree_node_level_type(AvsILTreeContext *ctx)
{
	return ctx->currentlevel->type;
}

int avs_il_tree_node_level_depth(AvsILTreeContext *ctx)
{
	return ctx->currentlevel->depth;
}

void avs_il_tree_node_level_mark(AvsILTreeContext *ctx, unsigned int mark)
{
	ctx->currentlevel->mark = mark;
}

int avs_il_tree_node_level_get_mark(AvsILTreeContext *ctx)
{
	return ctx->currentlevel->mark;
}

int avs_il_tree_node_up(AvsILTreeContext *ctx, AvsILTreeNodeType type)
{
	AvsILTreeNode *node;
	AvsILTreeNode *parent = ctx->currentlevel;
	
	if (!parent)
		return VISUAL_ERROR_GENERAL;

	/* Link node into tree */
	node = create_node(type);
	node->parent = parent;

	/* Set depth level */
	node->depth = parent->depth + 1;

	if (parent->end) {
		node->prev = parent->end;
		parent->end->next = node;
		parent->end = node;
	} else {
		parent->base = node;
		parent->end = node;
	}

	ctx->current = node;
	return VISUAL_OK;
}

int avs_il_tree_node_init(AvsILTreeContext *ctx)
{
	ctx->base = ctx->current = ctx->currentlevel = create_node(AvsILTreeNodeTypeBase);
	return VISUAL_OK;
}
