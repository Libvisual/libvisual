#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <lvconfig.h>
#include "lv_common.h"
#include "lv_video.h"
#include "lv_log.h"

typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

/* Depth conversions */
static int depth_transform_8_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_8_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_8_to_32_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);

static int depth_transform_16_to_8_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_16_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_16_to_32_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);

static int depth_transform_24_to_8_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_24_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_24_to_32_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);

static int depth_transform_32_to_8_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_32_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);
static int depth_transform_32_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal);

/**
 * @defgroup VisVideo VisVideo
 * @{
 */

/**
 * Creates a new VisVideo structure.
 *
 * @return A newly allocated VisVideo.
 */
VisVideo *visual_video_new ()
{
	VisVideo *video;

	video = malloc (sizeof (VisVideo));
	memset (video, 0, sizeof (VisVideo));

	return video;
}

/**
 * Creates a new VisVideo and also allocates a screenbuffer.
 *
 * @param width The width for the new buffer.
 * @param height The height for the new buffer.
 * @param depth The depth being used.
 *
 * @return A newly allocates VisVideo with a buffer allocated.
 */
VisVideo *visual_video_new_with_buffer (int width, int height, VisVideoDepth depth)
{
	VisVideo *video;
	
	video = visual_video_new ();

	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, width, height);

	visual_video_allocate_buffer (video);

	return video;
}

/**
 * Frees the VisVideo. This frees the VisVideo data structure.
 *
 * @param video Pointer to a VisVideo that needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_free (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -1);
	
	free (video);

	return 0;
}

/**
 * Frees the VisVideo and it's buffer. This frees the VisVideo and it's screenbuffer.
 *
 * @param video Pointer to a VisVideo that needs to be freed together with
 * 	it's screenbuffer.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_free_with_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -1);

	visual_video_free_buffer (video);

	free (video);

	return 0;
}

/**
 * Frees the screenbuffer that relates to the VisVideo.
 *
 * @param video Pointer to a VisVideo of which the screenbuffer needs to be freed.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_free_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -1);
	visual_log_return_val_if_fail (video->screenbuffer != NULL, -1);

	/*
	 * We doesn't know if we must freed!
	 *
	free (video->screenbuffer);*/

	return 0;
}

/**
 * Allocates a screenbuffer for the VisVideo. Allocates based on the
 * VisVideo it's information about the screen dimension and depth.
 *
 * @param video Pointer to a VisVideo that needs an allocated screenbuffer.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_allocate_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -1);

/*	printf ("[video-allocate-buffer] Allocating buffer with size: %d, width height bpp pitch %d %d %d %d calc %d\n", video->size,
			video->width, video->height, video->bpp, video->pitch, video->pitch * video->height);
*/
	video->screenbuffer = malloc (video->size);
	memset (video->screenbuffer, 0, video->size);

	return 0;
}

