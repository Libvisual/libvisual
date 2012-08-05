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

/* color.c */

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

/* A brief discussion about how this works:
 *
 * GDK packs the eight-bit R/G/B values into a 32-bit integer, as 0x00RRGGBB.
 * Blursk uses the 8 extra bits to store the intensity of the background
 * color.  The (*stylefunc)() function returns these 32-bit values, and the
 * color[] array stores them in this format.
 *
 * The background intensity is similar to an "alpha" channel, thought the
 * implementation details are different.  When blending the background color
 * with the RGB color, a background intensity of 0 means the RGB value is
 * used unchanged.  A background intensity of 255 adds the background color
 * to the RGB color WITHOUT DIMINISHING THE RGB COLOR.  If you're not careful,
 * you can cause overflow problems when adding the background color.  All of
 * the color styles implemented here have taken this into consideration;
 * They only combine high-intensity backgrounds with low-brightness RGB values.
 *
 * The actual blending is done in the color_bg() function.  It is called for
 * each frame, to support the "Flash bkgnd" setting.  With other background
 * settings, it usually just returns without doing anything.
 *
 * The end result of blending is a new GDK colormap.  The color[] array
 * itself is left unchanged.  It *must* be left unchanged, because changing
 * it would cause problems when you wanted to flash the background for the
 * next frame.
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <libvisual/libvisual.h>
#include "actor_blursk.h"
#include "blursk.h"

typedef struct
{
        double  hue, saturation, value;
} hsv_t;

/* This is where Blursk stores its version of the colors */
uint32_t colors[256];

/* This function pointer refers to a function which can be called to compute
 * the value of a given cell in color_map.  It is only called when setting up
 * the color_map; it is *not* called for every frame.
 */
static int32_t (*stylefunc)(int32_t);

/* These are the R/G/B components of the base color */
static int32_t red, green, blue;

/* This is a random color, which is used for the "Colored background" setting */
static int32_t tored, togreen, toblue;
static int tonew;
static int32_t fromred, fromgreen, fromblue;
static int32_t bgred, bggreen, bgblue;
static char bgletter; /* first letter of chosen bkgnd, after "Random" */

static int  transition_bound;

/*---------------------------------------------------------------------------*/

/* Convert a color from RGB format to HSV format */
static hsv_t *rgb_to_hsv(int32_t rgb)
{
    static hsv_t    hsv;    /* HSV value (saved between invocations */
    double      r, g, b;/* the RGB components, in range 0.0 - 1.0 */
    double      max, min;/* extremes from r, g, b */
    double      delta;  /* difference between max and min */

    /* extract the RGB components from rgb */
    r = (double)((rgb >> 16) & 0xff) / 255.0;
    g = (double)((rgb >> 8) & 0xff) / 255.0;
    b = (double)(rgb & 0xff) / 255.0;

    /* find max and min */
    if (r > g)
    {
        max = (b > r) ? b : r;
        min = (g > b) ? b : g;
    }
    else
    {
        max = (b > g) ? b : g;
        min = (r > b) ? b : r;
    }

    /* compute "value" */
    hsv.value = max;

    /* compute "saturation" */
    hsv.saturation = (max > 0.0) ? (max - min) / max : 0;

    /* compute "hue".  This is the hard one */
    delta = max - min;
    if (delta <= 0.001)
    {
        /* gray - any hue will work */
        hsv.hue = 0.0;
    }
    else
    {
        /* divide hexagonal color wheel into three sectors */
        if (max == r)
            /* color is between yellow and magenta */
            hsv.hue = (g - b) / delta;
        else if (max == g)
            /* color is between cyan and yellow */
            hsv.hue = 2.0 + (b - r) / delta;
        else /* max == b */
            /* color is between magenta and cyan */
            hsv.hue = 4.0 + (r - g) / delta;

        /* convert hue to degrees */
        hsv.hue *= 60.0;

        /* make sure hue is not negative */
        if (hsv.hue < 0.0)
            hsv.hue += 360.0;
    }

    /* return the computed color */
    return &hsv;
}


