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

/* blur.c */

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
#include <math.h>
#include <sys/time.h>
#include <unistd.h>
#include "actor_blursk.h"
#include "blursk.h"

/* This data type indicates which rule is used for lowering the signal */
typedef enum { LOWER_NO, LOWER_YES, LOWER_SPECTRUM } lower_t;

/* These controls the number of frames between random changes, when the
 * "Random" or "Random & fast" blur styles are used.
 */
#define KEEP_RANDOM_SLOW 300
#define KEEP_RANDOM   30

/* This is the maximum number of microseconds to use in each frame for
 * computing blur motion vectors.  This computation only occurs when
 * switching from one blur motion to another.  Some of the blur motion
 * functions are much more compute-intensive than others, to the extent that
 * they might actually cause the animation to become jerky.  This timeout
 * amount prevents that, by spreading the transition among more frames.
 * In effect, it turns a "Fast switch" into a "Slow switch" if necessary.
 */
#define MAXUSEC     10000

/* These control the transition dithering.  MAXTRANSITION is any convenient
 * number (preferably prime), and dither[] is simply a shuffled list of all
 * integers from 0 to MAXTRANSITION-1.
 */
#if 1
#define MAXTRANSITION 571
static int dither[MAXTRANSITION] =
{
    1, 235, 36, 63, 520, 112, 191, 438, 451, 368,
    272, 57, 208, 293, 543, 523, 362, 409, 545, 346,
    429, 138, 71, 247, 89, 228, 74, 553, 274, 394,
    395, 0, 349, 164, 364, 299, 320, 555, 167, 156,
    106, 117, 127, 209, 155, 201, 461, 524, 440, 51,
    265, 542, 273, 387, 508, 199, 256, 359, 120, 56,
    136, 342, 7, 270, 550, 377, 214, 434, 66, 325,
    303, 86, 249, 532, 531, 393, 162, 221, 326, 116,
    392, 94, 558, 502, 14, 188, 340, 476, 449, 198,
    163, 336, 306, 206, 250, 32, 419, 465, 390, 4,
    144, 322, 111, 93, 245, 253, 170, 480, 279, 105,
    448, 175, 255, 288, 92, 259, 154, 237, 212, 517,
    81, 78, 282, 67, 562, 533, 435, 220, 77, 210,
    213, 39, 217, 87, 510, 418, 121, 145, 490, 82,
    25, 560, 124, 297, 275, 453, 8, 430, 456, 487,
    530, 351, 378, 421, 143, 161, 307, 317, 263, 224,
    327, 431, 142, 546, 345, 115, 423, 352, 569, 50,
    497, 439, 181, 244, 486, 108, 21, 223, 495, 159,
    44, 315, 330, 258, 24, 295, 150, 231, 278, 19,
    329, 335, 232, 343, 491, 192, 195, 528, 252, 474,
    559, 424, 27, 45, 380, 52, 183, 318, 507, 509,
    370, 102, 193, 38, 400, 205, 432, 452, 298, 37,
    148, 371, 291, 61, 468, 356, 410, 152, 471, 443,
    539, 257, 203, 484, 262, 88, 234, 341, 196, 139,
    385, 100, 241, 219, 348, 473, 355, 72, 475, 285,
    384, 34, 289, 503, 347, 567, 69, 216, 557, 157,
    103, 396, 372, 3, 310, 40, 374, 500, 513, 64,
    240, 176, 178, 229, 204, 420, 442, 242, 354, 551,
    29, 319, 514, 334, 324, 397, 547, 186, 436, 399,
    381, 149, 361, 276, 549, 286, 470, 165, 137, 312,
    379, 516, 403, 499, 466, 10, 375, 141, 493, 172,
    304, 233, 407, 54, 113, 79, 415, 450, 360, 309,
    478, 413, 123, 98, 483, 290, 365, 564, 281, 73,
    544, 107, 189, 350, 35, 501, 23, 376, 254, 302,
    540, 171, 554, 369, 84, 26, 180, 109, 226, 230,
    467, 433, 337, 131, 95, 18, 504, 187, 15, 227,
    344, 339, 207, 91, 556, 70, 469, 425, 541, 11,
    457, 153, 248, 386, 488, 99, 128, 308, 494, 537,
    570, 296, 563, 404, 261, 65, 53, 238, 185, 140,
    447, 31, 46, 411, 76, 398, 126, 445, 184, 482,
    90, 311, 518, 496, 135, 271, 85, 12, 190, 382,
    9, 292, 101, 130, 239, 16, 300, 388, 173, 17,
    28, 166, 122, 313, 561, 6, 236, 97, 60, 194,
    174, 444, 357, 147, 49, 323, 129, 125, 515, 485,
    446, 406, 197, 338, 215, 459, 132, 218, 114, 200,
    202, 158, 333, 511, 522, 428, 119, 55, 58, 260,
    13, 462, 33, 521, 565, 373, 110, 481, 412, 416,
    492, 269, 405, 267, 512, 548, 134, 80, 59, 460,
    284, 179, 498, 264, 479, 427, 454, 552, 2, 489,
    314, 316, 417, 535, 455, 463, 506, 366, 83, 287,
    441, 41, 301, 160, 363, 519, 505, 566, 146, 305,
    414, 529, 525, 43, 472, 458, 536, 266, 280, 383,
    222, 568, 534, 328, 47, 401, 367, 332, 246, 389,
    22, 277, 391, 527, 243, 133, 331, 75, 225, 437,
    321, 464, 48, 104, 402, 151, 30, 5, 42, 538,
    96, 353, 268, 62, 182, 68, 526, 422, 251, 294,
    211, 426, 20, 118, 283, 177, 477, 358, 168, 408,
    169
};
#else
#define MAXTRANSITION 149
static int dither[MAXTRANSITION] =
{
    125, 58, 57, 118, 135, 10, 49, 19, 41, 42,
    48, 140, 51, 76, 141, 46, 27, 106, 47, 26,
    22, 36, 62, 24, 74, 3, 61, 8, 45, 32,
    13, 0, 53, 67, 105, 145, 87, 144, 43, 64,
    35, 115, 1, 130, 25, 111, 120, 15, 71, 85,
    40, 55, 88, 98, 129, 66, 82, 11, 68, 96,
    78, 37, 77, 126, 75, 34, 112, 113, 30, 99,
    79, 100, 80, 122, 18, 107, 33, 143, 95, 148,
    102, 119, 2, 131, 123, 14, 16, 12, 70, 65,
    142, 133, 108, 127, 9, 17, 56, 28, 101, 4,
    29, 63, 6, 50, 116, 83, 137, 7, 97, 147,
    117, 31, 54, 84, 94, 109, 72, 134, 124, 69,
    136, 21, 138, 44, 146, 139, 91, 20, 52, 38,
    132, 86, 5, 73, 104, 39, 92, 59, 81, 128,
    93, 89, 114, 110, 90, 60, 103, 23, 121
};
#endif

