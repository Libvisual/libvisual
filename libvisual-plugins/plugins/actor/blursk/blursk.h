/* $URL$
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

/* blursk.h */


#ifndef BLURSK_H
#define BLURSK_H

#include <libvisual/libvisual.h>

#define VISUAL_PI 3.14159265358979

#define QTY(array)  (sizeof(array) / sizeof(*(array)))

#define rand_0_to(n)   (visual_rand() % (n))

#define MAX(a, b) (a > b ? a : b)
#define MIN(a ,b) (a > b ? b : a)

typedef struct
{
    /* dimensions */
    int width;
    int height;

    /* color options */
    uint32_t color;
    char    *color_style;
    char    *fade_speed;
    char    *signal_color;
    int      contour_lines;
    int      hue_on_beats;
    char    *background;

    /* blur/fade options */
    char    *blur_style;
    char    *transition_speed;
    char    *blur_when;
    char    *blur_stencil;
    int      slow_motion;

    /* other effects */
    char    *signal_style;
    char    *plot_style;
    int      thick_on_beats;
    char    *flash_style;
    char    *overall_effect;
    char    *floaters;

    /* miscellany from the Advanced screen */
    char    *cpu_speed;
    char    *show_info;
    int     info_timeout;
    int     show_timestamp;

    /* beat detector */
    int32_t beat_sensitivity;
    
    /* config-string */
    char *config_string;
} BlurskConfig;

extern char config_default_color_style[];
extern char config_default_signal_color[];
extern char config_default_background[];
extern char config_default_blur_style[];
extern char config_default_transition_speed[];
extern char config_default_fade_speed[];
extern char config_default_blur_when[];
extern char config_default_blur_stencil[];
extern char config_default_signal_style[];
extern char config_default_plot_style[];
extern char config_default_flash_style[];
extern char config_default_overall_effect[];
extern char config_default_floaters[];
extern char config_default_cpu_speed[];
extern char config_default_show_info[];
extern char config_default_fullscreen_method[];


extern BlurskConfig config;

void __blursk_render_pcm (BlurskPrivate *priv, int16_t *pcmbuf);
void __blursk_init (BlurskPrivate *priv);
void __blursk_cleanup (BlurskPrivate *priv);


/* in config.c */
extern void config_change_param(BlurskPrivate *priv, VisParam *p);
extern void config_default(BlurskConfig *conf);
extern void config_string_genstring(BlurskPrivate *priv);


/* in blur.c */
extern int blur_stencil;
extern int blur(BlurskPrivate *, int, int);
extern char *blur_name(int);
extern char *blur_when_name(int);


/* in blursk.c */
extern int blurskinfo;
extern int nspectrums;
extern void blursk_genrender(void);
extern void blursk_event_newsong(VisSongInfo *newsong);
extern char *floaters_name(int);


/* in color.c */
extern uint32_t colors[256];
extern void color_transition(BlurskPrivate *, int, int, int);
extern void color_genmap(BlurskPrivate *, int);
extern void color_bg(BlurskPrivate *, int, int16_t*);
extern void color_cleanup(void);
extern char *color_name(int);
extern char *color_background_name(int);
extern int color_good_for_bump(char *);
extern void color_beat(BlurskPrivate *);


/* in img.c */
#define IMG_PIXEL(x,y)  (img_buf[(y) * img_bpl + (x)])
extern unsigned char *img_buf;
extern unsigned char *img_prev;
extern unsigned char *img_tmp;
extern unsigned char **img_source;
extern unsigned int img_height, img_physheight;
extern unsigned int img_width, img_physwidth;
extern unsigned int img_bpl, img_physbpl;
extern unsigned int img_chunks;
extern unsigned char img_rippleshift;
extern void img_resize(BlurskPrivate *, int, int);
extern void img_cleanup(void);
extern void img_copyback(void);
extern void img_invert(void);
extern unsigned char *img_expand(int *, int *, int *);
extern unsigned char *img_bump(int *, int *, int *);
extern unsigned char *img_travel(int *, int *, int *);
extern unsigned char *img_ripple(int *, int *, int *);


/* in loop.c or loopx86.s */
extern void loopblur(void);
extern void loopsmear(void);
extern void loopmelt(void);
extern void loopsharp(void);
extern void loopreduced1(void);
extern void loopreduced2(void);
extern void loopreduced3(void);
extern void loopreduced4(void);
extern void loopfade(int change);
extern void loopinterp(void);


/* in render.c */
extern void render_dot(int x, int y, unsigned char color);
extern void render(int thick, int center, int ndata, int16_t *data);
extern char *render_plotname(int);
extern char *signal_style_name(int i);


/* in bitmap.c */
extern int bitmap_index(char *str);
extern int bitmap_test(int bindex, int x, int y);
extern void bitmap_flash(int bindex);
extern char *bitmap_flash_name(int i);
extern char *bitmap_stencil_name(int i);


/* in paste.c */
extern BlurskConfig *paste_parsestring(char *str);
extern char *paste_genstring(void);


/* in text.c */
extern void textdraw(unsigned char *img, int height, int bpl, char *side, char *text);
extern void convert_ms_to_timestamp(char *buf, int ms);

#endif

