/* $Id$
 * $URL$
 *
 * Copyright (C) 2009 Scott Sibley <scott@starlon.net>
 * Copyright (C) 2010 Daniel Hiepler <daniel@niftylight.de>
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

#include "actor_blursk.h"
#include "blursk.h"

#include <libvisual/libvisual.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

/**
 * fill config with default values
 */
void config_default(BlurskConfig *conf)
{
    char config_default_color_style[] = "Rainbow";
    char config_default_signal_color[] = "Normal signal";
    char config_default_background[] = "Black bkgnd";
    char config_default_blur_style[] = "Random";
    char config_default_transition_speed[] = "Medium switch";
    char config_default_fade_speed[] = "Medium fade";
    char config_default_blur_when[] = "Full blur";
    char config_default_blur_stencil[] = "No stencil";
    char config_default_signal_style[] = "Stereo spectrum";
    char config_default_plot_style[] = "Line";
    char config_default_flash_style[] = "No flash";
    char config_default_overall_effect[] = "Normal effect";
    char config_default_floaters[] = "No floaters";
    char config_default_cpu_speed[] = "Fast CPU";
    char config_default_show_info[] = "Never show info";
    /*char config_default_fullscreen_method[] = "None";*/


    /* window geometry -- generally ignored */
    conf->width = 256;
    conf->height = 128;

    /* main options */
    conf->color = 0x00ffff;
    if(conf->color_style)
        visual_mem_free(conf->color_style);
    conf->color_style = visual_strdup(config_default_color_style);
    if(conf->signal_color)
        visual_mem_free(conf->signal_color);
    conf->signal_color = visual_strdup(config_default_signal_color);
    conf->contour_lines = FALSE;
    conf->hue_on_beats = FALSE;
    if(conf->background)
        visual_mem_free(conf->background);
    conf->background = visual_strdup(config_default_background);
    if(conf->blur_style)
        visual_mem_free(conf->blur_style);
    conf->blur_style = visual_strdup(config_default_blur_style);
    if(conf->transition_speed)
        visual_mem_free(conf->transition_speed);
    conf->transition_speed = visual_strdup(config_default_transition_speed);
    if(conf->fade_speed)
        visual_mem_free(conf->fade_speed);
    conf->fade_speed = visual_strdup(config_default_fade_speed);
    if(conf->blur_when)
        visual_mem_free(conf->blur_when);
    conf->blur_when = visual_strdup(config_default_blur_when);
    if(conf->blur_stencil)
        visual_mem_free(conf->blur_stencil);
    conf->blur_stencil = visual_strdup(config_default_blur_stencil);
    conf->slow_motion = FALSE;
    if(conf->signal_style)
        visual_mem_free(conf->signal_style);
    conf->signal_style = visual_strdup(config_default_signal_style);
    if(conf->plot_style)
        visual_mem_free(conf->plot_style);
    conf->plot_style = visual_strdup(config_default_plot_style);
    conf->thick_on_beats = TRUE;
    if(conf->flash_style)
        visual_mem_free(conf->flash_style);
    conf->flash_style = visual_strdup(config_default_flash_style);
    if(conf->overall_effect)
        visual_mem_free(conf->overall_effect);
    conf->overall_effect = visual_strdup(config_default_overall_effect);
    if(conf->floaters)
        visual_mem_free(conf->floaters);
    conf->floaters = visual_strdup(config_default_floaters);

    /* advanced options */
    if(conf->cpu_speed)
        visual_mem_free(conf->cpu_speed);
    conf->cpu_speed = visual_strdup(config_default_cpu_speed);
    if(conf->show_info)
        visual_mem_free(conf->show_info);
    conf->show_info = visual_strdup(config_default_show_info);
    conf->info_timeout = 4;
    conf->show_timestamp = 0;
    conf->beat_sensitivity = 4;
}

/**
 * generate a new config-string from current config
 */
