#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <lvconfig.h>
#include "lv_common.h"
#include "lv_video.h"
#include "lv_log.h"
#include "lv_mem.h"

/*#define FIXED_POINT_SCALER 1*/

#define HAVE_ALLOCATED_BUFFER(video)	((video)->flags & VISUAL_VIDEO_FLAG_ALLOCATED_BUFFER)
#define HAVE_EXTERNAL_BUFFER(video)	((video)->flags & VISUAL_VIDEO_FLAG_EXTERNAL_BUFFER)

/* 32-bit fixed point settings */
#define FP_FRACTIONAL_SIZE_LOG2		(fixed32_t) 8
#define FP_FRACTIONAL_SIZE		((fixed32_t) 1 << FP_FRACTIONAL_SIZE_LOG2)
#define FP_INT(i)			((fixed32_t) (i) << FP_FRACTIONAL_SIZE_LOG2)
#define FP_TO_INT(f)			((fixed32_t) (f) >> FP_FRACTIONAL_SIZE_LOG2)
#define FP_FLOAT(f)			((float) (f) * FP_FRACTIONAL_SIZE)
#define FP_TO_FLOAT(f)			((float) (f) / FP_FRACTIONAL_SIZE)
// FIXME Use in fixed point scaler, however it crashes right now! 
#define FP_HALF(f)			((fixed32_t) (f) << (FP_FRACTIONAL_SIZE_LOG2-1))
#define FP_ROUND_TO_INT(f)		FP_TO_INT((f)+FP_HALF(f))

typedef uint32_t fixed32_t;


typedef struct {
	uint16_t b:5, g:6, r:5;
} _color16;

/* The VisVideo dtor function */
static int video_dtor (VisObject *object);

/* Precomputation functions */
static void precompute_row_table (VisVideo *video);

/* Blit overlay functions */
static int blit_overlay_noalpha (VisVideo *dest, const VisVideo *src, int x, int y);
static int blit_overlay_alpha32 (VisVideo *dest, const VisVideo *src, int x, int y);
	
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

/* BGR to RGB conversions */
static int bgr_to_rgb16 (VisVideo *dest, const VisVideo *src);
static int bgr_to_rgb24 (VisVideo *dest, const VisVideo *src);
static int bgr_to_rgb32 (VisVideo *dest, const VisVideo *src);

/* Scaling functions */
static int scale_nearest_8 (VisVideo *dest, const VisVideo *src);


static int video_dtor (VisObject *object)
{
	VisVideo *video = VISUAL_VIDEO (object);

	if (HAVE_ALLOCATED_BUFFER (video)) {
		if (video->pixels != NULL)
			visual_video_free_buffer (video);

		video->pixels = NULL;
	}

	video->pixel_rows = NULL;

	return VISUAL_OK;
}


/**
 * @defgroup VisVideo VisVideo
 * @{
 */

/**
 * Creates a new VisVideo structure, without an associated screen buffer.
 *
 * @return A newly allocated VisVideo.
 */
VisVideo *visual_video_new ()
{
	VisVideo *video;

	video = visual_mem_new0 (VisVideo, 1);

	/* Do the VisObject initialization */
	VISUAL_OBJECT (video)->allocated = TRUE;
	VISUAL_OBJECT (video)->dtor = video_dtor;
	visual_object_ref (VISUAL_OBJECT (video));

	video->pixels = NULL;

	/*
	 * By default, we suppose an external buffer will be used.
	 */
	video->flags = VISUAL_VIDEO_FLAG_EXTERNAL_BUFFER;

	return video;
}

/**
 * Creates a new VisVideo and also allocates a buffer.
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
	int ret;
	
	video = visual_video_new ();

	visual_video_set_depth (video, depth);
	visual_video_set_dimension (video, width, height);

	video->pixels = NULL;
	ret = visual_video_allocate_buffer (video);

	if (ret < 0) {
		/*
		 * Restore the flag set by visual_video_new().
		 */
		video->flags = VISUAL_VIDEO_FLAG_EXTERNAL_BUFFER;
		visual_object_unref (VISUAL_OBJECT (video));
		
		return NULL;
	}

	return video;
}

