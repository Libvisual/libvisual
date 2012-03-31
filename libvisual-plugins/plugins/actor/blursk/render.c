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

/* render.c */

/*  Blursk - visualization plugin for XMMS
 *  Copyright (C) 1999  Steve Kirkendall
 *
 *  Portions of this file are derived from the XMMS "Blur Scope" plugin.
 *  XMMS is Copyright (C) 1998-1999  Peter Alm, Mikael Alm, Olle Hallnas, Thomas Nilsson and 4Front Technologies
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
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "actor_blursk.h"
#include "blursk.h"

/* The "highlow" rendering mode uses this threshold to trigger the drawing of a
 * bead.  Examine render_highlow() for a description of why this value was
 * chosen.
 */
#define BEAD_THRESHOLD  15000


/* Some of the plotting functions interpolate to generate extra data points.
 * The data points are stored here when that happens.
 */
#define MAXPOINTS   512
static int16_t  renderdata[MAXPOINTS];


/* Draw a line between two points, in a given color */
static void line(int x, int y, int x2, int y2, unsigned char color)
{
    int xdiff, ydiff;
    int error;
    int bpl;
    register unsigned char  *point;
    register int i;

    /* We only draw from left to right.  Swap endpoints otherwise */
    if (x > x2)
    {
        error = x;
        x = x2;
        x2 = error;
        error = y;
        y = y2;
        y2 = error;
    }
    xdiff = x2 - x;

    /* skip if either endpoint is offscreen */
    if (x < 0 || x2 >= img_width)
        return;

    /* Moving upward or downward? */
    if(y < y2)
    {
        /* downward */
        if (y < 0 || y2 >= img_height - 1)
            return;
        bpl = img_bpl;
        ydiff = y2 - y;
    }
    else
    {
        /* upward */
        if (y2 < 0 || y >= img_height - 1)
            return;
        bpl = -img_bpl;
        ydiff = y - y2;
    }

    /* locate the starting point */
    point = &IMG_PIXEL(x, y);

    /* different line strategy, depending on slope */
    if (xdiff == 0)
    {
        /* vertical line */
        for (i = ydiff + 1; --i >= 0; point += bpl)
            *point = color;
    }
    else if (ydiff == 0)
    {
        /* horizontal line */
        memset(point, color, xdiff);
    }
    else if (ydiff < xdiff)
    {
        /* mostly horizontal */
        for (i = xdiff, error = ydiff / 2; --i >= 0; )
        {
            *point++ = color;
            error += ydiff;
            if (error > xdiff)
            {
                error -= xdiff;
                point += bpl;
            }
        }
    }
    else
    {
        /* mostly vertical */
        for (i = ydiff, error = xdiff / 2; --i >= 0; )
        {
            *point = color;
            point += bpl;
            error += xdiff;
            if (error > ydiff)
            {
                error -= ydiff;
                point++;
            }
        }
    }
}

#define addclipped(ptr, add)    if (*(ptr) < 255 - (add)) \
                    *(ptr) += (add); \
                else \
                    *(ptr) = 255;

static void fuzzydot(int x, int y, int add)
{
    int xx, yy;
    int sum;
    unsigned char   *point;

    /* if too near the edge, then skip it */
    if (x < 5 || x >= img_width - 5 || y < 5 || y >= img_height - 5)
        return;

    /* For each point in the dot... */
    for (yy = -4; yy <= 4; yy++)
    {
        for (xx = -4, point = &IMG_PIXEL(x + xx, y + yy);
             xx <= 4;
             xx++, point++)
        {
            sum = (xx * xx + yy * yy + 1) * 4;
            if (sum <= add)
            {
                sum = add - sum;
                addclipped(point, sum);
            }
        }
    }
}

static void plussign(int x, int y, int add)
{
    int extent, i;
    unsigned char   *point;

    /* Compute the extent of this plus sign. */
    extent = add / 4;

    /* if too close to edge, then skip it */
    if (x < extent || x >= img_width - extent || y < extent || y >= img_height - extent)
        return;
    extent -= 1; /* <-- for safety */

    /* Plot the center of the + sign */
    point = &IMG_PIXEL(x, y);
    addclipped(point, add);
    add -= 4;

    /* fill in the corners */
    addclipped(point - img_bpl - 1, add);
    addclipped(point - img_bpl + 1, add);
    addclipped(point + img_bpl - 1, add);
    addclipped(point + img_bpl + 1, add);
    
    /* Plot the surrounding points */
    for (i = 1; i <= extent; i++, add -= 4)
    {
        point = &IMG_PIXEL(x - i, y);
        addclipped(point, add);
        point = &IMG_PIXEL(x + i, y);
        addclipped(point, add);
        point = &IMG_PIXEL(x, y - i);
        addclipped(point, add);
        point = &IMG_PIXEL(x, y + i);
        addclipped(point, add);
    }
}

