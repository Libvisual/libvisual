#include "lv_video_scale.h"
#include "lv_common.h"

#pragma pack(1)

/* NOTE: Although the following struct members are named 'r', 'g' and
   'b', the scale functions are very much pixel format agnostic. They
   only assume the number of components. */

typedef struct {
	uint16_t r:5, g:6, b:5;
} color16_t;

typedef struct {
	uint8_t r, g, b;
} color24_t;

#pragma pack()


void visual_video_zoom_color8 (VisVideo *dest, VisVideo *src)
{
	uint8_t *dbuf = visual_video_get_pixels (dest);
	uint8_t *sbuf = visual_video_get_pixels (src);
	int x, y;

	for (y = 0; y < src->height; y++) {
		for (x = 0; x < src->width; x++) {
			*(dbuf++) = *sbuf;
			*(dbuf++) = *sbuf;

			sbuf++;
		}

		sbuf += src->pitch - (src->width * src->bpp);
		dbuf += dest->pitch - (dest->width * dest->bpp);
	}
}

void visual_video_zoom_color16 (VisVideo *dest, VisVideo *src)
{
	uint16_t *dbuf = visual_video_get_pixels (dest);
	uint16_t *sbuf = visual_video_get_pixels (src);
	int x, y;

	for (y = 0; y < src->height; y++) {
		for (x = 0; x < src->width; x++) {
			*(dbuf++) = *sbuf;
			*(dbuf++) = *sbuf;

			sbuf++;
		}

		sbuf += src->pitch - (src->width * src->bpp);
		dbuf += dest->pitch - (dest->width * dest->bpp);
	}
}

void visual_video_zoom_color24 (VisVideo *dest, VisVideo *src)
{
}

void visual_video_zoom_color32 (VisVideo *dest, VisVideo *src)
{
	uint32_t *sbuf = visual_video_get_pixels (src);
	uint32_t *dbuf = visual_video_get_pixels (dest);
	int x, y;

	const int spdiff = src->pitch - src->width*src->bpp;
	for (y = 0; y < src->height; y++) {
		dbuf = dest->pixel_rows[y << 1];
		for (x = 0; x < src->width; x++) {
			*(dbuf + dest->width) = *sbuf;
			*(dbuf++) = *sbuf;
			*(dbuf + dest->width) = *sbuf;
			*(dbuf++) = *sbuf;

			sbuf++;
		}

		sbuf += spdiff;
	}
}

void visual_video_scale_nearest_color8 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint8_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint8_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += dest->pitch - dest->width;
	}
}

void visual_video_scale_nearest_color16 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint16_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint16_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += (dest->pitch / dest->bpp) - dest->width;
	}
}

/* FIXME this version is of course butt ugly */
/* IF color24_t is allowed use it here as well */
void visual_video_scale_nearest_color24 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	color24_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (color24_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += (dest->pitch / dest->bpp) - dest->width;
	}
}

void visual_video_scale_nearest_color32 (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint32_t *dest_pixel, *src_pixel_row;

	du = (src->width << 16) / dest->width;
	dv = (src->height << 16) / dest->height;
	v = 0;

	dest_pixel = visual_video_get_pixels (dest);

	for (y = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint32_t *) src->pixel_rows[v >> 16];

		if (v >> 16 >= src->height)
			v -= 0x10000;

		u = 0;
		for (x = 0; x < dest->width; x++, u += du)
			*dest_pixel++ = src_pixel_row[u >> 16];

		dest_pixel += (dest->pitch / dest->bpp) - dest->width;
	}
}

void visual_video_scale_bilinear_color8 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint8_t *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;

	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (uint8_t *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (uint8_t *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0;

		for (x = dest->width - 1; x--; u += du) {
			uint8_t cul, cll, cur, clr;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b0; /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul = src_pixel_rowu[u >> 16];
			cll = src_pixel_rowl[u >> 16];
			cur = src_pixel_rowu[(u >> 16) + 1];
			clr = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul;
			b0 += ll * cll;
			b0 += ur * cur;
			b0 += lr * clr;

			*dest_pixel++ = b0 >> 16;
		}

		dest_pixel += dest->pitch - (dest->width - 1);

	}
}