/**
 * Clones the information from a VisVideo to another.
 * This will clone the depth, dimension and screen pitch into another VisVideo.
 * It doesn't clone the palette or screenbuffer.
 *
 * @param dest Pointer to a destination VisVideo in which the information needs to
 * 	be placed.
 * @param src Pointer to a source VisVideo from which the information needs to
 * 	be obtained.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_clone (VisVideo *dest, VisVideo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -1);
	visual_log_return_val_if_fail (src != NULL, -1);

	visual_video_set_depth (dest, src->depth);
	visual_video_set_dimension (dest, src->width, src->height);
	visual_video_set_pitch (dest, src->pitch);

	return 0;
}

/**
 * Sets a palette to a VisVideo. Links a VisPalette to the
 * VisVideo.
 *
 * @param video Pointer to a VisVideo to which a VisPalette needs to be linked.
 * @param pal Pointer to a Vispalette that needs to be linked with the VisVideo.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_set_palette (VisVideo *video, VisPalette *pal)
{
	visual_log_return_val_if_fail (video != NULL, -1);

	video->pal = pal;

	return 0;
}

/**
 * Sets a screenbuffer to a VisVideo. Links a sreenbuffer to the
 * VisVideo.
 *
 * @param video Pointer to a VisVideo to which a screenbuffer needs to be linked.
 * @param buffer Pointer to a screenbuffer that needs to be linked with the VisVideo.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_set_buffer (VisVideo *video, void *buffer)
{
	visual_log_return_val_if_fail (video != NULL, -1);

	video->screenbuffer = buffer;

	return 0;
}

/**
 * Sets the dimension for a VisVideo. Used to set the dimension for a
 * surface.
 *
 * @param video Pointer to a VisVideo to which the dimension is set.
 * @param width The width of the surface.
 * @param height The height of the surface.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_set_dimension (VisVideo *video, int width, int height)
{
	visual_log_return_val_if_fail (video != NULL, -1);

	video->width = width;
	video->height = height;

	video->pitch = video->width * video->bpp;
	video->size = video->pitch * video->height;
	
	return 0;
}

/**
 * Sets the pitch for a VisVideo. Used to set the screen
 * pitch for a surface. If the pitch doesn't differ from the
 * screen width * bpp you only need to call the
 * visual_video_set_dimension method.
 *
 * @param video Pointer to a VisVideo to which the pitch is set.
 * @param pitch The screen pitch in bytes per line.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_set_pitch (VisVideo *video, int pitch)
{
	visual_log_return_val_if_fail (video != NULL, -1);

	if (video->bpp <= 0)
		return -1;

	video->pitch = pitch;
	video->size = video->pitch * video->height;

	return 0;
}

/**
 * Sets the depth for a VisVideo. Used to set the depth for
 * a surface. This will also define the number of bytes per pixel.
 *
 * @param video Pointer to a VisVideo to which the depth is set.
 * @param depth The depth choosen from the VisVideoDepth enumerate.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_set_depth (VisVideo *video, VisVideoDepth depth)
{
	visual_log_return_val_if_fail (video != NULL, -1);

	video->depth = depth;
	video->bpp = visual_video_bpp_from_depth (video->depth);

	return 0;
}

/**
 * Checks if a certain depth is supported by checking against an ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth that we want to test.
 *
 * @return 1 when supported, 0 when unsupported and -1 on error.
 */
int visual_video_depth_is_supported (int depthflag, VisVideoDepth depth)
{
	if (visual_video_depth_is_sane (depth) == 0)
		return -1;

	if ((depth & depthflag) > 0)
		return 1;

	return 0;
}

/**
 * Get the next depth from the ORred depthflag. By giving a depth and a depthflag
 * this returns the next supported depth checked from the depthflag.
 *
 * @see visual_video_depth_get_prev
 * 
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth of which we want the next supported depth.
 *
 * @return The next supported depth or VISUAL_VIDEO_DEPTH_ERROR on error.
 */
VisVideoDepth visual_video_depth_get_next (int depthflag, VisVideoDepth depth)
{
	int i = depth;
	
        if (visual_video_depth_is_sane (depth) == 0)
		                return VISUAL_VIDEO_DEPTH_ERROR;

	if (i == VISUAL_VIDEO_DEPTH_NONE) {
		i = VISUAL_VIDEO_DEPTH_8BIT;

		if ((i & depthflag) > 0)
			return i;
	}

	while (i < VISUAL_VIDEO_DEPTH_GL) {
		i *= 2;

		if ((i & depthflag) > 0)
			return i;
	}

	return depth;
}

/**
 * Get the previous depth from the ORred depthflag. By giving a depth and a depthflag
 * this returns the previous supported depth checked from the depthflag.
 *
 * @see visual_video_depth_get_next
 * 
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth of which we want the previous supported depth.
 *
 * @return The previous supported depth or VISUAL_VIDEO_DEPTH_ERROR on error.
 */
VisVideoDepth visual_video_depth_get_prev (int depthflag, VisVideoDepth depth)
{
	int i = depth;

	if (visual_video_depth_is_sane (depth) == 0)
		return -1;

	if (i == VISUAL_VIDEO_DEPTH_NONE)
		return VISUAL_VIDEO_DEPTH_NONE;

	while (i > VISUAL_VIDEO_DEPTH_NONE) {
		i >>= 1;

		if ((i & depthflag) > 0)
			return i;
	}

	return depth;
}

