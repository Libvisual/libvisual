#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bump_scope.h"

#define min(x,y) ((x)<(y)?(x):(y))

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
	while(i--)
	{
		sum = (iptr[-bpl] + iptr[-1] + iptr[1] + iptr[bpl]) >> 2;
		if(sum > 2)
			sum -= 2;
		*(iptr++) = sum;
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
			if (random()&1) {
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
			if (random()&1) {
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
			visual_color_to_hsv (&priv->color, &h, &s, &v);

			was_color = 1;

			if (random()&1) {
				hd = (random()&1)*2-1;
				sd = 0;
			} else {
				sd = 0.01 * ((random()&1)*2-1);
				hd = 0;
			}
		}

		visual_color_from_hsv (&priv->color, h, s, v);
		__bumpscope_generate_palette (priv, &priv->color);

		if (hd) {
			h += hd;
			if (h >= 360) h = 0;
			if (h < 0) h = 359;
			if ((random()%150) == 0) {
				if (random()&1) {
					hd = (random()&1)*2-1;
					sd = 0;
				} else {
					sd = 0.01 * ((random()&1)*2-1);
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
					h = random()%360;
					sd = 0.01;
				} else {
					if (random()&1) {
						hd = (random()&1)*2-1;
						sd = 0;
					} else {
						sd = 0.01 * ((random()&1)*2-1);
						hd = 0;
					}
				}
			}
		}
	} else if (was_color) {
		__bumpscope_generate_palette (priv, &priv->color);
		was_color = 0;
	}

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

	prev_y = priv->video->pitch + 1;
	
	for (dy = (-ly)+(priv->phongres/2), j = 0; j < priv->height; j++, dy++, prev_y+=priv->video->pitch-priv->width) {
		for (dx = (-lx)+(priv->phongres/2), i = 0; i < priv->width; i++, dx++, prev_y++) {

			xq = (priv->rgb_buf[prev_y-1]-priv->rgb_buf[prev_y+1])+dx;
			yq = (priv->rgb_buf[prev_y-priv->video->pitch]-priv->rgb_buf[prev_y+priv->video->pitch])+dy;
			
			if (yq<0 || yq>=priv->phongres ||
			    xq<0 || xq>=priv->phongres) {
			
				priv->rgb_buf2[prev_y] = 0;
			    	
				continue;
			}
			
			priv->rgb_buf2[prev_y] = priv->phongdat[(yq * priv->phongres) + xq];
		}
	}
}

void __bumpscope_generate_palette (BumpscopePrivate *priv, VisColor *col)
{
	int32_t i,r,g,b;

	for (i = 0; i < 256; i++) {
		r = ((float)(100*col->r/255)*priv->intense1[i]+priv->intense2[i]);
		if (r > 255) r = 255;

		g = ((float)(100*col->g/255)*priv->intense1[i]+priv->intense2[i]);
		if (g > 255) g = 255;

		b = ((float)(100*col->b/255)*priv->intense1[i]+priv->intense2[i]);
		if (b > 255) b = 255;

		priv->pal.colors[i].r = r;
		priv->pal.colors[i].g = g;
		priv->pal.colors[i].b = b;
		
	}
}

void __bumpscope_generate_phongdat (BumpscopePrivate *priv)
{
	int y, x;
	double i, i2;

	for (y = 0; y < (priv->phongres); y++) {
		for (x = 0; x < (priv->phongres); x++) {

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
				priv->phongdat[(y * priv->phongres) + x] = i;
				priv->phongdat[(((priv->phongres-1)-y) * priv->phongres) + x] = i;
				priv->phongdat[(y * priv->phongres) + ((priv->phongres-1) - x)] = i;
				priv->phongdat[(((priv->phongres-1)-y) * priv->phongres) + ((priv->phongres-1)-x)] = i;
			} else {
//				priv->phongdat[(y * priv->phongres) + x] = 0;
//				priv->phongdat[(((priv->phongres-1)-y) * priv->phongres) + x] = 0;
//				priv->phongdat[(y * priv->phongres) + ((priv->phongres-1)-x)] = 0;
//				priv->phongdat[(((priv->phongres-1)-y) * priv->phongres) + ((priv->phongres-1)-x)] = 0;
			}
		}
	}
#if 0
	/* Screw this, I can't get it right, and I don't care we're doing it the dirty way (Synap, Dennis Smit) */
	for (y = 0; y <= priv->phongres / 2; y++) {
		for (x = 0; x <= priv->phongres / 2; x++) {
			uint8_t tmp;

			/* Save bottom right */
			tmp = priv->phongdat[((y + priv->phongres / 2) * priv->phongres) + x + (priv->phongres / 2)];
			
			/* Set upper left in bottom right */
			priv->phongdat[((y + priv->phongres / 2) * priv->phongres) + x + (priv->phongres / 2)] = 
				priv->phongdat[(y * priv->phongres) + x];

			/* Set old bottom right in upper left */
			priv->phongdat[(y * priv->phongres) + x] = tmp;

			/* Save bottom left */
			tmp = priv->phongdat[((y + priv->phongres / 2) * priv->phongres) + x];

			/* Set upper right in bottom left */
			priv->phongdat[((y + priv->phongres / 2) * priv->phongres) + x] =
				priv->phongdat[(y * priv->phongres) + x + (priv->phongres / 2)];
			
			/* Set old bottom left in upper right */
			priv->phongdat[(y * priv->phongres) + x + (priv->phongres / 2)] = tmp;
		}
	}
#endif	
}

void __bumpscope_render_pcm (BumpscopePrivate *priv, short data[3][512])
{
	int i, y, prev_y;

	prev_y = (int)priv->height/(int)2 + ((int)data[2][0]*(int)priv->height)/(int)0x10000;

	if (prev_y < 0) prev_y = 0;
	if (prev_y >= priv->height) prev_y = priv->height-1;

	for(i = 0; i < priv->width; i++)
	{
		y = (i*511)/(priv->width-1);

		y = (int)priv->height/(int)2 + ((int)data[2][y]*(int)priv->height)/(int)0x10000;

		/* This should _never_ happen (and doesn't), but I test anyway. */
		if (y < 0) y = 0;
		if(y >= priv->height) y = priv->height - 1;

		draw_vert_line(priv->rgb_buf, i, prev_y, y, priv->video->pitch);
		prev_y = y;
	}

	bumpscope_blur_8(priv->rgb_buf, priv->width, priv->height, priv->video->pitch);
	bumpscope_draw (priv);
}

void __bumpscope_init (BumpscopePrivate *priv)
{
	priv->phongdat = visual_mem_malloc0 (priv->phongres * priv->phongres * 2);
	priv->rgb_buf = visual_mem_malloc0 (priv->video->size + (priv->video->pitch * 2) + 1);
	priv->rgb_buf2 = visual_mem_malloc0 (priv->video->size + (priv->video->pitch * 2) + 1);

	__bumpscope_generate_phongdat (priv);
	bumpscope_generate_intense (priv);
	__bumpscope_generate_palette (priv, &priv->color);
}

void __bumpscope_cleanup (BumpscopePrivate *priv)
{
	visual_mem_free (priv->phongdat);
	visual_mem_free (priv->rgb_buf);
	visual_mem_free (priv->rgb_buf2);
}