/**
 * Frees the buffer that relates to the VisVideo.
 *
 * @param video Pointer to a VisVideo of which the buffer needs to be freed.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL or -VISUAL_ERROR_VIDEO_NO_ALLOCATED
 *	on failure.
 */
int visual_video_free_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->pixels != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);

	if (HAVE_ALLOCATED_BUFFER (video)) {
		if (video->pixel_rows != NULL)
			visual_mem_free (video->pixel_rows);

		visual_mem_free (video->pixels);

	} else {
		return -VISUAL_ERROR_VIDEO_NO_ALLOCATED;
	}

	video->pixel_rows = NULL;
	video->pixels = NULL;

	video->flags = VISUAL_VIDEO_FLAG_NONE;

	return VISUAL_OK;
}

/**
 * Allocates a buffer for the VisVideo. Allocates based on the
 * VisVideo it's information about the screen dimension and depth.
 *
 * @param video Pointer to a VisVideo that needs an allocated buffer.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_HAS_PIXELS  on failure.
 */
int visual_video_allocate_buffer (VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (video->pixels != NULL) {
		if (HAVE_ALLOCATED_BUFFER (video)) {
			visual_video_free_buffer (video);
		} else {
			visual_log (VISUAL_LOG_CRITICAL, "Trying to allocate an screen buffer on "
					"a VisVideo structure which points to an external screen buffer");

			return -VISUAL_ERROR_VIDEO_HAS_PIXELS;
		}
	}

	if (video->size == 0) {
		video->pixels = NULL;
		video->flags = VISUAL_VIDEO_FLAG_NONE;

		return VISUAL_OK;
	}
	
	video->pixels = visual_mem_malloc0 (video->size);

	video->pixel_rows = visual_mem_new0 (void *, video->height);
	precompute_row_table (video);

	video->flags = VISUAL_VIDEO_FLAG_ALLOCATED_BUFFER;

	return VISUAL_OK;
}

/**
 * Checks if the given VisVideo has a private allocated buffer.
 *
 * @param video Pointer to the VisVideo of which we want to know if it has a private allocated buffer.
 *
 * @return TRUE if the VisVideo has an allocated buffer, or FALSE if not.
 */
int visual_video_have_allocated_buffer (const VisVideo *video)
{
	visual_log_return_val_if_fail (video != NULL, FALSE);

	if (HAVE_ALLOCATED_BUFFER (video))
		return TRUE;

	return FALSE;
}

static void precompute_row_table (VisVideo *video)
{
	void **table, *row;
	int y;

	visual_log_return_if_fail (video->pixel_rows != NULL);
	
	table = video->pixel_rows;
	
	for (y = 0, row = video->pixels; y < video->height; y++, row += video->pitch)
		*table++ = row;
}

/**
 * Clones the information from a VisVideo to another.
 * This will clone the depth, dimension and screen pitch into another VisVideo.
 * It doesn't clone the palette or buffer.
 *
 * @param dest Pointer to a destination VisVideo in which the information needs to
 *	be placed.
 * @param src Pointer to a source VisVideo from which the information needs to
 *	be obtained.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_clone (VisVideo *dest, const VisVideo *src)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	visual_video_set_depth (dest, src->depth);
	visual_video_set_dimension (dest, src->width, src->height);
	visual_video_set_pitch (dest, src->pitch);

	dest->flags = src->flags;

	return VISUAL_OK;
}

/**
 * Checks if two VisVideo objects are the same depth, pitch and dimension wise.
 *
 * @param src1 Pointer to the first VisVideo that is used in the compare.
 * @param src2 Pointer to the second VisVideo that is used in the compare.
 *
 * @return FALSE on different, TRUE on same, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_compare (const VisVideo *src1, const VisVideo *src2)
{
	visual_log_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (src1->depth != src2->depth)
		return FALSE;

	if (src1->width != src2->width)
		return FALSE;

	if (src1->height != src2->height)
		return FALSE;

	if (src1->pitch != src2->pitch)
		return FALSE;

	/* We made it to the end, the VisVideos are likewise in depth, pitch, dimensions */
	return TRUE;
}