void visual_video_scale_bilinear_color16 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	color16_t *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;
	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (color16_t *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (color16_t *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0.0;

		for (x = dest->width - 1; x--; u += du) {
			color16_t cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[	  */
			uint32_t b2, b1, b0;	 /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul = src_pixel_rowu[u >> 16];
			cll = src_pixel_rowl[u >> 16];
			cur = src_pixel_rowu[(u >> 16) + 1];
			clr = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul.r;
			b1 = ul * cul.g;
			b2 = ul * cul.b;

			b0 += ll * cll.r;
			b1 += ll * cll.g;
			b2 += ll * cll.b;

			b0 += ur * cur.r;
			b1 += ur * cur.g;
			b2 += ur * cur.b;

			b0 += lr * clr.r;
			b1 += lr * clr.g;
			b2 += lr * clr.b;

			b.r = b0 >> 16;
			b.g = b1 >> 16;
			b.b = b2 >> 16;

			*dest_pixel++ = b;
		}

		dest_pixel += (dest->pitch / dest->bpp) - ((dest->width - 1));
	}
}

void visual_video_scale_bilinear_color24 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	color24_t *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;
	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (color24_t *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (color24_t *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0;

		for (x = dest->width - 1; x--; u += du) {
			color24_t cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b2, b1, b0;	 /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul = src_pixel_rowu[u >> 16];
			cll = src_pixel_rowl[u >> 16];
			cur = src_pixel_rowu[(u >> 16) + 1];
			clr = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul.r;
			b1 = ul * cul.g;
			b2 = ul * cul.b;

			b0 += ll * cll.r;
			b1 += ll * cll.g;
			b2 += ll * cll.b;

			b0 += ur * cur.r;
			b1 += ur * cur.g;
			b2 += ur * cur.b;

			b0 += lr * clr.r;
			b1 += lr * clr.g;
			b2 += lr * clr.b;

			b.r = b0 >> 16;
			b.g = b1 >> 16;
			b.b = b2 >> 16;

			*dest_pixel++ = b;
		}

		dest_pixel += (dest->pitch / dest->bpp) - ((dest->width - 1));
	}
}

void visual_video_scale_bilinear_color32 (VisVideo *dest, VisVideo *src)
{
	uint32_t y;
	uint32_t u, v, du, dv; /* fixed point 16.16 */
	uint32_t *dest_pixel, *src_pixel_rowu, *src_pixel_rowl;

	dest_pixel = visual_video_get_pixels (dest);

	du = ((src->width - 1)  << 16) / dest->width;
	dv = ((src->height - 1) << 16) / dest->height;
	v = 0;

	for (y = dest->height; y--; v += dv) {
		uint32_t x;
		uint32_t fracU, fracV;     /* fixed point 24.8 [0,1[    */

		if (v >> 16 >= src->height - 1)
			v -= 0x10000;

		src_pixel_rowu = (uint32_t *) src->pixel_rows[v >> 16];
		src_pixel_rowl = (uint32_t *) src->pixel_rows[(v >> 16) + 1];

		/* fracV = frac(v) = v & 0xffff */
		/* fixed point format convertion: fracV >>= 8) */
		fracV = (v & 0xffff) >> 8;
		u = 0;

		for (x = dest->width - 1; x--; u += du) {
			union {
				uint8_t  c8[4];
				uint32_t c32;
			} cul, cll, cur, clr, b;
			uint32_t ul, ll, ur, lr; /* fixed point 16.16 [0,1[   */
			uint32_t b3, b2, b1, b0; /* fixed point 16.16 [0,255[ */

			/* fracU = frac(u) = u & 0xffff */
			/* fixed point format convertion: fracU >>= 8) */
			fracU  = (u & 0xffff) >> 8;

			/* notice 0x100 = 1.0 (fixed point 24.8) */
			ul = (0x100 - fracU) * (0x100 - fracV);
			ll = (0x100 - fracU) * fracV;
			ur = fracU * (0x100 - fracV);
			lr = fracU * fracV;

			cul.c32 = src_pixel_rowu[u >> 16];
			cll.c32 = src_pixel_rowl[u >> 16];
			cur.c32 = src_pixel_rowu[(u >> 16) + 1];
			clr.c32 = src_pixel_rowl[(u >> 16) + 1];

			b0 = ul * cul.c8[0];
			b1 = ul * cul.c8[1];
			b2 = ul * cul.c8[2];
			b3 = ul * cul.c8[3];

			b0 += ll * cll.c8[0];
			b1 += ll * cll.c8[1];
			b2 += ll * cll.c8[2];
			b3 += ll * cll.c8[3];

			b0 += ur * cur.c8[0];
			b1 += ur * cur.c8[1];
			b2 += ur * cur.c8[2];
			b3 += ur * cur.c8[3];

			b0 += lr * clr.c8[0];
			b1 += lr * clr.c8[1];
			b2 += lr * clr.c8[2];
			b3 += lr * clr.c8[3];

			b.c8[0] = b0 >> 16;
			b.c8[1] = b1 >> 16;
			b.c8[2] = b2 >> 16;
			b.c8[3] = b3 >> 16;

			*dest_pixel++ = b.c32;
		}

		dest_pixel += (dest->pitch / dest->bpp) - ((dest->width - 1));

	}
}
