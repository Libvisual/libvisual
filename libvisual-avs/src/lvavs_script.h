/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _LV_LVAVS_SCRIPT_H
#define _LV_LVAVS_SCRIPT_H

#include <libvisual/libvisual.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LVAVS_SCRIPT_ENVIRON(obj)				(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptEnviron))
#define LVAVS_SCRIPT_ENVIRON_FUNCTIONTABLEELEMENT(obj)		(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptEnvironFunctionTableElement))
#define LVAVS_SCRIPT_ENVIRON_FUNCTIONTABLE(obj)			(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptEnvironFunctionTable))
#define LVAVS_SCRIPT_ENVIRON_VARTABLE(obj)			(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptEnvironVarTable))

#define LVAVS_SCRIPT_VMCONTEXT(obj)				(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptVMContext))
#define LVAVS_SCRIPT_VMFUNCTIONTABLE(obj)			(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptVMFunctionTable))
#define LVAVS_SCRIPT_VMVARTABLE(obj)				(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptVMVarTable))

#define LVAVS_SCRIPT_TREE(obj)					(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptTree))
#define LVAVS_SCRIPT_TREE_ELEMENT(obj)				(VISUAL_CHECK_CAST ((obj), 0, LVAVSScriptTreeElement))

typedef struct _LVAVSScriptEnviron LVAVSScriptEnviron;
typedef struct _LVAVSScriptEnvironFunctionTableElement LVAVSScriptEnvironFunctionTableElement;
typedef struct _LVAVSScriptEnvironFunctionTable LVAVSScriptEnvironFunctionTable;
typedef struct _LVAVSScriptEnvironVarTable LVAVSScriptEnvironVarTable;

typedef struct _LVAVSScriptVMContext LVAVSScriptVMContext;

typedef struct _LVAVSScriptTree LVAVSScriptTree;
typedef struct _LVAVSScriptTreeElement LVAVSScriptTreeElement;

typedef int (*LVAVSScriptEnvironFunctionFunc)(LVAVSScriptTreeElement *selem);

typedef enum {
	LVAVS_SCRIPT_TREE_ELEMENT_TYPE_CONSTANT,
	LVAVS_SCRIPT_TREE_ELEMENT_TYPE_VARIABLE,
	LVAVS_SCRIPT_TREE_ELEMENT_TYPE_FUNCTION
} LVAVSScriptTreeElementType;

typedef enum {
	LVAVS_SCRIPT_ENVIRON_FUNCTION_TYPE_FUNCTION,
	LVAVS_SCRIPT_ENVIRON_FUNCTION_TYPE_ARITHMIC
} LVAVSScriptEnvironFunctionType;

struct _LVAVSScriptEnviron {
	VisObject			 object;

	LVAVSScriptEnvironFunctionTable	*jumptable;
	LVAVSScriptEnvironVarTable	*vartable;
};

struct _LVAVSScriptEnvironFunctionTableElement {
	VisObject			 object;

	char				*symbol;
	int				 nargs;

	LVAVSScriptEnvironFunctionType	 type;
	LVAVSScriptEnvironFunctionFunc	 function;
};

struct _LVAVSScriptEnvironFunctionTable {
	VisObject			 	 object;
	int					 index;

	LVAVSScriptEnvironFunctionTableElement	 functions[256];
};


struct _LVAVSScriptEnvironVarTable {
	VisObject			 object;
};

struct _LVAVSScriptVMContext {
	VisObject                        object;

	LVAVSScriptEnviron		*environ;

	char                            *bytestream;
	int                              streamlength;

	int                              instrptr;
};

struct _LVAVSScriptTree {
	VisObject                 	 object;

	VisList                  	*elements;
};

struct _LVAVSScriptTreeElement {
	VisObject                        object;

	LVAVSScriptTreeElementType       type;

	char                            *data;

	VisList                         *leaves;
};

/* Prototypes */
LVAVSScriptEnviron *lvavs_script_environ_new (void);
int lvavs_script_environ_add_function (LVAVSScriptEnviron *senviron, LVAVSScriptEnvironFunctionFunc func, char *symbol, int nargs);

LVAVSScriptVMContext *lvavs_script_vmcontext_new (void);
int lvavs_script_vmcontext_set_environ (LVAVSScriptVMContext *vmctx, LVAVSScriptEnviron *senviron);
int lvavs_script_vmcontext_compile (LVAVSScriptVMContext *vmctx, LVAVSScriptTree *stree);

LVAVSScriptTree *lvavs_script_tree_new (void);
int lvavs_script_tree_add_element (LVAVSScriptTree *stree, LVAVSScriptTreeElement *selement);

LVAVSScriptTreeElement *lvavs_script_tree_element_new (LVAVSScriptTreeElementType type, char *data);
int lvavs_script_tree_element_add_leaf (LVAVSScriptTreeElement *selement, LVAVSScriptTreeElement *leaf);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_LVAVS_SCRIPT_H */