/**
 * Sets a palette to a VisVideo. Links a VisPalette to the
 * VisVideo.
 *
 * @param video Pointer to a VisVideo to which a VisPalette needs to be linked.
 * @param pal Pointer to a Vispalette that needs to be linked with the VisVideo.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_palette (VisVideo *video, VisPalette *pal)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->pal = pal;

	return VISUAL_OK;
}

/**
 * Sets a buffer to a VisVideo. Links a sreenbuffer to the
 * VisVideo.
 *
 * @warning The given @a video must be one previously created with visual_video_new(),
 * and not with visual_video_new_with_buffer().
 *
 * @param video Pointer to a VisVideo to which a buffer needs to be linked.
 * @param buffer Pointer to a buffer that needs to be linked with the VisVideo.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_HAS_ALLOCATED on failure.
 */
int visual_video_set_buffer (VisVideo *video, void *buffer)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (HAVE_ALLOCATED_BUFFER (video)) {
		visual_log (VISUAL_LOG_CRITICAL, "Trying to set a screen buffer on "
				"a VisVideo structure which points to an allocated screen buffer");

		return -VISUAL_ERROR_VIDEO_HAS_ALLOCATED;
	}

	video->pixels = buffer;

	if (video->pixel_rows != NULL)
		visual_mem_free (video->pixel_rows);

	video->pixel_rows = visual_mem_new0 (void *, video->height);
	precompute_row_table (video);

	return VISUAL_OK;
}

/**
 * Sets the dimension for a VisVideo. Used to set the dimension for a
 * surface.
 *
 * @param video Pointer to a VisVideo to which the dimension is set.
 * @param width The width of the surface.
 * @param height The height of the surface.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_dimension (VisVideo *video, int width, int height)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->width = width;
	video->height = height;

	video->pitch = video->width * video->bpp;
	video->size = video->pitch * video->height;
	
	return VISUAL_OK;
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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_INVALID_BPP on failure.
 */
int visual_video_set_pitch (VisVideo *video, int pitch)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (video->bpp <= 0)
		return -VISUAL_ERROR_VIDEO_INVALID_BPP;

	video->pitch = pitch;
	video->size = video->pitch * video->height;

	return VISUAL_OK;
}

/**
 * Sets the depth for a VisVideo. Used to set the depth for
 * a surface. This will also define the number of bytes per pixel.
 *
 * @param video Pointer to a VisVideo to which the depth is set.
 * @param depth The depth choosen from the VisVideoDepth enumerate.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL on failure.
 */
int visual_video_set_depth (VisVideo *video, VisVideoDepth depth)
{
	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);

	video->depth = depth;
	video->bpp = visual_video_bpp_from_depth (video->depth);

	return VISUAL_OK;
}

/**
 * Checks if a certain depth is supported by checking against an ORred depthflag.
 *
 * @param depthflag The ORred depthflag that we check against.
 * @param depth The depth that we want to test.
 *
 * @return TRUE when supported, FALSE when unsupported and -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_depth_is_supported (int depthflag, VisVideoDepth depth)
{
	if (visual_video_depth_is_sane (depth) == 0)
		return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;

	if ((depth & depthflag) > 0)
		return TRUE;

	return FALSE;
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
 * @return The next supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
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
 * @return The previous supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
 */