/* convert a color from HSV format to RGB format */
static int32_t hsv_to_rgb(hsv_t *hsv)
{
    int32_t r, g, b;/* RGB color components */
    double  h;  /* copy of the "hsv.hue" */
    double  i, f;   /* integer and fractional parts of "h" */
    int p, q, t;/* permuted RGB values, in integer form */
    int v;  /* "hsv.value", in integer form */

    if (hsv->saturation < 0.01)
    {
        /* simple gray conversion */
        r = g = b = (int32_t)(hsv->value * 255.0);
    }
    else
    {
        /* convert hue to range [0,6) */
        h = hsv->hue / 60.0;
        while (h >= 6.0)
            h -= 6.0;

        /* break "h" down into integer and fractional parts. */
        i = floor(h);
        f = h - i;

        /* compute the permuted RGB values */
        v = (int)(hsv->value * 255.0);
        p = (int)((hsv->value * (1.0 - hsv->saturation)) * 255.0);
        q = (int)((hsv->value * (1.0 - (hsv->saturation * f))) * 255.0);
        t = (int)((hsv->value * (1.0 - (hsv->saturation * (1.0 - f)))) * 255.0);

        /* map v, p, q, and t into red, green, and blue values */
        switch ((int)i)
        {
          case 0:   r = v, g = t, b = p;    break;
          case 1:   r = q, g = v, b = p;    break;
          case 2:   r = p, g = v, b = t;    break;
          case 3:   r = p, g = q, b = v;    break;
          case 4:   r = t, g = p, b = v;    break;
          default:  r = v, g = p, b = q;    break;
        }
    }

    /* return the RGB value as a int32_t */
    return ((((int32_t)r & 0xff) << 16)
        | (((int32_t)g & 0xff) << 8)
        | ((int32_t)b & 0xff));
}


/*---------------------------------------------------------------------------*/


/* The following functions are all possible values for stylefunc.  A list of
 * them is stored at the end of the list.
 */


static int32_t dimming(int32_t i)
{
    return (((int32_t)(i * red / 256) << 16)
        | ((int32_t)(i * green / 256) << 8)
        | ((int32_t)(i * blue / 256))
        | ((255 - i) << 24));
}

static int32_t brightening(int32_t i)
{
    i = 255 - i;

    return (((int32_t)(i * red / 256) << 16)
        | ((int32_t)(i * green / 256) << 8)
        | ((int32_t)(i * blue / 256))
        | ((255 - i) << 24));
}

static int32_t milky(int32_t i)
{
    int32_t r, g, b, tmp, k;
    if (i < 128)
    {
        r = i * red / 128;
        g = i * green / 128;
        b = i * blue / 128;
        k = (127 - i) << 25;
    }
    else
    {
        tmp = 255 - i;
        r = 255 - (255 - red) * tmp / 128;
        g = 255 - (255 - green) * tmp / 128;
        b = 255 - (255 - blue) * tmp / 128;
        k = 0;
    }
    tmp = (r << 16) | (g << 8) | b;
    if (*config.overall_effect == 'B') /* "Bump effect" */
    {
#if 0
        if (i == 128)
            tmp -= (tmp >> 2) & 0x3f3f3f;
        else if (i == 127 || i == 129)
            tmp -= (tmp >> 3) & 0x1f1f1f;
#else
        if (i == 128)
        {
            tmp = (tmp >> 2) & 0x3f3f3f;
            k = 0xc0000000;
        }
        else if (i == 127 || i == 129)
        {
            tmp = (tmp >> 1) & 0x7f7f7f;
            k = 0x80000000;
        }
#endif
    }
    return tmp | k;
}

