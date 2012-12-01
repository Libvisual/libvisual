/* Libvisual-plugins - Standard plugins for libvisual
 *
 * Copyright (C) 1999, 2001 Zinx Verituse <zinx@xmms.org>
 *
 * Authors: Zinx Verituse <zinx@xmms.org>
 *	    Dennis Smit <ds@nerds-incorporated.org>
 *
 * Notes: Bumpscope plugin originally from XMMS.
 *
 * $Id: bump_scope.c,v 1.18 2006/02/05 18:47:26 synap Exp $
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bump_scope.h"

#define PI 3.14159265358979323846

static void bumpscope_blur_8 (uint8_t *ptr, int w, int h, int bpl);
static void bumpscope_generate_intense (BumpscopePrivate *priv);
static void bumpscope_translate (BumpscopePrivate *priv, int x, int y, int *xo, int *yo, int *xd, int *yd, int *angle);
static void bumpscope_draw (BumpscopePrivate *priv);
static inline void draw_vert_line(uint8_t *buffer, int x, int y1, int y2, int pitch);
static void bumpscope_render_light (BumpscopePrivate *priv, int lx, int ly);

static void bumpscope_blur_8 (uint8_t *ptr, int w, int h, int bpl)
{
	register unsigned int i,sum = 0;
	register uint8_t *iptr;

	iptr = ptr + bpl + 1;
	i = bpl * h;

	if (visual_cpu_has_mmx ()) {
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)

		__asm __volatile
			("pxor %%mm6, %%mm6"
			 ::);

		while(i -= 4)
		{
			__asm __volatile
				("\n\t movd %[dest], %%mm0"
				 "\n\t movd %[pix2], %%mm1"
				 "\n\t punpcklbw %%mm6, %%mm0"
				 "\n\t movd %[pix3], %%mm2"
				 "\n\t punpcklbw %%mm6, %%mm1"
				 "\n\t movd %[pix4], %%mm3"
				 "\n\t punpcklbw %%mm6, %%mm2"
				 "\n\t movd %[pix5], %%mm4"
				 "\n\t punpcklbw %%mm6, %%mm3"
				 "\n\t paddw %%mm1, %%mm2"
				 "\n\t punpcklbw %%mm6, %%mm4"
				 "\n\t paddw %%mm3, %%mm2"
				 "\n\t paddw %%mm2, %%mm4"
				 "\n\t psrlw $2, %%mm4"
				 "\n\t packuswb %%mm6, %%mm4"
				 "\n\t movd %%mm4, %[dest]"
				 :: [dest] "m" (*(iptr))
				 , [pix2] "m" (*(iptr - bpl))
				 , [pix3] "m" (*(iptr - 1))
				 , [pix4] "m" (*(iptr + 1))
				 , [pix5] "m" (*(iptr + bpl)));

			iptr += 4;
		}

		__asm __volatile
			("\n\t emms");
#endif
	} else {
		while(i--)
		{
			sum = (iptr[-bpl] + iptr[-1] + iptr[1] + iptr[bpl]) >> 2;
			*(iptr++) = sum;
		}
	}
}

static void bumpscope_generate_intense (BumpscopePrivate *priv)
{
	int32_t i;

	for(i = 255; i > 0; i--) {
		priv->intense1[i] = cos(((float)(255-i)*PI)/512.0);
		priv->intense2[i] = pow(priv->intense1[i], 250)*150;
	}

	priv->intense1[0] = priv->intense1[1];
	priv->intense2[0] = priv->intense2[1];
}

static void bumpscope_translate (BumpscopePrivate *priv, int x, int y, int *xo, int *yo, int *xd, int *yd, int *angle)
{
	/* try setting y to both maxes */
	*yo = priv->height/2;
	*angle = asin((float)(y-(priv->height/2))/(float)*yo)/(PI/180.0);
	*xo = (x-(priv->width/2))/cos(*angle*(PI/180.0));

	if (*xo>=-(priv->width/2) && *xo<=(priv->width/2)) {
		*xd = (*xo>0)?-1:1;
		*yd = 0;
		return;
	}

	*yo = -*yo;
	*angle = asin((float)(y-(priv->height/2))/(float)*yo)/(PI/180.0);
	*xo = (x-(priv->width/2))/cos(*angle*(PI/180.0));

	if (*xo>=-(priv->width/2) && *xo<=(priv->width/2)) {
		*xd = (*xo>0)?-1:1;
		*yd = 0;
		return;
	}

	/* try setting x to both maxes */
	*xo = priv->width/2;
	*angle = acos((float)(x-(priv->width/2))/(float)*xo)/(PI/180.0);
	*yo = (y-(priv->height/2))/sin(*angle*(PI/180.0));

	if (*yo>=-(priv->height/2) && *yo<=(priv->height/2)) {
		*yd = (*yo>0)?-1:1;
		*xd = 0;
		return;
	}

	*xo = -*xo;
	*angle = acos((float)(x-(priv->width/2))/(float)*xo)/(PI/180.0);
	*yo = (y-(priv->height/2))/sin(*angle*(PI/180.0));

	/* if this isn't right, it's out of our range and we don't care */
	*yd = (*yo>0)?-1:1;
	*xd = 0;
}