void config_string_genstring(BlurskPrivate *priv)
{
    char *string = paste_genstring();

    VisParamList *params = visual_plugin_get_params(priv->plugin);

    VisParam *param = visual_param_list_get(params, "config_string");

    /* don't set if it has already been set */
    if(strcmp(string, visual_param_get_value_string(param)) != 0)
        visual_param_set_value_string(param, string);

    priv->update_config_string = 0;
}

/**
 * reflect the new values.
 */
static void _config_load_preset(BlurskPrivate *priv, BlurskConfig *conf)
{
    struct
    {
        const char *name;
        VisParamType type;
        void *val;
    }entries[] =
    {
        {"color", VISUAL_PARAM_TYPE_COLOR, &conf->color},
        {"color_style", VISUAL_PARAM_TYPE_STRING, &conf->color_style},
        {"fade_speed", VISUAL_PARAM_TYPE_STRING, &conf->fade_speed},
        {"signal_color", VISUAL_PARAM_TYPE_STRING, &conf->signal_color},
        {"contour_lines", VISUAL_PARAM_TYPE_BOOL, &conf->contour_lines},
        {"hue_on_beats", VISUAL_PARAM_TYPE_BOOL, &conf->hue_on_beats},
        {"background", VISUAL_PARAM_TYPE_STRING, &conf->background},
        {"blur_style", VISUAL_PARAM_TYPE_STRING, &conf->blur_style},
        {"transition_speed", VISUAL_PARAM_TYPE_STRING, &conf->transition_speed},
        {"blur_when", VISUAL_PARAM_TYPE_STRING, &conf->blur_when},
        {"blur_stencil", VISUAL_PARAM_TYPE_STRING, &conf->blur_stencil},
        {"slow_motion", VISUAL_PARAM_TYPE_BOOL, &conf->slow_motion},
        {"signal_style", VISUAL_PARAM_TYPE_STRING, &conf->signal_style},
        {"plot_style", VISUAL_PARAM_TYPE_STRING, &conf->plot_style},
        {"thick_on_beats", VISUAL_PARAM_TYPE_BOOL, &conf->thick_on_beats},
        {"flash_style", VISUAL_PARAM_TYPE_STRING, &conf->flash_style},
        {"overall_effect", VISUAL_PARAM_TYPE_STRING, &conf->overall_effect},
        {"floaters", VISUAL_PARAM_TYPE_STRING, &conf->floaters},
    };

    int i;

    for(i = 0; i < QTY(entries); i++)
    {
        VisParamList *params = visual_plugin_get_params(priv->plugin);
        VisParam *ptmp = visual_param_list_get(params, entries[i].name);

        switch(entries[i].type)
        {
            case VISUAL_PARAM_TYPE_INTEGER:
            {
                int *integer = entries[i].val;

                /* only update if values differ */
                if(*integer != visual_param_get_value_integer(ptmp))
                    visual_param_set_value_integer(ptmp, *integer);
                break;
            }


            case VISUAL_PARAM_TYPE_COLOR:
            {
                VisColor *color = visual_param_get_value_color(ptmp);
                VisColor ncolor;
                uint32_t nicolor = (((color->b)<<16) + ((color->g)<<8) + color->r);
                uint32_t *icolor = entries[i].val;

                /* only update if values differ */
                if(*icolor != nicolor)
                {
                    visual_color_set(&ncolor, (*icolor&0xFF0000)>>16, (*icolor&0xFF00)>>8, (*icolor&0xFF));
                    visual_param_set_value_color(ptmp, &ncolor);
                }
                break;
            }


            case VISUAL_PARAM_TYPE_STRING:
            {
                char **string = entries[i].val;

                /* only update if values differ */
                if(strcmp(*string, visual_param_get_value_string(ptmp)) != 0)
                    visual_param_set_value_string(ptmp, *string);
                break;
            }

            default:
                break;
        }
    }
}

/**
 * signal main loop to update colormap
 */
static void __color_genmap(BlurskPrivate *priv)
{
    priv->update_colmap = 1;
}