/* These store random numbers that are held constant for all pixels in a
 * given blur style.  Each blur function can use up to MAXRANDOM random
 * numbers when generating pixel motion vectors.
 */
#define MAXRANDOM   64
static int randval[MAXRANDOM];

/* The "Slow switch" setting performs less that one transition loop per frame.
 * For example, setting this constant to 3 causes one transition loop on every
 * third frame, for a very slow change.
 */
#define SWITCH_FRACTION 3

/* These store the source offsets for the blurloop function. */
static int  blurwidth, blurheight;
static int  blurxcenter, blurycenter;
static int  blurlast;

/* These are used to compute the transition from one blur style to another */
static char stylename[50];
static char stencilname[50];
static char blurname[50];
static int  isspectrum; /* boolean: is current signal_style a spectrum? */
static char blurchar;   /* first letter of blurname, or random */
static int  (*stylefunc)(int offset);
static int  styletransition = -1;
static int  stylekeeprandom;
static int  stylelower, styleprevlower;

/* This is used to help hide anomalies from some of the blur styles */
static int  salt;

/**
 * This stores the id of the current stencil bitmap, or -1 for no bitmap.
 * It is a global variable because it also affects bitmap flashes.
 */
int blur_stencil;

/**
 * This flag indicates whether blur motions should be happy about pulling
 * their source pixels from within a stencil.  Most blur motions try to work
 * around stencils, but a few look better with this turned on.
 */
static int blurintostencil;

/**
 * This flag indicates whether we prefer smooth edges, or smooth areas, for
 * this particular blur motion.  This is a minor feature; you need to look
 * close to see the difference.  With this flag turned off, the edges of some
 * blur motions may seem to vibrate; with it turned on, large areas which
 * should be smooth may appear to have a checkerboard pattern.
 */
 static int edgesmooth;


/**
 * every pixel is blurred from the pixels around it 
 */
static int simple(int offset)
{
    if (randval[0] == 0)
        return 0;
    switch (randval[0] & 0x7)
    {
      case 0:   return 1;
      case 1:   return img_bpl + 1;
      case 2:   return img_bpl;
      case 3:   return img_bpl - 1;
      case 4:   return -1;
      case 5:   return -img_bpl - 1;
      case 6:   return -img_bpl;
      default:  return -img_bpl + 1;
    }
}

/**
 * every pixel is blurred from pixels surrounding a neighbor 
 */
static int grainy(int offset)
{
    if (++salt >= 14) salt = 0;
    switch (salt)
    {
      case 0:   return -img_bpl - 1;
      case 1:   return -img_bpl;
      case 2:   return -img_bpl + 1;
      case 3:   return 1;
      case 4:   return img_bpl + 1;
      case 5:   return img_bpl;
      case 6:   return img_bpl - 1;
      case 7:   return -1;
      case 8:   return img_bpl + 2;
      case 9:   return 2;
      case 10:  return img_bpl - 2;
      case 11:  return -img_bpl - 2;
      case 12:  return -2;
      default:  return -img_bpl + 2;
    }
}

/**
 * Pixels go up, down, left, and right 
 */
static int fourway(int offset)
{
    int x, y;

    x = offset % img_bpl;
    y = offset / img_bpl;
    switch (((y & 1) << 1) | (x & 1))
    {
      case 0:   return -2;
      case 1:   return 2 * img_bpl;
      case 2:   return -2 * img_bpl;
      default:  return 2;
    }
}

/**
 * every pixel is blurred from pixels slightly below it, which causes the
 * blur to drift upward.
 */
static int rise(int offset)
{
    return img_bpl;
}

static int wiggle(int offset)
{
    int y = (offset / img_bpl) + (offset & 0x1);
    if ((y & 0x0f) < 3)
        return img_bpl;
    else if (y & 0x10)
        return img_bpl - 1;
    else
        return img_bpl + 1;
}

/**
 * pixels above the middle blur up, and pixels below the middle blur down 
 */
static int updown(int offset)
{
    offset /= img_bpl;
    if (offset < blurycenter)
        return img_bpl;
    else
        return -img_bpl;
}

/**
 * pixels on the left move leftward, and pixels on the right move rightward 
 */
static int leftright(int offset)
{
    offset %= img_bpl;
    if (offset < blurxcenter / 2)
        return 2;
    else if (offset < blurxcenter)
        return 1;
    else if (offset < (blurxcenter + blurwidth) / 2)
        return -1;
    else
        return -2;
}

/**
 * pixels are blurred from pixels closer to the center, so the blurring tends
 * to move outward.  This is done in a way which causes the blur to move faster
 * near the edge.
 */