static int32_t cloud(int32_t i)
{
    int32_t faded;  /* r/g/b level of gray version of color */
    int32_t r, g, b, k;

    /* Compute the gray version */
    faded = (red * 4 + green * 5 + blue * 3) / 12;

    /* handle a few specific colors */
    if (i == 128 && *config.overall_effect == 'B') /* "Bump effect" */
    {
        /* Use the given color */
        r = red;
        g = green;
        b = blue;
        k = 0;
    }
    else if ((i == 129 || i == 127) && *config.overall_effect == 'B') /* "Bump effect" */
    {
        /* Use a faded version of the color */
        r = (red + faded) / 2;
        g = (green + faded) / 2;
        b = (blue + faded) / 2;
        k = 0;
    }
    else if (i > 192)
    {
        /* transition between the given color and white */
        i -= 192;
        r = (red * i + 255 * (63 - i)) / 64;
        g = (green * i + 255 * (63 - i)) / 64;
        b = (blue * i + 255 * (63 - i)) / 64;
        k = 0;
    }
    else if (i > 128)
    {
        /* transition between white and faded */
        i -= 128;
        r = g = b = (255 * i + faded * (63 - i)) / 64;
        k = 0;
    }
    else
    {
        /* transition between faded and black */
        r = g = b = faded * i / 128;
        k = (127 - i) << 25;
    }

    /* Construct a color value from r/g/b, and return it */
    return (r << 16) | (g << 8) | b | k;
}

static int32_t metal(int32_t i)
{
    int32_t r, g, b, k;

    if (i < 128)
    {
        r = red;
        g = green;
        b = blue;
    }
    else
    {
        r = g = b = 0xe0;
        i = 255 - i;
    }
    if (i > 120)
    {
        i = 120 - (i - 120) * 120 / 7;
    }
    r = r * i / 120;
    g = g * i / 120;
    b = b * i / 120;
    k = (120 - i) << 25;
    return ((r << 16) | (g << 8) | b | k);
}

static int32_t layers(int32_t i)
{
    int32_t k;

    /* compute the brightness */
    if (i < 0x40)
    {
        k = 63 - i;
        i = i << 2;
    }
    else if (i < 0x80)
    {
        k = 0;
        i = (i << 2) & 0xff;
    }
    else
    {
        k = 0;
        i = (i << 1) & 0xff;
    }

    /* set this color */
    return (((int32_t)(i * red / 256) << 16)
        | ((int32_t)(i * green / 256) << 8)
        | ((int32_t)(i * blue / 256))
        | (k << 26));
}

static int32_t colorlayers(int32_t i)
{
    int32_t tmp, r, g, b, k;

    /* shift the hue */
    r = red;
    g = green;
    b = blue;
    switch (i & 0xc0)
    {
      case 0x00:
        tmp = r;
        r = (r + g * 2) / 3;
        g = (g + b * 2) / 3;
        b = (b + tmp * 2) / 3;
        /* fall through, so color gets shifted twice... */

      case 0x40:
        tmp = r;
        r = (r + g * 2) / 3;
        g = (g + b * 2) / 3;
        b = (b + tmp * 2) / 3;
        break;
    }

    /* compute the brightness and k */
    if (i < 0x40)
    {
        tmp = i << 2;
        k = 63 - i;
    }
    else if (i < 0x80)
    {
        tmp = (i << 2) & 0xff;
        k = 0;
    }
    else
    {
        tmp = (i << 1) & 0xff;
        k = 0;
    }

    /* set this color */
    return (((int32_t)(tmp * r / 256) << 16)
        | ((int32_t)(tmp * g / 256) << 8)
        | ((int32_t)(tmp * b / 256))
        | k << 26);
}

static int32_t colorstandoff(int32_t i)
{
    int32_t tmp, r, g, b, k;

    /* shift the hue */
    r = red;
    g = green;
    b = blue;
    switch (i & 0xc0)
    {
      case 0x00:
        tmp = r;
        r = (r + g * 2) / 3;
        g = (g + b * 2) / 3;
        b = (b + tmp * 2) / 3;
        /* fallthrough, so color gets shifted twice... */

      case 0x40:
        tmp = r;
        r = (r + g * 2) / 3;
        g = (g + b * 2) / 3;
        b = (b + tmp * 2) / 3;
        break;
    }

    /* compute the brightness and k */
    if (i < 0x80)
    {
        tmp = (i & 0x3f) << 3;
    }
    else
    {
        tmp = (i & 0x7f) << 2;
    }
    if (tmp >= 256)
        tmp = 511 - tmp;
    if (i < 0x20)
        k = (31 - i);
    else
        k = 0;

    /* set this color */
    return (((int32_t)(tmp * r / 256) << 16)
        | ((int32_t)(tmp * g / 256) << 8)
        | ((int32_t)(tmp * b / 256))
        | k << 27);
}