static void bumpscope_draw (BumpscopePrivate *priv)
{
	/* FIXME remove the statics */
	static int angle, xo, yo, xd, yd, lx, ly;
	static int was_moving = 0, was_color = 0;
	static float sd = 0;
	static float h, s, v;
	static int hd = 0;

	lx = priv->light_x;
	ly = priv->light_y;
	if (priv->moving_light) {
		if (!was_moving) {
			bumpscope_translate (priv, lx, ly, &xo, &yo, &xd, &yd, &angle);
			was_moving = 1;
		}

		lx = priv->width/2+cos(angle*(PI/180.0))*xo;
		ly = priv->height/2+sin(angle*(PI/180.0))*yo;

		angle += 2; if (angle >= 360) angle = 0;

		xo += xd;
		if ((int)xo > ((int)priv->width/(int)2) || (int)xo < -((int)priv->width/(int)2)) {
			xo = (xo>0)?(priv->width/2):-(priv->width/2);
			if (visual_random_context_int(priv->rcontext)&1) {
				xd = (xd>0)?-1:1;
				yd = 0;
			} else {
				yd = (yd>0)?-1:1;
				xd = 0;
			}
		}

		yo += yd;
		if ((int)yo > ((int)priv->height/(int)2) || (int)yo < -((int)priv->height/(int)2)) {
			yo = (yo>0)?(priv->height/2):-(priv->height/2);
			if (visual_random_context_int(priv->rcontext)&1) {
				xd = (xd>0)?-1:1;
				yd = 0;
			} else {
				yd = (yd>0)?-1:1;
				xd = 0;
			}
		}
	} else {
		if (was_moving) {
			was_moving = 0;
		}
	}

	if (priv->color_cycle) {
		if (!was_color) {
			visual_color_get_hsv (&priv->color, &h, &s, &v);

			was_color = 1;

			if (visual_random_context_int(priv->rcontext)&1) {
				hd = (visual_random_context_int(priv->rcontext)&1)*2-1;
				sd = 0;
			} else {
				sd = 0.01 * ((visual_random_context_int(priv->rcontext)&1)*2-1);
				hd = 0;
			}
		}

		visual_color_set_hsv (&priv->color, h, s, v);
		__bumpscope_generate_palette (priv, &priv->color);

		if (hd) {
			h += hd;
			if (h >= 360) h = 0;
			if (h < 0) h = 359;
			if ((visual_random_context_int(priv->rcontext)%150) == 0) {
				if (visual_random_context_int(priv->rcontext)&1) {
					hd = (visual_random_context_int(priv->rcontext)&1)*2-1;
					sd = 0;
				} else {
					sd = 0.01 * ((visual_random_context_int(priv->rcontext)&1)*2-1);
					hd = 0;
				}
			}
		} else {
			s += sd;

			if (s <= 0 || s >= 0.5) {
				if (s < 0) s = 0;

				if (s > 0.52) {
					sd = -0.01;
				} else if (s == 0) {
					h = visual_random_context_int(priv->rcontext)%360;
					sd = 0.01;
				} else {
					if (visual_random_context_int(priv->rcontext)&1) {
						hd = (visual_random_context_int(priv->rcontext)&1)*2-1;
						sd = 0;
					} else {
						sd = 0.01 * ((visual_random_context_int(priv->rcontext)&1)*2-1);
						hd = 0;
					}
				}
			}
		}
	} else if (was_color) {
		__bumpscope_generate_palette (priv, &priv->color);
		was_color = 0;
	}

	if (visual_color_compare (&priv->old_color, &priv->color) == FALSE)
		priv->colorchanged = TRUE;
	else
		priv->colorchanged = FALSE;

	visual_color_copy (&priv->old_color, &priv->color);

	bumpscope_render_light (priv, lx, ly);
}

static inline void draw_vert_line(uint8_t *buffer, int x, int y1, int y2, int pitch)
{
	int y;
	uint8_t *p;
	if(y1 < y2)
	{
		p = buffer+((y1+1)*pitch)+x+1;
		for(y = y1; y <= y2; y++) {
			*p = 0xff;
			p += pitch;
		}
	}
	else if(y2 < y1)
	{
		p = buffer+((y2+1)*pitch)+x+1;
		for(y = y2; y <= y1; y++) {
			*p = 0xff;
			p += pitch;
		}
	}
	else {
		buffer[((y1+1)*pitch)+x+1] = 0xff;
	}
}

