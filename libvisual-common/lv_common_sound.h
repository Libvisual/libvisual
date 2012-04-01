/* Libvisual-AVS - Advanced visual studio for libvisual
 * 
 * Copyright (C) 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: avs_sound.h,v 1.2 2006-09-19 19:05:46 synap Exp $
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

#ifndef _LV_AVS_SOUND_H
#define _LV_AVS_SOUND_H

#include <libvisual/libvisual.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum {
	AVS_SOUND_SOURCE_TYPE_SCOPE,
	AVS_SOUND_SOURCE_TYPE_SPECTRUM
} AVSSoundSourceType;

typedef enum {
	AVS_SOUND_CHANNEL_TYPE_LEFT,
	AVS_SOUND_CHANNEL_TYPE_RIGHT,
	AVS_SOUND_CHANNEL_TYPE_CENTER
} AVSSoundChannelType;

/* Prototypes */
//short avs_sound_get_from_source (VisAudio *audio, AVSSoundSourceType source, AVSSoundChannelType channel, int index);
int lvavs_sound_get_from_source (VisAudio *audio, float ***data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_AVS_SOUND_H */
