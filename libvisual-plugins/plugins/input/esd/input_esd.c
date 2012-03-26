/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_esd.c,v 1.17 2006/01/22 13:25:27 synap Exp $
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

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include <gettext.h>

#include <esd.h>

#include <libvisual/libvisual.h>

#define PCM_BUF_SIZE	4096

const VisPluginInfo *get_plugin_info (int *count);

typedef struct {
	int esdhandle;
} EsdPrivate;

static int inp_esd_init (VisPluginData *plugin);
static int inp_esd_cleanup (VisPluginData *plugin);
static int inp_esd_upload (VisPluginData *plugin, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisInputPlugin input[] = {{
		.upload = inp_esd_upload
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "esd",
		.name = "esd",
		.author = "Dennis Smit <ds@nerds-incorporated.org>",
		.version = "0.1",
		.about = N_("ESOUND capture plugin"),
		.help = N_("Use this plugin to capture PCM data from the ESD daemon"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = inp_esd_init,
		.cleanup = inp_esd_cleanup,

		.plugin = VISUAL_OBJECT (&input[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

static int inp_esd_init (VisPluginData *plugin)
{
	EsdPrivate *priv;

	visual_return_val_if_fail (plugin != NULL, -1);

#if ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, LOCALEDIR);
#endif

	priv = visual_mem_new0 (EsdPrivate, 1);
	visual_return_val_if_fail (priv != NULL, -1);
	visual_object_set_private (VISUAL_OBJECT (plugin), priv);

	priv->esdhandle = esd_monitor_stream (ESD_BITS16 | ESD_STEREO | ESD_STREAM | ESD_MONITOR, 44100, NULL, "lv_esd_plugin");

	if (priv->esdhandle <= 0)
		return -1;

	return 0;
}

static int inp_esd_cleanup (VisPluginData *plugin)
{
	EsdPrivate *priv = visual_object_get_private (VISUAL_OBJECT (plugin));

	visual_return_val_if_fail( plugin != NULL, -1 );
	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_return_val_if_fail( priv != NULL, -1 );

	esd_close (priv->esdhandle);

	visual_mem_free (priv);

	return 0;
}

static int inp_esd_upload (VisPluginData *plugin, VisAudio *audio)
{
	EsdPrivate *priv = NULL;
	short data[PCM_BUF_SIZE];
	struct timeval tv;
	int r;

	visual_return_val_if_fail(audio != NULL, -1);
	visual_return_val_if_fail(plugin != NULL, -1);
	priv = visual_object_get_private (VISUAL_OBJECT (plugin));
	visual_return_val_if_fail(priv != NULL, -1);

	do {
		fd_set rdset;

		FD_ZERO (&rdset);
		FD_SET (priv->esdhandle, &rdset);

		tv.tv_sec = 0;
		tv.tv_usec = 10000;

		r = select (priv->esdhandle + 1, &rdset, NULL, NULL, &tv);

		/* Select time out */
		if (r == 0)
			return -1;

		if (r < 0) {
			visual_log (VISUAL_LOG_ERROR, _("ESD: Select error (%d, %s)"), errno, strerror (errno));

			return -1;
		}

		r = read (priv->esdhandle, data, PCM_BUF_SIZE);

		if (r > 0) {
			VisBuffer buffer;

			visual_buffer_init (&buffer, data, r, NULL);

			visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
					VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);
		}

		if (r < 0) {
			visual_log (VISUAL_LOG_ERROR, _("ESD: Error while reading data"));

			return -1;
		}

	} while (r > 0);

	return 0;
}

