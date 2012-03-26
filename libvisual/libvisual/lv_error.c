/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_error.c,v 1.39 2006/01/27 20:18:26 synap Exp $
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

	[VISUAL_ERROR_AUDIO_NULL] =			N_("VisAudio is NULL"),
	[VISUAL_ERROR_AUDIO_SAMPLEPOOL_NULL] =		N_("The VisAudioSamplePool is NULL"),
	[VISUAL_ERROR_AUDIO_SAMPLEPOOL_CHANNEL_NULL] =	N_("The VisAudioSamplePoolChannel is NULL"),
	[VISUAL_ERROR_AUDIO_SAMPLE_NULL] =		N_("The VisAudioSample is NULL"),

	[VISUAL_ERROR_BMP_NO_BMP] =			N_("Bitmap is not a bitmap file"),
	[VISUAL_ERROR_BMP_NOT_FOUND] =			N_("Bitmap can not be found"),
	[VISUAL_ERROR_BMP_NOT_SUPPORTED] =		N_("Bitmap is not supported"),
	[VISUAL_ERROR_BMP_CORRUPTED] =			N_("Bitmap is corrupted"),

	[VISUAL_ERROR_BUFFER_NULL] =			N_("VisBuffer is NULL"),
	[VISUAL_ERROR_BUFFER_OUT_OF_BOUNDS] =		N_("The requested operation on the VisBuffer is out of bounds"),

	[VISUAL_ERROR_CACHE_NULL] =			N_("The VisCache is NULL"),

	[VISUAL_ERROR_COLLECTION_NULL] =		N_("VisCollection is NULL"),
	[VISUAL_ERROR_COLLECTION_ITER_NULL] =		N_("VisCollectionIter is NULL"),

	[VISUAL_ERROR_COLOR_NULL] =			N_("VisColor is NULL"),

	[VISUAL_ERROR_CONFIG_REGISTRY_NULL] =		N_("VisConfigRegistry is NULL"),
	[VISUAL_ERROR_CONFIG_REGISTRY_SECTION_NULL] =	N_("VisConfigRegistrySection is NULL"),

	[VISUAL_ERROR_CPU_INVALID_CODE] =		N_("The code can not run on this architecture"),
	[VISUAL_ERROR_CPU_FEATURE_NOT_SUPPORTED] =	N_("CPU feature is not supported"),

	[VISUAL_ERROR_ERROR_HANDLER_NULL] =		N_("Global error handler is NULL"),

	[VISUAL_ERROR_EVENT_NULL] =			N_("VisEvent is NULL"),
	[VISUAL_ERROR_EVENT_QUEUE_NULL] =		N_("VisEventQueue is NULL"),

	[VISUAL_ERROR_FOURIER_NULL] =			N_("VisFourier is NULL"),
	[VISUAL_ERROR_FOURIER_NOT_INITIALIZED]	=	N_("The VisFourier subsystem is not initialized"),

	[VISUAL_ERROR_GL_FUNCTION_NOT_SUPPORTED] =	N_("The native callback for a gl related function is not present."),

	[VISUAL_ERROR_HASHLIST_NULL] =			N_("The VisHashlist is NULL"),

	[VISUAL_ERROR_HASHMAP_NULL] =			N_("VisHashmap is NULL"),
	[VISUAL_ERROR_HASHMAP_NOT_IN_MAP] =		N_("Key is not in hashmap"),
	[VISUAL_ERROR_HASHMAP_INVALID_KEY_TYPE] =	N_("Key type is not valid"),

	[VISUAL_ERROR_INPUT_NULL] =			N_("VisInput is NULL"),
	[VISUAL_ERROR_INPUT_PLUGIN_NULL] =		N_("VisInput it's plugin is NULL"),

	[VISUAL_ERROR_LIBVISUAL_NO_PATHS] =		N_("No paths were given to seek for plugins"),
	[VISUAL_ERROR_LIBVISUAL_ALREADY_INITIALIZED] =	N_("Libvisual is already initialized"),
	[VISUAL_ERROR_LIBVISUAL_NOT_INITIALIZED] =	N_("Libvisual is not initialized"),
	[VISUAL_ERROR_LIBVISUAL_NO_REGISTRY] =		N_("Libvisual has not build a plugin registry"),

	[VISUAL_ERROR_LIST_NULL] =			N_("VisList is NULL"),
	[VISUAL_ERROR_LIST_ENTRY_NULL] =		N_("VisListEntry is NULL"),
	[VISUAL_ERROR_LIST_ENTRY_INVALID] =		N_("VisListEntry is invalid"),

	[VISUAL_ERROR_MEM_NULL] =			N_("Given memory pointer is NULL"),

	[VISUAL_ERROR_MORPH_NULL] =			N_("VisMorph is NULL"),
	[VISUAL_ERROR_MORPH_PLUGIN_NULL] =		N_("VisMorph it's plugin is NULL"),

	[VISUAL_ERROR_OS_SCHED] =			N_("The scheduler related call wasn't succesful."),
	[VISUAL_ERROR_OS_SCHED_NOT_SUPPORTED] =		N_("Scheduler operations are not supported on the platform."),

	[VISUAL_ERROR_PALETTE_NULL] =			N_("VisPalette is NULL"),
	[VISUAL_ERROR_PALETTE_SIZE] =			N_("VisPalette it's size conflicts"),

	[VISUAL_ERROR_PARAM_NULL] =			N_("VisParamEntry is NULL"),
	[VISUAL_ERROR_PARAM_CONTAINER_NULL] =		N_("VisParamContainer is NULL"),
	[VISUAL_ERROR_PARAM_NOT_FOUND] =		N_("VisParamEntry not found in VisParamContainer"),
	[VISUAL_ERROR_PARAM_CALLBACK_NULL] =		N_("VisParamEntry it's change notify callback is NULL"),
	[VISUAL_ERROR_PARAM_CALLBACK_TOO_MANY] =	N_("VisParamEntry contains too many change notify callbacks"),
	[VISUAL_ERROR_PARAM_INVALID_TYPE] =		N_("VisParamEntry is of invalid type"),

	[VISUAL_ERROR_PLUGIN_NULL] =			N_("VisPluginData is NULL"),
	[VISUAL_ERROR_PLUGIN_INFO_NULL] =		N_("VisPluginInfo is NULL"),
	[VISUAL_ERROR_PLUGIN_REF_NULL] =		N_("VisPluginRef is NULL"),
	[VISUAL_ERROR_PLUGIN_ENVIRON_NULL] =		N_("VisPluginEnvironElement is NULL"),
	[VISUAL_ERROR_PLUGIN_NO_EVENT_HANDLER] =	N_("Plugin does not have an event handler"),
	[VISUAL_ERROR_PLUGIN_HANDLE_NULL] =		N_("Plugin handle is NULL"),
	[VISUAL_ERROR_PLUGIN_ALREADY_REALIZED] =	N_("Plugin is already realized"),
	[VISUAL_ERROR_PLUGIN_NO_LIST] =			N_("Plugin list can not be found in memory"),
	[VISUAL_ERROR_PLUGIN_NOT_FOUND] =       N_("Plugin cannot be found"),

	[VISUAL_ERROR_RANDOM_CONTEXT_NULL] =		N_("VisRandomContext is NULL"),

	[VISUAL_ERROR_RECTANGLE_NULL] =			N_("VisRectangle is NULL"),
	[VISUAL_ERROR_RECTANGLE_OUT_OF_BOUNDS] =	N_("The VisRectangle operation is out of bounds"),

	[VISUAL_ERROR_RINGBUFFER_NULL] =		N_("The VisRingBuffer is NULL"),
	[VISUAL_ERROR_RINGBUFFER_ENTRY_NULL] =		N_("The VisRingBufferEntry is NULL"),
	[VISUAL_ERROR_RINGBUFFER_DATAFUNC_NULL] =	N_("The VisRingBufferDataFunc data provider function callback is NULL"),

	[VISUAL_ERROR_SONGINFO_NULL] =			N_("VisSongInfo is NULL"),

	[VISUAL_ERROR_THREAD_NULL] =			N_("VisThread is NULL"),
	[VISUAL_ERROR_THREAD_NOT_INITIALIZED] =		N_("The VisThread subsystem is not initialized"),
	[VISUAL_ERROR_THREAD_NOT_SUPPORTED] =		N_("The VisThread subsystem couldn't find any threading model to use"),
	[VISUAL_ERROR_THREAD_NOT_ENABLED] =		N_("The VisThread subsystem is not enabled"),

	[VISUAL_ERROR_MUTEX_NULL] =			N_("VisMutex is NULL"),
	[VISUAL_ERROR_MUTEX_LOCK_FAILURE] =		N_("VisMutex lock failed"),
	[VISUAL_ERROR_MUTEX_TRYLOCK_FAILURE] =		N_("VisMutex trylock failed"),
	[VISUAL_ERROR_MUTEX_UNLOCK_FAILURE] =		N_("VisMutex unlock failed"),

	[VISUAL_ERROR_TRANSFORM_NULL] =			N_("VisTransform is NULL"),
	[VISUAL_ERROR_TRANSFORM_NEGOTIATE] =		N_("The VisTransform negotiate with the target VisVideo failed"),
	[VISUAL_ERROR_TRANSFORM_PLUGIN_NULL] =		N_("The VisTransform it's plugin is NULL"),
	[VISUAL_ERROR_TRANSFORM_VIDEO_NULL] =		N_("The VisTransform it's video is NULL"),
	[VISUAL_ERROR_TRANSFORM_PALETTE_NULL] =		N_("The VisTransform it's palette is NULL"),

	[VISUAL_ERROR_OBJECT_DTOR_FAILED] =		N_("VisObject destruction failed"),
	[VISUAL_ERROR_OBJECT_NULL] =			N_("VisObject is NULL"),
	[VISUAL_ERROR_OBJECT_NOT_ALLOCATED] =		N_("VisObject is not allocated"),

	[VISUAL_ERROR_TIME_NULL] =			N_("VisTime is NULL"),
	[VISUAL_ERROR_TIME_NO_USLEEP] =			N_("visual_time_usleep() is not supported"),
	[VISUAL_ERROR_TIMER_NULL] =			N_("VisTimer is NULL"),

	[VISUAL_ERROR_VIDEO_ATTRIBUTE_OPTIONS_NULL] =	N_("The VisVideoAttributeOptions is NULL."),
	[VISUAL_ERROR_VIDEO_NULL] =			N_("VisVideo is NULL"),
	[VISUAL_ERROR_VIDEO_HAS_ALLOCATED] =		N_("VisVideo has allocated pixel buffer"),
	[VISUAL_ERROR_VIDEO_BUFFER_NULL] =		N_("VisVideo has no VisBuffer object"),
	[VISUAL_ERROR_VIDEO_PIXELS_NULL] =		N_("VisVideo it's pixel buffer is NULL"),
	[VISUAL_ERROR_VIDEO_NO_ALLOCATED] =		N_("VisVideo it's pixel buffer is not allocated"),
	[VISUAL_ERROR_VIDEO_HAS_PIXELS] =		N_("VisVideo has pixel buffer"),
	[VISUAL_ERROR_VIDEO_INVALID_BPP] =		N_("VisVideo is of invalid bytes per pixel"),
	[VISUAL_ERROR_VIDEO_INVALID_DEPTH] =		N_("VisVideo is of invalid depth"),
	[VISUAL_ERROR_VIDEO_INVALID_SCALE_METHOD] =	N_("Invalid scale method given"),
	[VISUAL_ERROR_VIDEO_INVALID_ROTATE] =		N_("Invalid rotate degrees given"),
	[VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS] =		N_("Given coordinates are out of bounds"),
	[VISUAL_ERROR_VIDEO_NOT_INDENTICAL] =		N_("Given VisVideos are not indentical"),
	[VISUAL_ERROR_VIDEO_NOT_TRANSFORMED] =		N_("VisVideo is not depth transformed as requested")
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

#if !defined(VISUAL_OS_WIN32)
	raise (SIGTRAP);
#endif

	exit (EXIT_FAILURE);
}
