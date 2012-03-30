#include "lv_video_fill.h"
#include "lv_mem.h"

#pragma pack(1)

typedef struct {
	uint16_t b:5, g:6, r:5;
} rgb16_t;

#pragma pack()


/* Color fill functions */
void visual_video_fill_color_index8 (VisVideo *video, VisColor *color)
{
	int y;
	uint8_t *buf = visual_video_get_pixels (video);
	int8_t col = ((color->r + color->g + color->b) / 3);

	for (y = 0; y < video->height; y++) {
		visual_mem_set (buf, col, video->width);

		buf += video->pitch;
	}
}

void visual_video_fill_color_rgb16 (VisVideo *video, VisColor *color)
{
	int y;
	uint16_t *buf = visual_video_get_pixels (video);
	int16_t col;
	rgb16_t *col16 = (rgb16_t *) &col;
	col16->r = color->r >> 3;
	col16->g = color->g >> 2;
	col16->b = color->b >> 3;

	for (y = 0; y < video->height; y++) {
		visual_mem_set16 (buf, col, video->width);

		buf += (video->pitch / video->bpp);
	}
}

void visual_video_fill_color_rgb24 (VisVideo *video, VisColor *color)
{
	int x, y;
	uint32_t *buf;
	uint8_t *rbuf = visual_video_get_pixels (video);
	uint8_t *buf8;

	int32_t cola =
		(color->b << 24) |
		(color->g << 16) |
		(color->r << 8) |
		(color->b);
	int32_t colb =
		(color->g << 24) |
		(color->r << 16) |
		(color->b << 8) |
		(color->g);
	int32_t colc =
		(color->r << 24) |
		(color->b << 16) |
		(color->g << 8) |
		(color->r);

	for (y = 0; y < video->height; y++) {
		buf = (uint32_t *) rbuf;

		for (x = video->width; x >= video->bpp; x -= video->bpp) {
			*(buf++) = cola;
			*(buf++) = colb;
			*(buf++) = colc;
		}

		buf8 = (uint8_t *) buf;
		*(buf8++) = color->b;
		*(buf8++) = color->g;
		*(buf8++) = color->r;


		rbuf += video->pitch;
	}
}

void visual_video_fill_color_argb32 (VisVideo *video, VisColor *color)
{
	int y;
	uint32_t *buf = visual_video_get_pixels (video);
	uint32_t col =
		(color->r << 16) |
		(color->g << 8) |
		(color->b);

	for (y = 0; y < video->height; y++) {
		visual_mem_set32 (buf, col, video->width);

		buf += (video->pitch / video->bpp);
	}
}
