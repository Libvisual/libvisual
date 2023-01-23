#include <config.h>

#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "renderer.h"
#include "display.h"
#include "main.h"

#define wrap(a) ( a < 0 ? 0 : ( a > 255 ? 255 : a ))
#define assign_max(p,a) ( *p = ( *p > a ? *p : a ))
#define PI 3.14159

void _inf_generate_colors(InfinitePrivate *priv)
{
	int i,k;
	float colors[NB_PALETTES][2][3]={{{1,1,1},{1,1,1}},
					 {{2,1.5,0},{0,0.5,2}},
					 {{0,1,2},{0,1,0}},
					 {{0,2,1},{0,0,1}},
					 {{2,0,0},{0,1,1}}};

	 for (k=0;k<NB_PALETTES;k++) {
		for ( i=0; i<128; i++ )
			{
				priv->color_table[k][i].r = colors[k][0][0]*i;
				priv->color_table[k][i].g = colors[k][0][1]*i;
				priv->color_table[k][i].b = colors[k][0][2]*i;
			}
		for ( i=0; i<128; i++ )
			{
				priv->color_table[k][i+128].r = colors[k][0][0]*127+colors[k][1][0]*i;
				priv->color_table[k][i+128].g = colors[k][0][1]*127+colors[k][1][1]*i;
				priv->color_table[k][i+128].b = colors[k][0][2]*127+colors[k][1][2]*i;
			}
	}
}

void _inf_change_color(InfinitePrivate *priv, int t2,int t1,int w)
{
	VisColor *pal_colors = visual_palette_get_colors (priv->pal);

	int i;

	for (i=0;i<256;i++) {
		int r,g,b;
		r=(priv->color_table[t1][i].r*w+priv->color_table[t2][i].r*(256-w))>>8;
		g=(priv->color_table[t1][i].g*w+priv->color_table[t2][i].g*(256-w))>>8;
		b=(priv->color_table[t1][i].b*w+priv->color_table[t2][i].b*(256-w))>>8;

		pal_colors[i].r = r;
		pal_colors[i].g = g;
		pal_colors[i].b = b;
	}
}

