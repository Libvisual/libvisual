/** input_mplayer.c
 *
 *    Input plugin to import data mmap()'ed from MPlayer (export audio filter)
 *
 * @author Gustavo Sverzut Barbieri <gsbarbieri@yahoo.com.br>
 * License: GNU Lesser General Public License (GNU/LGPL)
 ******************************************************************************
 * $Header: /home/starlon/Downloads/libvisual-cvs/backup/libvisual-plugins/plugins/input/mplayer/input_mplayer.c,v 1.4 2004-07-23 13:29:34 synap Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#include <libvisual/libvisual.h>

#ifndef SHARED_FILE
#define SHARED_FILE ".mplayer/mplayer-af_export" /**< default file name,
						    relative to $HOME */
#endif /* SHARED_FILE */


/* Data structures ***********************************************************/

typedef struct
{
  int nch;                  /**< number of channels */
  int bs;                   /**< buffer size */
  unsigned long long count; /**< sample counter */
} mplayer_data_t;


typedef struct 
{
  int fd;                    /**< file descriptor to mmaped area */
  char *sharedfile;          /**< shared file name */
  mplayer_data_t *mmap_area; /**< mmap()'ed area */

  int loaded;                /**< plugin state */
} mplayer_priv_t;




/* Functions *****************************************************************/
int inp_mplayer_init( VisInputPlugin *plugin );
int inp_mplayer_cleanup( VisInputPlugin *plugin );
int inp_mplayer_upload( VisInputPlugin *plugin, VisAudio *audio );

/**
 * set up plugin
 *
 * @param ref plugin reference
 *
 * @return plugin ready for use
 */
LVPlugin *get_plugin_info( VisPluginRef *ref )
{
  LVPlugin          *plugin        = NULL;
  VisInputPlugin    *mplayer_input = NULL;
  mplayer_priv_t *priv          = NULL;
	
  /* Get plugin structure */
  plugin        = visual_plugin_new();
  mplayer_input = visual_plugin_input_new();

  visual_log_return_val_if_fail( plugin != NULL, NULL );
  visual_log_return_val_if_fail( mplayer_input != NULL, NULL );

  /* Fill plugin structure */
  mplayer_input->name    = "mplayer";
  mplayer_input->init    = inp_mplayer_init;
  mplayer_input->cleanup = inp_mplayer_cleanup;
  mplayer_input->upload  = inp_mplayer_upload;

  mplayer_input->info = visual_plugin_info_new( "mplayer", 
						"Gustavo Sverzut Barbieri " \
						"<gsbarbieri@users.sourceforge.net>", 
						"$Revision: 1.4 $",
						"Use data exported from "   \
						"MPlayer", "This plugin "   \
						"uses data exported from "  \
						"'mplayer -af export'." );
  

  priv = malloc( sizeof( mplayer_priv_t ) );

  visual_log_return_val_if_fail( priv != NULL, NULL );
  memset( priv, 0, sizeof( mplayer_priv_t ) );

  priv->sharedfile = malloc( sizeof( char ) * 
			     ( strlen( SHARED_FILE ) + 
			       strlen( getenv( "HOME" ) ) + 2 ) );
  visual_log_return_val_if_fail( priv->sharedfile != NULL, NULL );
  strcpy( priv->sharedfile, getenv( "HOME" ) );
  strcat( priv->sharedfile, "/" );
  strcat( priv->sharedfile, SHARED_FILE );

  mplayer_input->priv = priv;

  plugin->type = VISUAL_PLUGIN_TYPE_INPUT;
  /* Link */
  plugin->plugin.inputplugin = mplayer_input;

  return plugin;
}



/**
 * Initialize plugin
 *
 * @param plugin plugin to be initialized.
 *
 * @return 0 on success.
 */
