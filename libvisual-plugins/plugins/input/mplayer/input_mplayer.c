/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2004, 2005, 2006 Gustavo Sverzut Barbieri <gsbarbieri@yahoo.com.br>
 *
 * Authors: Gustavo Sverzut Barbieri <gsbarbieri@yahoo.com.br>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_mplayer.c,v 1.19 2006/01/22 13:25:27 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "gettext.h"

#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include <libvisual/libvisual.h>

VISUAL_PLUGIN_API_VERSION_VALIDATOR

#ifndef SHARED_FILE
#define SHARED_FILE ".mplayer/mplayer-af_export" /**< default file name, relative to $HOME */
#endif /* SHARED_FILE */

typedef struct {
	int nch;                  /**< number of channels */
	int bs;                   /**< buffer size */
	unsigned long long count; /**< sample counter */
} mplayer_data_t;

typedef struct {
	int fd;                    /**< file descriptor to mmaped area */
	char *sharedfile;          /**< shared file name */
	mplayer_data_t *mmap_area; /**< mmap()'ed area */
	unsigned long long prev_count; /**< to detect duplicates */

	int loaded;                /**< plugin state */
} mplayer_priv_t;

static int  inp_mplayer_init    (VisPluginData *plugin);
static void inp_mplayer_cleanup (VisPluginData *plugin);
static int  inp_mplayer_upload  (VisPluginData *plugin, VisAudio *audio);

const VisPluginInfo *get_plugin_info( void )
{
	static VisInputPlugin input = {
		.upload = inp_mplayer_upload
	};

	static VisPluginInfo info = {
		.type     = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "mplayer",
		.name     = "mplayer",
		.author   = "Gustavo Sverzut Barbieri <gsbarbieri@users.sourceforge.net>",
		.version  = "1.19",
		.about    = N_("Use data exported from MPlayer"),
		.help     = N_("This plugin uses data exported from 'mplayer -af export'"),
		.license  = VISUAL_PLUGIN_LICENSE_LGPL,

		.init     = inp_mplayer_init,
		.cleanup  = inp_mplayer_cleanup,
		.plugin   = &input
	};

	return &info;
}

static int inp_mplayer_init( VisPluginData *plugin )
{
	mplayer_priv_t *priv = NULL;

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALE_DIR);
#endif

	priv = visual_mem_new0(mplayer_priv_t, 1);
	visual_plugin_set_private (plugin, priv);

	priv->sharedfile = visual_mem_malloc0( sizeof( char ) *
			( strlen( SHARED_FILE ) +
			  strlen( getenv( "HOME" ) ) + 2 ) );

	strcpy( priv->sharedfile, getenv( "HOME" ) );
	strcat( priv->sharedfile, "/" );
	strcat( priv->sharedfile, SHARED_FILE );

	visual_return_val_if_fail( priv->sharedfile != NULL, FALSE );

	priv->fd = open( priv->sharedfile, O_RDONLY );

	if ( priv->fd < 0 )
	{
		/*
		 * FIXME this will cause the application to abort,
		 * may be we must print a warning and clean all
		 * before to return the error value.
		 */
		visual_log( VISUAL_LOG_CRITICAL,
				"Could not open file '%s': %s",
				priv->sharedfile, strerror( errno ) );
		return FALSE;
	}

	priv->mmap_area = mmap( 0, sizeof( mplayer_data_t ),
			PROT_READ, MAP_SHARED, priv->fd, 0 );
	visual_return_val_if_fail( (intptr_t)priv->mmap_area != -1, FALSE );

	if ( priv->mmap_area->nch == 0 )
	{
		visual_log( VISUAL_LOG_CRITICAL, "No audio channel available" );
		return FALSE;
	}

	if ( ( priv->mmap_area->nch != 2 ) ||
			( priv->mmap_area->bs  != 2048 ) )
	{
		visual_log( VISUAL_LOG_CRITICAL,
				"Data in wrong format. It should be 2 channels" \
				" with 512 16bit samples. There are %d channels %d 16bit " \
				"samples in it (buffer is %d bytes)",
				priv->mmap_area->nch,
				priv->mmap_area->bs / 2 / priv->mmap_area->nch,
				priv->mmap_area->bs );
		return FALSE;
	}

	priv->mmap_area = mremap( priv->mmap_area, sizeof( mplayer_data_t ),
			sizeof( mplayer_data_t ) + priv->mmap_area->bs,
			0 );
	if ( (intptr_t)priv->mmap_area == -1 )
	{
		visual_log( VISUAL_LOG_CRITICAL,
				"Could not mremap() area from file '%s' " \
				" (%p from %" VISUAL_SIZE_T_FORMAT " to %" VISUAL_SIZE_T_FORMAT " bytes): %s",
				priv->sharedfile,
				(void *) priv->mmap_area, sizeof( mplayer_data_t ),
				sizeof( mplayer_data_t ) + priv->mmap_area->bs,
				strerror( errno ) );
		return FALSE;
	}
	priv->prev_count = ULLONG_MAX;

	priv->loaded = TRUE;

	return TRUE;
}

static void inp_mplayer_cleanup( VisPluginData *plugin )
{
	mplayer_priv_t *priv = visual_plugin_get_private (plugin);

	if ( priv->loaded == 1 )
	{
		void *mmap_area  = (void*)priv->mmap_area;
		int   mmap_count = priv->mmap_area->bs + sizeof( mplayer_data_t );

		if ( priv->fd > 0 )
		{
			if ( close( priv->fd ) != 0 )
			{
				visual_log( VISUAL_LOG_CRITICAL,
						"Could not close file descriptor %d: %s",
						priv->fd, strerror( errno ) );
			}
			priv->fd = -1;
		}
		else
		{
			visual_log( VISUAL_LOG_CRITICAL, "Wrong file descriptor %d",
					priv->fd );
		}

		if ( munmap( mmap_area, mmap_count ) != 0 )
		{
			visual_log( VISUAL_LOG_CRITICAL,
					"Could not munmap() area %p+%d. %s",
					mmap_area, mmap_count,
					strerror( errno ) );
		}
	}

	visual_mem_free( priv->sharedfile );
	visual_mem_free( priv );
}

static int inp_mplayer_upload( VisPluginData *plugin, VisAudio *audio )
{
	mplayer_priv_t *priv = visual_plugin_get_private (plugin);

	// NOTE: This avoids accounting for the same samples multiple times
	if (priv->mmap_area->count == priv->prev_count) {
	    return 0;
	}
	priv->prev_count = priv->mmap_area->count;

	VisBuffer *buffer = visual_buffer_new_wrap_data ( (uint8_t *)priv->mmap_area + sizeof( mplayer_data_t ), 2048, FALSE );
	visual_audio_input (audio, buffer,
	                    VISUAL_AUDIO_SAMPLE_RATE_44100,
	                    VISUAL_AUDIO_SAMPLE_FORMAT_S16,
	                    VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
	visual_buffer_unref (buffer);

	return TRUE;
}