static void _inf_compute_surface(InfinitePrivate *priv, t_interpol* vector_field)
{
	int i,j;
	int add_dest=0,add_src;
	t_interpol *interpol;
	register uint8_t* ptr_pix;
	int color = 0;
	uint8_t* ptr_swap;

	/* FIXME OK we're going to take a different approach:
	 * have a whole set of interpolate buffers:
	 * every buffer containing one element, the different weights, the upper and lower part of
	 * the interpolation coords etc, with this setup it's MUCH easier to vectorize it damn hard.
	 */

	/* FIXME ok this sucked, is een mislukte actie geweest zie ik, omdat ik mmx regs te kort kwam
	 * zat ik met restore problemen, waardoor het uiteindelijk even snel werd, moet de hele blur routine
	 * van deze plugin from scratch doen... */
	if (0) {
#if 0
#if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64)
		int add_src2[2];
		uint8_t *ptr_pix2[2];
		int plugwidth = priv->plugwidth;
		uint8_t *ptr_surf = priv->surface1;
		int aflag = 0xffff;

		/* Prep general used regs, mm6 as empty (un)packer, mm7 to contain plugwidth for packed words
		 * and we use mm5 to contain 0x0000ffff0000ffff */
		__asm __volatile
			("\n\t movd %[plugw], %%mm6"
			 "\n\t movd %[plugw], %%mm7"
			 "\n\t movd %[aflag], %%mm5"
			 "\n\t movd %[aflag], %%mm4"

			 "\n\t psllq $32, %%mm6"
			 "\n\t por %%mm6, %%mm7"

			 "\n\t psllq $32, %%mm4"
			 "\n\t por %%mm4, %%mm5"

			 "\n\t pxor %%mm6, %%mm6"
			 "\n\t pxor %%mm4, %%mm4"
			 :: [plugw] "m" (plugwidth)
			 , [aflag] "m" (aflag));

		for (j = 0; j < priv->plugheight; j++) {
			for (i = 0; i < priv->plugwidth; i += 2) {
				int coords[2];

				interpol = (vector_field += 2); // load 4 from.

				coords[0] = interpol[0].coord;
				coords[1] = interpol[1].coord;

//				return;
//				add_src = (interpol->coord & 0xFFFF) * priv->plugwidth + (interpol->coord >> 16); // pack and do in mmx!

//				ptr_pix = priv->surface1 + add_src; // pack and do in mmx!

				/* Do in two pieces! (first coords 0, 1, then 2, 3) */
				__asm __volatile (
						"\n\t movq %[interpol], %%mm0"	/* Start loading in the babies */
						"\n\t movq %[interpol], %%mm1"

						"\n\t psrld $16, %%mm0" /* the >> 16 coord elems */
						"\n\t pand %%mm5, %%mm1" /* 0xffff coord elems */
						"\n\t pmullw %%mm7, %%mm1" /* multiply with plugwidth */
						"\n\t paddd %%mm0, %%mm1" /* Ading >> 16 coords. mm1 now contains the right addsrc offset */

						"\n\t movd %[surf1], %%mm0"
						"\n\t movd %[surf1], %%mm4"
						"\n\t psllq $32, %%mm0"
						"\n\t por %%mm4, %%mm0" /* Contains the ptr_surf 2 times */
						"\n\t paddd %%mm1, %%mm0" /* Now contains the right ptr_pix2 value */

						"\n\t movq %%mm0, %[pix]"
						:: [interpol] "m" (*(coords))
						, [addsrc2] "m" (add_src2)
						, [surf1] "m" (ptr_surf)
						, [pix] "m" (*ptr_pix2));
				int color1 = 0, color2 = 0;
/*				int color1 = (*(ptr_pix2[0])
						+ *(ptr_pix2[0] + 1)
						+ *(ptr_pix2[0] + priv->plugwidth)
						+ *(ptr_pix2[0] + priv->plugwidth + 1));

				int color2 = (*(ptr_pix2[1])
						+ *(ptr_pix2[1] + 1)
						+ *(ptr_pix2[1] + priv->plugwidth)
						+ *(ptr_pix2[1] + priv->plugwidth + 1));
*/
				priv->surface2[add_dest] = color1;
				priv->surface2[add_dest + 1] = color2;

				add_dest += 2;
			}
		}

		__asm __volatile
			("emms");
#endif /* #if defined(VISUAL_ARCH_X86) || defined(VISUAL_ARCH_X86_64) */
#endif /* #if 0 */
	} else {
		const uint8_t *ptr_pix_end = priv->surface1 + (priv->plugwidth * priv->plugheight);

		for (j=0;j<priv->plugheight;j++) {
			for (i=0;i<priv->plugwidth;i++) {
				interpol = &vector_field[add_dest];
				const uint16_t y = interpol->coord & 0xffff;
				const uint16_t x = interpol->coord >> 16;
				(void)add_src;
				ptr_pix = priv->surface1 + y * priv->plugwidth + x;

				color = ptr_pix[0] * (interpol->weight >> 24);

				// right neigbor
				if (ptr_pix + 1 < ptr_pix_end)
				    color += ptr_pix[1] * ((interpol->weight >> 16) & 0xFF);

				// bottom neigbor
				if (ptr_pix + priv->plugwidth < ptr_pix_end)
				    color += ptr_pix[priv->plugwidth] * ((interpol->weight >> 8) & 0xFF);

				// bottom right neigbor
				if (ptr_pix + priv->plugwidth + 1 < ptr_pix_end)
				    color += ptr_pix[priv->plugwidth + 1] * (interpol->weight & 0xFF);

				color >>= 8;
/*
				color= (*(ptr_pix) // *                       (interpol->weight >> 24)
					+*(ptr_pix + 1) // *                   ((interpol->weight & 0xFFFFFF) >> 16)
					+*(ptr_pix + priv->plugwidth) // *     ((interpol->weight & 0xFFFF) >> 8)
					+*(ptr_pix + priv->plugwidth + 1)); // * (interpol->weight & 0xFF)) >> 8;

*/				priv->surface2[add_dest]=color;
				add_dest++;
			}
		}
	}


	ptr_swap=priv->surface1;
	priv->surface1=priv->surface2;
	priv->surface2=ptr_swap;
}

void _inf_display (InfinitePrivate *priv, uint8_t *surf, int pitch)
{
	int i;

	for (i = 0; i < priv->plugheight; i++) {

		visual_mem_copy (surf, priv->surface1 + (i * priv->plugwidth), priv->plugwidth);

		surf += pitch;
	}
}

