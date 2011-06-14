/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 *
 * This file is part of Blurks-libvisual.
 *
 * Blurks-libvisual is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Blurks-libvisual is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Blurks-libvisual.  If not, see <http://www.gnu.org/licenses/>.
 */

/* bitmap.c */

/*  Blursk - visualization plugin for XMMS
 *  Copyright (C) 1999  Steve Kirkendall
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdlib.h>
#include <string.h>
#include "actor_blursk.h"
#include "blursk.h"

#define XBM_TEST(w,bits,x,y)    ((bits)[(((w) + 7) >> 3) * (y) + ((x) >> 3)] & (1 << ((x) & 7)))

#include "bitmaps/redhat.xbm"
#include "bitmaps/suse.xbm"
#include "bitmaps/blursk.xbm"
#include "bitmaps/tux.xbm"
#include "bitmaps/xmms.xbm"
#include "bitmaps/xlogo.xbm"
#include "bitmaps/gnome.xbm"
#include "bitmaps/notes.xbm"
#include "bitmaps/clef.xbm"
#include "bitmaps/flower.xbm"
#include "bitmaps/peace.xbm"
#include "bitmaps/question.xbm"
#include "bitmaps/bob.xbm"

static struct bdx_s {
    char    *flashname; /* name, when used in a flash */
    char    *stencilname;   /* name, when used in a stencil */
    int width, height;  /* size of bitmap */
    unsigned char   *bits;      /* bitmap data */
} bitmaps[] =
{
    { "Red Hat flash",  "Red Hat stencil",  redhat_width,   redhat_height,  redhat_bits},
    { "SuSE flash",     "SuSE stencil",     suse_width, suse_height,    suse_bits},
    { "Blursk flash",   "Blursk stencil",   blursk_width,   blursk_height,  blursk_bits},
    { "Tux flash",      "Tux stencil",      tux_width,  tux_height, tux_bits},
    { "XMMS flash",     "XMMS stencil",     xmms_width, xmms_height,    xmms_bits},
    { "X flash",        "X stencil",        xlogo_width,    xlogo_height,   xlogo_bits},
    { "Gnome flash",    "Gnome stencil",    gnome_width,    gnome_height,   gnome_bits},
    { "Notes flash",    "Notes stencil",    notes_width,    notes_height,   notes_bits},
    { "Clef flash",     "Clef stencil",     clef_width, clef_height,    clef_bits},
    { "Flower flash",   "Flower stencil",   flower_width,   flower_height,  flower_bits},
    { "Peace flash",    "Peace stencil",    peace_width,    peace_height,   peace_bits},
    { "Question flash", "Question stencil", question_width, question_height,question_bits},
    { "Bob flash",      "Bob stencil",      bob_width,  bob_height, bob_bits}
};


/* If str is the name of a bitmap followed by some other word, then return the
 * bitmap's index; else return -1.
 */
int bitmap_index(char *str)
{
    int bindex;

    /* Treat "Maybe stencil" as though it was "Random stencil" or
     * "No stencil", randomly.
     */
    if (!strcmp(str, "Maybe stencil"))
    {
        bindex = rand_0_to(QTY(bitmaps) * 5);
        if (bindex >= QTY(bitmaps))
            bindex = -1;
        return bindex;
    }

    /* Accept "Random" as though it was a valid name */
    if (!strncmp(str, "Random ", 7))
    {
        /* If we're using a random stencil then treat any other "Random"
         * bitmap as a synonym for the stencil bitmap.
         */
        if ((!strcmp(config.blur_stencil, "Random stencil")
            || !strcmp(config.blur_stencil, "Maybe stencil"))
         && blur_stencil != -1
         && strcmp(str, "Random stencil"))
            return blur_stencil;

        /* Otherwise, this can be any bitmap */
        return rand_0_to(QTY(bitmaps));
    }

    /* Scan through bitmaps[] for the name */
    for (bindex = 0; bindex < QTY(bitmaps); bindex++)
    {
        if (!strcmp(bitmaps[bindex].flashname, str)
         || !strcmp(bitmaps[bindex].stencilname, str))
            return bindex;
    }

    /* unknown bitmap */
    return -1;
}


/* Return FALSE for background pixels, TRUE for foreground pixels */
int bitmap_test(int bindex, int x, int y)
{
    int factor;
    static int xnum, xdenom, xtrans;
    static int ynum, ydenom, ytrans;
    static int prevwidth, prevheight;
    static struct bdx_s *bdx;

    /* If first time, then precompute some scaling factors */
    if (prevwidth != img_width || prevheight != img_height || bdx != &bitmaps[bindex])
    {
        /* remember the screen size, so we can skip this next time */
        prevwidth = img_width;
        prevheight = img_height;
        bdx = &bitmaps[bindex];

        /* For the "Medium CPU" setting, tweak the aspect ratio. */
        if (*config.cpu_speed == 'M')
            factor = 2;
        else
            factor = 1;

        /* Compute the conversion factors, maintaining the same aspect
         * ratio.  (including the above tweak)
         */
        if (img_width * bdx->height * factor < img_height * bdx->width) 
        {
            /* Scale so width matches exactly */
            xnum = bdx->width;
            xdenom = img_width;
            xtrans = 0;
            ynum = bdx->width;
            ydenom = img_width * factor;
            ytrans = ((int)img_height - bdx->height * ydenom / ynum) / 2;
        }
        else
        {
            /* Scale so height matches exactly */
            xnum = bdx->height * factor;
            xdenom = img_height;
            xtrans = ((int)img_width - bdx->width * xdenom / xnum) / 2;
            ynum = bdx->height;
            ydenom = img_height;
            ytrans = 0;
        }
    }

    /* Scale (x,y) to fit the bitmap into the window. */
    x = (x - xtrans) * xnum / xdenom;
    y = (y - ytrans) * ynum / ydenom;

    /* if in bitmap, and the bit is set, then return TRUE.  Else FALSE */
    if (x >= 0 && x < bdx->width && y >= 0 && y < bdx->height
        && XBM_TEST(bdx->width, bdx->bits, x, y))
    {
        return TRUE;
    }
    return FALSE;
}



/* Perform a flash by drawing a logo on the screen */
void bitmap_flash(int bindex)
{
    int x, y;
    unsigned char   *pixel;

    for (y = 0, pixel = img_buf; y < img_height; y++, pixel += img_bpl - img_width)
        for (x = 0; x < img_width; x++, pixel++)
            if (bitmap_test(bindex, x, y))
                *pixel = 160;
}



/* Return the names of logo flashes, and other flash styles */
char *bitmap_flash_name(int i)
{
    switch (i)
    {
      case 0:
        return "No flash";

      case QTY(bitmaps) + 1:
        return "Random flash";

      case QTY(bitmaps) + 2:
        return "Full flash";

      case QTY(bitmaps) + 3:
        return "Invert flash";

      default:
        if (i > QTY(bitmaps))
            return NULL;
        return bitmaps[i - 1].flashname;
    }
}


char *bitmap_stencil_name(int i)
{
    switch (i)
    {
      case 0:
        return "No stencil";

      case QTY(bitmaps) + 1:
        return "Random stencil";

      case QTY(bitmaps) + 2:
        return "Maybe stencil";

      default:
        if (i > QTY(bitmaps))
            return NULL;
        return bitmaps[i - 1].stencilname;
    }
}
