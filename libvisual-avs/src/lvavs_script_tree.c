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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <libvisual/libvisual.h>

#include "lvavs_script.h"

/* Prototypes */
static int lvavs_script_tree_dtor (VisObject *object);
static int lvavs_script_tree_element_dtor (VisObject *object);


/* Object destructors */
static int lvavs_script_tree_dtor (VisObject *object)
{
	LVAVSScriptTree *script_tree = LVAVS_SCRIPT_TREE (object);

	if (script_tree->elements != NULL)
		visual_object_unref (VISUAL_OBJECT (script_tree->elements));
	
	return VISUAL_OK;
}

static int lvavs_script_tree_element_dtor (VisObject *object)
{
	LVAVSScriptTreeElement *element = LVAVS_SCRIPT_TREE_ELEMENT (object);

	if (element->data != NULL)
		visual_mem_free (element->data);

	if (element->leaves != NULL)
		visual_object_unref (VISUAL_OBJECT (element->leaves));

	element->data = NULL;

	return VISUAL_OK;
}


LVAVSScriptTree *lvavs_script_tree_new (void)
{
	LVAVSScriptTree *stree;

	stree = visual_mem_new0 (LVAVSScriptTree, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (stree), TRUE, lvavs_script_tree_dtor);

	stree->elements = visual_list_new (visual_object_list_destroyer);

	return stree;
}

int lvavs_script_tree_add_element (LVAVSScriptTree *stree, LVAVSScriptTreeElement *selement)
{
	visual_list_add (stree->elements, selement);

	return 0;
}

LVAVSScriptTreeElement *lvavs_script_tree_element_new (LVAVSScriptTreeElementType type, char *data)
{
	LVAVSScriptTreeElement *selement;

	selement = visual_mem_new0 (LVAVSScriptTreeElement, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (selement), TRUE, lvavs_script_tree_element_dtor);

	selement->type = type;
	selement->data = strdup (data);
	selement->leaves = visual_list_new (visual_object_list_destroyer);

	return selement;
}

int lvavs_script_tree_element_add_leaf (LVAVSScriptTreeElement *selement, LVAVSScriptTreeElement *leaf)
{
	visual_list_add (selement->leaves, leaf);

	return 0;
}