void _inf_blur(InfinitePrivate *priv, t_interpol* vector_field)
{
	_inf_compute_surface(priv, vector_field);
}

static void _inf_plot1(InfinitePrivate *priv, int x,int y,int c)
{
	if (x>0 && x<priv->plugwidth-3 && y>0 && y<priv->plugheight-3)
		assign_max(&(priv->surface1)[x+y*priv->plugwidth],c);
}


static void _inf_plot2(InfinitePrivate *priv, int x,int y,int c)
{
	int ty;

	if (x>0 && x<priv->plugwidth-3 && y>0 && y<priv->plugheight-3) {
		ty = y*priv->plugwidth;
		assign_max(&(priv->surface1)[x+ty],c);
		assign_max(&(priv->surface1)[x+1+ty],c);
		assign_max(&(priv->surface1)[x+ty+priv->plugwidth],c);
		assign_max(&(priv->surface1)[x+1+ty+priv->plugwidth],c);
	}
}

#define SWAP(x,y)	\
	x = x + y;	\
	y = x - y;	\
	x = x - y;

static void _inf_line(InfinitePrivate *priv, int x1, int y1, int x2, int y2, int c) {
	int dx, dy, cxy,dxy;
	/* calculate the distances */
	dx = abs(x1 - x2);
	dy = abs(y1 - y2);

	cxy = 0;
	if (dy > dx) {
		/* Follow Y axis */
		if (y1 > y2) {
			SWAP(y1, y2);
			SWAP(x1, x2);
		}

		if (x1 > x2)
			dxy = -1;
		else
			dxy = 1;

		for (; y1<y2; y1++) {
			cxy += dx;
			if (cxy >= dy) {
				x1+= dxy;
				cxy -= dy;
			}
			_inf_plot1(priv, x1, y1, c);
		}
	} else {
		/* Follow X axis */
		if (x1 > x2) {
			SWAP(x1, x2);
			SWAP(y1, y2);
		}

		if (y1 > y2)
			dxy = -1;
		else
			dxy = 1;

		for (; x1<x2; x1++) {
			cxy += dy;
			if (cxy >= dx) {
				y1+=dxy;
				cxy -= dx;
			}
		 	_inf_plot1(priv, x1, y1, c);
		}
	}
}

struct sincos {
	int i;
	float *f;
};

/* Little optimization for cos/sin functions */
static struct sincos cosw = { 0, NULL };
static struct sincos sinw = { 0, NULL };

