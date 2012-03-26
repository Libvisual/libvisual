/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 * Copyright (C) 1999  Steve Kirkendall
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

#include "actor_blursk.h"
#include "blursk.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static int textheight;  /* height of the tallest character, plus 1 */
static int frame;   /* frame counter, used for color-cycling */
static int textbg;  /* background color for text */

/* this is a map of the shapes for 0-9, A-Z, and a few punctuation characters.
 * A space char indicates that the pixel should be unchanged, a period that it
 * should be set to white, and and 'x' that it should be set to black.  White
 * is used to outline the black symbols, so they can be read even on dark
 * backgrounds.  Single-character strings mark the start of each character,
 * and NULL marks the end of the list.  After run-time initialization, the
 * single-character strings are converted to NULLs.
 */
static char *shapes[] = {
    "_",
    "    ",

    " ",
    "         ",

    "0",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",

    "1",
    "   ...   ",
    "  .xxx.  ",
    " .xxxx.  ",
    "  ..xx.  ",
    "   .xx.  ",
    "   .xx.  ",
    "   .xx.  ",
    " ...xx.. ",
    ".xxxxxxx.",
    " ....... ",
    
    "2",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    " ..  .xx.",
    "  ...xx. ",
    " .xxxx.  ",
    ".xx...   ",
    ".xx..... ",
    ".xxxxxxx.",
    " ....... ",
    
    "3",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    " .. ..xx.",
    "   .xxx. ",
    "    ..xx.",
    " ..  .xx.",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",
    
    "4",
    "    ...  ",
    "   .xxx. ",
    "  .xxxx. ",
    " .xx.xx. ",
    ".xx..xx. ",
    ".xx..xx. ",
    ".xxxxxxx.",
    " ....xx. ",
    "    .xx. ",
    "     ..  ",
    
    "5",
    " ....... ",
    ".xxxxxxx.",
    ".xx..... ",
    ".xx....  ",
    ".xxxxxx. ",
    "......xx.",
    " ..  .xx.",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",

    "6",
    "    ..   ",
    "   .xx.  ",
    "  .xx.   ",
    " .xx.    ",
    ".xx....  ",
    ".xxxxxx. ",
    ".xx...xx.",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",

    "7",
    " ....... ",
    ".xxxxxxx.",
    ".x....xx.",
    " .  .xx. ",
    "    .xx. ",
    "   .xx.  ",
    "   .xx.  ",
    "   .xx.  ",
    "   .xx.  ",
    "    ..   ",

    "8",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    ".xx...xx.",
    " .xxxxx. ",
    ".xx...xx.",
    ".xx. .xx.",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",

    "9",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    ".xx...xx.",
    " .xxxxxx.",
    "  ....xx.",
    "    .xx. ",
    "   .xx.  ",
    "  .xx.   ",
    "   ..    ",

    "a",
    "         ",
    "         ",
    "  .....  ",
    " .xxxxx. ",
    "  ....xx.",
    " .xxxxxx.",
    ".xx. .xx.",
    ".xx. .xx.",
    " .xxxxxx.",
    "  ...... ",

    "b",
    " ..       ",
    ".xx.      ",
    ".xx.....  ",
    ".xx.xxxx. ",
    ".xxx...xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xxx...xx.",
    ".xx.xxxx. ",
    " .. ....  ",

    "c",
    "         ",
    "         ",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    ".xx.  .. ",
    ".xx.     ",
    ".xx..... ",
    " .xxxxxx.",
    "  .....  ",

    "d",
    "       .. ",
    "      .xx.",
    "  .....xx.",
    " .xxxx.xx.",
    ".xx...xxx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx...xxx.",
    " .xxxx.xx.",
    "  .... .. ",

    "e",
    "         ",
    "         ",
    "  .....  ",
    " .xxxxx. ",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xxxxxx. ",
    ".xx....  ",
    " .xxxxx. ",
    "  .....  ",

    "f",
    "   ....  ",
    "  .xxxx. ",
    " .xx..xx.",
    " .xx. .. ",
    " .xx.    ",
    ".xxxx.   ",
    " .xx.    ",
    " .xx.    ",
    " .xx.    ",
    "  ..     ",

    "g",
    "         ",
    "         ",
    " .. .... ",
    " .xxx.xx.",
    ".xx..xxx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx..xxx.",
    " .xxx.xx.",
    "  ....xx.",
    " .xxxxx. ",
    "  .....  ",

    "h",
    " ..      ",
    ".xx.     ",
    ".xx....  ",
    ".xx.xxx. ",
    ".xxx..xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    " ..   .. ",

    "i",
    " .. ",
    ".xx.",
    " .. ",
    ".xx.",
    ".xx.",
    ".xx.",
    ".xx.",
    ".xx.",
    ".xx.",
    " .. ",

    "j",
    "   .. ",
    "  .xx.",
    "   .. ",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    " ..xx.",
    ".xxx. ",
    " ...  ",

    "k",
    " ..      ",
    ".xx.     ",
    ".xx.  .. ",
    ".xx. .xx.",
    ".xx..xx. ",
    ".xxxxx.  ",
    ".xx..xx. ",
    ".xx. .xx.",
    ".xx. .xx.",
    " ..   .. ",

    "l",
    " ...  ",
    ".xxx. ",
    " .xx. ",
    " .xx. ",
    " .xx. ",
    " .xx. ",
    " .xx. ",
    " .xx. ",
    ".xxxx.",
    " .... ",

    "m",
    "              ",
    "              ",
    " .. ... ....  ",
    ".xx.xxx.xxxx. ",
    ".xxx..xxx..xx.",
    ".xx. .xx. .xx.",
    ".xx. .xx. .xx.",
    ".xx. .xx. .xx.",
    ".xx. .xx. .xx.",
    " ..   ..   .. ",

    "n",
    "          ",
    "          ",
    " .. ....  ",
    ".xx.xxxx. ",
    ".xxx...xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    " ..    .. ",

    "o",
    "         ",
    "         ",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",

    "p",
    "          ",
    "          ",
    " .. ....  ",
    ".xx.xxxx. ",
    ".xxx...xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xxx...xx.",
    ".xx.xxxx. ",
    ".xx.....  ",
    ".xx.      ",
    " ..       ",

    "q",
    "          ",
    "          ",
    "  .... .. ",
    " .xxxx.xx.",
    ".xx...xxx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx...xxx.",
    " .xxxx.xx.",
    "  .....xx.",
    "      .xx.",
    "       .. ",

    "r",
    "        ",
    "        ",
    " .. ... ",
    ".xx.xxx.",
    ".xxxx.. ",
    ".xxx.   ",
    ".xx.    ",
    ".xx.    ",
    ".xx.    ",
    " ..     ",

    "s",
    "         ",
    "         ",
    "  .....  ",
    " .xxxxx. ",
    ".xx...xx.",
    " .xxx..  ",
    "  ..xxx. ",
    ".xx...xx.",
    " .xxxxx. ",
    "  .....  ",

    "t",
    "   ..    ",
    "  .xx.   ",
    " ..xx... ",
    ".xxxxxxx.",
    " ..xx... ",
    "  .xx.   ",
    "  .xx.   ",
    "  .xx.   ",
    "   .xx.  ",
    "    ..   ",

    "u",
    "          ",
    "          ",
    " ..    .. ",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx.  .xx.",
    ".xx....xx.",
    " .xxxxxx. ",
    "  ......  ",

    "v",
    "         ",
    "         ",
    " ..   .. ",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    " .xx.xx. ",
    " .xx.xx. ",
    "  .xxx.  ",
    "   ...   ",

    "w",
    "             ",
    "             ",
    " ..       .. ",
    ".xx.     .xx.",
    ".xx.  .  .xx.",
    ".xx. .x. .xx.",
    " .xx.xxx.xx. ",
    " .xx.xxx.xx. ",
    "  .xxx.xxx.  ",
    "   ... ...   ",

    "x",
    "         ",
    "         ",
    " ..   .. ",
    ".xx. .xx.",
    " .xx.xx. ",
    "  .xxx.  ",
    "  .xxx.  ",
    " .xx.xx. ",
    ".xx. .xx.",
    " ..   .. ",

    "y",
    "         ",
    "         ",
    " ..   .. ",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx. .xx.",
    ".xx..xxx.",
    " .xxx.xx.",
    "  ....xx.",
    " .xxxxx. ",
    "  .....  ",

    "z",
    "         ",
    "         ",
    " ....... ",
    ".xxxxxxx.",
    " ....xx. ",
    "   .xx.  ",
    "  .xx.   ",
    " .xx.... ",
    ".xxxxxxx.",
    " ....... ",

    "-",
    "        ",
    "        ",
    "        ",
    " ...... ",
    ".xxxxxx.",
    " ...... ",

    "~",
    "       ",
    "       ",
    "       ",
    "  .... ",
    " .xx.x.",
    ".x.xx. ",
    " ....  ",


    ".",
    "    ",
    "    ",
    "    ",
    "    ",
    "    ",
    "    ",
    " .. ",
    ".xx.",
    ".xx.",
    " .. ",

    "!",
    " .. ",
    ".xx.",
    ".xx.",
    ".xx.",
    ".xx.",
    ".xx.",
    " .. ",
    " .. ",
    ".xx.",
    ".xx.",
    " .. ",

    ":",
    "    ",
    " .. ",
    ".xx.",
    ".xx.",
    " .. ",
    "    ",
    " .. ",
    ".xx.",
    ".xx.",
    " .. ",

    "#",
    "    .  .  ",
    "   .x..x. ",
    "  ..x..x. ",
    " .xxxxxxx.",
    "  .x..x.. ",
    " ..x..x.  ",
    ".xxxxxxx. ",
    " .x..x..  ",
    " .x..x.   ",
    "  .  .    ",

    "[",
    " .... ",
    ".xxxx.",
    ".xx.  ",
    ".xx.  ",
    ".xx.  ",
    ".xx.  ",
    ".xx.  ",
    ".xx.  ",
    ".xxxx.",
    " .... ",

    "]",
    " .... ",
    ".xxxx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    ".xxxx.",
    " .... ",

    "(",
    "   .. ",
    "  .xx.",
    " .xx. ",
    ".xx.  ",
    ".xx.  ",
    ".xx.  ",
    ".xx.  ",
    " .xx. ",
    "  .xx.",
    "   .. ",

    ")",
    " ..   ",
    ".xx.  ",
    " .xx. ",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    "  .xx.",
    " .xx. ",
    ".xx.  ",
    " ..   ",

    "'",
    "  .. ",
    " .xx.",
    " .xx.",
    ".xx. ",
    " ..  ",

    ",",
    "      ",
    "      ",
    "      ",
    "      ",
    "  ..  ",
    " .xx. ",
    ".xxxx.",
    " .xx. ",
    ".xx.  ",
    " ..   ",

    "%",
    "        ",
    " ..     ",
    ".xx. .. ",
    " .. .xx.",
    "   .xx. ",
    "  .xx.  ",
    " .xx.   ",
    ".xx. .. ",
    " .. .xx.",
    "     .. ",

    "/",
    "        ",
    "        ",
    "     .. ",
    "    .xx.",
    "   .xx. ",
    "  .xx.  ",
    " .xx.   ",
    ".xx.    ",
    " ..     ",

    ">",
    "        ",
    " ...    ",
    ".xxx..  ",
    " ..xxx. ",
    "   ..xx.",
    " ..xxx. ",
    ".xxx..  ",
    " ...    ",

    "<",
    "        ",
    "    ... ",
    "  ..xxx.",
    " .xxx.. ",
    ".xx..   ",
    " .xxx.. ",
    "  ..xxx.",
    "    ... ",

    NULL
};


