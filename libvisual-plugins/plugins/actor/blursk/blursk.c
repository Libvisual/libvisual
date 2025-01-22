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

/* blursk.c */

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

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "actor_blursk.h"
#include "blursk.h"

#define DUMPCORE
#ifdef DUMPCORE
# include <signal.h>
#endif

/* To detect rhythms, blursk tracks the loudness of the signal across multiple
 * frames.  This is the maximum number of frames, and should correspond to the
 * slowest rhythm.
 */
#define BEAT_MAX    200

static void blursk_render_pcm(BlurskPrivate *priv, int16_t *data);

/* This is used for slow motion */
static int oddeven;

int nspectrums;

static int32_t  beathistory[BEAT_MAX];
static int  beatbase;
static int  beatquiet;  /* force "quiet" situation? */

int blurskinfo = FALSE;
VisSongInfo *songinfo = NULL;


/**
 * Return the name of the i'th floater method
 */
char *floaters_name(int i)
{
    static char *names[] =
    {
        "No floaters", "Dots", "Slow floaters", "Fast floaters",
        "Retro floaters",
        NULL
    };
    return names[i];
}

/* Detect beats.  This involves more than just comparing the loudness to a
 * preset trigger level -- it also compensates for overall loudness of the
 * music, and even tries to detect the rhythm.
 *
 * It returns TRUE for beats, FALSE otherwise.  It also sets *thickref to a
 * thickness value from 0 to 3, and it detects the start of silence for
 * the "Random quiet" setting.
 */
static int detect_beat(int32_t loudness, int *thickref, int *quietref)
{
    static int32_t  aged;       /* smoothed out loudness */
    static int32_t  lowest;     /* quietest point in current beat */
    static int  elapsed;    /* frames since last beat */
    static int  isquiet;    /* was previous frame quiet */
    static int  prevbeat;   /* period of previous beat */
    int     beat, i, j;
    int32_t     total;
    int     sensitivity;

    /* Incorporate the current loudness into history */
    aged = (aged * 7 + loudness) >> 3;
    elapsed++;

    /* If silent, then clobber the beat */
    if (aged < 2000 || elapsed > BEAT_MAX)
    {
        elapsed = 0;
        lowest = aged;
        memset(beathistory, 0, sizeof beathistory);
    }
    else if (aged < lowest)
        lowest = aged;

    /* Beats are detected by looking for a sudden loudness after a lull.
     * They are also limited to occur no more than once every 15 frames,
     * so the beat flashes don't get too annoying.
     */
    j = (beatbase + elapsed) % BEAT_MAX;
    beathistory[j] = loudness - aged;
    beat = FALSE;
    if (elapsed > 15 && aged > 2000 && loudness * 4 > aged * 5)
    {
        /* Compute the average loudness change, assuming this is beat */
        for (i = BEAT_MAX / elapsed, total = 0;
             --i > 0;
             j = (j + BEAT_MAX - elapsed) % BEAT_MAX)
        {
            total += beathistory[j];
        }
        total = total * elapsed / BEAT_MAX;

        /* Tweak the sensitivity to emphasize a consistent rhythm */
        sensitivity = config.beat_sensitivity;
        i = 3 - abs(elapsed - prevbeat)/2;
        if (i > 0)
            sensitivity += i;

        /* If average change is significantly positive, this is a beat.
         */
        if (total * sensitivity > aged)
        {
            prevbeat = elapsed;
            beatbase = (beatbase + elapsed) % BEAT_MAX;
            lowest = aged;
            elapsed = 0;
            beat = TRUE;
        }
    }

    /* Thickness is computed from the difference between the instantaneous
     * loudness and the aged loudness.  Thus, a sudden increase in volume
     * will produce a thick line, regardless of rhythm.
     */
    if (aged < 1500)
        *thickref = 0;
    else if (!config.thick_on_beats)
        *thickref = 1;
    else
    {
        *thickref = loudness * 2 / aged;
        if (*thickref > 3)
            *thickref = 3;
    }

    /* Silence is computed from the aged loudness.  The quietref value is
     * set to TRUE only at the start of silence, not throughout the silent
     * period.  Also, there is some hysteresis so that silence followed
     * by a slight noise and more silence won't count as two silent
     * periods -- that sort of thing happens during many fade edits, so
     * we have to account for it.
     */
    if (beatquiet || aged < (isquiet ? 1500 : 500))
    {
        /* Quiet now -- is this the start of quiet? */
        *quietref = !isquiet;
        isquiet = TRUE;
        beatquiet = FALSE;
    }
    else
    {
        *quietref = FALSE;
        isquiet = FALSE;
    }

    /* return the result */
    return beat;
}

