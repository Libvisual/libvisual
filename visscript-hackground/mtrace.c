/* 	Rusty - The shiny irc client
 * 	Copyright (C) 2002,2003 Dennis Smit <synap@nerds-incorporated.org>,
 *			  	Sepp Wijnands <mrrazz@nerds-incorporated.org>,
 * 			   	Tom Wimmenhove <nohup@nerds-incorporated.org>
 * 
 * 	$Id: mtrace.c,v 1.1.1.1 2006-02-09 20:18:01 razziana Exp $
 *
 *		                            d8P
 *      		                  d888888P
 *		  88bd88b?88   d8P .d888b,  ?88'  ?88   d8P
 *		  88P'  `d88   88  ?8b,     88P   d88   88
 *		 d88     ?8(  d88    `?8b   88b   ?8(  d88
 *		d88'     `?88P'?8b`?888P'   `?8b  `?88P'?8b
 *      	 	                                 )88
 *              	 	                        ,d8P
 *                      	 	             `?888P'
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#define	_MALLOC_INTERNAL
#include <malloc.h>
#include <mcheck.h>
#include <bits/libc-lock.h>

#include <dlfcn.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static char tracebuffer[4096];
static FILE *tracefp;

/* Old hook values.  */
static void (*tr_old_free_hook)();
static void * (*tr_old_malloc_hook)();
static void * (*tr_old_realloc_hook)();

static void tr_freehook (void * ptr, const void * caller)
{
	fprintf(tracefp, "0x%08x 0x%08x -\n", ptr, caller);
	
	if (ptr == NULL)
		return;
	/* Be sure to print it first.  */
	__free_hook = tr_old_free_hook;
	if (tr_old_free_hook != NULL)
		(*tr_old_free_hook) (ptr, caller);
	else
		free (ptr);
	__free_hook = tr_freehook;
}

static void * tr_mallochook (size_t size, const void * caller)
{	 void * hdr;

	__malloc_hook = tr_old_malloc_hook;
	if (tr_old_malloc_hook != NULL)
		hdr = (*tr_old_malloc_hook) (size, caller);
	else
		hdr = malloc (size);
	__malloc_hook = tr_mallochook;

	/* We could be printing a NULL here; that's OK.  */
	fprintf(tracefp, "0x%08x 0x%08x + 0x%08x\n", hdr, caller, size);
	return hdr;
}

static void * tr_reallochook (void * ptr, size_t size, void * caller)
{	void * hdr;

	__free_hook = tr_old_free_hook;
	__malloc_hook = tr_old_malloc_hook;
	__realloc_hook = tr_old_realloc_hook;
	if (tr_old_realloc_hook != NULL)
		hdr = (*tr_old_realloc_hook) (ptr, size, caller);
	else
		hdr = realloc (ptr, size);
	__free_hook = tr_freehook;
	__malloc_hook = tr_mallochook;
	__realloc_hook = tr_reallochook;
	
	if (hdr == NULL)
	/* Failed realloc.  */
		fprintf(tracefp, "0x%08x 0x%08x -\n", ptr, caller);
	else if (ptr == NULL) 
		fprintf(tracefp, "0x%08x 0x%08x + 0x%08x\n", hdr, caller, size);
	else {
		if (ptr != hdr) {
			fprintf(tracefp, "0x%08x 0x%08x -\n", ptr, caller);
			fprintf(tracefp, "0x%08x 0x%08x + 0x%08x\n", hdr, caller, size);
		} else 
			fprintf(tracefp, "0x%08x 0x%08x > 0x%08x\n", hdr, caller, size);
    	}

	return hdr;
}

void rusty_mtrace (char *tracefile)
{
	tracefp = fopen(tracefile, "w");
	if (tracefp == NULL)
		return;
	
	/* Be sure it doesn't malloc its buffer!  */
	setvbuf (tracefp, tracebuffer, _IOLBF, sizeof(tracebuffer));
	
	tr_old_free_hook = __free_hook;
	__free_hook = tr_freehook;
	tr_old_malloc_hook = __malloc_hook;
	__malloc_hook = tr_mallochook;
	tr_old_realloc_hook = __realloc_hook;
	__realloc_hook = tr_reallochook;
}