static int forward(int offset)
{
    int x, y;
    int dirx, diry;

    /* convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* Separate the sign from the magnitude.  We must do this to get
     * consistent behavior from the "/" operator in all quadrants.
     */
    dirx = diry = 1;
    if (x < 0)
        dirx = -1, x = -x;
    if (y < 0)
        diry = -1, y = -y;

    /* Convert coordinates to source offset, by subtracting a scaled-down
     * version of them from themselves.
     */
    y -= (y * 63 + salt) / 64;
    x -= (x * 63 + salt) / 64;
    if (++salt >= 63) salt = 0;

    /* adjust for quadrants */
    y *= diry;
    x *= dirx;

    /* return the offset of the source point, relative to this one */
    return -y * img_bpl - x;
}

/**
 * A more extreme version of forward() 
 */
static int fastfwd(int offset)
{
    int x, y;
    int dirx, diry;

    /* convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* Separate the sign from the magnitude.  We must do this to get
     * consistent behavior from the "/" operator in all quadrants.
     */
    dirx = diry = 1;
    if (x < 0)
        dirx = -1, x = -x;
    if (y < 0)
        diry = -1, y = -y;

    /* Convert coordinates to source offset, by subtracting a scaled-down
     * version of them from themselves.
     */
    y -= (y * 15 + salt) >> 4;
    x -= (x * 15 + salt) >> 4;
    if (++salt >= 16) salt = 0;

    /* adjust for quadrants */
    y *= diry;
    x *= dirx;

    /* return the offset of the source point, relative to this one */
    return -y * img_bpl - x;
}

static int spray(int offset)
{
    int x, y;
    x = offset % img_bpl;
    y = offset / img_bpl;
    y >>= 1;
    offset = y * img_bpl + x;
    return forward(offset);
}

/**
 * pixels are blurred from pixels farther to the center, so the blurring tends
 * to move inward.  This is done in a way which causes the blur to move faster
 * near the edge.  Also, it supports an optional random twisting motion.
 */
static int backward(int offset)
{
    int     x, y;
    int     dirx, diry;
    static int  wobble = 0, dir = 1;

    /* convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* adjust the wobble amount */
    if (randval[0] == 0)
        wobble = 0;
    else
    {
        if (randval[0] != 3)
        {
            if (wobble == -2)
                dir = 1;
            else if (wobble == 2)
                dir = -1;
            wobble += dir;
            randval[0] = 3;
        }
    }

    /* spin the image slightly, based on a random number */
    diry = y;
    switch (wobble)
    {
      case -2:
        y += x;
        x -= diry;
        break;

      case -1:
        y += (x>>1);
        x -= (diry>>1);
        break;

      case 0:
        /* no twist */
        break;

      case 1:
        y -= (x>>1);
        x += (diry>>1);
        break;

      case 2:
        y -= x;
        x += diry;
        break;
    }

    /* Separate the sign from the magnitude.  We must do this to get
     * consistent behavior from the "/" operator in all quadrants.
     */
    dirx = diry = 1;
    if (x < 0)
        dirx = -1, x = -x;
    if (y < 0)
        diry = -1, y = -y;

    /* Convert coordinates to source offset, by subtracting a scaled-up
     * version of them from themselves.
     */
    y -= (y * 65 + salt) / 64;
    x -= (x * 65 + salt) / 64;
    if (++salt >= 63) salt = 0;

    /* adjust for quadrants */
    y *= diry;
    x *= dirx;

    /* return the offset of the source point, relative to this one */
    return -y * img_bpl - x;
}

/**
 * This divides the screen into four quadrants, and then reduces & rotates
 * them to duplicate the image into each quadrant.
 */
static int fractal(int offset)
{
    int x, y;

    /* Compute the position within a quadrant, and then scale that quadrant
     * up to the size of the whole image.
     */
    x = (offset % img_bpl) * 2 % img_width;
    y = (offset / img_bpl) * 2 % img_height;

    /* return that offset */
    return y * img_bpl + x - offset;
}

static int sphere(int offset)
{
    int x, y;
    int dist2;
    int radius2;
    double  angle, through;

    /* Convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* For "Medium CPU", double X to preserve aspect ratio.  For "Slow CPU"
     * double both of them to preserve size. */
    if (*config.cpu_speed != 'F')
    {
        x *= 2;
        if (*config.cpu_speed == 'S')
            y *= 2;
    }

    /* compute the square of the distance from the center. */
    dist2 = x * x + y * y;
    radius2 = blurycenter * blurycenter;
    if (*config.cpu_speed != 'S')
        radius2 >>= 1;
    else
        radius2 <<= 1;

    /* If outside the "sphere" then use one of the other motions. */
    if (randval[0] != 0 && radius2 < dist2)
        return fractal(offset);

    /* the center could cause problems -- just use 0 as the offset there */
    if (dist2 < 5)
        return 0;

    /* in the sphere, we want to go fast near the center, and slow near
     * the edges.  This is like forward() except that we want to translate
     * the point through the center, by a distance of 1 radius.
     */
    angle = atan2((double)y, (double)x);
    through = sqrt((double)abs(radius2 - dist2) / 6.0);
    if (radius2 < dist2)
        through = -through;
    x = blurxcenter + (int)(through * cos(angle));
    y = blurycenter + (int)(through * sin(angle));
    return fastfwd(y * img_bpl + x);
}


/**
 * rotate left, right, or both. 
 */