static int32_t flame(int32_t i)
{
    hsv_t   hsv;
    int32_t k;

    /* Get the base color */
    hsv = *rgb_to_hsv(config.color);

    /* Change the hue, and maybe brightness, depending on i */
    hsv.hue += (255 - i) / 4;
    if (hsv.hue >= 360.0)
        hsv.hue -= 360.0;
    k = 0;
    if (i < 64)
    {
        hsv.value *= (double)i / 64.0;
        k = 63 - i;
    }
    else if (i > 192)
        hsv.value *= (double)(319 - i) / 128;

    /* Convert it back to RGB */
    return hsv_to_rgb(&hsv) | (k << 26);
}

static int32_t rainbow(int32_t i)
{
    hsv_t   hsv;
    int32_t k;

    /* Get the base color */
    hsv = *rgb_to_hsv(config.color);

    /* Change the hue, and maybe brightness, depending on i */
    hsv.hue += 2 * (255 - i);
    if (hsv.hue >= 360.0)
        hsv.hue -= 360.0;
    if (i < 64)
    {
        hsv.value *= (double)i / 64.0;
        k = (255 - i) << 26;
    }
    else
        k = 0;

    /* Convert it back to RGB */
    return hsv_to_rgb(&hsv) | k;
}

static int32_t standoff(int32_t i)
{
    int k;

    /* compute the brightness */
    k = 0;
    if (i >= 128)
    {
        i = 0;
    }
    else
    {
        if (i >= 64)
            i = (128 - i) * 4;
        else
        {
            i *= 4;
            k = 255 - i;
        }
        if (i > 254)
            i = 254;
    }

    /* set this color */
    return (((int32_t)(i * red / 256) << 16)
        | ((int32_t)(i * green / 256) << 8)
        | ((int32_t)(i * blue / 256))
        | (k << 24));
}

static int32_t threshold(int32_t i)
{
    /* always return the base color.  This is only interesting when it
     * is modified via contour lines, or by the standard rule that color
     * 0 is always black.
     */
    return config.color;
}

static int32_t stripes(int32_t i)
{
    int32_t tmp, k;

    /* compute the brightness and k */
    k = 0;
    if (i >= 0xd0)
    {
        tmp = 254;
    }
    else
    {
        switch (i & 0x18)
        {
          case 0x00: tmp = (i & 0x7) << 5;  break;
          case 0x18: tmp = ((~i) & 0x7) << 5;   break;
          default: tmp = 254;
        }
        if (i < 64)
        {
            tmp = (tmp * i) >> 6;
            k = 63 - i;
        }
    }

    /* set this color */
    return (((int32_t)(tmp * red / 256) << 16)
        | ((int32_t)(tmp * green / 256) << 8)
        | ((int32_t)(tmp * blue / 256))
        | (k << 26));
}

static int32_t colorstripes(int32_t i)
{
    int32_t r, g, b, k, tmp;
    static int32_t brightness[] = {0, 64, 128, 192, 254, 254, 254, 254, 254, 254, 254, 254, 254, 192, 128, 64};

    /* compute the hue */
    tmp = i & 0x30;
    switch (i & 0xc0)
    {
      case 0x40:
        r = (green * tmp + red * (0x3f - tmp)) >> 6;
        g = (blue * tmp + green * (0x3f - tmp)) >> 6;
        b = (red * tmp + blue * (0x3f - tmp)) >> 6;
        break;

      case 0x80:
        r = (blue * tmp + green * (0x3f - tmp)) >> 6;
        g = (red * tmp + blue * (0x3f - tmp)) >> 6;
        b = (green * tmp + red * (0x3f - tmp)) >> 6;
        break;

      default:
        r = (red * tmp + blue * (0x3f - tmp)) >> 6;
        g = (green * tmp + red * (0x3f - tmp)) >> 6;
        b = (blue * tmp + green * (0x3f - tmp)) >> 6;
    }

    /* compute the brightness and k */
    k = 0;
    if (i >= 0xf0)
    {
        tmp = 254;
    }
    else
    {
        tmp = brightness[i & 0xf];
        if (i < 64)
        {
            tmp = (tmp * i) >> 6;
            k = 64 - i;
        }
    }

    /* set this color */
    return (((int32_t)(tmp * r / 256) << 16)
        | ((int32_t)(tmp * g / 256) << 8)
        | ((int32_t)(tmp * b / 256))
        | (k << 26));
}