static char **chmap[127];

static int row;
static int big;

static void textinit()
{
    int i, h;

    /* if already initialized, then do nothing */
    if (!shapes[0])
        return;

    /* scan shapes for the characters defined there */
    for (h = i = 0; shapes[i]; i++, h++)
    {
        /* start of new char? */
        if (!shapes[i][1])
        {
            /* was previous char the tallest? */
            if (h >= textheight)
                textheight = h;
            h = 0;

            /* remember the shape */
            chmap[(int)shapes[i][0]] = &shapes[i + 1];
            shapes[i] = NULL;
        }
    }
}


static unsigned char *normaltext(unsigned char *cursor, int bpl, char **shape, int color)
{
    int x, y;

    for (x = 0; shape[0][x]; x++, cursor++)
        for (y = 0; shape[y]; y++)
            switch (shape[y][x])
            {
              case '.': cursor[y * bpl] = textbg; break;
              case 'x': cursor[y * bpl] = color; break;
            }

    return cursor;
}


static unsigned char *bigtext(unsigned char *cursor, int bpl, char **shape, int color)
{
    int x, y;
    unsigned char   *raster;

    for (x = 0; shape[0][x]; x++, cursor += 2)
        for (y = 0, raster = cursor; shape[y]; y++, raster += 2 * bpl)
            switch (shape[y][x])
            {
              case '.':
                raster[0] = raster[1] = raster[bpl]
                    = raster[bpl + 1] = textbg;
                break;

              case 'x':
                raster[0] = raster[1] = raster[bpl]
                    = raster[bpl + 1] = color;
                break;
            }

    return cursor;
}