static void drawfloaters(int beat)
{
    static int prevfloaters;
    static struct {int x, y, age; uint8_t color;} floater[10];
    static int oddeven;
    int nfloaters;
    int i, j, delta, dx, dy;

    /* choose the number of floaters */
    switch (*config.floaters)
    {
      case 'N': /* No floaters */
        nfloaters = 0;
        break;

      case 'D': /* Dots */
        nfloaters = 1;
        break;

      case 'S': /* Slow */
        oddeven++;
        /* fall through... */

      default: /* Slow/Fast/Retro floaters */
        nfloaters = 1 + img_width * img_height / 20000;
        if (nfloaters > 10)
            nfloaters = 10;
    }

    /* for each floater... */
    for (i = 0; i < nfloaters; i++)
    {
        /* if Dots, new, old, beat, or off-screen... */
        if (*config.floaters == 'D'
         || i >= prevfloaters
         || floater[i].age++ > 80 + i * 13
         || beat
         || floater[i].x < 0 || floater[i].x >= img_width
         || floater[i].y < 0 || floater[i].y >= img_height)
        {
            /* Pretend motion is 0.  This will cause blursk to
             * choose a new position, later in this function.
             */
            delta = 0;
        }
        else
        {
            /* find the real motion */
            j = floater[i].y * img_bpl + floater[i].x;
            delta = &img_buf[j] - img_source[j];
        }

        /* if motion isn't 0, then move the floater */
        if (delta != 0)
        {
            /* decompose the delta into dx & dy.  Watch signs! */
            dx = (j + delta) % img_bpl - floater[i].x;
            dy = (j + delta) / img_bpl - floater[i].y;

            /* move the floater */
            switch (*config.floaters)
            {
              case 'S': /* Slow floaters */
                if ((oddeven ^ i) & 0x1)
                    dx = dy = 0;
                break;

              case 'F': /* Fast floaters */
                dx *= 2;
                dy *= 2;
                break;

              case 'R': /* Retro floaters */
                dx = -dx;
                dy = -dy;
                break;
            }
            floater[i].x += dx;
            floater[i].y += dy;
        }

        /* if no motion, or motion carries it off the screen, then
         * choose a new random position & contrasting color.
         */
        if (delta == 0
         || floater[i].x < 0 || floater[i].x >= img_width
         || floater[i].y < 0 || floater[i].y >= img_height)
        {
            /* choose a new random position */
            floater[i].x = rand_0_to(img_width - 9) + 2;
            floater[i].y = rand_0_to(img_height - 9) + 2;
            if (IMG_PIXEL(floater[i].x, floater[i].y) > 0x80)
                floater[i].color = 0;
            else
                floater[i].color = 0xfe;
            floater[i].age = 0;
        }

        /* draw the floater */
        render_dot(floater[i].x, floater[i].y, floater[i].color);
    }
    prevfloaters = nfloaters;
}

/* This detects libvisual songinfo events and updates title when appropriate.
 * It should be called once for each frame.
 */