VisVideoDepth visual_video_depth_get_prev (int depthflag, VisVideoDepth depth)
{
	int i = depth;

	if (visual_video_depth_is_sane (depth) == 0)
		return VISUAL_VIDEO_DEPTH_ERROR;

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
 * @return The lowest supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
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
 * @return The highest supported depth or VISUAL_VIDEO_DEPTH_ERROR on failure.
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
 *	VISUAL_VIDEO_DEPTH_ERROR on failure.
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

	} else {
		return depth;
	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * Checks if a certain value is a sane depth.
 *
 * @param depth Depth to be checked if it's sane.
 *
 * @return TRUE if the depth is sane, FALSE if the depth is not sane.
 */
int visual_video_depth_is_sane (VisVideoDepth depth)
{
	int count = 0;
	int i = 1;

	if (depth == VISUAL_VIDEO_DEPTH_NONE)
		return TRUE;

	if (depth >= VISUAL_VIDEO_DEPTH_ENDLIST)
		return FALSE;
	
	while (i < VISUAL_VIDEO_DEPTH_ENDLIST) {
		if ((i & depth) > 0)
			count++;

		if (count > 1)
			return FALSE;

		i <<= 1;
	}

	return TRUE;
}

/**
 * Returns the number of bits per pixel from a VisVideoDepth enumerate value.
 *
 * @param depth The VisVideodepth enumerate value from which the bits per pixel
 *	needs to be returned.
 *
 * @return The bits per pixel or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
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
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}

	return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
}

/**
 * Returns a VisVideoDepth enumerate value from bits per pixel.
 *
 * @param depthvalue Integer containing the number of bits per pixel.
 *
 * @return The corespondending enumerate value or VISUAL_VIDEO_DEPTH_ERROR on failure.
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

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * Returns the number of bytes per pixel from the VisVideoDepth enumerate.
 *
 * @param depth The VisVideodepth enumerate value from which the bytes per pixel
 *	needs to be returned.
 *
 * @return The number of bytes per pixel, -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
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
			return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
	}

	return -VISUAL_ERROR_IMPOSSIBLE;
}

/**
 * This function blits a VisVideo into another VisVideo. Placement can be done and there
 * is support for the alpha channel.
 *
 * @param dest Pointer to the destination VisVideo in which the source is overlayed.
 * @param src Pointer to the source VisVideo which is overlayed in the destination.
 * @param x Horizontal placement offset.
 * @param y Vertical placement offset.
 * @param alpha Sets if we want to check the alpha channel. Use FALSE or TRUE here.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_INVALID_DEPTH or -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS on failure.
 */
int visual_video_blit_overlay (VisVideo *dest, const VisVideo *src, int x, int y, int alpha)
{
	VisVideo *transform = NULL;
	const VisVideo *srcp = NULL;

	/* We can't overlay GL surfaces so don't even try */
	visual_log_return_val_if_fail (dest->depth != VISUAL_VIDEO_DEPTH_GL ||
			src->depth != VISUAL_VIDEO_DEPTH_GL, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);
	

	/* Placement is outside dest buffer, no use to continue */
	if (x > dest->width)
		return -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS;

	if (y > dest->height)
		return -VISUAL_ERROR_VIDEO_OUT_OF_BOUNDS;

	/* We're not the same depth, converting */
	if (dest->depth != src->depth) {
		transform = visual_video_new ();

		visual_video_set_depth (transform, dest->depth);
		visual_video_set_dimension (transform, src->width, src->height);

		visual_video_allocate_buffer (transform);

		visual_video_depth_transform (transform, src);
	}
	
	/* Setting all the pointers right */
	if (transform != NULL)
		srcp = transform;
	else
		srcp = src;
	
	
	/** @todo fix negative, broken as it is right now... */

	/* We're looking at exactly the same types of VisVideo objects */
	if (visual_video_compare (dest, src) == TRUE && alpha == FALSE && x == 0 && y == 0)
		memcpy (dest->pixels, src->pixels, dest->size);
	else if (alpha == FALSE || src->depth != VISUAL_VIDEO_DEPTH_32BIT)
		blit_overlay_noalpha (dest, srcp, x, y);
	/* Add a noalpha 32 version, that is optimized and fast for 32 bit */
//	else if (alpha == FALSE && src->depth == VISUAL_VIDEO_DEPTH_32BIT)
//		blit_overlay_noalpha32 ();
	else
		blit_overlay_alpha32 (dest, srcp, x, y);

	if (transform != NULL)
		visual_object_unref (VISUAL_OBJECT (transform));
	
	return VISUAL_OK;
}

static int blit_overlay_noalpha (VisVideo *dest, const VisVideo *src, int x, int y)
{
	int height, wrange, hrange, amount;
	int xmoff = 0, ymoff = 0;
	int xbpp;
	uint8_t *destbuf;
	uint8_t *srcbuf;

	destbuf = dest->pixels;
	srcbuf = src->pixels;

	/* Get the smallest size from both the VisVideos */
	wrange = dest->width > src->width ? src->width : dest->width;
	hrange = dest->height > src->height ? src->height : dest->height;

	/* Negative X offset value */
	if (x < 0) {
		xmoff = abs (x);
	
		x = 0;
	}

	/* Negative Y offset value */
	if (y < 0) {
		ymoff = abs (y);

		y = 0;
	}

	int xps = (x * dest->bpp) + src->pitch;
	/* Blit it to the dest video */
	for (height = y; height < (hrange + y) - ymoff; height++) {

		/* We've reached the end */
		if (height > (dest->height - 1))
			break;

		if (xps > dest->pitch + (xmoff * dest->bpp)) {
			amount = (dest->pitch - (x * dest->bpp));
		} else {
			if (xmoff > 0 && dest->width == wrange)
				amount = (dest->width - (dest->width - (src->width - xmoff))) * dest->bpp;
			else
				amount = (wrange - xmoff) * dest->bpp;
		}

		memcpy (destbuf + (height * dest->pitch) + (x * dest->bpp),
				srcbuf + (((height - y) + ymoff) * src->pitch) + (xmoff * dest->bpp),
				amount);
	}

	return VISUAL_OK;
}

/* FIXME Loads of optimze to be done */
static int blit_overlay_alpha32 (VisVideo *dest, const VisVideo *src, int x, int y)
{
	int height, wrange, hrange, amount;
	int xa, ya;
	int xmoff = 0, ymoff = 0;
	int xbpp;
	uint8_t *destbuf;
	uint8_t *srcpbuf;
	uint32_t *srcbuf;

	int yaddage = 0;
	int aindex = (ymoff * (src->pitch / src->bpp)) + xmoff;
	int si = (ymoff * src->pitch) + (xmoff * src->bpp);
	int di = (y * dest->pitch) + (x * dest->bpp);

	destbuf = dest->pixels;
	srcbuf = src->pixels;
	srcpbuf = src->pixels;

	/* Get the smallest size from both the VisVideos */
	wrange = dest->width > src->width ? src->width : dest->width;
	hrange = dest->height > src->height ? src->height : dest->height;

	/* Negative X offset value */
	if (x < 0) {
		xmoff = abs (x);
	
		x = 0;
	}

	/* Negative Y offset value */
	if (y < 0) {
		ymoff = abs (y);

		y = 0;
	}

	xbpp = x * dest->bpp;

	if (dest->height - src->height	< 0)
		yaddage = abs (dest->height - (src->height));

	/* Blit it to the dest video */
	for (ya = y; ya < (hrange + yaddage) - ymoff; ya++) {

		if (ya + y > dest->height - 1)
			break;

		if ((x * dest->bpp) + src->pitch > dest->pitch) {
			amount = (dest->pitch / dest->bpp) - (dest->width - (src->width - xmoff));
		} else {
			amount = wrange - xmoff;
		}

		for (xa = 0; xa < amount; xa++) {
			uint8_t alpha;
			int bppl;

			alpha = srcbuf[aindex++] >> 24;

			for (bppl = 0; bppl < dest->bpp; bppl++) {
				destbuf[di] =
					(alpha * (srcpbuf[si] -	 destbuf[di]) / 255 + destbuf[di]);

				si++;
				di++;
			}
		}

		if (amount > (dest->pitch / dest->bpp) - x) {
			/* FIXME do something here to fix screwage */
		}

		aindex += (src->pitch / src->bpp) - amount;
		si += src->pitch - (amount * src->bpp);
		di += dest->pitch - (amount * dest->bpp);
	}

	return VISUAL_OK;
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
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_alpha_color (VisVideo *video, uint8_t r, uint8_t g, uint8_t b, uint8_t density)
{
	int col = 0;
	int i;
	uint32_t *vidbuf;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	col = (r << 16 | g << 8 | b);

	vidbuf = video->pixels;

	for (i = 0; i < video->size / video->bpp; i++) {
		if ((vidbuf[i] & 0x00ffffff) == col)
			vidbuf[i] = col;
		else
			vidbuf[i] += (density << 24);
	}

	return VISUAL_OK;
}

/**
 * Sets a certain alpha value for the complete buffer in the VisVideo. This function
 * can be only used on VISUAL_VIDEO_DEPTH_32BIT surfaces.
 *
 * @param video Pointer to the VisVideo in which the alpha channel density is set.
 * @param density The alpha density that is to be set.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_alpha_fill (VisVideo *video, uint8_t density)
{
	int i;
	uint32_t *vidbuf;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (video->depth == VISUAL_VIDEO_DEPTH_32BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);

	vidbuf = video->pixels;

	for (i = 0; i < video->size / video->bpp; i++)
		vidbuf[i] += (density << 24);

	return VISUAL_OK;
}

/**
 * Video color transforms one VisVideo bgr pixel ordering into bgr pixel ordering.
 * 
 * @param dest Pointer to the destination VisVideo, which should be a clone of the source VisVideo
 *	depth, pitch, dimension wise.
 * @param src Pointer to the source VisVideo from which the bgr data is read.
 *
 * @return VISUAL_OK on succes, -VISUAL_ERROR_VIDEO_NOT_INDENTICAL, -VISUAL_ERROR_VIDEO_PIXELS_NULL or
 *	-VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_color_bgr_to_rgb (VisVideo *dest, const VisVideo *src)
{
	visual_log_return_val_if_fail (visual_video_compare (dest, src) == TRUE, -VISUAL_ERROR_VIDEO_NOT_INDENTICAL);
	visual_log_return_val_if_fail (dest->pixels != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);
	visual_log_return_val_if_fail (src->pixels != NULL, -VISUAL_ERROR_VIDEO_PIXELS_NULL);
	visual_log_return_val_if_fail (dest->depth != VISUAL_VIDEO_DEPTH_8BIT, -VISUAL_ERROR_VIDEO_INVALID_DEPTH);
	
	if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT)
		bgr_to_rgb16 (dest, src);
	else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT)
		bgr_to_rgb24 (dest, src);
	else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT)
		bgr_to_rgb32 (dest, src);

	return VISUAL_OK;
}

/**
 * Video depth transforms one VisVideo into another using the depth information
 * stored within the VisVideos. The dimension should be equal however the pitch
 * value of the destination may be set.
 *
 * @param viddest Pointer to the destination VisVideo to which the source
 *	VisVideo is transformed.
 * @param vidsrc Pointer to the source VisVideo.
 *
 * @return VISUAL_OK on succes, error values returned by visual_video_blit_overlay or
 *	visual_video_depth_transform_to_buffer on failure.
 */
int visual_video_depth_transform (VisVideo *viddest, const VisVideo *vidsrc)
{
	/* We blit overlay it instead of just memcpy because the pitch can still be different */
	if (viddest->depth == vidsrc->depth)
		return visual_video_blit_overlay (viddest, vidsrc, 0, 0, FALSE);
	
	return visual_video_depth_transform_to_buffer (viddest->pixels,
			vidsrc, vidsrc->pal, viddest->depth, viddest->pitch);
}

/**
 * Less abstract video depth transform used by visual_video_depth_transform.
 *
 * @see visual_video_depth_transform
 *
 * @param dest Destination buffer.
 * @param video Source VisVideo.
 * @param pal Pointer to a VisPalette that can be set by full color to indexed color transforms.
 * @param destdepth The destination depth.
 * @param pitch The destination number of bytes per line.
 *
 * return VISUAL_OK on succes -VISUAL_ERROR_VIDEO_NULL, -VISUAL_ERROR_PALETTE_NULL, -VISUAL_ERROR_PALETTE_SIZE
 *	or -VISUAL_ERROR_VIDEO_NOT_TRANSFORMED on failure.
 */
int visual_video_depth_transform_to_buffer (uint8_t *dest, const VisVideo *video,
		VisPalette *pal, VisVideoDepth destdepth, int pitch)
{
	uint8_t *srcbuf = video->pixels;
	int width = video->width;
	int height = video->height;

	visual_log_return_val_if_fail (video != NULL, -VISUAL_ERROR_VIDEO_NULL);
	
	if (destdepth == VISUAL_VIDEO_DEPTH_8BIT || video->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		visual_log_return_val_if_fail (pal != NULL, -VISUAL_ERROR_PALETTE_NULL);
		visual_log_return_val_if_fail (pal->ncolors == 256, -VISUAL_ERROR_PALETTE_SIZE);
	}

	/* Destdepth is equal to sourcedepth case */
	if (video->depth == destdepth) {
		memcpy (dest, video->pixels, video->width * video->height * video->bpp);

		return VISUAL_OK;
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

	return -VISUAL_ERROR_VIDEO_NOT_TRANSFORMED;
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
			destr[i].r = pal->colors[src[j]].r >> 3;
			destr[i].g = pal->colors[src[j]].g >> 2;
			destr[i].b = pal->colors[src[j]].b >> 3;
			i++;
			j++;
		}

		i += pitchdiff;
	}
	
	return VISUAL_OK;
}

static int depth_transform_8_to_24_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	int pitchdiff = pitch - (width * 3);

	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dest[i++] = pal->colors[src[j]].r;
			dest[i++] = pal->colors[src[j]].g;
			dest[i++] = pal->colors[src[j]].b;
			j++;
		}

		i += pitchdiff;
	}

	return VISUAL_OK;
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
			col += pal->colors[src[j]].r << 16;
			col += pal->colors[src[j]].g << 8;
			col += pal->colors[src[j]].b;
			j++;

			destr[i++] = col;
		}

		i += pitchdiff;
	}

	return VISUAL_OK;
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

			/* FIXME optimize */
			col = (r + g + b) / 3;

			pal->colors[col].r = r;
			pal->colors[col].g = g;
			pal->colors[col].b = b;

			dest[i++] = col;
		}

		i += pitchdiff;	
	}

	return VISUAL_OK;
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

	return VISUAL_OK;
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
	
	return VISUAL_OK;
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
			b = src[j++];
			g = src[j++];
			r = src[j++];

			/* FIXME optimize */
			col = (b + g + r) / 3;
			
			pal->colors[col].r = r;
			pal->colors[col].g = g;
			pal->colors[col].b = b;

			dest[i++] = col;
		}

		i += pitchdiff;	
	}

	return VISUAL_OK;
}

