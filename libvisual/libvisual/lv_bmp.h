/* Libvisual - The audio visualisation framework.
 * 
 * Copyright (C) 2004, 2005, 2006 Dennis Smit <ds@nerds-incorporated.org>
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 *
 * $Id: lv_bmp.h,v 1.6 2006/01/22 13:23:37 synap Exp $
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

#ifndef _LV_BMP_H
#define _LV_BMP_H

#include <libvisual/lv_video.h>

VISUAL_BEGIN_DECLS

/**
 * @defgroup VisBitmap VisBitmap
 * @{
 */

/**
 * Loads a BMP file into a VisVideo. The buffer will be located
 * for the VisVideo.
 *
 * Keep in mind that you need to free the palette by hand.
 *
 * @param video Destination video where the bitmap should be loaded in.
 * @param filename The filename of the bitmap to be loaded.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_BMP_NOT_FOUND,
 * 	-VISUAL_ERROR_BMP_NO_BMP, -VISUAL_ERROR_BMP_NOT_SUPPORTED or -VISUAL_ERROR_BMP_CORRUPTED
 * 	on failure.
 */
int visual_bitmap_load (VisVideo *video, const char *filename);

/**
 * Loads a bitmap into a VisVideo and return this, so it's not needed to
 * allocate a VisVideo before by hand.
 *
 * @see visual_bitmap_load
 *
 * @param filename The filename of the bitmap to be loaded.
 *
 * @return The VisVideo containing the bitmap or NULL on failure.
 */
VisVideo *visual_bitmap_load_new_video (const char *filename);

VISUAL_END_DECLS

/**
 * @}
 */

#endif /* _LV_BMP_H */