static unsigned char *show_info(unsigned char *img, int height, int bpl)
{
    int pos, length;
    time_t now;
    static int prevpos;
    static char buf[1000];
    static time_t start, then;
    static int persistent = FALSE;
    char showinfo;
    char posstr[32], lenstr[32];

    if(songinfo == NULL || visual_songinfo_get_type(songinfo) == VISUAL_SONGINFO_TYPE_NULL)
        return img;

    time(&now);
    if(now != then)
    {
        then = now;
        pos = visual_songinfo_get_elapsed (songinfo);

        convert_ms_to_timestamp(posstr, pos);
        length = visual_songinfo_get_length(songinfo);
        convert_ms_to_timestamp(lenstr, length);
        if(pos != prevpos)
        {
            prevpos = pos;
            beatquiet = TRUE;
            switch(visual_songinfo_get_type(songinfo))
            {
                case VISUAL_SONGINFO_TYPE_SIMPLE:
                    if(config.show_timestamp)
                    {
                        sprintf(buf, "{%s/%s} %s", posstr, lenstr, visual_songinfo_get_simple_name(songinfo));
                        break;
                    }
                    else
                    {
                        sprintf(buf, "%s", visual_songinfo_get_simple_name (songinfo));
                    }

                case VISUAL_SONGINFO_TYPE_ADVANCED:
                    if(config.show_timestamp)
                    {
                        if(strcmp(visual_songinfo_get_artist(songinfo), "(null)") == 0)
                        {
                            if(length >= 0)
                                sprintf(buf, "{%s/%s} %s", posstr, lenstr, visual_songinfo_get_song(songinfo));
                            else
                                sprintf(buf, "(%s) %s", posstr, visual_songinfo_get_song(songinfo));
                        }
                        else
                        {
                            if(length >= 0)
                                sprintf(buf, "{%s/%s} %s by %s", posstr, lenstr,
                                    visual_songinfo_get_song(songinfo), visual_songinfo_get_artist(songinfo));
                            else
                                sprintf(buf, "(%s) %s by %s", posstr,
                                    visual_songinfo_get_song(songinfo), visual_songinfo_get_artist(songinfo));
                        }
                    }
                    else
                    {
                        if(strcmp(visual_songinfo_get_artist(songinfo), "(null)") == 0)
                        {
                            if(strcmp(visual_songinfo_get_song(songinfo), "(null)") != 0)
                                sprintf(buf, "%s", visual_songinfo_get_song(songinfo));
                        }
                        else
                        {
                            sprintf(buf, "%s by %s", visual_songinfo_get_song(songinfo), visual_songinfo_get_artist(songinfo));
                        }
                    }
                    break;


                default:
                    break;
            }
        }
    }

    showinfo = *config.show_info;
    if(blurskinfo || persistent)
    {
        if(showinfo == 'N')
            return img;

        if(blurskinfo)
        {
            start = now;
            persistent = TRUE;
        }
        blurskinfo = FALSE;
    }

    /* If not supposed to show text, then we're done */
    switch(showinfo) {
        case 'N': /* Never show info */
            return img;
        case 'T': /* 4 second info */
            if(now - start > config.info_timeout)
            {
                persistent = FALSE;
                return img;
            }
        case 'A': /* Always show info */
            break;
    }

    /* We don't want to draw onto the main image, because then the text
     * would leave blur trails.  Most combinations of cpu_speed and
     * overall_effect copy the image data into a temporary buffer, but
     * the specific combination of cpu_speed=Fast and overall_effect=Normal
     * (which is very common!) normally leaves the image in the main buffer.
     * We need to detect this, and copy the image before we draw the text.
     */
    if (img != img_tmp)
    {
        memcpy(img_tmp, img, img_chunks * 8);
        img = img_tmp;
    }

    /* draw the text */
    textdraw(img, height, bpl, "Center", buf);
    return img;
}


/* This is a generic rendering function.  It works for all signal styles.
 * The input always looks like one big PCM sample; if the input is really
 * a spectrum, then it will have been transformed by blurk_render_pcm()
 * into 256 PCM samples, with 20000 meaning "no sound" and smaller/negative
 * values representing a lot of sound.  This particular transformation was
 * chosen simply because the normal PCM plotter can then produce a nice-looking
 * spectrum graph.
 *
 * This function supports a variety of ways to plot the [pseudo-]PCM samples.
 * In addition to the usual line graph, it can also mirror the line graph or
 * produce a "bead" graph by passing the data off to render_bead().
 * The decision of how to plot is based on the value of "variation".
 */