/**
 * Return the lowest supported graphical depth from the ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 * 
 * @return The lowest supported depth or VISUAL_VIDEO_DEPTH_ERROR on error.
 */
VisVideoDepth visual_video_depth_get_lowest (int depthflag)
{
	return visual_video_depth_get_next (depthflag, VISUAL_VIDEO_DEPTH_NONE);
}

/**
 * Return the highest supported graphical depth from the ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The highest supported depth or VISUAL_VIDEO_DEPTH_ERROR on error.
 */
VisVideoDepth visual_video_depth_get_highest (int depthflag)
{
	VisVideoDepth highest = VISUAL_VIDEO_DEPTH_NONE;
	VisVideoDepth i = 0;
	int firstentry = TRUE;

	while (highest != i || firstentry == TRUE) {
		highest = i;

		i = visual_video_depth_get_next (depthflag, i);

		firstentry = FALSE;
	}

	return highest;
}

/**
 * Return the highest supported depth that is NOT openGL.
 *
 * @param depthflag The ORred depthflag that we check against.
 *
 * @return The highest supported depth that is not openGL or
 * VISUAL_VIDEO_DEPTH_ERROR on error.
 */
VisVideoDepth visual_video_depth_get_highest_nogl (int depthflag)
{
	VisVideoDepth depth;

	depth = visual_video_depth_get_highest (depthflag);

	/* Get previous depth if the highest is openGL */
	if (depth == VISUAL_VIDEO_DEPTH_GL) {
		depth = visual_video_depth_get_prev (depthflag, depth);

		/* Is it still on openGL ? Return an error */
		if (depth == VISUAL_VIDEO_DEPTH_GL)
			return VISUAL_VIDEO_DEPTH_ERROR;

	} else
		return depth;

	return VISUAL_VIDEO_DEPTH_ERROR;
}

/**
 * Checks if a certain value is a sane depth.
 *
 * @param depth Depth to be checked if it's sane.
 *
 * @return 1 if the depth is sane, 0 if the depth is not sane.
 */
int visual_video_depth_is_sane (VisVideoDepth depth)
{
	int count = 0;
	int i = 1;

	if (depth == VISUAL_VIDEO_DEPTH_NONE)
		return 1;

	if (depth >= VISUAL_VIDEO_DEPTH_ENDLIST)
		return 0;
	
	while (i < VISUAL_VIDEO_DEPTH_ENDLIST) {
		if ((i & depth) > 0)
			count++;

		if (count > 1)
			return 0;

		i <<= 1;
	}

	return 1;
}

/**
 * Returns the number of bits per pixel from a VisVideoDepth enumerate value.
 *
 * @param depth The VisVideodepth enumerate value from which the bits per pixel
 * 	needs to be returned.
 *
 * @return The bits per pixel or -1 on error.
 */
int visual_video_depth_value_from_enum (VisVideoDepth depth)
{
	switch (depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			return 8;

		case VISUAL_VIDEO_DEPTH_16BIT:
			return 16;

		case VISUAL_VIDEO_DEPTH_24BIT:
			return 24;

		case VISUAL_VIDEO_DEPTH_32BIT:
			return 32;

		default:
			return -1;
	}

	return -1;
}

/**
 * Returns a VisVideoDepth enumerate value from bits per pixel.
 *
 * @param depthvalue Integer containing the number of bits per pixel.
 *
 * @return The corespondending enumerate value or VISUAL_VIDEO_DEPTH_ERROR.
 */
VisVideoDepth visual_video_depth_enum_from_value (int depthvalue)
{
	switch (depthvalue) {
		case 8:
			return VISUAL_VIDEO_DEPTH_8BIT;

		case 16:
			return VISUAL_VIDEO_DEPTH_16BIT;

		case 24:
			return VISUAL_VIDEO_DEPTH_24BIT;

		case 32:
			return VISUAL_VIDEO_DEPTH_32BIT;

		default:
			return VISUAL_VIDEO_DEPTH_ERROR;

	}

	return -1;
}