/**
 * parse a blursk-config-string (tm)
 */
static void _change_config_string(BlurskPrivate *priv, char **string, VisParam *p, int *(validator)(void *value))
{

    if(!validator || validator((void*)visual_param_get_value_string(p)))
    {
        BlurskConfig *c;

        /* free previous string? */
        if(*string)
            visual_mem_free(*string);

        *string = visual_strdup(visual_param_get_value_string(p));

        /* parse the string */
        c = paste_parsestring(*string);

        /* use this configuration */
        _config_load_preset(priv, c);

    }
    /* reset to previous value */
    else
        visual_param_set_value_string(p, *string);


}

/**
 * callback to change a color parameter (called by config_change_param)
 */
static void _change_color(BlurskPrivate *priv, uint32_t *color, VisParam *p, int *(validator)(void *value))
{
    VisColor *c;

    c = visual_param_get_value_color(p);
    *color = ((c->r)<<16) + ((c->g)<<8) + c->b;
    priv->update_config_string = 1;
}

/**
 * callback to change a string parameter (called by config_change_param)
 */
static void _change_string(BlurskPrivate *priv, char **string,
                           VisParam *p, int *(validator)(void *value))
{
    visual_return_if_fail(visual_param_get_value_string(p));

    if(!validator || validator((void*)visual_param_get_value_string(p)))
    {
        /* free previous string? */
        if(*string)
            visual_mem_free(*string);

        *string = visual_strdup(visual_param_get_value_string(p));

        priv->update_config_string = 1;
    }
    /* reset to previous value */
    else
        visual_param_set_value_string(p, *string);
}


/**
 * callback to change a bool parameter (called by config_change_param)
 */
static void _change_bool(BlurskPrivate *priv, int *boolean, VisParam *p, int *(validator)(void *value))
{
    int t = visual_param_get_value_bool(p);

    /* validate boolean */
    if(t == 0 || t == 1)
    {
        *boolean = t;

        priv->update_config_string = 1;
    }
    /* reset to previous value */
    else
        visual_param_set_value_bool(p, *boolean);
}

/**
 * callback to change an integer parameter (called by config_change_param)
 */
static void _change_int(BlurskPrivate *priv, int *integer, VisParam *p, int *(validator)(void *value))
{
    *integer = visual_param_get_value_integer(p);

    priv->update_config_string = 1;
}



/**
 * check if fade-speed string is valid
 * @p speed - fade-speed string
 * @return 0 if invalid, 1 if valid
 */
static int _fade_speed_validate(char *speed)
{
    switch(speed[0])
    {
        case 'N':
        case 'S':
        case 'M':
        case 'F':
            return 1;
    }

    return 0;
}

/**
 * check if flash-style string is valid
 * @p style - flash_style string
 * @return 0 if invalid, 1 if valid
 */
static int _flash_style_validate(char *style)
{
    if(strcmp(style, "Full flash") == 0 ||
       strcmp(style, "Invert flash") == 0 ||
       strcmp(style, "No flash") == 0 ||
       strcmp(style, "Random flash"))
        return 1;

    return 0;
}

/**
 * check if overall-effect string is valid
 * @p effect - overall_effect string
 * @return 0 if invalid, 1 if valid
 */
static int _overall_effect_validate(char *effect)
{
    if(strcmp(effect, "Bump effect") == 0 ||
       strcmp(effect, "Anti-fade effect") == 0 ||
       strcmp(effect, "Ripple effect") == 0 ||
       strcmp(effect, "Normal effect") == 0)
        return 1;

    return 0;
}

/**
 * check if floaters string is valid
 * @p floaters - floaters string
 * @return 0 if invalid, 1 if valid
 */
static int _floaters_validate(char *floaters)
{
    char *tmp;
    int i = 0;

    while((tmp = floaters_name(i)))
    {
        if(strcmp(floaters, tmp) == 0)
            return 1;
        i++;
    }

    return 0;
}