static int depth_transform_24_to_16_c (uint8_t *dest, uint8_t *src, int width, int height, int pitch, VisPalette *pal)
{
	int x, y;
	int i = 0, j = 0;
	_color16 *destr = (_color16 *) dest;
	int pitchdiff = (pitch - (width * 2)) >> 1;
	
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			destr[i].b = src[j++] >> 3;
			destr[i].g = src[j++] >> 2;
			destr[i].r = src[j++] >> 3;
			i++;
		}

		i += pitchdiff;
	}
	
	return VISUAL_OK;
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
	
	return VISUAL_OK;
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

			/* FIXME optimize */
			col = (r + g + b) / 3;

			pal->colors[col].r = r;
			pal->colors[col].g = g;
			pal->colors[col].b = b;

			dest[i++] = col;
		}

		i += pitchdiff;	
	}
	
	return VISUAL_OK;
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

	return VISUAL_OK;
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
	
	return VISUAL_OK;
}

static int bgr_to_rgb16 (VisVideo *dest, const VisVideo *src)
{
	_color16 *destbuf, *srcbuf;
	int x, y;
	int i = 0;
	int pitchdiff = (dest->pitch - (dest->width * 2)) >> 1;
	
	destbuf = (_color16 *) dest->pixels;
	srcbuf = (_color16 *) src->pixels;
	
	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			destbuf[i].b = srcbuf[i].r;
			destbuf[i].g = srcbuf[i].g;
			destbuf[i].r = srcbuf[i].b;
			i++;
		}

		i += pitchdiff;
	}
	
	return VISUAL_OK;
}