/**
 * Returns the number of bytes per pixel from the VisVideoDepth enumerate.
 *
 * @param depth The VisVideodepth enumerate value from which the bytes per pixel
 * 	needs to be returned.
 *
 * @return The number of bytes per pixel or -1 on error.
 */
int visual_video_bpp_from_depth (VisVideoDepth depth)
{
	switch (depth) {
		case VISUAL_VIDEO_DEPTH_8BIT:
			return 1;
		
		case VISUAL_VIDEO_DEPTH_16BIT:
			return 2;

		case VISUAL_VIDEO_DEPTH_24BIT:
			return 3;

		case VISUAL_VIDEO_DEPTH_32BIT:
			return 4;

		case VISUAL_VIDEO_DEPTH_GL:
			return 0;

		default:
			return -1;
	}

	return -1;
}

/**
 * This function blits a VisVideo into another VisVideo. Placement can be done and there
 * is support for the alpha channel.
 *
 * @param dest Pointer to the destination VisVideo in which the source is overlayed.
 * @param src Pointer to the source VisVideo which is overlayed in the destination.
 * @param x Horizontal placement offset.
 * @param y Vertical placement offset.
 * @param alpha Sets if we want to check the alpha channel. Use FALSE or TRUE here/
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_blit_overlay (VisVideo *dest, VisVideo *src, int x, int y, int alpha)
{
	VisVideo *transform = NULL, *srcp = NULL;
	VisPalette temppal;
	int height, wrange, hrange, amount;
	int xa, ya;
	int xbpp;
	uint8_t *destbuf;
	uint8_t *srcpbuf;
	uint32_t *srcbuf;

	/* We can't overlay GL surfaces so don't even try */
	visual_log_return_val_if_fail (dest->depth != VISUAL_VIDEO_DEPTH_GL ||
			src->depth != VISUAL_VIDEO_DEPTH_GL, -1);
	
	/* Get the smallest size from both the VisVideos */
	wrange = dest->width > src->width ? src->width : dest->width;
	hrange = dest->height > src->height ? src->height : dest->height;
	
	visual_log_return_val_if_fail (x < dest->width, -1);
	visual_log_return_val_if_fail (y < dest->height, -1);

	/** @todo Add support for negative x, y values */
	
	/* We're not the same depth, converting */
	if (dest->depth != src->depth) {
		transform = visual_video_new ();

		visual_video_set_depth (transform, dest->depth);
		visual_video_set_dimension (transform, src->width, src->height);

		visual_video_allocate_buffer (transform);

		visual_video_set_palette (src, &temppal);

		visual_video_depth_transform (transform, src);
	}
	
	if (transform != NULL)
		srcp = transform;
	else
		srcp = src;

	destbuf = dest->screenbuffer;
	srcbuf = src->screenbuffer;
	srcpbuf = srcp->screenbuffer;

	/* No alpha, fast method */
	if (alpha == FALSE || src->depth != VISUAL_VIDEO_DEPTH_32BIT) {
		/* Blit it to the dest video */
		for (height = y; height < hrange + y; height++) {

			/* We've reached the end */
			if (height > dest->height - 1)
				break;

			if ((x * dest->bpp) + srcp->pitch > dest->pitch)
				amount = dest->pitch - (x * dest->bpp);
			else
				amount = wrange * dest->bpp;

			memcpy (destbuf + (height * dest->pitch) + (x * dest->bpp),
					srcpbuf + ((height - y) * srcp->pitch),
					amount);
		}
	} else {
		int aindex = 0;
		int si = 0;
		int di = (y * dest->pitch) + (x * dest->bpp);
		
		xbpp = x * dest->bpp;

		/* Blit it to the dest video */
		for (ya = 0; ya < hrange; ya++) {
	
			if (ya + y > dest->height - 1)
				break;
			
			if ((x * dest->bpp) + srcp->pitch > dest->pitch)
				amount = (dest->pitch / dest->bpp) - x;
			else
				amount = wrange;

			for (xa = 0; xa < amount; xa++) {
				uint8_t alpha;
				int bppl;
				
				alpha = srcbuf[aindex++] >> 24;
				
				for (bppl = 0; bppl < dest->bpp; bppl++) {
					destbuf[di] =
						(alpha * (srcpbuf[si] -  destbuf[di]) / 255 + destbuf[di]);

					si++;
					di++;
				}
			}

			aindex += (src->pitch / src->bpp) - amount;
			si += srcp->pitch - (amount * srcp->bpp);
			di += dest->pitch - (amount * dest->bpp);
		}
	}

	if (transform != NULL)
		visual_video_free_with_buffer (transform);
	
	return 0;
}