static int32_t colorbands(int32_t i)
{
    int32_t r, g, b, k, tmp;

    /* compute the hue */
    tmp = i & 0x20;
    switch (i & 0xc0)
    {
      case 0x40:
        r = (green * tmp + red * (0x3f - tmp)) >> 6;
        g = (blue * tmp + green * (0x3f - tmp)) >> 6;
        b = (red * tmp + blue * (0x3f - tmp)) >> 6;
        break;

      case 0x80:
        r = (blue * tmp + green * (0x3f - tmp)) >> 6;
        g = (red * tmp + blue * (0x3f - tmp)) >> 6;
        b = (green * tmp + red * (0x3f - tmp)) >> 6;
        break;

      default:
        r = (red * tmp + blue * (0x3f - tmp)) >> 6;
        g = (green * tmp + red * (0x3f - tmp)) >> 6;
        b = (blue * tmp + green * (0x3f - tmp)) >> 6;
    }

    /* compute the brightness & k */
    if (i >= 0x40)
    {
        tmp = 254;
        k = 0;
    }
    else
    {
        tmp = i * 4;
        k = 63 - i;
    }

    /* set this color */
    return (((int32_t)(tmp * r / 256) << 16)
        | ((int32_t)(tmp * g / 256) << 8)
        | ((int32_t)(tmp * b / 256))
        | (k << 26));
}

static int32_t graying(int32_t i)
{
    int32_t faded, tmp;

    /* Compute the fully faded color's intensity.  Note that we actually
     * make it slightly dimmer than the base color, because it seems to
     * look better that way.
     */
    faded = (red * 4 + green * 5 + blue * 3) / 16;

    /* colormap is divided into two phases: fading and dimming */
    if (i < 64)
    {
        /* full gray, becoming dimmer */
        return ((faded * i * 4) >> 8) * 0x010101 | ((63 - i) << 26);
    }
    else
    {
        /* full brightness, but fading to gray */
        i -= 64;
        tmp = 192 - i;
        return (((i * red + tmp * faded) / 192) << 16)
            | (((i * green + tmp * faded) / 192) << 8)
            | ((i * blue + tmp * faded) / 192);
    }
}

static int32_t noise(int32_t i)
{
    if (rand_0_to(256) < i)
        return config.color;
    else
        return 0xff000000;
}

/* This is a list of the known color styles */
static struct colorstyles
{
    char     *name;
    int32_t (*func)(int32_t);
    int good_for_bump;
} colorstyles[17] =
{
    {"Dimming",     dimming,    TRUE},
    {"Brightening",     brightening,    TRUE},
    {"Milky",       milky,      TRUE},
    {"Graying",     graying,    TRUE},
    {"Flame",       flame,      TRUE},
    {"Cloud",       cloud,      TRUE},
    {"Metal",       metal,      TRUE},
    {"Layers",      layers,     FALSE},
    {"Color layers",    colorlayers,    FALSE},
    {"Standoff",        standoff,   FALSE},
    {"Color standoff",  colorstandoff,  FALSE},
    {"Threshold",       threshold,  FALSE},
    {"Stripes",     stripes,    FALSE},
    {"Color stripes",   colorstripes,   FALSE},
    {"Color bands",     colorbands, FALSE},
    {"Rainbow",     rainbow,    FALSE},
    {"Noise",       noise,      FALSE}
};

/* Compute the color of a single cell in the colormap.  This uses (*stylefunc)()
 * and also checks the other relevant options.
 */