static int bgr_to_rgb24 (VisVideo *dest, const VisVideo *src)
{
	uint8_t *destbuf, *srcbuf;
	int x, y;
	int i = 0;
	int pitchdiff = dest->pitch - (dest->width * 3);

	destbuf = dest->pixels;
	srcbuf = src->pixels;
	
	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			destbuf[i + 2] = srcbuf[i];
			destbuf[i + 1] = srcbuf[i + 1];
			destbuf[i] = srcbuf[i + 2];
		
			i += 3;
		}

		i += pitchdiff;
	}

	return VISUAL_OK;
}

static int bgr_to_rgb32 (VisVideo *dest, const VisVideo *src)
{
	uint8_t *destbuf, *srcbuf;
	int x, y;
	int i = 0;
	int pitchdiff = dest->pitch - (dest->width * 4);

	destbuf = dest->pixels;
	srcbuf = src->pixels;
	
	for (y = 0; y < dest->height; y++) {
		for (x = 0; x < dest->width; x++) {
			destbuf[i + 2] = srcbuf[i];
			destbuf[i + 1] = srcbuf[i + 1];
			destbuf[i] = srcbuf[i + 2];

			destbuf[i + 3] = srcbuf[i + 3];

			i += 4;
		}

		i += pitchdiff;
	}

	return VISUAL_OK;
}

