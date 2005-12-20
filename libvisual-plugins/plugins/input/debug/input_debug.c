/* Libvisual-plugins - Standard plugins for libvisual
 * 
 * Copyright (C) 2005 Dennis Smit <dsmit@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_debug.c,v 1.2 2005-12-20 18:49:14 synap Exp $
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
#include <gettext.h>

#include <libvisual/libvisual.h>

static int inp_debug_init (VisPluginData *plugin);
static int inp_debug_cleanup (VisPluginData *plugin);
static int inp_debug_upload (VisPluginData *plugin, VisAudio *audio);

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisInputPlugin input[] = {{
		.upload = inp_debug_upload
	}};

	static VisPluginInfo info[] = {{
		.struct_size = sizeof (VisPluginInfo),
		.api_version = VISUAL_PLUGIN_API_VERSION,
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "debug",
		.name = "debug",
		.author = "Vitaly V. Bursov <vitalyvb@urk.net>",
		.version = "0.1",
		.about = N_("ALSA capture plugin"),
		.help = N_("Use this plugin to capture PCM data from the ALSA record device"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,

		.init = inp_debug_init,
		.cleanup = inp_debug_cleanup,

		.plugin = VISUAL_OBJECT (&input[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

int inp_debug_init (VisPluginData *plugin)
{
	return 0;
}

int inp_debug_cleanup (VisPluginData *plugin)
{
	return 0;
}

int inp_debug_upload (VisPluginData *plugin, VisAudio *audio)
{
	int16_t data[32768];
	int i;

	for (i = 0; i < 32768; i++)
		data[i] = visual_random_int ();

	VisBuffer buffer;
	visual_buffer_init (&buffer, data, (visual_random_int () % 16384) * 2, NULL);

	visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
			VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

	return 0;
}

