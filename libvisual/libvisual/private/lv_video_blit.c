#include "config.h"
#include "lv_video_blit.h"
#include "lv_common.h"
#include "lv_cpu.h"

#pragma pack(1)

typedef struct {
#if VISUAL_LITTLE_ENDIAN == 1
	uint16_t b:5, g:6, r:5;
#else
	uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

int blit_overlay_noalpha (VisVideo *dest, VisVideo *src)
{
	int y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);

	/* src and dest are completely equal, do one big mem copy instead of a per line mem copy.
	 * Also check if the pitch is equal to it's width * bpp, this is because of subregions. */
	if (visual_video_compare_attrs (dest, src) && (src->pitch == (src->width * src->bpp))) {
		visual_mem_copy (destbuf, srcbuf, visual_video_get_size (dest));

		return VISUAL_OK;
	}

	for (y = 0; y < src->height; y++) {
		visual_mem_copy (destbuf, srcbuf, src->width * src->bpp);

		destbuf += dest->pitch;
		srcbuf += src->pitch;
	}

	return VISUAL_OK;
}

int blit_overlay_alphasrc (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf  = visual_video_get_pixels (src);
	uint8_t alpha;

	if (visual_cpu_has_mmx ())
		return _lv_blit_overlay_alphasrc_mmx (dest, src);

	for (y = 0; y < src->height; y++) {
		for (x = 0; x < src->width; x++) {
			alpha = srcbuf[3];

			destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
			destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
			destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

			destbuf += dest->bpp;
			srcbuf  += src->bpp;
		}

		destbuf += dest->pitch - (dest->width * dest->bpp);
		srcbuf  += src->pitch  - (src->width  * src->bpp);
	}

	return VISUAL_OK;
}

int blit_overlay_colorkey (VisVideo *dest, VisVideo *src)
{
	unsigned int i;
	unsigned int pixel_count = dest->width * dest->height;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		uint8_t *destbuf = visual_video_get_pixels (dest);
		uint8_t *srcbuf = visual_video_get_pixels (src);
		VisPalette *pal = src->pal;

		if (pal == NULL) {
			blit_overlay_noalpha (dest, src);

			return VISUAL_OK;
		}

		int index = visual_palette_find_color (pal, src->colorkey);

		for (i = 0; i < pixel_count; i++) {
			if (*srcbuf != index)
				*destbuf = *srcbuf;

			destbuf++;
			srcbuf++;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		uint16_t *destbuf = visual_video_get_pixels (dest);
		uint16_t *srcbuf = visual_video_get_pixels (src);
		uint16_t color = visual_color_to_uint16 (src->colorkey);

		for (i = 0; i < pixel_count; i++) {
			if (color != *srcbuf)
				*destbuf = *srcbuf;

			destbuf++;
			srcbuf++;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
		uint8_t *destbuf = visual_video_get_pixels (dest);
		uint8_t *srcbuf = visual_video_get_pixels (src);
		uint8_t r = src->colorkey->r;
		uint8_t g = src->colorkey->g;
		uint8_t b = src->colorkey->b;

		for (i = 0; i < pixel_count; i++) {
			if (b != srcbuf[0] && g != srcbuf[1] && r != srcbuf[2]) {
				destbuf[0] = srcbuf[0];
				destbuf[1] = srcbuf[1];
				destbuf[2] = srcbuf[2];
			}

			destbuf += 3;
			srcbuf  += 3;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		uint32_t *destbuf = visual_video_get_pixels (dest);
		uint32_t *srcbuf = visual_video_get_pixels (src);
		uint32_t color = visual_color_to_uint32 (src->colorkey);

		for (i = 0; i < pixel_count; i++) {
			if (color != *srcbuf)
				*destbuf = *srcbuf;

			destbuf++;
			srcbuf++;
		}
	}

	return VISUAL_OK;
}

int blit_overlay_surfacealpha (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = src->density;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf  += src->pitch  - (src->width  * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {

		for (y = 0; y < src->height; y++) {
			rgb16_t *destr = (rgb16_t *) destbuf;
			rgb16_t *srcr  = (rgb16_t *) srcbuf;

			for (x = 0; x < src->width; x++) {
				destr->r = (alpha * (srcr->r - destr->r) >> 8) + destr->r;
				destr->g = (alpha * (srcr->g - destr->g) >> 8) + destr->g;
				destr->b = (alpha * (srcr->b - destr->b) >> 8) + destr->b;

				destr++;
				srcr++;
			}

			destbuf += dest->pitch;
			srcbuf += src->pitch;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
				destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
				destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

				destbuf += dest->bpp;
				srcbuf  += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf  += src->pitch  - (src->width  * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
				destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
				destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

				destbuf += dest->bpp;
				srcbuf  += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf  += src->pitch  - (src->width  * src->bpp);
		}
	}

	return VISUAL_OK;
}

int blit_overlay_surfacealphacolorkey (VisVideo *dest, VisVideo *src)
{
	int x, y;
	uint8_t *destbuf = visual_video_get_pixels (dest);
	uint8_t *srcbuf = visual_video_get_pixels (src);
	uint8_t alpha = src->density;

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		VisPalette *pal = src->pal;

		if (pal == NULL) {
			blit_overlay_noalpha (dest, src);

			return VISUAL_OK;
		}

		int index = visual_palette_find_color (pal, src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (*srcbuf != index)
					*destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		uint16_t color = visual_color_to_uint16 (src->colorkey);

		for (y = 0; y < src->height; y++) {
			rgb16_t *destr = (rgb16_t *) destbuf;
			rgb16_t *srcr  = (rgb16_t *) srcbuf;

			for (x = 0; x < src->width; x++) {
				if (color != *((uint16_t *) srcr)) {
					destr->r = (alpha * (srcr->r - destr->r) >> 8) + destr->r;
					destr->g = (alpha * (srcr->g - destr->g) >> 8) + destr->g;
					destr->b = (alpha * (srcr->b - destr->b) >> 8) + destr->b;
				}

				destr++;
				srcr++;
			}

			destbuf += dest->pitch;
			srcbuf += src->pitch;
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
		uint8_t r = src->colorkey->r;
		uint8_t g = src->colorkey->g;
		uint8_t b = src->colorkey->b;

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (b != *srcbuf && g != *(srcbuf + 1) && r != *(srcbuf + 2)) {
					destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
					destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
					destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];
				}

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}

	} else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
		uint32_t color = visual_color_to_uint32 (src->colorkey);

		for (y = 0; y < src->height; y++) {
			for (x = 0; x < src->width; x++) {
				if (color == *((uint32_t *) destbuf)) {
					destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
					destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
					destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];
				}

				destbuf += dest->bpp;
				srcbuf += src->bpp;
			}

			destbuf += dest->pitch - (dest->width * dest->bpp);
			srcbuf += src->pitch - (src->width * src->bpp);
		}
	}

	return VISUAL_OK;
}