/**
 * Scale video.
 *
 * @param dest Pointer to VisVideo object for storing scaled image.
 * @param src Ppointer to VisVideo object whose image is to be scaled.
 * @param scale_method Scaling method to use.
 *
 * @return VISUAL_OK on success, -VISUAL_ERROR_VIDEO_NULL or -VISUAL_ERROR_VIDEO_INVALID_DEPTH on failure.
 */
int visual_video_scale (VisVideo *dest, const VisVideo *src, VisVideoScaleMethod scale_method)
{
	visual_log_return_val_if_fail (dest != NULL, -VISUAL_ERROR_VIDEO_NULL);
	visual_log_return_val_if_fail (src != NULL, -VISUAL_ERROR_VIDEO_NULL);

	if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
		if (scale_method == VISUAL_VIDEO_SCALE_NEAREST)
			return scale_nearest_8 (dest, src);

                /* default to nearest if scale method is invalid */
                return scale_nearest_8 (dest, src);
	}

	return -VISUAL_ERROR_VIDEO_INVALID_DEPTH;
}

#ifndef FIXED_POINT_SCALER

/* 8-bit nearest pixel scaler, floating point version */
static int scale_nearest_8 (VisVideo *dest, const VisVideo *src)
{
	int x, y;
	float u, v, du, dv;
	uint8_t *dest_pixel, *src_pixel_row;

	u = 0; du = (float) src->width	/ dest->width;
	v = 0; dv = (float) src->height / dest->height;

	dest_pixel = dest->pixels;

	for (y = 0, v = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint8_t *) src->pixel_rows[(int) v];

		for (x = 0, u = 0; x < dest->width; x++, u += du) {
			*dest_pixel++ = src_pixel_row[(int) u];
		}
	}

	return VISUAL_OK;
}

#else

/* 8-bit nearest pixel scaler, fixed point version */
static int scale_nearest_8 (VisVideo *dest, const VisVideo *src)
{	 
	int x, y;
	fixed32_t u, v, du, dv;
	uint8_t *dest_pixel, *src_pixel_row;

	u = 0; du = FP_INT(src->width)	/ dest->width;
	v = 0; dv = FP_INT(src->height) / dest->height;

	dest_pixel = dest->pixels;

	for (y = 0, v = 0; y < dest->height; y++, v += dv) {
		src_pixel_row = (uint8_t *) src->pixel_rows[FP_TO_INT(v)];

		for (x = 0, u = 0; x < dest->width; x++, u += du) {
			*dest_pixel++ = src_pixel_row[FP_TO_INT(u)];
		}
	}

	return VISUAL_OK;
}

#endif