static int spinhelp(int offset, int right, int spiral, int twist)
{
    int x, y;
    int dirx, diry;
    int dx, dy;
    int radius;

    /* convert offset to (x,y) coordinates */
    y = offset / img_bpl;
    x = offset % img_bpl;

    if (right)
    {
        /* The right half of the top scanline, and the left half of
         * the bottom scan line, both need to copy pixels from the
         * other half of the scan line, to prevent "shadows" from
         * the perimeter.
         */
        if (y == 1 && x > blurxcenter + 12)
            return blurxcenter;
        if (y == 2 && x > blurxcenter + 20)
            return -img_bpl - blurxcenter;
        if (y == blurheight - 3 && x < blurxcenter - 20)
            return img_bpl + blurxcenter;
        if (y == blurheight - 2 && x < blurxcenter - 12)
            return -blurxcenter;
    }
    else
    {
        /* The left half of the top scanline, and the right half of
         * the bottom scan line, both need to copy pixels from the
         * other half of the scan line, to prevent "shadows" from
         * the perimeter.
         */
        if (y == 1 && x < blurxcenter - 12)
            return img_bpl + blurxcenter;
        if (y == 2 && x < blurxcenter - 20)
            return -blurxcenter;
        if (y == blurheight - 3 && x > blurxcenter + 20)
            return blurxcenter;
        if (y == blurheight - 2 && x > blurxcenter + 12)
            return -img_bpl - blurxcenter;
    }

    /* Adjust so (0,0) is at center */
    y -= blurycenter;
    x -= blurxcenter;

    /* Separate the sign from the magnitude.  We must do this to get
     * consistent behavior from the "/" operator in all quadrants.
     */
    dirx = diry = 1;
    if (x < 0)
        dirx = -1, x = -x;
    if (y < 0)
        diry = -1, y = -y;

    /* Convert coordinates to source offsets.  For the "Medium CPU"
     * setting, we need to tweak the aspect ratio.
     */
    if (*config.cpu_speed == 'M')
    {
        x *= 2;
        radius = x + y + 5;
        if (twist)
        {
            if (radius < blurycenter * 2)
                radius = blurycenter - radius/2;
            else
                radius = 5;
        }
        if (++salt >= radius * 2) salt = 0;
        dx = (y * 2 + salt) / radius;
        dy = (x * 4 + salt) / radius;
    }
    else
    {
        radius = x + y + 5;
        if (twist)
        {
#if 1
            radius = blurycenter - radius/2;
            if (radius < 5)
                radius = 5;
#else
            radius = (blurycenter + blurxcenter + 10) / radius + 5;
#endif
        }
        if (++salt * 2 >= radius * 3) salt = 0;
        dx = (y * 4 + salt) / radius;
        dy = (x * 4 + salt) / radius;
    }

    /* adjust for quadrants, depending on spin direction */
    if (right)
    {
        dy *= -dirx;
        dx *= diry;
    }
    else
    {
        dy *= dirx;
        dx *= -diry;
    }

    if (spiral)
    {
        dx -= dirx;
        dy -= diry;
    }

    if (twist)
    {
        x = dx;
        dx = dy;
        dy = -x;
    }

    /* return the offset of the source point, relative to this one */
    return dy * img_bpl + dx;
}

/**
 * pixels are blurred from pixels that are rotated around the image center 
 */
static int spin(int offset)
{
    return spinhelp(offset, randval[0] & 1, FALSE, FALSE);
}

static int bullseye(int offset)
{
    int x, y;

    /* Convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* For "Medium CPU", double X to preserve aspect ratio.  For "Slow CPU"
     * double both of them to preserve size. */
    if (*config.cpu_speed != 'F')
    {
        x *= 2;
        if (*config.cpu_speed == 'S')
            y *= 2;
    }
    
    /* Based on distance to center, spin left or right */
    if ((x * x + y * y + 3000) & 4096)
        return spinhelp(offset, TRUE, FALSE, FALSE);
    else
        return spinhelp(offset, FALSE, FALSE, FALSE);
}

static int spiral(int offset)
{
    return spinhelp(offset, randval[0] & 1, TRUE, FALSE);
}

static int drain(int offset)
{
    return -spiral(offset);
}

static int ripple(int offset)
{
    int x, y;

    /* Convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* For "Medium CPU", double X to preserve aspect ratio.  For "Slow CPU"
     * double both of them to preserve size. */
    if (*config.cpu_speed != 'F')
    {
        x *= 2;
        if (*config.cpu_speed == 'S')
            y *= 2;
    }
    
    /* Based on distance to center, spin left or right */
    if ((x * x + y * y + 5000) & 2048)
        return spinhelp(offset, TRUE, TRUE, FALSE);
    else
        return spinhelp(offset, FALSE, TRUE, FALSE);
}

static int prismatic(int offset)
{
    int x, y, d;

    /* Convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* Choose a direction by reducing x & y to square coords instead of
     * pixel coords, and then checking their odd/evenness.  This is easier
     * that it sounds, because we just need to check a single bit from
     * each.
     */
    switch ((y & 0x08) | ((x >> 1) & 0x04))
    {
      case 0x00: d = -1;        break;
      case 0x04: d = img_bpl;   break;
      case 0x08: d = -img_bpl;  break;
      default:   d = 1;     break;
    }

    return d;
}

static int swirl(int offset)
{
    int x, y, d;

    /* Convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    salt = (salt + 1) & 0x7;
    switch (salt >> 1)
    {
      case 0:   y += 2; break;
      case 1:   x += 2; break;
      case 2:   y -= 2; break;
      case 3:   x -= 2; break;
    }

    /* Choose a direction by reducing x & y to square coords instead of
     * pixel coords, and then checking their odd/evenness.  This is easier
     * that it sounds, because we just need to check a single bit from
     * each.
     *
     * Note that this differs from "Prismatic" only in that here we use
     * diagonal directions, instead of Parquet's orthogonal directions.
     * Oh, and the squares are larger.
     */
    d = 1 + (salt & 1);
    switch ((y & 0x10) | ((x >> 1) & 0x08))
    {
      case 0x00: d = img_bpl - d;   break;
      case 0x08: d = -img_bpl - d;  break;
      case 0x10: d = img_bpl + d;   break;
      default:   d = -img_bpl + d;  break;
    }

    return d;
}