static int32_t cell(int i)
{
    int32_t c;

    /* The white_signal option forces color 255 to be white */
    if (i == 255 && *config.signal_color == 'W')
        return 0x00ffffff;

    /* The last three cells are always the background color */
    if (i < 3)
        return 0xff000000;

    /* We can also force contour lines to be white.  These look
     * better if we also have a half-white/half-colored value on
     * either side of it; notice the tricky way we accomplish that.
     */
    if (config.contour_lines)
    {
        switch ((i + 8) & 0x1f)
        {
          case 0x00:
          case 0x01:
          case 0x1f:
          case 0x1e:
            /* solid white */
            c = 0x00ffffff;
            break;

          case 0x02:
          case 0x1d:
            /* mixed white & computed color*/
            c = (*stylefunc)(i);
            c = (((c & 0xfefefe) + 0xfefefe) / 2);
            break;

          default:
            /* Just compute the color */
            c = (*stylefunc)(i);
        }
    }
    else
        c = (*stylefunc)(i);

    /* Return the color */
    return c;
}

static void choosebg(int do_random)
{
    /* "Random", then choose a background */
    if (do_random)
    {
        if (!strncmp(config.background, "Random", 6))
            bgletter = "BWDSCF"[rand_0_to(6)];
        else
            bgletter = *config.background;
    }

    /* Choose new background color.  Note that we don't handle
     * "Flash bkgnd" here.
     */
    switch (bgletter)
    {
      case 'W': /* White bkgnd */
        tored = togreen = toblue = 230;
        break;

      case 'D': /* Dark bkgnd */
        tored = red / 2;
        togreen = green / 2;
        toblue = blue / 2;
        break;

      case 'S': /* Shift bkgnd */
        tored = blue;
        togreen = red;
        toblue = green;
        break;

      case 'C': /* Color bkgnd */
        if (do_random)
        {
            tored = rand_0_to(255);
            togreen = rand_0_to(255);
            toblue = rand_0_to(255);
        }
        else
        {
            tored = fromred;
            togreen = fromgreen;
            toblue = fromblue;
        }
        break;

      default: /* Black bkgnd, and also fake Flash bkgnd */
        tored = togreen = toblue = 0;
    }
    tonew = TRUE;
}


/* This recalculates a portion of the colors, and then activates the new
 * colormap.  If from==scale then it also chooses a new random color style.
 * This is called from blur() during its transitions.
 */
void color_transition(
    BlurskPrivate *priv,
    int from,   /* highest-numbered cell to change, scaled */
    int to,     /* lowest-numbered cell to change, scaled */
    int scale)  /* highest possible value of to & from */
{
    VisColor *pal_colors = visual_palette_get_colors (priv->pal);

#if 1
    if (to < 0)
        to = 0;
#endif
    /* Choose a random "Colored background" color at the start of the
     * transition, if necessary
     */
    if (from == scale)
    {
        /* Previous transition must be complete, I guess */
        fromred = tored;
        fromgreen = togreen;
        fromblue = toblue;

        choosebg(TRUE);
    }

    /* Do the background color transition */
    if (to <= 0)
    {
        bgred = tored;
        bggreen = togreen;
        bgblue = toblue;
    }
    else
    {
        bgred = (tored * (scale - to) + fromred * to) / scale;
        bggreen = (togreen * (scale - to) + fromgreen * to) / scale;
        bgblue = (toblue * (scale - to) + fromblue * to) / scale;
    }

    /* if colorstyle isn't "random" then do nothing more */
    if (strcmp(config.color_style, "Random"))
        return;

    /* if from==scale then choose a new random color style */
    if (from == scale)
        stylefunc = colorstyles[rand_0_to(QTY(colorstyles))].func;

    /* scale the numbers to match the size of the color table */
    from = from * 255 / scale;
    to = to * 255 / scale;

    /* recompute ONLY the affected cells */
    for (; from > to; from--)
    {
        colors[from] = cell(from);
        visual_color_set_from_uint32(&pal_colors[from], colors[from]);
    }

    /* Adjust the background, and then activate the new colormap.  */
    tonew = TRUE;
    color_bg(priv, 0, NULL);

    /* Remember the lower bound of the transition.  Other color changes
     * will be limited to be no lower than this value, so that changing
     * the hue or contour won't defeat the gradual transition.  (The
     * hue or contour change will be effected for the remaining color
     * cells as a natural consequence of the transition.)
     */
    transition_bound = to;
}