/**
 * Sets a certain color as the alpha channel and the density for the non alpha channel
 * colors. This function can be only used on VISUAL_VIDEO_DEPTH_32BIT surfaces.
 *
 * @param video Pointer to the VisVideo in which the alpha channel is made.
 * @param r The red value for the alpha channel color.
 * @param g The green value for the alpha channel color.
 * @param b The blue value for the alpha channel color.
 * @param density The alpha density for the other colors.
 * 
 * @return 0 on succes -1 on error.
 */
int visual_video_alpha_color (VisVideo *video, uint8_t r, uint8_t g, uint8_t b, uint8_t density)
{
	int col = 0;
	int i;
	uint32_t *vidbuf;

	visual_log_return_val_if_fail (video != NULL, -1);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -1);

	col = (r << 16 | g << 8 | b);

	vidbuf = video->screenbuffer;

	for (i = 0; i < video->size / video->bpp; i++) {
		if ((vidbuf[i] & 0x00ffffff) == col)
			vidbuf[i] = col;
		else
			vidbuf[i] += (density << 24);
	}

	return 0;
}

/**
 * Sets a certain alpha value for the complete buffer in the VisVideo. This function
 * can be only used on VISUAL_VIDEO_DEPTH_32BIT surfaces.
 *
 * @param video Pointer to the VisVideo in which the alpha channel density is set.
 * @param density The alpha density that is to be set.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_alpha_fill (VisVideo *video, uint8_t density)
{
	int i;
	uint32_t *vidbuf;

	visual_log_return_val_if_fail (video != NULL, -1);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -1);

	vidbuf = video->screenbuffer;

	for (i = 0; i < video->size / video->bpp; i++)
		vidbuf[i] += (density << 24);

	return 0;
}

/**
 * Video depth transforms one VisVideo into another using the depth information
 * stored within the VisVideos. The dimension should be equal however the pitch
 * value of the destination may be set.
 *
 * @param viddest Pointer to the destination VisVideo to which the source
 * 	VisVideo is transformed.
 * @param vidsrc Pointer to the source VisVideo.
 *
 * @return 0 on succes -1 on error.
 */
int visual_video_depth_transform (VisVideo *viddest, VisVideo *vidsrc)
{
	return visual_video_depth_transform_to_buffer (viddest->screenbuffer,
			vidsrc, vidsrc->pal, viddest->depth, viddest->pitch);
}

/**
 * Less abstract video depth transform used by visual_video_depth_transform.
 *
 * @see visual_video_depth_transform
 *
 * @param dest Destination screenbuffer.
 * @param video Source VisVideo.
 * @param pal Pointer to a VisPalette that can be set by full color to indexed color transforms.
 * @param destdepth The destination depth.
 * @param pitch The destination number of bytes per line.
 *
 * return 0 on succes -1 on error.
 */
