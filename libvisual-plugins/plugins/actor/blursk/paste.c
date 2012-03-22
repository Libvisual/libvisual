/* paste.c */

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

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include "actor_blursk.h"
#include "blursk.h"



/* Convert the leading words in a value into a single letter and '.' */
static char *abbreviate(char *value)
{
    char        full[40];   /* full value */
    static char abbr[40];   /* abbreviated value */
    char        *word;

    /* Strip off a trailing "stencil" or "flash" word */
    strcpy(full, value);
    word = strrchr(full, ' ');
    if (word && (!strcmp(word, " stencil") || !strcmp(word, " flash")))
        *word = '\0';

    /* generate the abbreviated version of the string */
    word = strrchr(full, ' ');
    if (word)
        sprintf(abbr, "%.1s.%s", full, word + 1);
    else
        strcpy(abbr, full);
    return abbr;
}

/* Convert a value to its shortest possible form */
static void genfield(
    char **buf,     /* where to stuff the short value */
    char *current,      /* current value */
    char *(*namefunc)(int), /* called to generate names of items */
    ...)            /* NULL-terminated list of hardcoded items */
{
    char    str[40];    /* abbreviated value */
    char    *value;
    int i,len, found;
    va_list ap;

    /* generate the abbreviated version of the string */
    strcpy(str, abbreviate(current));

    /* compare to other values, to see how short we can make this */
    va_start(ap, namefunc);
    i = 0;
    value = namefunc ? (*namefunc)(i) : NULL;
    if (!value)
    {
        namefunc = NULL;
        value = va_arg(ap, char *);
    }

    /* For each arg... */
    for (found = FALSE, len = 1; value; )
    {
        /* abbreviate this possible value */
        value = abbreviate(value);

        /* if this is the initial value, remember that. */
        if (!strcmp(value, str))
            found = TRUE;
        else
            /* make sure "len" is big enough to distinguish this
             * item from any preceding item.
             */
            while (!found && !strncmp(value, str, len))
                len++;

        /* get the next value, from either the function or args */
        i++;
        value = namefunc ? (*namefunc)(i) : NULL;
        if (!value)
        {
            namefunc = NULL;
            value = va_arg(ap, char *);
        }
    }
    va_end(ap);

    /* Add the shortest abbreviation of this item to the buffer */
    strncpy(*buf, str, len);
    (*buf) += len;
    **buf = '\0';
}


/* return a string which describes the current configuration */
char *paste_genstring(void)
{
    static char buf[100];
    char    *str;
    
    /* start with the color, as a decimal number */
    sprintf(buf, "%d", config.color);
    str = buf + strlen(buf);

    /* Add the color options */
    genfield(&str, config.color_style, color_name, NULL);
    genfield(&str, config.fade_speed, NULL, "No fade", "Slow fade",
        "Medium fade", "Fast fade", NULL);
    genfield(&str, config.signal_color, NULL, "Normal signal",
        "White signal", "Cycling signal", NULL);
    *str++ = config.contour_lines ? 'Y' : 'N';
    *str++ = config.hue_on_beats ? 'Y' : 'N';
    genfield(&str, config.background, color_background_name, NULL);
    *str++ = '/';

    /* Add the blur options */
    genfield(&str, config.blur_style, blur_name, NULL);
    genfield(&str, config.transition_speed, NULL, "Slow switch",
        "Medium switch", "Fast switch", NULL);
    genfield(&str, config.blur_when, blur_when_name, NULL);
    genfield(&str, config.blur_stencil, bitmap_stencil_name, NULL);
    *str++ = config.slow_motion ? 'Y': 'N';
    *str++ = '/';

    /* Add the effects options */
    genfield(&str, config.signal_style, signal_style_name, NULL);
    genfield(&str, config.plot_style, render_plotname, NULL);
    *str++ = config.thick_on_beats ? 'Y' : 'N';
    genfield(&str, config.flash_style, bitmap_flash_name, NULL);
    genfield(&str, config.overall_effect, NULL, "Normal effect",
        "Bump effect", "Anti-fade effect", "Ripple effect", NULL);
    genfield(&str, config.floaters, floaters_name, NULL);
    *str = '\0';
    return buf;
}

/* Convert a single field.  The behavior of this function has been tweaked to
 * make it easy to convert a long series of fields...
 *   o It moves "field" past the end of the converted field.
 *   o If the field can't be converted, "field" is set to NULL
 *   o If "field" was already NULL when the function is called, it does nothing.
 *   o If "field" points to a slash, then the default is returned.
 *   o Allocated strings are stored at "out", and then "out" moved past it.
 *   o The return value points to the start of the value.
 * The idea is that "field" pointing at the start of a field, and "out" pointing
 * at the start of a buffer.  Then you call parsefield() repeatedly without
 * bothering to check for errors.  At the end of a series, if "field" is NULL
 * then there was an error.
 */