/* Generate a colormap, and install it.  This function is called once when the
 * plugin starts, and again whenever the configuration is changed in a way
 * which affects the colormap.
 */
void color_genmap(BlurskPrivate *priv, int do_random)
{
    VisColor *pal_colors = visual_palette_get_colors (priv->pal);
    int32_t i;

    /* Decompose the dominant color into R/G/B components */
    red = (int32_t)(config.color / 0x10000);
    green = (int32_t)((config.color % 0x10000)/0x100);
    blue = (int32_t)(config.color % 0x100);

    /* Choose a new background, if appropriate */
    choosebg(do_random);
    bgred = fromred = tored;
    bggreen = fromgreen = togreen;
    bgblue = fromblue = toblue;
    tonew = TRUE;

    /* Find the name in the colorstyles[] table */
    if ((do_random || !stylefunc) && !strcmp(config.color_style, "Random"))
    {
        /* Choose a "Random" colorstyle */
        stylefunc = colorstyles[rand_0_to(QTY(colorstyles))].func;
    }
    else if (!stylefunc || strcmp(config.color_style, "Random"))
    {
        /* Use the named colorstyle */
        for (i = 0;
             i < QTY(colorstyles)
            && strcmp(colorstyles[i].name, config.color_style);
             i++)
        {
        }
        if (i >= QTY(colorstyles))
            i = 0;
        stylefunc = colorstyles[i].func;

        /* Transitions only affect "Random" colorstyle, not this one */
        transition_bound = 0;
    }

    /* Generate the basic colormap */
    for (i = 255; i >= transition_bound; i--)
    {
        colors[i] = cell(i);
        visual_color_set_from_uint32(&pal_colors[i], colors[i]);
    }

    /* Adjust the background, and then activate the new colormap.  */
    tonew = TRUE;
    color_bg(priv, 0, NULL);
}

/* This function is called once for each frame, before the frame's image is
 * output.  It adjusts the colormap's background color in response to the music.
 */