static int shred(int offset)
{
    switch (randval[0] & 3)
    {
      case 0:
        if ((offset % (img_bpl - 1)) & 0x10)
            return img_bpl - 1;
        else
            return -img_bpl + 1;

      case 1:
        if ((offset % (img_bpl + 1)) & 0x10)
            return img_bpl + 1;
        else
            return -img_bpl - 1;

      case 2:
        if ((offset % img_bpl) & 0x10)
            return img_bpl;
        else
            return -img_bpl;

      default:
        if ((offset / img_bpl) & 0x10)
            return 1;
        else
            return -1;
    }
}

/**
 * This gives an interesting binary tree effect 
 */
static int binary(int offset)
{
    return offset;
}

/**
 * Gravity -- images accelerate downward 
 */
static int gravity(int offset)
{
    /* compute height */
    offset = offset / img_bpl;
    
    /* Compute dy from the height, with salt */
    offset = (offset * 3 + salt) / blurheight;
    if (++salt >= blurheight) salt = 0;

    /* Return an offset, derived from dy */
    return offset * -img_bpl;
}

static int cylinder(int offset)
{
    /* compute height, with salt */
    offset = offset / img_bpl;

    /* return sin(height) */
    if (++salt >= 100) salt = 0;
    offset = (int)((double)salt/100.0 + 2.5 * sin((double)offset / (double)img_height * VISUAL_MATH_PI));
    return offset * img_bpl;
}


/**
 * Each 16x16 pixel square moves in a random direction 
 */
static int tangram(int offset)
{
    int x, y;

    /* Decide which square this pixel is in.  We only have enough random
     * numbers to control 64 squares, but we don't want the same 8x8 pattern
     * to repeat, so we shift the columns of 8x8 squares slightly.  Each
     * piece of the 8x8 square is actually a 16x16-pixel area.  All of this
     * complicates our computation somewhat.
     */
    x = ((offset % img_bpl - blurxcenter) >> 4);
    y = (((offset / img_bpl - blurycenter) >> 4) + (x >> 3)) & 0x7;
    x &= 0x7;

    /* return an offset based on that square's random number */
    switch (randval[(y << 3) + x] & 0x7)
    {
      case 0:   return img_bpl - 1;
      case 1:   return img_bpl + 1;
      case 2:   return -img_bpl - 1;
      case 3:   return -img_bpl + 1;
      case 4:   return -1;
      case 5:   return 1;
      case 6:   return img_bpl;
      default:  return -img_bpl;
    }
}

/**
 * The screen is divided into about 12 random sections, each of which moves
 * in a random direction.  The division is based on 3 mostly-vertical lines
 * and 2 mostly-horizontal lines.
 */
static int divided(int offset)
{
    int x, y, i;

    /* if first time, then convert random numbers to edge coordinates */
    if (salt == 0)
    {
        salt = 1;

        /* Convert mostly-vertical values */
        for (i = 0; i < 3; i++)
        {
            randval[i * 2] %= img_width;
            randval[i * 2 + 1] = (randval[i * 2 + 1] & 0xff) - 127;
        }

        /* Convert mostly-horizontal values */
        for (i = 3; i < 5; i++)
        {
            randval[i * 2] %= img_height;
            randval[i * 2 + 1] = (randval[i * 2 + 1] & 0xff) - 127;
        }

        /* Convert the motion values */
        for (i = 10; i < 42; i++)
        {
            switch (randval[i] % 20)
            {
              case 0:   randval[i] = -2 * img_bpl - 1;  break;
              case 1:   randval[i] = -2 * img_bpl;  break;
              case 2:   randval[i] = -2 * img_bpl + 1;  break;
              case 3:   randval[i] = -img_bpl - 2;  break;
              case 4:   randval[i] = -img_bpl - 1;  break;
              case 5:   randval[i] = -img_bpl;      break;
              case 6:   randval[i] = -img_bpl + 1;  break;
              case 7:   randval[i] = -img_bpl + 1;  break;
              case 8:   randval[i] = -2;        break;
              case 9:   randval[i] = -1;        break;
              case 10:  randval[i] = 1;         break;
              case 11:  randval[i] = 2;         break;
              case 12:  randval[i] = img_bpl - 2;   break;
              case 13:  randval[i] = img_bpl - 1;   break;
              case 14:  randval[i] = img_bpl;       break;
              case 15:  randval[i] = img_bpl + 1;   break;
              case 16:  randval[i] = img_bpl + 2;   break;
              case 17:  randval[i] = 2 * img_bpl - 1;   break;
              case 18:  randval[i] = 2 * img_bpl;   break;
              case 19:  randval[i] = 2 * img_bpl + 1;   break;
            }
        }
    }
        
    /* get the pixel coordinates of this point */
    x = offset % img_bpl;
    y = offset / img_bpl;

    /* Use each line as a divider, and merge a '1' or '0' bit into the
     * chunk id based on which side of each line the point is on.
     */
    i = 0;
    if (x - randval[0] < (y * randval[1]) >> 8)
        i |= 1;
    if (x - randval[2] < (y * randval[3]) >> 8)
        i |= 2;
    if (x - randval[4] < (y * randval[5]) >> 8)
        i |= 4;
    if (y - randval[6] < (x * randval[7]) >> 8)
        i |= 8;
    if (y - randval[8] < (x * randval[9]) >> 8)
        i |= 16;

    /* Return the motion vector for that chunk */
    return randval[i + 10];
}