void render_dot(int x, int y, unsigned char color)
{
    int x2, y2;

    /* convert (x,y) from center of dot to corner of dot */
    x -= 2;
    y -= 2;

    /* ignore if outside the image */
    if (x < 0 || y < 0 || x + 5 >= img_width || y + 5 >= img_height)
        return;

    /* draw the dot */
    for (x2 = 0; x2 <= 4; x2++)
    {
        if (x2 == 0 || x2 == 4)
        {
            for (y2 = 1; y2 <= 3; y2++)
                IMG_PIXEL(x + x2, y + y2) = color;
        }
        else
        {
            for (y2 = 0; y2 <= 4; y2++)
                IMG_PIXEL(x + x2, y + y2) = color;
        }
    }
}


static void airbrush(int x, int y, unsigned char color)
{
    static unsigned char bits[] = {0x24,0x12,0x29,0x1a,0x54,0x02,0x10};
    int i, j, bit;
    unsigned char   *pixel;

    /* ignore if outside the image */
    if ((x -= 3) < 0 || (y -= 3) < 0 || x + 6 >= img_width || y + 6 >= img_height)
        return;

    /* draw a whole collection of points */
    for (i = 0; i <= 6; i++)
    {
        pixel = &IMG_PIXEL(x, y + i);
        for (j = 0, bit = 1; j <= 6; j++, bit += bit)
        {
            if (bits[i] & bit)
                *pixel = color;
            pixel++;
        }
    }
}

static void edges(int x, int y, int thick)
{
    double  frac;   /* X, scaled to be between 0.0 and 1.0 */
    int iw, ih; /* image width & height, minus 20 */
    int color;

    /* verify that the image size is big enough for us to work with */
    if (img_width < 30 || img_height < 30
        || x < 0 || x >= img_width
        || y < 0 || y >= img_height)
        return;

    /* x is scaled to width, and y is scaled to height.  We want to derive
     * a color from y, scaled from 0 - 255, and choose new x & y by scaling
     * x to the length of the perimeter.
     *
     * We begin by computing a new color size
     */
    iw = img_width - 20;
    ih = img_height - 20;

    if (config.thick_on_beats)
        color = (ih - y) * (1600 - 200 * thick) / ih;
    else
        color = (ih - y) * 1300 / ih;
    if (color > 255)
        color = 255;
    else if (color < 0)
        color = 0;

    /* Note: the following divides the perimeter into segments.  The
     * perimeter is assumed to have approximately a 4:3 aspect ratio,
     * so the overall perimeter is scaled to 14 units.  The first segment
     * is the left half of the bottom, which is two units long.  Then
     * it progresses up the left edge, across the top, down the right,
     * and back to the middle again, for a total of 5 segments.  Within
     * each segment, "frac" is first scaled to be between 0.0 and 1.0,
     * and then x and y are computed from that.
     */
    frac = (double)x / (double)img_width * 14.0;
    if ((frac -= 2.0) < 0.0)
    {
        frac = frac / -4.0;
        x = 10 + iw * frac;
        y = 10;
    }
    else if ((frac -= 3.0) < 0.0)
    {
        frac = 1.0 - (frac / -3.0);
        x = 10;
        y = 10 + ih * frac;
    }
    else if ((frac -= 4.0) < 0)
    {
        frac = frac / -4.0;
        x = 10 + iw - iw * frac;
        y = 10 + ih;
    }
    else if ((frac -= 3.0) < 0)
    {
        frac = frac / -3.0;
        x = 10 + iw;
        y = 10 + ih * frac;
    }
    else
    {
        frac = 0.5 + (2.0 - frac) / 4.0;
        x = 10 + iw * frac;
        y = 10;
    }

    /* Now we can plot! */
    render_dot(x, y, color);
}


