/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
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


static const char *__lv_error_human_readable[] = {
	[VISUAL_OK] =					N_("There was no error"),

	[VISUAL_ERROR_GENERAL] =			N_("General error occurred"),
	[VISUAL_ERROR_NULL] =				N_("General NULL pointer error"),
	[VISUAL_ERROR_IMPOSSIBLE] =			N_("An impossible event occurred"),
	[VISUAL_ERROR_FAILED_CHECK] =       N_("Assertion check failed"),

	[VISUAL_ERROR_ACTOR_NULL] =			N_ ("VisActor is NULL"),
	[VISUAL_ERROR_ACTOR_VIDEO_NULL] =		N_("VisActor it's video is NULL"),
	[VISUAL_ERROR_ACTOR_PLUGIN_NULL] =		N_("VisActor it's plugin is NULL"),
	[VISUAL_ERROR_ACTOR_GL_NEGOTIATE] =		N_("VisActor failed while trying to forcefully negotiate a GL surface"),

	[VISUAL_ERROR_COLLECTION_NULL] =		N_("VisCollection is NULL"),
	[VISUAL_ERROR_COLLECTION_ITER_NULL] =		N_("VisCollectionIter is NULL"),

	[VISUAL_ERROR_ERROR_HANDLER_NULL] =		N_("Global error handler is NULL"),

	[VISUAL_ERROR_INPUT_NULL] =			N_("VisInput is NULL"),
	[VISUAL_ERROR_INPUT_PLUGIN_NULL] =		N_("VisInput it's plugin is NULL"),

	[VISUAL_ERROR_LIST_NULL] =			N_("VisList is NULL"),
	[VISUAL_ERROR_LIST_ENTRY_NULL] =		N_("VisListEntry is NULL"),
	[VISUAL_ERROR_LIST_ENTRY_INVALID] =		N_("VisListEntry is invalid"),

	[VISUAL_ERROR_MORPH_NULL] =			N_("VisMorph is NULL"),
	[VISUAL_ERROR_MORPH_PLUGIN_NULL] =		N_("VisMorph it's plugin is NULL"),

	[VISUAL_ERROR_OS_SCHED] =			N_("The scheduler related call wasn't succesful."),
	[VISUAL_ERROR_OS_SCHED_NOT_SUPPORTED] =		N_("Scheduler operations are not supported on the platform."),

	[VISUAL_ERROR_PLUGIN_NULL] =			N_("VisPluginData is NULL"),
	[VISUAL_ERROR_PLUGIN_INFO_NULL] =		N_("VisPluginInfo is NULL"),
	[VISUAL_ERROR_PLUGIN_REF_NULL] =		N_("VisPluginRef is NULL"),
	[VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER] =	N_("Plugin does not have an event handler"),
	[VISUAL_ERROR_PLUGIN_HANDLE_NULL] =		N_("Plugin handle is NULL"),
	[VISUAL_ERROR_PLUGIN_ALREADY_REALIZED] =	N_("Plugin is already realized"),
	[VISUAL_ERROR_PLUGIN_NO_LIST] =			N_("Plugin list can not be found in memory"),
	[VISUAL_ERROR_PLUGIN_NOT_FOUND] =       N_("Plugin cannot be found"),

	[VISUAL_ERROR_TRANSFORM_NULL] =			N_("VisTransform is NULL"),
	[VISUAL_ERROR_TRANSFORM_NEGOTIATE] =		N_("The VisTransform negotiate with the target VisVideo failed"),
	[VISUAL_ERROR_TRANSFORM_PLUGIN_NULL] =		N_("The VisTransform it's plugin is NULL"),
	[VISUAL_ERROR_TRANSFORM_VIDEO_NULL] =		N_("The VisTransform it's video is NULL"),
	[VISUAL_ERROR_TRANSFORM_PALETTE_NULL] =		N_("The VisTransform it's palette is NULL"),

	[VISUAL_ERROR_OBJECT_DTOR_FAILED] =		N_("VisObject destruction failed"),
	[VISUAL_ERROR_OBJECT_NULL] =			N_("VisObject is NULL"),
	[VISUAL_ERROR_OBJECT_NOT_ALLOCATED] =		N_("VisObject is not allocated"),

	[VISUAL_ERROR_VIDEO_INVALID_DEPTH] =		N_("VisVideo is of invalid depth"),
};

static int log_and_exit (int error);

static VisErrorHandlerFunc error_handler = NULL;
static void *error_handler_priv = NULL;

int visual_error_raise (int error)
{
	if (error_handler != NULL) {
		return error_handler (error, error_handler_priv);
	} else {
		return log_and_exit (error);
	}
}

void visual_error_set_handler (VisErrorHandlerFunc handler, void *priv)
{
	error_handler = handler;
	error_handler_priv = priv;
}

const char *visual_error_to_string (int error)
{
	error = abs (error);

	if (error < VISUAL_ERROR_LIST_END) {
		return _(__lv_error_human_readable[error]);
	} else {
		return _("Unknown error");
	}
}

static int log_and_exit (int error)
{
	visual_log (VISUAL_LOG_CRITICAL, "Aborting due to error: %s",
		visual_error_to_string (error));

#ifdef VISUAL_OS_POSIX
	raise (SIGTRAP);
#endif

	exit (EXIT_FAILURE);
}