void color_bg(BlurskPrivate *priv, int ndata, int16_t *data)
{
    VisColor *pal_colors = visual_palette_get_colors (priv->pal);

    int32_t bgr, bgb, bgg, k, bg;
    int i, j;
    int16_t max, min;
    int32_t totdelta;
    int32_t newcolors[256];
    static int32_t fallr, fallg, fallb;

    /* if we aren't doing "Flash bkgnd" and we've reached our final color,
     * then do nothing
     */
    if (bgletter != 'F'
     && bgred == tored && bggreen == togreen && bgblue == toblue)
    {
        if (!tonew)
            return;
        tonew = FALSE;
    }

    /* force colors[0] to be the background color */
    colors[0] = 0xff000000;

    /* compute the RGB background color, based on data */
    if (bgletter != 'F' || ndata == 0)
    {
        /* Use the transition colors */
        bgr = bgred;
        bgg = bggreen;
        bgb = bgblue;
    }
    else /* "Flash bkgnd" */
    {
        if (nspectrums == 0)
        {
            /* data is samples */

            /* examine the data */
            max = min = data[0];
            totdelta = 0;
            for (i = 1; i < ndata; i++)
            {
                if (data[i] < min)
                    min = data[i];
                else if (data[i] > max)
                    max = data[i];
                totdelta += abs(data[i] - data[i - 1]);
            }

            /* compute red from the difference between max & min */
            bgr = (max - min) >> 8;

            /* compute blue from delta. */
            bgb = totdelta / (ndata * 16);

            /* green is just the average of those two */
            bgg = (bgr + bgb) >> 1;
        }
        else
        {
            /* data is spectrum.  If stereo then we only care
             * about the second one.  (the first is mostly a
             * duplicate of the second, and the first also
             * suffers from being backward -- which looks cool
             * in a graph, but would hurt us here.
             */
            if (nspectrums == 2)
                ndata /= 2, data += ndata;

            /* the lower frequencies are used for red, middle
             * for green, and upper for blue.  The spectrum
             * energy has already been conditioned so we can
             * use the first 1/3 for red, next 1/3 for green
             * and last 1/3 for blue.  The values have also
             * been conditioned to fill the int16_t range.
             */
            for (i = 0, bgr = 0, j = ndata / 3; i < j; i++)
                bgr += data[i];
            bgr /= j;
            for (bgg = 0; i < j * 2; i++)
                bgg += data[i];
            bgg /= j;
            for (bgb = 0; i < ndata; i++)
                bgb += data[i];
            bgb /= (ndata - 2 * j);

            /* we need to scale the values to fit in 0..255 range */
            bgr = (20000 - bgr) >> 7;
            bgg = (20000 - bgg) >> 7;
            bgb = (20000 - bgb) >> 7;
        }

        /* during transition from colored to flash, we never want to
         * be darker than the old color.
         */
        if (bgr < bgred) bgr = bgred;
        if (bgg < bggreen) bgg = bggreen;
        if (bgb < bgblue) bgb = bgblue;

        /* clamp the background color values to be within 0...255.  Also
         * try to avoid dark gray backgrounds by ignoring values < 30
         */
        if (bgr < 30) bgr = 0;
        else if (bgr > 255) bgr = 255;
        if (bgg < 30) bgg = 0;
        else if (bgg > 255) bgg = 255;
        if (bgb < 30) bgb = 0;
        else if (bgb > 255) bgb = 255;

        /* limit the fall-off speed */
        if (bgr < fallr)
            bgr = fallr;
        fallr = bgr - ((bgr + 15) >> 4);
        if (bgg < fallg)
            bgg = fallg;
        fallg = bgg - ((bgg + 15) >> 4);
        if (bgb < fallb)
            bgb = fallb;
        fallb = bgb - ((bgb + 15) >> 4);
    }

    /* build a new colormap, derived from the black-background one */
    for (i = 0; i < 256; i++)
    {
        /* extract the bg brightness.  If 0, then copy unchanged */
        k = (colors[i] >> 24) & 0xff;
        if (k == 0)
        {
            newcolors[i] = colors[i];
            visual_color_set_from_uint32(&pal_colors[i], newcolors[i]);
            continue;
        }

        /* else we need to blend -- extract foreground RGB */
        bg = (((bgr * k) << 8) & 0x00ff0000)
           | ( (bgg * k)       & 0x0000ff00)
           | (((bgb * k) >> 8) & 0x000000ff);
        newcolors[i] = colors[i] + bg;
        visual_color_set_from_uint32(&pal_colors[i], newcolors[i]);
    }
}


/* Return the name of the i'th color style */
char *color_name(int i)
{
    if (i < QTY(colorstyles))
        return colorstyles[i].name;
    else if (i == QTY(colorstyles))
        return "Random";
    return NULL;
}


/* Return the name of the i'th background style */
char *color_background_name(int i)
{
    static char *names[] = { "Black bkgnd", "White bkgnd",
        "Dark bkgnd", "Shift bkgnd", "Color bkgnd",
        "Flash bkgnd", "Random bkgnd", NULL};
    return names[i];
}


/* Return the index of a color style that's good for "bump" mode.  If the given
 * color style is good, return it; else choose a different one and return that.
 * Note that we return its index, not its name, because that works better in
 * the config.c file where this function is used.
 */
int color_good_for_bump(char *given)
{
    int i;

    /* find the given color style in the list */
    for (i = 0; i < QTY(colorstyles); i++)
        if (!strcmp(given, colorstyles[i].name))
            break;

    /* if valid & good for bumps, return it */
    if (i < QTY(colorstyles) && colorstyles[i].good_for_bump)
        return i;

    /* else return the name of one that is known to be good */
    return 0;
}

/* Allow colors to respond to beats */
void color_beat(BlurskPrivate *priv)
{
    hsv_t   hsv;

    /* if hue_on_beats isn't set, then do nothing */
    if (!config.hue_on_beats)
        return;

    /* Compute a new base color.  Tell the config window about it. */
    hsv = *rgb_to_hsv(config.color);
    hsv.hue += 60.0;
    if (hsv.hue > 360.0)
        hsv.hue -= 360.0;
    config.color = hsv_to_rgb(&hsv);

    /* regenerate color map */
    color_genmap(priv, FALSE);

}