static int plotfirst;
static int plotthick;
static unsigned char plotcolor;
static double plottheta = 0.0;
static double plotsin, plotcos;
static double plotprevsin, plotprevcos;
static int plotcount;
static int plotmax;
static int plotx[512], ploty[512];
static int plotprevmax;
static int plotprevx[512], plotprevy[512];

static void plotbefore(int thick, int max)
{
    plotthick = thick;
    plotfirst = TRUE;
    plotcount = 0;
    plotmax = max;
    if (*config.signal_color != 'C')
        plotcolor = 255;
    else if (plotcolor == 255)
        plotcolor = 76;
    else
        plotcolor++;
}

static void plot(int x, int y)
{
    static int fromx, fromy;
    int i;

    switch (*config.plot_style)
    {
      case 'D': /* Dots */
        render_dot(x, y, plotcolor);
        break;

      case 'F': /* Fuzzy dots */
        fuzzydot(x, y, plotthick * 5 + 50);
        break;

      case 'P': /* Plus signs */
        plussign(x, y, (plotthick + 4) * 6);
        break;

      case 'A': /* Airbrush */
        airbrush(x, y, plotcolor);
        break;

      case 'E': /* Edges */
        edges(x, y, plotthick);
        break;

      case 'M': /* Mirrored */
        if (!plotfirst)
        {
            line(img_width - 1 - fromx, img_height - 1 - fromy, img_width - 1 - x, img_height - 1 - y, plotcolor);
            if (plotthick >= 2)
                line(img_width - 1 - fromx, img_height - fromy, img_width - 1 - x, img_height - y, plotcolor);
            if (plotthick >= 3)
                line(img_width - fromx, img_height - 1 - fromy, img_width - x, img_height - 1 - y, plotcolor);
        }
        /* fall through... */

      case 'L': /* Line */
        if (plotfirst)
            plotfirst = FALSE;
        else
        {
            line(fromx, fromy, x, y, plotcolor);
            if (plotthick >= 2)
                line(fromx, fromy - 1, x, y - 1, plotcolor);
            if (plotthick >= 3)
                line(fromx - 1, fromy, x - 1, y, plotcolor);
        }
        fromx = x;
        fromy = y;
        break;

      case 'S': /* Starburst */
        line(img_width / 2, img_height / 2, x, y, plotcolor);
        break;

      case 'R': /* Radar */
        x = img_width + 10 - x;
        y = (img_height - 10 - y) * 450 / img_height;
        if (y > 255)
            y = 255;
        else if (y < 0)
            y = 0;
        line((int)(x * plotcos) + img_width/2,
             (int)(x * plotsin) + img_height/2,
             (int)(x * plotprevcos) + img_width/2,
             (int)(x * plotprevsin) + img_height/2,
             y);
        break;

      case 'I': /* Inertia */
        plotx[plotcount] = x;
        ploty[plotcount] = y;
        i = (plotcount * plotprevmax) / plotmax;
        plotcount++;
        line(plotprevx[i], plotprevy[i], x, y, plotcolor);
        if (plotthick >= 2)
            line(plotprevx[i], plotprevy[i]-1, x, y-1, plotcolor);
        if (plotthick >= 3)
            line(plotprevx[i]-1, plotprevy[i], x-1, y, plotcolor);
        break;
    }
}

static void plotafter(void)
{
    int     i, j;
    int x, y;

    /* For "Radar", compute the plotsin & plotcos for the next wedge. */
    if (*config.plot_style == 'R')
    {
        plotprevsin = plotsin;
        plotprevcos = plotcos;
        plottheta = plottheta + 0.1;
        if (plottheta > 2 * VISUAL_MATH_PI)
            plottheta -= 2 * VISUAL_MATH_PI;
        plotsin = (sin(plottheta) * (double)img_height) / (2.1 * (double)img_width);
        plotcos = cos(plottheta) / 2.1;
    }

    /* For "Inertia", compute new plotprevx & plotprevy values */
    if (*config.plot_style == 'I')
    {
        for (i = 0; i < plotmax; i++)
        {
            j = (i * plotprevmax) / plotmax;
            x = (plotx[i] + 7 * plotprevx[j] + 4) >> 3;
            y = (ploty[i] + 7 * plotprevy[j] + 4) >> 3;
            if (abs(x - plotx[i]) > 4 || abs(y - ploty[i]) > 4)
            {
                plotx[i] = x;
                ploty[i] = y;
            }
        }
        memcpy(plotprevx, plotx, plotmax * sizeof *plotprevx);
        memcpy(plotprevy, ploty, plotmax * sizeof *plotprevy);
        plotprevmax = plotmax;

    }
}



