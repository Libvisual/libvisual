/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_sound.c,v 1.3 2006-09-19 19:05:46 synap Exp $
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

#include <math.h>

#include <libvisual/libvisual.h>

#include "avs_sound.h"


int lvavs_sound_get_from_source (VisAudio *audio, float ***data)
{
    VisBuffer pcmbuf1;
    VisBuffer pcmbuf2;
    VisBuffer spmbuf1;
    VisBuffer spmbuf2;
    VisBuffer tmp;
    
    visual_buffer_init_allocate(&tmp, sizeof(float) * 1024, visual_buffer_destroyer_free);

    /* Left audio */
    visual_buffer_set_data_pair(&pcmbuf1, data[0][0], sizeof(float) * 1024);

    if(visual_audio_get_sample(audio, &tmp, VISUAL_AUDIO_CHANNEL_LEFT) == VISUAL_OK)

        visual_audio_sample_buffer_mix(&pcmbuf1, &tmp, TRUE, 1.0);

    visual_buffer_set_data_pair(&spmbuf1, &data[1][0], sizeof(float) * 1024);

    visual_audio_get_spectrum_for_sample (&spmbuf1, &tmp, TRUE);

    /* Right audio */
    visual_buffer_set_data_pair(&pcmbuf2, data[0][1], sizeof(float) * 1024);

    if(visual_audio_get_sample(audio, &tmp, VISUAL_AUDIO_CHANNEL_LEFT) == VISUAL_OK)

        visual_audio_sample_buffer_mix(&pcmbuf2, &tmp, TRUE, 1.0);

    visual_buffer_set_data_pair(&spmbuf2, data[1][1], sizeof(float) * 1024);

    visual_audio_get_spectrum_for_sample(&spmbuf2, &tmp, TRUE);

    visual_object_unref(VISUAL_OBJECT(&tmp));

    return 0;
}