static int weave(int offset)
{
    int x, y, g;
    int xsize, ysize;

    /* Convert offset to (x,y) coordinates, with (0,0) at center */
    y = offset / img_bpl - blurycenter;
    x = offset % img_bpl - blurxcenter;

    /* The weave pattern consists of a 4x4 grid of squares.  Figure out
     * where this pixel is in the grid.  Also set x & y to the position
     * within the square, because sometimes that matters.
     */
    switch (*config.cpu_speed)
    {
      case 'S': /* Slow CPU */
        xsize = 8;
        ysize = 8;  
        g = ((y >> 1) & 0xc) | ((x >> 3) & 0x3);
        x &= 0x7;
        y &= 0x7;
        break;

      case 'M': /* Medium CPU */
        xsize = 8;
        ysize = 16; 
        g = ((y >> 2) & 0xc) | ((x >> 3) & 0x3);
        x &= 0x7;
        y &= 0xf;
        break;

      default:  /* Fast CPU */
        xsize = 16;
        ysize = 16; 
        g = ((y >> 2) & 0xc) | ((x >> 4) & 0x3);
        x &= 0xf;
        y &= 0xf;
        break;
    }

    /* Choose a direction based mostly on the grid position */
    switch (g)
    {
      case 1:
        if (y == 0)
            return -(ysize + 1) * img_bpl;
        /* else fall through... */
      case 5:
      case 9:
        return -img_bpl;

      case 3:
        if (y == ysize - 1)
            return (ysize + 1) * img_bpl;
        /* else fall through... */
      case 11:
      case 15:
        return img_bpl;

      case 4:
        if (x == xsize - 1)
            return xsize + 1;
        /* else fall through.. */
      case 6:
      case 7:
        return 1;

      case 12:
        if (x == 0)
            return -(xsize + 1);
        /* else fall through... */
      case 13:
      case 14:
        return -1;

      default:
        return 0;
    }
}

/**
 * This function contains the common code shared by all the flow functions.
 * It is passed in the (x,y) coordinates of a point and a flag for mixing
 * in/out vectors.  It is also passed pointers to int variables which will
 * receive the (dx,dy) motion vector.  It returns 0 normally, or 1 if the
 * point is located exactly on a flow point; when this function returns 1,
 * the flow function that called it should return a 0 offset.
 */
static int flow_help(int x, int y, int *totdxref, int *totdyref)
{
    int i, h, w;
    double  dx, dy, r2, dxpart, dypart, scale;

    /* If first time, then generate random flow points */
    if (salt == 0)
    {
        salt = 1;

        /* It turns out that totally random points don't usually give
         * a very good effect.  So instead we'll divide the window into
         * 9 subsections and put one point in each.  Then we'll add a
         * 10th totally random point.
         */
        w = img_width / 4;
        h = img_height / 4;
        for (i = 0; i < 9; i++)
        {
            randval[i * 2] = (i % 3) * w + rand_0_to(w) + w/2;
            randval[i * 2 + 1] = (i / 3) * h + rand_0_to(h) + h/2;
        }
        randval[18] = rand_0_to(img_width);
        randval[19] = rand_0_to(img_height);
    }

    /* Add the flow factor from each flow point */
    dx = dy = 0.0;
    scale = (double)(img_width + img_height) / 300.0;
    for (i = 0; i < 20; i += 2)
    {
        /* if point is exactly on a flow point, then don't move. */
        if (x == randval[i] && y == randval[i + 1])
            return 1;

        /* Compute a flow vector from this point */
        dxpart = (double)(randval[i] - x);
        dypart = (double)(randval[i + 1] - y);
        r2 = sqrt(dxpart * dxpart + dypart * dypart + 15.0) / scale;
        dxpart /= r2;
        dypart /= r2;

        /* Invert vectors for half of the flow points */
        if ((i & 2) == 0)
        {
            dxpart = -dxpart;
            dypart = -dypart;
        }

        /* Add it to this point's overall flow */
        dx += dxpart;
        dy += dypart;
    }

    /* Convert the flow vectors to ints, with salt */
    if (++salt > 81) salt = 1;
    *totdxref = dx + (double)(salt % 9 - 4) / 4.0;
    *totdyref = dy + (double)((salt - 1) / 9 - 4) / 4.0;
    return 0;
}

static int flow(int offset)
{
    int x, y;
    int dx, dy;

    /* Convert offset to x & y coordinates */
    x = offset % img_bpl;
    y = offset / img_bpl;

    /* Compute the flow vector */
    if (flow_help(x, y, &dx, &dy))
        return 0;

    /* Convert flow vector to an offset, and return it */
    return dy * img_bpl + dx;
}

static int flowaround(int offset)
{
    int x, y;
    int dx, dy;

    /* Convert offset to x & y coordinates */
    x = offset % img_bpl;
    y = offset / img_bpl;

    /* Compute the flow vector */
    if (flow_help(x, y, &dx, &dy))
        return 0;

    /* For the "Medium CPU" setting, we need to tweak the aspect ratio. */
    if (*config.cpu_speed == 'M')
        dx <<= 1; /* really dy because of the following swap */

    /* Convert flow vector to an offset, and return it.  Note that we
     * swap dx & dy, and negate dy, to achieve a spin effect.
     */
    return dx * img_bpl - dy;
}


/**
 * This is a list of the names and functions for all supported blur styles 
 */