static char *parsefield(
    char **field,       /* pointer into string of fields */
    char *dflt,     /* default return value */
    char *(*namefunc)(int), /* called to generate names of items */
    ...)            /* NULL-terminated list of hardcoded items */
{
    char    *value, *abbr;
    int i,len;
    char    *found;
    va_list ap;

    /* if "field" is NULL or points to a slash, do nothing */
    if (!*field || !**field || **field == '/')
    {
        return dflt;
    }

    /* count the length of this abbreviated field */
    len = 1;
    if ((*field)[1] == '.')
        len = 2;
    while (islower((*field)[len]))
        len++;


    /* compare to each possible value, to find the unabbreviated form */
    va_start(ap, namefunc);
    i = 0;
    value = namefunc ? (*namefunc)(i) : NULL;
    if (!value)
    {
        namefunc = NULL;
        value = va_arg(ap, char *);
    }
    for (found = NULL; value; )
    {
        /* abbreviate this possible value */
        abbr = abbreviate(value);

        /* if this is the value value, remember that. */
        if (!found && !strncmp(abbr, *field, len))
            found = value;

        /* get the next value, from either the function or args */
        i++;
        value = namefunc ? (*namefunc)(i) : NULL;
        if (!value)
        {
            namefunc = NULL;
            value = va_arg(ap, char *);
        }
    }
    va_end(ap);

    /* did we find it? */
    if (found)
    {
        /* Yes - move past the field, and then return long version */
        *field += len;
        return found;
    }
    else
    {
        /* No - skip to next slash or end of string & return default */
        while (**field && **field != '/')
            (*field)++;
        return dflt;
    }
}

/* This is like parsefield(), but for booleans */
static int parsebool(
    char **field,   /* pointer to current field within pasted string */
    int dflt)   /* default value */
{
    if (!*field || !**field || **field == '/')
        return dflt;
    if (**field == 'Y')
    {
        (*field)++;
        return TRUE;
    }
    if (**field == 'N')
    {
        (*field)++;
        return FALSE;
    }

    /* Not recognized -- skip to next slash or end of string */
    while (**field && **field != '/')
        (*field)++;
    return dflt;
}

/* parse a configuration string & set the current configuration accordingly */
BlurskConfig *paste_parsestring(char *str)
{
    char        *afternumber;
    uint32_t     newcolor;
    static BlurskConfig c;
    static int first_call = TRUE;

    if(first_call)
    {
        memset(&c, 0, sizeof(BlurskConfig));    
        config_default(&c);
        first_call = FALSE;
    }

    /* skip leading whitespace */
    while (isspace(*str))
        str++;

    /* skip title, if any */
    if (*str == '[')
    {
        ++str;
        while (*str && *str != ']')
            str++;
        if (*str)
            *str++ = '\0';
    }

    /* parse the color */
    newcolor = (uint32_t) strtol(str, &afternumber, 10);

    /* no color parsed? */
    if (afternumber == str)
        return &c;

    c.color = newcolor;
    str = afternumber;

    /* parse the color options */
    c.color_style = parsefield(&str, c.color_style, color_name,NULL);
    c.fade_speed = parsefield(&str, c.fade_speed, NULL, "No fade",
        "Slow fade", "Medium fade", "Fast fade", NULL);
    c.signal_color = parsefield(&str, c.signal_color, NULL,
        "Normal signal", "White signal", "Cycling signal", NULL);
    c.contour_lines = parsebool(&str, c.contour_lines);
    c.hue_on_beats = parsebool(&str, c.hue_on_beats);
    c.background = parsefield(&str, c.background,
        color_background_name, NULL);
    if (!str)
        return &c;
    while (*str && *str != '/')
        str++;
    if (*str == '/')
        str++;

    /* parse the blur options */
    c.blur_style = parsefield(&str, c.blur_style, blur_name, NULL);
    c.transition_speed = parsefield(&str, c.transition_speed, NULL,
        "Slow switch", "Medium switch", "Fast switch", NULL);
    c.blur_when = parsefield(&str, c.blur_when, blur_when_name, NULL);
    c.blur_stencil = parsefield(&str, c.blur_stencil,
        bitmap_stencil_name, NULL);
    c.slow_motion = parsebool(&str, c.slow_motion);
    if (!str)
        return &c;
    while (*str && *str != '/')
        str++;
    if (*str == '/')
        str++;

    /* parse the effects options */
    c.signal_style = parsefield(&str, c.signal_style, signal_style_name,
        NULL);
    c.plot_style = parsefield(&str, c.plot_style, render_plotname,
        NULL);
    c.thick_on_beats = parsebool(&str, c.thick_on_beats);
    c.flash_style = parsefield(&str, c.flash_style,
        bitmap_flash_name, NULL);
    c.overall_effect = parsefield(&str, c.overall_effect, NULL,
        "Normal effect", "Bump effect", "Anti-fade effect",
        "Ripple effect", NULL);
    c.floaters = parsefield(&str, c.floaters, floaters_name, NULL);

    return &c;
}