int inp_mplayer_init( VisInputPlugin *plugin )
{
  mplayer_priv_t *priv = NULL;

  visual_log_return_val_if_fail( plugin != NULL, -1 );
  priv = plugin->priv;

  visual_log_return_val_if_fail( priv != NULL, -1 );
  visual_log_return_val_if_fail( priv->sharedfile != NULL, -1 );

  priv->fd = open( priv->sharedfile, O_RDONLY );

  if ( priv->fd < 0 )
    {
      visual_log( VISUAL_LOG_ERROR,
		  "Could not open file '%s': %s\n",
		  priv->sharedfile, strerror( errno ) );
      return -3;
    }

  priv->mmap_area = mmap( 0, sizeof( mplayer_data_t ),
			  PROT_READ, MAP_SHARED, priv->fd, 0 );
  visual_log_return_val_if_fail( priv->mmap_area != NULL, -1 );

  if ( priv->mmap_area->nch == 0 )
    {
      visual_log( VISUAL_LOG_ERROR, "No audio channel available!\n" );
      return -5;
    }

  if ( ( priv->mmap_area->nch != 2 ) ||
       ( priv->mmap_area->bs  != 2048 ) )
    {
      visual_log( VISUAL_LOG_ERROR,
		  "Data in wrong format. It should be 2 channels" \
		  " with 512 16bit samples. There are %d channels %d 16bit " \
		  "samples in it (buffer is %d bytes).\n",
		  priv->mmap_area->nch,
		  priv->mmap_area->bs / 2 / priv->mmap_area->nch,
		  priv->mmap_area->bs );
      return -6;
    }

  priv->mmap_area = mremap( priv->mmap_area, sizeof( mplayer_data_t ),
			    sizeof( mplayer_data_t ) + priv->mmap_area->bs,
			    0 );
  if ( ! priv->mmap_area )
    {
      visual_log( VISUAL_LOG_CRITICAL, 
		  "Could not mremap() area from file '%s' " \
		  " (%p from %d to %d bytes): %s\n",
		  priv->sharedfile, 
		  priv->mmap_area, sizeof( mplayer_data_t ),
		  sizeof( mplayer_data_t ) + priv->mmap_area->bs,
		  strerror( errno ) );
      return -7;
    }  
  
  priv->loaded = 1;
  return 0;
}

/**
 * cleanup plugin (release resources)
 *
 * @param plugin plugin to be cleaned up.
 *
 * @return 0 on success.
 */
int inp_mplayer_cleanup( VisInputPlugin *plugin )
{
  int unclean = 0;
  mplayer_priv_t *priv = NULL;

  visual_log_return_val_if_fail( plugin != NULL, -1 );
  priv = plugin->priv;
  visual_log_return_val_if_fail( priv != NULL, -1 );

  if ( priv->loaded == 1 )
    {
      void *mmap_area  = (void*)priv->mmap_area;
      int   mmap_count = priv->mmap_area->bs + sizeof( mplayer_data_t );

      if ( priv->fd > 0 )
	{
	  if ( close( priv->fd ) != 0 )
	    {
	      visual_log( VISUAL_LOG_ERROR,
			  "Could not close file descriptor %d: %s\n", 
			  priv->fd, strerror( errno ) );
	      unclean |= 1;
	    }
	  priv->fd = -1;
	}
      else
	{
	  visual_log( VISUAL_LOG_ERROR, "Wrong file descriptor %d\n.", 
		      priv->fd );
	  unclean |= 2;
	}

      if ( munmap( mmap_area, mmap_count ) != 0 )
	{
	  visual_log( VISUAL_LOG_ERROR,
		      "Could not munmap() area %p+%d. %s\n",
		      mmap_area, mmap_count,
		      strerror( errno ) );
	  unclean |= 4;
	}      
    }

  free( priv );
  plugin->priv = NULL;
  
  return - unclean;
}


/**
 * upload data to libvisual
 *
 * @param plugin plugin that should upload data.
 * @param where to upload data.
 * 
 * @return 0 on success.
 */
int inp_mplayer_upload( VisInputPlugin *plugin, VisAudio *audio )
{
  mplayer_priv_t *priv = NULL;

  visual_log_return_val_if_fail( audio != NULL, -1 );
  visual_log_return_val_if_fail( plugin != NULL, -1 );
  priv = plugin->priv;
  visual_log_return_val_if_fail( priv != NULL, -1 );
  visual_log_return_val_if_fail( priv->mmap_area != NULL, -1 );

  memcpy( audio->plugpcm, 
	  ((void *)priv->mmap_area) + sizeof( mplayer_data_t ),
	  2048 );
  
  return 0;
}

