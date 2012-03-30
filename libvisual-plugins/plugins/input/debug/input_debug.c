/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 2005 Dennis Smit <dsmit@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: input_debug.c,v 1.3 2005/12/22 21:50:10 synap Exp $
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
#include <math.h>
#include <string.h>
#include <gettext.h>

#include <libvisual/libvisual.h>

const VisPluginInfo *get_plugin_info (int *count);

static int inp_debug_init (VisPluginData *plugin);
static int inp_debug_cleanup (VisPluginData *plugin);
static int inp_debug_upload (VisPluginData *plugin, VisAudio *audio);

VISUAL_PLUGIN_API_VERSION_VALIDATOR

const VisPluginInfo *get_plugin_info (int *count)
{
	static VisInputPlugin input[] = {{
		.upload = inp_debug_upload
	}};

	static VisPluginInfo info[] = {{
		.type = VISUAL_PLUGIN_TYPE_INPUT,

		.plugname = "debug",
		.name = "debug",
		.author = "Vitaly V. Bursov <vitalyvb@urk.net>",
		.version = "0.2",
		.about = N_("debug input plugin"),
		.help = N_("this will generate a sine wave for debugging purposes"),
		.license = VISUAL_PLUGIN_LICENSE_LGPL,
		.init = inp_debug_init,
		.cleanup = inp_debug_cleanup,
		.plugin = VISUAL_OBJECT (&input[0])
	}};

	*count = sizeof (info) / sizeof (*info);

	return info;
}

static int inp_debug_init (VisPluginData *plugin)
{
	return 0;
}

static int inp_debug_cleanup (VisPluginData *plugin)
{
	return 0;
}

static int inp_debug_upload (VisPluginData *plugin, VisAudio *audio)
{
#define STEP 0.628318
#define AMPLITUDE 65535/2

	int16_t data[4096];
	int i;
	static double count = 0;

	for(i = 0; i < VISUAL_TABLESIZE(data); i++) {
		data[i] = (int16_t) (AMPLITUDE/2*sin(count));
		count += STEP;
	}

	VisBuffer buffer;
	visual_buffer_init (&buffer, data, VISUAL_TABLESIZE(data), NULL);

	visual_audio_samplepool_input (audio->samplepool, &buffer, VISUAL_AUDIO_SAMPLE_RATE_44100,
			VISUAL_AUDIO_SAMPLE_FORMAT_S16, VISUAL_AUDIO_SAMPLE_CHANNEL_STEREO);

	return 0;
}