/**
 * Check if show_info string is valid
 * @p show_info - show info string
 * @return 0 if invalid, 1 if valid
 */
static int _show_info_validate(char *showinfo)
{
    if(*showinfo == 'A' ||
        *showinfo == 'N' ||
        *showinfo == 'T')
        return 1;

    return 0;
}

/**
 * check if blur-style is valid
 * @p style - blur_style string
 * @return 0 if invalid, 1 if valid
 */
static int _blur_style_validate(char *style)
{
    char *tmp;
    int i = 0;

    while((tmp = blur_name(i)))
    {
        if(strcmp(style, tmp) == 0)
            return 1;
        i++;
    }

    return 0;
}

/**
 * check if transition_speed string is valid
 * @p speed - transition speed string
 * @return 0 if invalid, 1 if valid
 */
static int _blur_transition_speed_validate(char *speed)
{
    switch(speed[0])
    {
        /* slow */
        case 'S':
        /* medium */
        case 'M':
        /* fast */
        case 'F':
            return 1;
    }

    return 0;
}

/**
 * check if blur_when string is valid
 * @p when - blur_when string
 * @return 0 if invalid, 1 if valid
 */
static int _blur_when_validate(char *when)
{
    char *tmp;
    int i = 0;

    while((tmp = blur_when_name(i)))
    {
        if(strcmp(when, tmp) == 0)
            return 1;
        i++;
    }

    return 0;
}

/**
 * check if cpu_speed string is valid
 * @p speed - cpu_speed string
 * @return 0 if invalid, 1 if valid
 */
static int _cpu_speed_validate(char *speed)
{
    switch(speed[0])
    {
        /* slow */
        case 'S':
        /* medium */
        case 'M':
        /* fast */
        case 'F':
            return 1;
    }

    return 0;
}

/**
 * check if signal-style string is valid
 * @p style - signal_style string
 * @return 0 if invalid, 1 if valid
 */
static int _signal_style_validate(char *style)
{
    switch(style[0])
    {
        /* High/Low plot */
        case 'H':
        /* Stereo spectrum */
        case 'S':
        /* Mono spectrum */
        case 'M':
        /* Flower */
        case 'F':
        /* Oscilloscope */
        case 'O':
        /* Radial spectrum */
        case 'R':
        /* Phase shift */
        case 'P':
            return 1;
    }

    return 0;
}

/**
 * check if plot-style string is valid
 * @p style - signal_style string
 * @return 0 if invalid, 1 if valid
 */
static int _plot_style_validate(char *style)
{
    switch(style[0])
    {
        /* Line */
        case 'L':
        /* Inertia */
        case 'I':
        /* Radar */
        case 'R':
        /* Dots */
        case 'D':
        /* Fuzzy dots */
        case 'F':
        /* Plus signs */
        case 'P':
        /* Airbrush */
        case 'A':
        /* Edges */
        case 'E':
        /* Mirrored */
        case 'M':
        /* Starburst */
        case 'S':
        /* None */
        case 'N':
            return 1;
    }

    return 0;
}

/**
 * check if colorstyle string is valid
 * @p name - name of colorstyle
 * @return 0 if invalid, 1 if valid
 */
static int _color_style_validate(char *name)
{
    int i;
    char *oname;


    for(i = 0; (oname = color_name(i)); i++)
    {
        if(strcmp(name, oname) == 0)
            return 1;
    }

    if(strcmp(name, "Random") == 0)
        return 1;

    return 0;
}

/**
 * check if sginal_color string is valid
 * @p color - color string
 * @return 0 if invalid, 1 if valid
 */
static int _color_signal_validate(char *color)
{
    switch(color[0])
    {
        case 'N':
        case 'W':
        case 'C':
            return 1;
    }

    return 0;
}

/**
 * check if background_color string is valid
 * @p bg - background color string
 * @return 0 if invalid, 1 if valid
 */