int visual_video_depth_transform_to_buffer (uint8_t *dest, VisVideo *video,
		VisPalette *pal, VisVideoDepth destdepth, int pitch)
{
	uint8_t *srcbuf = video->screenbuffer;
	int width = video->width;
	int height = video->height;

	visual_log_return_val_if_fail (video != NULL, -1);
	
	if (destdepth == VISUAL_VIDEO_DEPTH_8BIT || video->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		visual_log_return_val_if_fail (pal != NULL, -1);
	}

	/* Destdepth is equal to sourcedepth case */
	if (video->depth == destdepth) {
		memcpy (dest, video->screenbuffer, video->width * video->height * video->bpp);

		return 0;
	}

	if (video->depth == VISUAL_VIDEO_DEPTH_8BIT) {

		if (destdepth == VISUAL_VIDEO_DEPTH_16BIT)
			return depth_transform_8_to_16_c (dest, srcbuf, width, height, pitch, pal);

		if (destdepth == VISUAL_VIDEO_DEPTH_24BIT)
			return depth_transform_8_to_24_c (dest, srcbuf, width, height, pitch, pal);

		if (destdepth == VISUAL_VIDEO_DEPTH_32BIT)
			return depth_transform_8_to_32_c (dest, srcbuf, width, height, pitch, pal);

	} else if (video->depth == VISUAL_VIDEO_DEPTH_16BIT) {
		
		if (destdepth == VISUAL_VIDEO_DEPTH_8BIT)
			return depth_transform_16_to_8_c (dest, srcbuf, width, height, pitch, pal);

		if (destdepth == VISUAL_VIDEO_DEPTH_24BIT)
			return depth_transform_16_to_24_c (dest, srcbuf, width, height, pitch, NULL);

		if (destdepth == VISUAL_VIDEO_DEPTH_32BIT)
			return depth_transform_16_to_32_c (dest, srcbuf, width, height, pitch, NULL);
	
	} else if (video->depth == VISUAL_VIDEO_DEPTH_24BIT) {

		if (destdepth == VISUAL_VIDEO_DEPTH_8BIT)
			return depth_transform_24_to_8_c (dest, srcbuf, width, height, pitch, pal);

		if (destdepth == VISUAL_VIDEO_DEPTH_16BIT)
			return depth_transform_24_to_16_c (dest, srcbuf, width, height, pitch, NULL);

		if (destdepth == VISUAL_VIDEO_DEPTH_32BIT)
			return depth_transform_24_to_32_c (dest, srcbuf, width, height, pitch, NULL);

	} else if (video->depth == VISUAL_VIDEO_DEPTH_32BIT) {

		if (destdepth == VISUAL_VIDEO_DEPTH_8BIT)
			return depth_transform_32_to_8_c (dest, srcbuf, width, height, pitch, pal);

		if (destdepth == VISUAL_VIDEO_DEPTH_16BIT)
			return depth_transform_32_to_16_c (dest, srcbuf, width, height, pitch, NULL);

		if (destdepth == VISUAL_VIDEO_DEPTH_24BIT)
			return depth_transform_32_to_24_c (dest, srcbuf, width, height, pitch, NULL);
	}

	return -1;
}

/**
 * @}
 */

/* Depth transform C code */
/* FIXME TODO depths:	c	sse	mmx	altivec
 * 8 - 16		x
 * 8 - 24		x
 * 8 - 32		x
 * 16 - 8		x
 * 16 - 24		x
 * 16 - 32		x
 * 24 - 8		x
 * 24 - 16		x
 * 24 - 32		x
 * 32 - 8		x
 * 32 - 24		x
 * 32 - 16		x
 */

static int depth_transform_8_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *destr = (_color16 *) dest;
	int pitchdiff = (pitch - (width * 2)) >> 1;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			destr[i].r = pal->r[src[j]] >> 3;
			destr[i].g = pal->g[src[j]] >> 2;
			destr[i].b = pal->b[src[j]] >> 3;
			i++;
			j++;
		}

		i += pitchdiff;
	}
	
	return 0;
}

static int depth_transform_8_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	int pitchdiff = pitch - (width * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dest[i++] = pal->r[src[j]];
			dest[i++] = pal->g[src[j]];
			dest[i++] = pal->b[src[j]];
			j++;
		}

		i += pitchdiff;
	}

	return 0;
}