static struct styles {
    char    *name;
    int (*stylefunc)(int offset);
    lower_t lower;      /* when to move the signal lower in window? */
    int nrandoms;   /* qty of random numbers in randval[] */
    int blurintostencil;/* TRUE if motion should stop at stencil */
    int edgesmooth; /* TRUE to prefer smooth edges, not areas */
} styles[] =
{
    {"Simple",  simple,     LOWER_NO,   0,  FALSE,  FALSE},
    {"Wobble",  simple,     LOWER_NO,   1,  FALSE,  FALSE},
    {"Grainy",  grainy,     LOWER_NO,   0,  FALSE,  TRUE},
    {"Four way",    fourway,    LOWER_NO,   0,  FALSE,  TRUE},
    {"Rise",    rise,       LOWER_YES,  0,  FALSE,  TRUE},
    {"Wiggle",  wiggle,     LOWER_YES,  0,  FALSE,  FALSE},
    {"Cylinder",    cylinder,   LOWER_YES,  0,  FALSE,  TRUE},
    {"Gravity", gravity,    LOWER_NO,   0,  FALSE,  TRUE},
    {"Up down", updown,     LOWER_NO,   0,  FALSE,  TRUE},
    {"Left right",  leftright,  LOWER_NO,   0,  FALSE,  FALSE},
    {"Spray",   spray,      LOWER_YES,  0,  FALSE,  TRUE},
    {"Forward", forward,    LOWER_NO,   0,  FALSE,  FALSE},
    {"Fast forward",fastfwd,    LOWER_NO,   0,  FALSE,  FALSE},
    {"Backward",    backward,   LOWER_YES,  0,  FALSE,  FALSE},
    {"Wobble back", backward,   LOWER_YES,  1,  FALSE,  FALSE},
    {"Sphere",  sphere,     LOWER_SPECTRUM, 0,  FALSE,  TRUE},
    {"Spin",    spin,       LOWER_NO,   1,  FALSE,  FALSE},
    {"Bullseye",    bullseye,   LOWER_NO,   0,  FALSE,  FALSE},
    {"Spiral",  spiral,     LOWER_NO,   1,  FALSE,  FALSE},
    {"Drain",   drain,      LOWER_NO,   1,  FALSE,  FALSE},
    {"Ripple",  ripple,     LOWER_NO,   0,  FALSE,  FALSE},
    {"Prismatic",   prismatic,  LOWER_NO,   0,  FALSE,  TRUE},
    {"Swirl",   swirl,      LOWER_NO,   0,  FALSE,  TRUE},
    {"Tangram", tangram,    LOWER_NO,   64, FALSE,  FALSE},
    {"Divided", divided,    LOWER_NO,   42, FALSE,  TRUE},
    {"Shred",   shred,      LOWER_NO,   1,  FALSE,  TRUE},
    {"Weave",   weave,      LOWER_NO,   0,  FALSE,  TRUE},
    {"Binary",  binary,     LOWER_YES,  0,  TRUE,   FALSE},
    {"Fractal", fractal,    LOWER_NO,   0,  TRUE,   TRUE},
    {"Fractal sphere", sphere,  LOWER_SPECTRUM, 1,  TRUE,   TRUE},
    {"Flow between",flow,       LOWER_NO,   0,  FALSE,  FALSE},
    {"Flow around", flowaround, LOWER_NO,   0,  FALSE,  FALSE}
};



/**
 * This is the main blur function.  The img should have a width and height
 * that is slightly larger than the displayed image, because the perimeter
 * pixels are always reset to 0, and also because pixels are blurred in
 * groups of 8.  For example, if the window is 256x128 then the image should
 * have size 258x130, and img should store ((258*130+7)&~7)=33544 pixels.
 */
