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
static int lvavs_script_environ_dtor (VisObject *object);
static int lvavs_script_environ_functiontable_dtor (VisObject *object);
static int lvavs_script_environ_functiontableelement_dtor (VisObject *object);
static int lvavs_script_environ_vartable_dtor (VisObject *object);

int environ_function_table_add_standards (LVAVSScriptEnviron *senviron);

/* Object destructors */
static int lvavs_script_environ_dtor (VisObject *object)
{
	LVAVSScriptEnviron *senviron = LVAVS_SCRIPT_ENVIRON (object);

	if (senviron->jumptable != NULL)
		visual_object_unref (VISUAL_OBJECT (senviron->jumptable));
	
	if (senviron->vartable != NULL)
		visual_object_unref (VISUAL_OBJECT (senviron->vartable));


	senviron->jumptable = NULL;
	senviron->vartable = NULL;

	return VISUAL_OK;
}

static int lvavs_script_environ_functiontable_dtor (VisObject *object)
{
	LVAVSScriptEnvironFunctionTable *ftable = LVAVS_SCRIPT_ENVIRON_FUNCTIONTABLE (object);
	int i;

	for (i = 0; i < 256; i++)
		visual_object_unref (VISUAL_OBJECT (&ftable->functions[i]));

	return VISUAL_OK;
}

static int lvavs_script_environ_functiontableelement_dtor (VisObject *object)
{
	LVAVSScriptEnvironFunctionTableElement *felem = LVAVS_SCRIPT_ENVIRON_FUNCTIONTABLEELEMENT (object);

	if (felem->symbol != NULL)
		visual_mem_free (felem->symbol);

	felem->symbol = NULL;

	return 0;
}

static int lvavs_script_environ_vartable_dtor (VisObject *object)
{
	LVAVSScriptEnvironVarTable *vtable = LVAVS_SCRIPT_ENVIRON_VARTABLE (object);
	
	return VISUAL_OK;
}


LVAVSScriptEnviron *lvavs_script_environ_new ()
{
	LVAVSScriptEnviron *senviron;
	int i;

	senviron = visual_mem_new0 (LVAVSScriptEnviron, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (senviron), TRUE, lvavs_script_environ_dtor);

	senviron->jumptable = visual_mem_new0 (LVAVSScriptEnvironFunctionTable, 1);
	visual_object_initialize (VISUAL_OBJECT (senviron), TRUE, lvavs_script_environ_functiontable_dtor);

	for (i = 0; i < 256; i++)
		visual_object_initialize (VISUAL_OBJECT (&senviron->jumptable->functions[i]), FALSE,
				lvavs_script_environ_functiontableelement_dtor);
	
	environ_function_table_add_standards (senviron);
	
	senviron->vartable = visual_mem_new0 (LVAVSScriptEnvironVarTable, 1);
	visual_object_initialize (VISUAL_OBJECT (senviron), TRUE, lvavs_script_environ_vartable_dtor);

	return senviron;
}

int lvavs_script_environ_add_function (LVAVSScriptEnviron *senviron, LVAVSScriptEnvironFunctionFunc func,
		char *symbol, int nargs, LVAVSScriptEnvironFunctionType type)
{
	int index = senviron->jumptable->index;
	LVAVSScriptEnvironFunctionTableElement *felem;

	/* Function table is full, bummer */
	if (index >= 256)
		return -1;

	felem = &senviron->jumptable->functions[index];

	felem->symbol = strdup (symbol);
	felem->nargs = nargs;
	felem->type = type;
	felem->function = func;

	return 0;
}

int environ_function_table_add_standards (LVAVSScriptEnviron *senviron)
{
/*	lvavs_script_environ_add_function (senviron, lvavs_script_vmrun_callback_assign, "=", 2,
			LVAVS_SCRIPT_ENVIRON_FUNCTION_TYPE_ARITHMIC);

	lvavs_script_environ_add_function (senviron, lvavs_script_vmrun_callback_plus, "+", 2,
			LVAVS_SCRIPT_ENVIRON_FUNCTION_TYPE_ARITHMIC);
*/
	return 0;
}