void _inf_spectral(InfinitePrivate *priv, t_effect* current_effect, float data[2][512])
{
	int i, halfheight, halfwidth;
	float old_y1,old_y2;
	float y1=(((data[0][0]+data[1][0]) * 128)*current_effect->spectral_amplitude*priv->plugheight) * (1.0 / 4096.0);
	float y2=(((data[0][0]+data[1][0]) * 128)*current_effect->spectral_amplitude*priv->plugheight) * (1.0 / 4096.0);
	const int density_lines=5;
	const int step=4;
	const int shift=(current_effect->spectral_shift*priv->plugheight)>>8;

	if (cosw.i != priv->plugwidth || sinw.i != priv->plugwidth) {
		if (cosw.f != NULL)
			visual_mem_free(cosw.f);

		if (sinw.f != NULL)
			visual_mem_free(sinw.f);

		sinw.f = cosw.f = NULL;
		sinw.i = cosw.i = 0;
	}

	if (cosw.i == 0 || cosw.f == NULL) {
		float halfPI  = (float)PI/2;
		cosw.i = priv->plugwidth;
		cosw.f = visual_mem_malloc0(sizeof(float)*priv->plugwidth);
		for (i=0; i<priv->plugwidth;i+=step)
			cosw.f[i] = cos((float)i/priv->plugwidth*PI+halfPI);
	}

	if (sinw.i == 0 || sinw.f == NULL) {
		float halfPI = (float)PI/2;
		sinw.i = priv->plugwidth;
		sinw.f = visual_mem_malloc0(sizeof(float)*priv->plugwidth);
		for (i=0; i<priv->plugwidth;i+=step)
			sinw.f[i] = sin((float)i/priv->plugwidth*PI+halfPI);
	}

	if (current_effect->mode_spectre==3) {
			if (y1<0)
				y1=0;
			if (y2<0)
				y2=0;
	}

	halfheight = priv->plugheight >> 1;
	halfwidth  = priv->plugwidth >> 1;
	for (i=step;i<priv->plugwidth;i+=step) {
		old_y1=y1;
		old_y2=y2;
		y1=((data[1][(i<<9)/priv->plugwidth/density_lines] * 256)*
		    current_effect->spectral_amplitude*priv->plugheight) * (1.0 / 4096.0);
		y2=((data[0][(i<<9)/priv->plugwidth/density_lines] * 256)*
		    current_effect->spectral_amplitude*priv->plugheight) * (1.0 / 4096.0);

		switch (current_effect->mode_spectre) {
		case 0:
			_inf_line(priv, i-step,halfheight+shift+old_y2,
			     i,halfheight+shift+y2,
			     current_effect->spectral_color);
			break;
		case 1:
			_inf_line(priv, i-step,halfheight+shift+old_y1,
			     i,halfheight+shift+y1,
			     current_effect->spectral_color);
			_inf_line(priv, i-step,halfheight-shift+old_y2,
			     i,halfheight-shift+y2,
			     current_effect->spectral_color);
			break;
		case 2:
			_inf_line(priv, i-step,halfheight+shift+old_y1,
			     i,halfheight+shift+y1,
			     current_effect->spectral_color);
			_inf_line(priv, i-step,halfheight-shift+old_y1,
			     i,halfheight-shift+y1,
			     current_effect->spectral_color);
			_inf_line(priv, halfwidth+shift+old_y2,i-step,
			     halfwidth+shift+y2,i,
			     current_effect->spectral_color);
			_inf_line(priv, halfwidth-shift+old_y2,i-step,
			     halfwidth-shift+y2,i,
			     current_effect->spectral_color);
			break;
		case 3:
			if (y1<0)
				y1=0;
			if (y2<0)
				y2=0;
		case 4:
			_inf_line(priv, halfwidth  + cosw.f[i-step] * (shift+old_y1),
			     halfheight + sinw.f[i-step] * (shift+old_y1),
			     halfwidth  + cosw.f[i]      * (shift+y1),
			     halfheight + sinw.f[i]      * (shift+y1),
			     current_effect->spectral_color);
			_inf_line(priv, halfwidth  - cosw.f[i-step] * (shift+old_y2),
			     halfheight + sinw.f[i-step] * (shift+old_y2),
			     halfwidth  - cosw.f[i]      * (shift+y2),
			     halfheight + sinw.f[i]      * (shift+y2),
			     current_effect->spectral_color);
			break;
		}
	}

	if (current_effect->mode_spectre==3 || current_effect->mode_spectre==4)
		_inf_line(priv, halfwidth  + cosw.f[priv->plugwidth - step] * (shift+y1),
		     halfheight + sinw.f[priv->plugwidth - step] * (shift+y1),
		     halfwidth  - cosw.f[priv->plugwidth - step] * (shift+y2),
		     halfheight + sinw.f[priv->plugwidth - step] * (shift+y2),
		     current_effect->spectral_color);
}

void _inf_curve(InfinitePrivate *priv, t_effect* current_effect)
{
	int i,j,k;
	float v,vr;
	float x,y;
	float amplitude=(float)current_effect->curve_amplitude/256;

	for (j=0;j<2;j++) {
		v=80;
		vr=0.001;
		k=current_effect->x_curve;
		for (i=0;i<64;i++) {
			x=cos((float)(k)/(v+v*j*1.34))*priv->plugheight*amplitude;
			y=sin((float)(k)/(1.756*(v+v*j*0.93)))*priv->plugheight*amplitude;
			_inf_plot2(priv, x*cos((float)k*vr)+y*sin((float)k*vr)+priv->plugwidth/2,
			      x*sin((float)k*vr)-y*cos((float)k*vr)+priv->plugheight/2,
			      current_effect->curve_color);
			k++;
		}
	}
	current_effect->x_curve=k;
}

void _inf_init_display(InfinitePrivate *priv)
{
	int allocsize;
	priv->plugwidth = priv->plugwidth;
	priv->plugheight = priv->plugheight;

	allocsize = priv->plugwidth * priv->plugheight;

	priv->surface1 = (uint8_t *) visual_mem_malloc0(allocsize);
	priv->surface2 = (uint8_t *) visual_mem_malloc0(allocsize);
}

