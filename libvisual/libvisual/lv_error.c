/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012-2013 Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_error.h"
#include "lv_log.h"
#include "lv_checks.h"
#include "gettext.h"
#include <stdlib.h>
#include <signal.h>

static void log_and_exit (const char *error);

static VisErrorHandlerFunc error_handler = NULL;
static void *error_handler_priv = NULL;

void visual_error_raise (const char *error)
{
	if (error_handler) {
		error_handler (error, error_handler_priv);
	} else {
		log_and_exit (error);
	}
}

void visual_error_set_handler (VisErrorHandlerFunc handler, void *priv)
{
	error_handler = handler;
	error_handler_priv = priv;
}

static void log_and_exit (const char *error)
{
	visual_log (VISUAL_LOG_CRITICAL, "Aborting due to error: %s", error);

#ifdef VISUAL_OS_POSIX
	raise (SIGTRAP);
#endif

	exit (EXIT_FAILURE);
}