static int _color_background_validate(char *bg)
{
    switch(bg[0])
    {
        case 'R':
        case 'B':
        case 'W':
        case 'D':
        case 'S':
        case 'C':
        case 'F':
            return 1;
    }

    return 0;
}

/**
 * user changed a config parameter
 *
 * @p p - libvisual parameter
 * @p c - blursk configuration structure
 */
void config_change_param(BlurskPrivate *priv, VisParam *p)
{
    /**
     * structure defining handler functions for configuration values
     */
    struct
    {
        /* parameter-name */
        char *name;
        /* blursk storage for parameter value */
        void *value;
        /* validator function */
        int (*validator)(void *value);
        /* function called to change parameter */
        void (*change)(BlurskPrivate *priv, void **value,
                       VisParam *parameter, int (*validator)(void *value));
        /* function called after parameter change */
        void (*postchange)(BlurskPrivate *priv);
    } parms[] =
    {
        {"color", &config.color, NULL, (void *) _change_color, __color_genmap},
        {"color_style", &config.color_style, (void *) _color_style_validate, (void *) _change_string, __color_genmap},
        {"signal_color", &config.signal_color, (void *) _color_signal_validate, (void *) _change_string, NULL},
        {"contour_lines", &config.contour_lines, NULL, (void *) _change_bool, NULL},
        {"hue_on_beats", &config.hue_on_beats, NULL, (void *) _change_bool, NULL},
        {"slow_motion", &config.slow_motion, NULL, (void *) _change_bool, NULL},
        {"thick_on_beats", &config.thick_on_beats, NULL, (void *) _change_bool, NULL},
        {"background", &config.background, (void *) _color_background_validate, (void *) _change_string, NULL},
        {"blur_style", &config.blur_style, (void *) _blur_style_validate, (void *) _change_string, NULL},
        {"transition_speed", &config.transition_speed, (void *) _blur_transition_speed_validate, (void *) _change_string, NULL},
        {"blur_when", &config.blur_when, (void *) _blur_when_validate, (void *) _change_string, NULL},
        {"blur_stencil", &config.blur_stencil, NULL, (void *) _change_string, NULL},
        {"fade_speed", &config.fade_speed, (void *) _fade_speed_validate, (void *) _change_string, NULL},
        {"signal_style", &config.signal_style, (void *) _signal_style_validate, (void *) _change_string, NULL},
        {"plot_style", &config.plot_style, (void *) _plot_style_validate, (void *) _change_string, NULL},
        {"flash_style", &config.flash_style, (void *) _flash_style_validate, (void *) _change_string, NULL},
        {"overall_effect", &config.overall_effect, (void *) _overall_effect_validate, (void *) _change_string, NULL},
        {"floaters", &config.floaters, (void *) _floaters_validate, (void *) _change_string, NULL},
        {"cpu_speed", &config.cpu_speed, (void *) _cpu_speed_validate, (void *) _change_string, NULL},
        {"beat_sensitivity", &config.beat_sensitivity, NULL, (void *) _change_int, NULL},
        {"config_string", &config.config_string, NULL, (void *) _change_config_string, NULL},
        {"show_info", &config.show_info, (void *) _show_info_validate, (void *) _change_string, NULL},
        {"info_timeout", &config.info_timeout, NULL, (void *) _change_int, NULL},
        {"show_timestamp", &config.show_timestamp, NULL, (void *) _change_bool, NULL}
    };



    /** look for parameter in our structure */
    int i;
    for(i = 0; i < QTY(parms); i++)
    {
        /* not our parameter? -> continue the quest */
        if(!visual_param_has_name(p, parms[i].name)) {
            continue;
        }

        /* call this parameters' change handler */
        if(parms[i].change)
            parms[i].change(priv, parms[i].value, p, parms[i].validator);

        /* call this parameters' post-change handler */
        if(parms[i].postchange)
            parms[i].postchange(priv);

        return;
    }

    visual_log(VISUAL_LOG_WARNING, "Unknown param '%s'", visual_param_get_name(p));
}
