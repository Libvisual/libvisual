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
static int lvavs_script_vmcontext_dtor (VisObject *object);


/* Object destructors */
static int lvavs_script_vmcontext_dtor (VisObject *object)
{
	LVAVSScriptVMContext *vmctx = LVAVS_SCRIPT_VMCONTEXT (object);

	
	return VISUAL_OK;
}

LVAVSScriptVMContext *lvavs_script_vmcontext_new (void)
{
	LVAVSScriptVMContext *vmctx;

	vmctx = visual_mem_new0 (LVAVSScriptVMContext, 1);

	/* Do the VisObject initialization */
	visual_object_initialize (VISUAL_OBJECT (vmctx), TRUE, lvavs_script_vmcontext_dtor);

	return vmctx;
}

int lvavs_script_vmcontext_set_environ (LVAVSScriptVMContext *vmctx, LVAVSScriptEnviron *senviron)
{
	vmctx->environ = senviron;

	return 0;
}

int lvavs_script_vmcontext_compile (LVAVSScriptVMContext *vmctx, LVAVSScriptTree *stree)
{
	VisListEntry *le = NULL;
	LVAVSScriptTreeElement *selem;

	while ((selem = visual_list_next (stree->elements, &le)) != NULL) {
		printf ("VMCTX COMPILE LINE START: %s\n", selem->data);
		script_vmcontext_compile_tree (vmctx, selem);
	}

	return 0;
}

int script_vmcontext_compile_tree (LVAVSScriptVMContext *vmctx, LVAVSScriptTreeElement *selem)
{
	VisListEntry *le = NULL;
	LVAVSScriptTreeElement *sm;

	while ((sm = visual_list_next (selem->leaves, &le)) != NULL) {
		printf ("\tVMCTX COMPILE: %s\n", sm->data);
		
		script_vmcontext_compile_tree (vmctx, sm);
	}

	return 0;
}