/* Adjust the number of data points to be between max/2 and max.  Upon return,
 * the points are copied into renderdata[].  The return value indicates the
 * final number of data points.
 */
static int condition_data(int max, int ndata, int16_t **dataref)
{
    int i, j, step;

    /* if number of data points is already good, just copy it */
    if (ndata > max/2 && ndata <= max)
    {
        return ndata;
    }

    /* if too many data points, then reduce */
    if (ndata > max)
    {
        for (step = (ndata + max - 1) / max, i = 0, j = step/2;
             j < ndata;
             j += step, i++)
        {
            renderdata[i] = (*dataref)[j];
        }
        *dataref = renderdata;
        return i;
    }

    /* else it must be too small.  Expand it, interpolating values and
     * smoothing on each iteration, until it is large enough.
     */
    while (ndata < max/2)
    {
        /* interpolate */
        for (j = ndata - 1, i = ndata * 2 - 2;
             j >= 0;
             j--, i -= 2)
        {
            renderdata[i] = (*dataref)[j];
            renderdata[i + 1] = (renderdata[i] + renderdata[i + 2]) / 2;
        }
        ndata = ndata * 2 - 1;
        *dataref = renderdata;

        /* smooth */
        for (i = 1; i < ndata - 2; i++)
            renderdata[i] = (int16_t)((renderdata[i - 1] + renderdata[i + 1]) * 3 + renderdata[i] * 10) >> 4;
    }
    return ndata;
}

/* Plot a radial spectrum. */
static void render_radial(int thick, int center, int ndata, int16_t *data)
{
    double  theta, step, sig;
    int i;
    int x, y, xscale, xcenter, yscale;
    static int16_t prev[MAXPOINTS];
    static int nprev;
    int32_t trigger;

    /* We'd like to have between 127 and 256 data samples */
    ndata = condition_data(256, ndata, &data);

#if 1
    /* If same number of points as last time (which should normally be the
     * case) then merge previous data with current data.
     */
    if (ndata == nprev)
    {
        for (i = 0; i < ndata; i++)
            prev[i] = ((int32_t)prev[i] + (int32_t)data[i]) >> 1;
        data = prev;
    }
    else
    {
        nprev = ndata;
        memcpy(prev, data, ndata * sizeof(int16_t));
    }
#endif

    /* Some image geometry constants */
    xscale = xcenter = img_width / 2;
    yscale = img_height / 2;

    /* Take samples starting from the center out */
    plotbefore(thick, ndata);
    trigger = BEAD_THRESHOLD * 5;
    for (theta = 0.0, step = 2.0 * VISUAL_MATH_PI / ndata, i = 0;
         i < ndata;
         theta += step, i++)
    {
        trigger -= (50000 - data[i]);
        if (trigger < 0)
        {
            trigger += BEAD_THRESHOLD * 10;
            if (*config.signal_style == 'F')
                sig = (double)(14000 - data[i]) / 30000.0;
            else
                sig = (double)(20000 - data[i]) / 20000.0;
            x = xcenter - xscale * sig * sin(theta);
            y = center - yscale * sig * cos(theta);
            plot(x, y);
        }
    }
    plotafter();
}

/* Draw some dots, based on phase-shifted sample points */
static void render_phase(int thick, int center, int ndata, int16_t *data)
{
    int npoints, i, sample, x, y;

    /* We'd like to have between 256 and 512 data samples */
    ndata = condition_data(512, ndata, &data);

    /* Choose a number of points to draw, based on thick */
    npoints = 12 + 8 * thick;

    /* For each point... */
    plotbefore(thick, ndata);
    for (i = 0; i < npoints; i++)
    {
        /* Derive x & y from uniformly-separated samples */
        sample = i * 3 * ndata / (8 * npoints);
        x = ((int16_t)data[sample] >> 8) + img_width/2;
        y = ((int16_t)data[sample + ndata/8 - 1] >> 9) + center;

        /* Plot the point */
        plot(x, y);
    }
    plotafter();
}