/* Find the number of chars in words that fit */
static int fitwords(int maxwidth, char *text, int *width)
{
    int scale = big ? 2 : 1;
    int i, w, last, lastw, ch;
    int fudgefactor = 0;

    for (i = last = lastw = w = 0; text[i] && w < maxwidth - fudgefactor; i++)
    {
        ch = tolower(text[i]);
        switch (ch)
        {
          case '{':
            scale = 2;
            break;

          case '}':
            scale = 1;
            break;

          case '\n':
            *width = w;
            return i;

          default:
            /* ignore if it can't be shown */
            if (ch < 0 || ch > 126 || !chmap[ch])
                break;

            /* if end of word, remember it */
            if (ch == ' ')
            {
                last = i;
                lastw = w;
            }

            /* prefer to break lines before certain punctuation */
            if (strchr("-([,", ch))
                fudgefactor = 100;
            else
                fudgefactor = 0;

            /* add the width of this character */
            w += scale * strlen(chmap[ch][0]);
        }
    }
    if (!text[i])
    {
        *width = w;
        return i;
    }
    if (last > 0)
    {
        *width = lastw;
        return last;
    }
    *width = maxwidth;
    return i - 1;
}


static void textdrawrow(unsigned char *img, int height, int bpl, char *side, char *text, int max, int rowwidth)
{
    int big = FALSE;
    unsigned char   *cursor;
    int bpt = bpl * (textheight / 2);
    int hasbig;
    int rowheight;
    int i, ch, color;

    /* determine whether this involves any big chars */
    if (big)
        hasbig = TRUE;
    else
    {
        for (i = 0; i < max && text[i] && text[i] != '{'; i++)
        {
        }
        hasbig = (i < max && text[i] == '{');
    }

    /* if too high, then skip it */
    rowheight = (hasbig ? textheight * 2 : textheight);
    if (row + rowheight >= height)
        return;

    /* choose the starting position for this row */
    cursor = img + row * bpl;
    switch (*side)
    {
      case 'R':
        cursor += (bpl - 3 - rowwidth);
        break;

      case 'L':
        cursor += 1;
        break;

      default:
        cursor += ((bpl - 3 - rowwidth)/2);
    }

    /* if big text, then start the base line a little lower to make room */
    if (hasbig)
        cursor += bpt;

    /* for each text character... */
    for (i = 0; i < max && text[i]; i++)
    {
        /* handle special or unsupported characters */
        ch = tolower(text[i]);
        if (ch == '{')
        {
            big = TRUE;
            continue;
        }
        if (ch == '}')
        {
            big = FALSE;
            continue;
        }
        if (ch < 0 || ch > 126 || !chmap[ch])
            continue;

        /* choose a color */
        color = (( frame - i) * 3) & 0xff;
        if (color < 128)
            color = color ^ 0xff;

        /* draw the character */
        if (big)
        {
            cursor -= bpt;
            cursor = bigtext(cursor, bpl, chmap[ch], color);
            cursor += bpt;
        }
        else
        {
            cursor = normaltext(cursor, bpl, chmap[ch], color);
        }
    }

    /* increment row by the height of this text row */
    row += (hasbig ? textheight * 2 : textheight);
}

/* Convert time in ms to useful string */
void convert_ms_to_timestamp(char *buf, int time)
{
    int m = (time / 1000 / 60);
    int s = (time / 1000) % 60;
    sprintf(buf, "%d:%d", m, s);
}

/* Draw text into an image.  */
void textdraw(unsigned char *img, int height, int bpl, char *side, char *text)
{
    int max, twidth;

    /* parse the font table, if necessary */
    textinit();

    /* initialize */
    frame++;
    row = 0;
    big = FALSE;
    textbg = (*config.overall_effect == 'B' ? 0x80 : 0);

    /* for each row of text... */
    while (text)
    {
        /* find out how much can fit on this row */
        max = fitwords(bpl - 3, text, &twidth);

        /* draw it */
        textdrawrow(img, height, bpl, side, text, max, twidth);

        /* move to start of next row */
        text += max;
        while (isspace(*text))
            text++;
    }
}