static void bumpscope_render_light (BumpscopePrivate *priv, int lx, int ly)
{
	int i, j, prev_y, dy, dx, xq, yq;
	int video_pitch;

	video_pitch = visual_video_get_pitch (priv->video);

	prev_y = video_pitch + 1;

	for (dy = (-ly)+(priv->phongres/2), j = 0; j < priv->height; j++, dy++, prev_y+=video_pitch-priv->width) {
		for (dx = (-lx)+(priv->phongres/2), i = 0; i < priv->width; i++, dx++, prev_y++) {

			xq = (priv->rgb_buf[prev_y-1]-priv->rgb_buf[prev_y+1])+dx;
			yq = (priv->rgb_buf[prev_y-video_pitch]-priv->rgb_buf[prev_y+video_pitch])+dy;

			if (yq<0 || yq>=priv->phongres ||
			    xq<0 || xq>=priv->phongres) {

				priv->rgb_buf2[prev_y] = 0;

				continue;
			}

			priv->rgb_buf2[prev_y] = priv->phongdat[(xq * priv->phongres) + yq];
		}
	}
}

void __bumpscope_generate_palette (BumpscopePrivate *priv, VisColor *col)
{
	VisColor *pal_colors = visual_palette_get_colors (priv->pal);

	int32_t i,r,g,b;

	for (i = 0; i < 256; i++) {
		r = ((float)(100*col->r/255)*priv->intense1[i]+priv->intense2[i]);
		g = ((float)(100*col->g/255)*priv->intense1[i]+priv->intense2[i]);
		b = ((float)(100*col->b/255)*priv->intense1[i]+priv->intense2[i]);

		pal_colors[i].r = r;
		pal_colors[i].g = g;
		pal_colors[i].b = b;
	}
}

void __bumpscope_generate_phongdat (BumpscopePrivate *priv)
{
	int y, x;
	double i, i2, cont = 0;
	int phresd2 = priv->phongres / 2;

	for (y = phresd2; y < priv->phongres; y++) {
		for (x = phresd2; x < priv->phongres; x++) {

			i = (double)x/((double)priv->phongres)-1;
			i2 = (double)y/((double)priv->phongres)-1;

			if (priv->diamond)
				i = 1 - pow(i*i2,.75) - i*i - i2*i2;
			else
				i = 1 - i*i - i2*i2;

			if (i >= 0) {
				if (priv->diamond)
					i = i*i*i * 255.0;
				else
					i = i*i*i * 255.0;

				if (i > 255) i = 255;

				cont = i;

				if (cont < 110)
					cont = 0;

				priv->phongdat[((y - phresd2) * priv->phongres) + x - phresd2] = cont;
				priv->phongdat[(((priv->phongres-1)-y + phresd2) * priv->phongres) + x - phresd2] = cont;
				priv->phongdat[(((y - phresd2) * priv->phongres) + ((priv->phongres-1) - x)) + phresd2] = cont;
				priv->phongdat[(((priv->phongres-1)-y + phresd2) * priv->phongres) + ((priv->phongres-1)-x + phresd2)] = cont;
			} else {
				priv->phongdat[((y - phresd2) * priv->phongres) + x - phresd2] = cont;
				priv->phongdat[(((priv->phongres-1)-y + phresd2) * priv->phongres) + x - phresd2] = cont;
				priv->phongdat[(((y - phresd2) * priv->phongres) + ((priv->phongres-1) - x)) + phresd2] = cont;
				priv->phongdat[(((priv->phongres-1)-y + phresd2) * priv->phongres) + ((priv->phongres-1)-x + phresd2)] = cont;
			}
		}
	}
}

void __bumpscope_render_pcm (BumpscopePrivate *priv, float *data)
{
	int i, y, prev_y, video_pitch;

	video_pitch = visual_video_get_pitch (priv->video);

	prev_y = (int)priv->height/(int)2 + (data[0] * (priv->height / 2));

	if (prev_y < 0) prev_y = 0;
	if (prev_y >= priv->height) prev_y = priv->height-1;

	for(i = 0; i < priv->width; i++)
	{
		y = (i*511)/(priv->width-1);

		y = (int)priv->height/(int)2 + (data[y % 512] * priv->height);

		/* This should _never_ happen (and doesn't), but I test anyway. */
		if (y < 0) y = 0;
		if(y >= priv->height) y = priv->height - 1;

		draw_vert_line(priv->rgb_buf, i, prev_y, y, video_pitch);
		prev_y = y;
	}

	bumpscope_blur_8(priv->rgb_buf, priv->width, priv->height, video_pitch);
	bumpscope_draw (priv);
}

void __bumpscope_init (BumpscopePrivate *priv)
{
	priv->phongdat = visual_mem_malloc0 (priv->phongres * priv->phongres * 2);
	priv->rgb_buf  = visual_mem_malloc0 (priv->width*2 * priv->height + 1);
	priv->rgb_buf2 = visual_mem_malloc0 (priv->width*2 * priv->height + 1);

	__bumpscope_generate_phongdat (priv);
	bumpscope_generate_intense (priv);
	__bumpscope_generate_palette (priv, &priv->color);
}

void __bumpscope_cleanup (BumpscopePrivate *priv)
{
	if (priv->phongdat != NULL)
		visual_mem_free (priv->phongdat);

	if (priv->rgb_buf != NULL)
		visual_mem_free (priv->rgb_buf);

	if (priv->rgb_buf2 != NULL)
		visual_mem_free (priv->rgb_buf2);
}