/* Draw some dots, based on the sample values. */
static void render_highlow(int thick, int center, int ndata, int16_t *data)
{
    int16_t *d1, *d2;
    int32_t trigger;
    int i, x, y;

    /* We take values from the first half of the sample data and the
     * second half of the sample data, and use those as X & Y values.
     */
    ndata >>= 1;
    d1 = data;
    d2 = data + ndata;

    /* We don't plot every possible bead.  We accumulate d1 & d2 until
     * their sum exceeds a threshold, and then we plot one bead and reduce
     * the accumulated value.
     *
     * We want to plot perhaps 10 beads per frame.  Assuming they average
     * about 2500 in each sample, and we have 64 sample pairs, we'll want
     * to draw a bead when the trigger hits about 15000.  We'd like to
     * initialize the trigger to about half of that for a medium-thickness
     * sound, so...
     */
    trigger = BEAD_THRESHOLD / 2;

    /* We'll use "thick" to widen the image when loud.  Tweak it. */
    thick++;

    /* Okay, now let's check each pair */
    plotbefore(thick, ndata);
    for (i = 0; i < ndata; i++)
    {
        /* Accumulate the amounts that these differ from 20000 */
        trigger += abs(40000 - d1[i] - d2[i]);

        /* Have we hit the threshold? */
        if (trigger >= BEAD_THRESHOLD)
        {
            trigger -= BEAD_THRESHOLD;

            /* Okay, we need to plot a point derived from the X
             * and Y values in the graph.  We'd like for the graph
             * to be centered horizontally, so we compute X from
             * the current index value, since we know its range.
             * The Y value is computed from the difference between
             * the samples.
             */
            x = img_width / 2 + (((i - ndata/2) * (int32_t)img_width * thick) >> 7);
            y = center + (((int32_t)d1[i] - (int32_t)d2[i]) >> 8);
            plot(x, y);
        }
    }
    plotafter();
}

/* Draw a line graph for the next image.  The input data consists of img_width/2
 * samples, which may be either PCM data, or spectrum data which has been
 * tweaked to resemble PCM data.
 */
static void render_curve(int thick, int center, int ndata, int16_t *data)
{
    int i, x, y;
    int32_t scale;

    /* We'd like to have about 256 sample points.  */
    ndata = condition_data(256, ndata, &data);

    scale = img_height / 4;
    plotbefore(thick, ndata);
    for(i = 0; i < ndata; i++)
    {
        x = i * (img_width - 1) / (ndata - 1);
        y = center + (((int32_t)data[i] * scale) >> 14);
        if(y < 0)
            y = 0;
        if(y >= img_height - 1)
            y = img_height - 2;
        plot(x, y);
    }
    plotafter();
}

/* Generic data rendering function */
void render(int thick, int center, int ndata, int16_t *data)
{
    /* if thick=0, then most plot styles do nothing, but the Radar plot
     * style treats that as though thick=1.
     */
    if (thick == 0)
    {
        if (*config.plot_style == 'R')
            thick = 1;
        else
            return;
    }

    /* Plot the signal */
    switch (*config.signal_style)
    {
      case 'R': /* Radial spectrum */
      case 'F': /* Flower */
        render_radial(thick, center, ndata, data);
        break;

      case 'O': /* Oscilloscope */
      case 'M': /* Mono spectrum */
      case 'S': /* Stereo spectrum */
        render_curve(thick, center, ndata, data);
        break;

      case 'P': /* Phase shift */
        render_phase(thick, center, ndata, data);
        break;

      case 'H': /* High/Low spectrum */
        render_highlow(thick, center, ndata, data);
        break;

      case 'N': /* None */
        break;
    }

}

/**
 * Returns the i'th plot-style
 */
char *render_plotname(int i)
{
    static char *name[] = {
        "Line", "Mirrored", "Dots", "Fuzzy dots", "Plus signs",
        "Airbrush", "Starburst", "Inertia", "Radar", "Edges", "None"
    };

    if (i >= QTY(name))
        return NULL;
    else
        return name[i];
}
/**
 * Returns the i'th signal-style
 */
char *signal_style_name(int i)
{
    static char *names[] =
    {
        "Oscilloscope", "Phase shift", "Flower", "Radial spectrum",
        "High/Low plot", "Stereo spectrum", "Mono spectrum",
        NULL
    };
    return names[i];
}