int blur(BlurskPrivate *priv,
         int beat /* Boolean: is this a beat? */,
         int quiet /* Boolean: is this the start of a quiet period? */)
{
    int     i, j, k;
    int     transition, transfrom;
    static int  blur_phase = 0, blur_phase2 = 0;
    void        (*blurfunc)(void);
    struct timeval now, start;
    int     newspectrum;    /* boolean: is new signal_style a spectrum? */

    /* convert "transition speed" to a number */
    switch (*config.transition_speed)
    {
      case 'S': transition = 1 + MAXTRANSITION / 200;   break;
      case 'M': transition = 1 + MAXTRANSITION / 50;    break;
      case 'F': transition = 1 + MAXTRANSITION / 8; break;
      default:  transition = MAXTRANSITION;
    }

    /* if size has changed, then start a transition */
    if (img_width != blurwidth || img_height != blurheight)
    {
        /* remember the new size */
        blurwidth = img_width;
        blurheight = img_height;
        blurxcenter = blurwidth / 2;
        blurycenter = blurheight / 2;
        blurlast = img_height * img_bpl;

        /* this counts as a style change, but do it instantly */
        transition = styletransition = MAXTRANSITION;
        stylekeeprandom = 0;
    }

    /* If "Random", and we aren't in a transition, then that counts as
     * a blur change (so we continually transition from one random blur
     * style to another).
     */
    if (!strcmp(config.blur_style, "Random quiet"))
    {
        if (quiet)
            *stylename = '\0';
    }
    else if ((!strncmp(config.blur_style, "Random", 6)
            || !strncmp(config.blur_style, "Flow", 4)
            || !strncmp(config.blur_style, "Wobble", 6))
        && styletransition < 0
        && --stylekeeprandom < 0)
    {
        *stylename = '\0';
    }

    /* If blur style or stencil has changed, then switch to new style &
     * stencil, and start a transition to make it take effect.
     */
    newspectrum = (*config.signal_style == 'M'   /* Mono spectrum */
            || *config.signal_style == 'S'); /* Stereo spectrum */
    if (strcmp(config.blur_style, stylename)
     || strcmp(config.blur_stencil, stencilname)
     || strcmp(config.blur_when, blurname)
     || newspectrum != isspectrum)
    {
        /* store the new info */
        strcpy(stylename, config.blur_style);
        strcpy(stencilname, config.blur_stencil);
        strcpy(blurname, config.blur_when);
        isspectrum = newspectrum;

        /* find the setup function for this style */
        if (!strcmp(config.blur_style, "Random quiet"))
        {
            i = rand_0_to(QTY(styles));
            stylekeeprandom = 0;
        }
        else if (!strcmp(config.blur_style, "Random slow"))
        {
            i = rand_0_to(QTY(styles));
            stylekeeprandom = KEEP_RANDOM_SLOW;
        }
        else if (!strcmp(config.blur_style, "Random"))
        {
            i = rand_0_to(QTY(styles));
            stylekeeprandom = KEEP_RANDOM;
        }
        else
        {
            for (i = 0; i < QTY(styles) && strcmp(styles[i].name, stylename); i++)
            {
            }
        }

        /* if invalid, then use the first function */
        if (i >= QTY(styles))
        {
            i = 0;
        }

        /* remember the new style setup function */
        stylefunc = styles[i].stylefunc;

        /* remember how this motion interacts with stencils */
        blurintostencil = styles[i].blurintostencil;

        /* remember how this motion prefers to handle edge/area smooth*/
        edgesmooth = styles[i].edgesmooth;

        /* reset the transition counter */
        salt = 0;
        styletransition = MAXTRANSITION;

        /* remember whether this style lowers the signal */
        styleprevlower = stylelower;
        switch (styles[i].lower)
        {
          case LOWER_NO:    stylelower = FALSE; break;
          case LOWER_YES:   stylelower = TRUE;  break;
          case LOWER_SPECTRUM:  stylelower = isspectrum;break;
        }

        /* if this blur function needs random numbers, generate now */
        randval[0] = 0;
        for (j = 0; j < styles[i].nrandoms; j++)
            randval[j] = visual_rand();

        /* choose a stencil */
        blur_stencil = bitmap_index(config.blur_stencil);

        /* choose a blur intensity */
        if (!strcmp(config.blur_when, "Random blur"))
            blurchar = "NRFMS"[rand_0_to(5)];
        else
            blurchar = *config.blur_when;
    }

    /* Decide which blur function to use */
    switch (blurchar)
    {
      case 'N': /* No blur */
        blurfunc = loopsharp;
        break;

      case 'R':     /* Reduced blur */
        blur_phase = (blur_phase % 5) + 1;
        switch (blur_phase)
        {
          case 1:   blurfunc = loopreduced1;    break;
          case 2:   blurfunc = loopreduced2;    break;
          case 3:   blurfunc = loopreduced4;    break;
          case 4:   blurfunc = loopreduced3;    break;
          default:
            blur_phase2 = (blur_phase2 & 0x3) + 1;
            switch (blur_phase2)
            {
              case 1:   blurfunc = loopreduced1;    break;
              case 2:   blurfunc = loopreduced2;    break;
              case 3:   blurfunc = loopreduced4;    break;
              default:  blurfunc = loopreduced3;    break;
            }
        }
        break;

      case 'F': /* Full blur */
        blurfunc = loopblur;
        break;

      case 'S': /* Smear */
        blurfunc = loopsmear;
        break;

      case 'M': /* Melt */
        blurfunc = loopmelt;
        break;

      default: /* case 'B': Blur on beats */
        blurfunc = beat ? loopblur : loopsharp;
        break;
    }

    /* If simple motion & not blurring, then we're done */
    if (styletransition < 0 && stylefunc == simple && blurfunc == loopsharp)
    {
        return 0;
    }

    /* if in transition, then do some more dithered points */
    transfrom = styletransition;
    gettimeofday(&start, NULL);
    while (transition > 0 && styletransition >= 0)
    {
        transition--;
        styletransition--;
        for (i =  dither[styletransition];
             i < blurlast;
             i += MAXTRANSITION)
        {
            /* edges & stencil are always 0, else use stylefunc */
            if (i % img_bpl < img_width &&
                (blur_stencil < 0 ||
                !bitmap_test(blur_stencil, i % img_bpl, i / img_bpl)))
            {
                /* call stylefunc to find the source delta */
                j = i + (*stylefunc)(i);

                /* Work around the stencil; i.e., if the source
                 * would be in the stencil then try to move
                 * through the stencil to find the pixel on the
                 * other side of it.  EXCEPT if no motion then
                 * that would be wasted effort so skip it.
                 */
                if (j != i && blur_stencil >= 0 && !blurintostencil)
                {
                    for (k = 10;
                         --k >= 0 &&
                        j >= 0 &&
                        j <= blurlast &&
                        bitmap_test(blur_stencil, j % img_bpl, j / img_bpl);
                         j += (*stylefunc)(j))
                    {
                    }
                }

                /* Verify that the result is reasonable.  It's
                 * easier to check here than in every styelfunc.
                 */
                if (j < 0 || j > blurlast)
                {
                    j = i;
                }
                img_source[i] = &img_buf[j];
            }
            else
                img_source[i] = &img_buf[i];
        }

        /* Never allow more than MAXUSEC per frame */
        gettimeofday(&now, NULL);
        if ((now.tv_sec - start.tv_sec) * 1000000 + now.tv_usec - start.tv_usec > MAXUSEC)
            break;
    }

    /* Give the colormap a chance to transition smoothly too. */
    color_transition(priv, transfrom, styletransition, MAXTRANSITION);

    /* Perform the blur */
    if (edgesmooth)
        /* Normal blurring, usually gives stable edges */
        (*blurfunc)();
    else
    {
        /* Alternate blurring, usually gives smoother areas */
        static int odd = 1;
        odd = -odd;
        img_bpl *= odd;
        (*blurfunc)();
        img_bpl *= odd;
    }
    img_copyback();

    /* Return the amount by which the signal should be lowered */
    if (stylelower && !styleprevlower)
        return (blurheight * (MAXTRANSITION - styletransition + 1))
                / (6 * MAXTRANSITION);
    else if (styleprevlower && !stylelower)
        return (blurheight * (styletransition + 1))
                / (6 * MAXTRANSITION);
    else if (stylelower && styleprevlower)
        return blurheight / 6;
    else
        return 0;
}

/**
 * Return the name of the i'th blur style (including "Random") 
 */
char *blur_name(int i)
{
    if (i < QTY(styles))
        return styles[i].name;
    if (i == QTY(styles))
        return "Random";
    if (i == QTY(styles) + 1)
        return "Random slow";
    if (i == QTY(styles) + 2)
        return "Random quiet";
    return NULL;
}

/**
 * Return the name of the i'th blur_when method 
 */
char *blur_when_name(int i)
{
    static char *names[] = {
        "No blur", "Reduced blur", "Full blur", "Melt", "Smear",
        "Random blur", "Blur on beats"
    };

    if (i < QTY(names))
        return names[i];
    return NULL;
}