static int depth_transform_8_to_32_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	uint32_t *destr = (uint32_t *) dest;
	uint32_t col;
	int pitchdiff = (pitch - (width * 4)) >> 2;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			col = 0;
			col += pal->r[src[j]] << 16;
			col += pal->g[src[j]] << 8;
			col += pal->b[src[j]];
			j++;

			destr[i++] = col;
		}

		i += pitchdiff;
	}

	return 0;
}

static int depth_transform_16_to_8_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *srcr = (_color16 *) src;
	uint8_t r, g, b;
	uint8_t col;
	int pitchdiff = pitch - width;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			r = srcr[j].r << 3;
			g = srcr[j].g << 2;
			b = srcr[j].b << 3;
			j++;

			col  = (r >> 5);
			col |= (g >> 6) << 3;
			col |= (b >> 5) << 5;

			pal->r[col] = r;
			pal->g[col] = g;
			pal->b[col] = b;

			dest[i++] = col;
		}

		i += pitchdiff;	
	}

	return 0;
}

static int depth_transform_16_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *srcr = (_color16 *) src;
	int pitchdiff = pitch - (width * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dest[j++] = srcr[i].r << 3;
			dest[j++] = srcr[i].g << 2;
			dest[j++] = srcr[i].b << 3;
			i++;	
		}

		j += pitchdiff;
	}

	return 0;
}

static int depth_transform_16_to_32_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *srcr = (_color16 *) src;
	int pitchdiff = pitch - (width * 4);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dest[j++] = srcr[i].r << 3;
			dest[j++] = srcr[i].g << 2;
			dest[j++] = srcr[i].b << 3;
			dest[j++] = 0;
			i++;
		}
	
		j += pitchdiff;
	}
	
	return 0;
}

static int depth_transform_24_to_8_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	uint8_t r, g, b;
	uint8_t col;
	int pitchdiff = pitch - width;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			r = src[j++];
			g = src[j++];
			b = src[j++];

			col  = (r >> 5);
			col |= (g >> 6) << 3;
			col |= (b >> 5) << 5;

			pal->r[col] = r;
			pal->g[col] = g;
			pal->b[col] = b;

			dest[i++] = col;
		}

		i += pitchdiff;	
	}

	return 0;
}

static int depth_transform_24_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *destr = (_color16 *) dest;
	int pitchdiff = (pitch - (width * 2)) >> 1;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			destr[i].r = src[j++] >> 3;
			destr[i].g = src[j++] >> 2;
			destr[i].b = src[j++] >> 3;
			i++;
		}

		i += pitchdiff;
	}
	
	return 0;
}

static int depth_transform_24_to_32_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	int pitchdiff = pitch - (width * 4);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dest[j++] = src[i++];
			dest[j++] = src[i++];
			dest[j++] = src[i++];
			dest[j++] = 0;
		}

		j += pitchdiff;
	}
	
	return 0;
}

static int depth_transform_32_to_8_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	uint8_t r, g, b;
	uint8_t col;
	int pitchdiff = pitch - width;

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			r = src[j++];
			g = src[j++];
			b = src[j++];
			j++;

			col  = (r >> 5);
			col |= (g >> 6) << 3;
			col |= (b >> 5) << 5;

			pal->r[col] = r;
			pal->g[col] = g;
			pal->b[col] = b;

			dest[i++] = col;
		}

		i += pitchdiff;	
	}
	
	return 0;
}

static int depth_transform_32_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *destr = (_color16 *) dest;
	int pitchdiff = (pitch - (width * 2)) >> 1;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			destr[i].r = src[j++] >> 3;
			destr[i].g = src[j++] >> 2;
			destr[i].b = src[j++] >> 3;
			j++;
			i++;
		}

		i += pitchdiff;
	}

	return 0;
}

static int depth_transform_32_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	int pitchdiff = pitch - (width * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dest[i++] = src[j++];
			dest[i++] = src[j++];
			dest[i++] = src[j++];
			j++;
		}
		
		i += pitchdiff;
	}
	
	return 0;
}