static void update_image(BlurskPrivate *priv, int32_t loudness, int ndata, int16_t *data)
{
    int i, thick, quiet, center;
    int beat;
    int width, height, bpl;


    /* Detect whether this is a beat, and choose a line thickness */
    beat = detect_beat(loudness, &thick, &quiet);

    /* Perform the blurring.  This also affects whether the center of the
     * signal will be moved lower in the window.
     */
    center = img_height/2 + blur(priv, beat, quiet);

    /* Perform the fade or solid flash */
    if (beat && !strcmp(config.flash_style, "Full flash"))
        i = 60;
    else
    {
        switch (config.fade_speed[0])
        {
          case 'S': i = -1; break;  /* Slow */
          case 'M': i = -3; break;  /* Medium */
          case 'F': i = -9; break;  /* Fast */
          case 'N':
          default:  i = 0;      /* None */
        }
    }
    if (i != 0)
        loopfade(i);

    /* special processing for "Invert" & bitmap logo flashes */
    if (beat)
    {
        if (!strcmp(config.flash_style, "Invert flash"))
            img_invert();
        else if ((i = bitmap_index(config.flash_style)) >= 0)
            bitmap_flash(i);
    }

    /* Maybe change hue on beats */
    if (beat)
        color_beat(priv);

    /* Add the signal data to the image */
    render(thick, center, ndata, data);

    /* Add floaters */
    drawfloaters(beat);

    /* shift the "ripple effect" from one frame to another */
    img_rippleshift += 3; /* cyclic, since img_rippleshift is a unsigned char */

    /* Apply the overall effect, if any */
    if (!strcmp(config.overall_effect, "Bump effect"))
    {
        priv->rgb_buf = img_bump(&width, &height, &bpl);
    }
    else if (!strcmp(config.overall_effect, "Anti-fade effect"))
    {
        priv->rgb_buf = img_travel(&width, &height, &bpl);
    }
    else if (!strcmp(config.overall_effect, "Ripple effect"))
    {
        priv->rgb_buf = img_ripple(&width, &height, &bpl);
    }
    else /* "Normal effect" */
    {
        priv->rgb_buf = img_expand(&width, &height, &bpl);
    }

    priv->rgb_buf = show_info(priv->rgb_buf, height, bpl);

    /* Allow the background color to change */
    color_bg(priv, ndata, data);
}


/* This is the entry point for the pcm view.  Normally it just calls the
 * renderer with the input data.
 */
static void blursk_render_pcm(BlurskPrivate *priv, int16_t *data)
{
    int i, imin, imax, start;
    int32_t loudness, delta_sum;

    /* If slow motion, then ignore odd-numbered frames */
    oddeven = !oddeven;
    if (config.slow_motion && oddeven)
        return;

    /* Find the maximum and minimum, with the restriction that
     * the minimum must occur after the maximum.
     */
    for (i = 1, imin = imax = 0, delta_sum = 0; i < 255 / 2; i++)
    {
        if (data[i] < data[imin])
            imin = i;
        if (data[i] > data[imax])
            imin = imax = i;
        delta_sum += abs(data[i] - data[i - i]);
    }

    /* Triggered sweeps start halfway between min & max */
    start = (imax + imin) / 2;

    /* Compute the loudness.  We don't want to do a full spectrum analysis
     * to do this, but we can guess the low-frequency sound is proportional
     * to the maximum difference found (because loud low frequencies need
     * big signal changes), and that high-frequency sound is proportional
     * to the differences between adjacent samples.  We want to be sensitive
     * to both of those, while ignoring the mid-range sound.
     *
     * Because we have only one low-frequency difference, but hundreds of
     * high-frequency differences, we need to give more weight to the
     * low-frequency difference (even though each high-frequency difference
     * is small).
     */
    loudness = (((int32_t)data[imax] - (int32_t)data[imin]) * 60 + delta_sum) / 75;

    /* Draw it */
    update_image(priv, loudness, 256, &data[start]);
}


void blursk_event_newsong(VisSongInfo *newsong)
{
    visual_return_if_fail(newsong != NULL);
    visual_songinfo_copy(songinfo, newsong);
    blurskinfo = TRUE;
}

void __blursk_render_pcm (BlurskPrivate *priv, int16_t *pcmbuf) {
    blursk_render_pcm(priv, pcmbuf);
}

void __blursk_init (BlurskPrivate *priv) {
    color_genmap(priv, FALSE);
    img_resize(priv, config.width, config.height);
    songinfo = visual_songinfo_new(VISUAL_SONGINFO_TYPE_NULL);
}

void __blursk_cleanup (BlurskPrivate *priv) {
    img_cleanup();
    visual_songinfo_free(songinfo);

    /* cleanup config strings */
    visual_mem_free(config.color_style);
    visual_mem_free(config.signal_color);
    visual_mem_free(config.background);
    visual_mem_free(config.blur_style);
    visual_mem_free(config.transition_speed);
    visual_mem_free(config.blur_when);
    visual_mem_free(config.blur_stencil);
    visual_mem_free(config.fade_speed);
    visual_mem_free(config.signal_style);
    visual_mem_free(config.plot_style);
    visual_mem_free(config.flash_style);
    visual_mem_free(config.overall_effect);
    visual_mem_free(config.floaters);
    visual_mem_free(config.cpu_speed);
    visual_mem_free(config.show_info);

    visual_mem_set(&config, 0, sizeof(config));
}


