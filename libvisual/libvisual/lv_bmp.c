#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lv_common.h"
#include "lv_log.h"
#include "lv_bmp.h"

#define BI_RGB	0
#define BI_RLE8	1
#define BI_RLE	2

/**
 * @defgroup VisBitmap VisBitmap
 * @{
 */

/**
 * Loads a BMP file into a VisVideo. The buffer will be located
 * for the VisVideo.
 *
 * The loader currently only supports uncompressed bitmaps in the form
 * of either 8 bits indexed or 24 bits RGB.
 *
 * Keep in mind that you need to free the palette by hand.
 * 
 * @param video Destination video where the bitmap should be loaded in.
 * @param filename The filename of the bitmap to be loaded.
 *
 * @return 0 on succes -1 on error.
 */
int visual_bitmap_load (VisVideo *video, char *filename)
{
	/* The win32 BMP header */
	char magic[2];
	u_int32_t bf_size = 0;
	u_int32_t bf_bits = 0;

	/* The win32 BITMAPINFOHEADER */
	int32_t bi_size = 0;
	int32_t bi_width = 0;
	int32_t bi_height = 0;
	int16_t bi_bitcount = 0;	
	u_int32_t bi_compression;
	u_int32_t bi_clrused;

	/* File read vars */
	int fd;

	/* Worker vars */
	u_int8_t *data;
	int pad;
	int i;
	char temp;	

	fd = open (filename, O_RDONLY);

	if (fd < 0) {
		/* FIXME fix visual log to support va args */
		visual_log (VISUAL_LOG_WARNING, "File not found");
	}

	/* Read the magic string */
	read (fd, magic, 2);

	if (strncmp (magic, "BM", 2) != 0) {
		visual_log (VISUAL_LOG_WARNING, "Not a bitmap file"); 
	
		return -1;
	}

	/* Read the file size */
	read (fd, &bf_size, 4);

	/* Skip past the reserved bits */
	lseek (fd, 4, SEEK_CUR);

	/* Read the offset bits */
	read (fd, &bf_bits, 4);

	/* Read the info structure size */
	read (fd, &bi_size, 4);

	if (bi_size == 12) {
		/* And read the width, height */
		read (fd, &bi_width, 2);
		read (fd, &bi_height, 2);

		/* Skip over the planet */
		lseek (fd, 2, SEEK_CUR);

		/* Read the bits per pixel */
		read (fd, &bi_bitcount, 2);

		bi_compression = BI_RGB;
	} else {
		/* And read the width, height */
		read (fd, &bi_width, 4);
		read (fd, &bi_height, 4);

		/* Skip over the planet */
		lseek (fd, 2, SEEK_CUR);

		/* Read the bits per pixel */
		read (fd, &bi_bitcount, 2);

		/* Read the compression flag */
		read (fd, &bi_compression, 4);

		/* Skip over the nonsense we don't want to know */
		lseek (fd, 12, SEEK_CUR);

		/* Number of colors in palette */
		read (fd, &bi_clrused, 4);

		/* Skip over the other nonsense */
		lseek (fd, 4, SEEK_CUR);
	}

	/* Check if we can handle it */
	if (bi_bitcount != 8 && bi_bitcount != 24) {
		visual_log (VISUAL_LOG_CRITICAL, "Only bitmaps with 8 bits or 24 bits per pixel are supported");
		
		return -1;
	}

	/* We only handle uncompressed bitmaps */
	if (bi_compression != BI_RGB) {
		visual_log (VISUAL_LOG_CRITICAL, "Only uncompressed bitmaps are supported");

		return -1;
	}

	/* Load the palette */
	if (bi_bitcount == 8) {
		if (video->pal == NULL)
			video->pal = visual_palette_new ();

		if (bi_clrused == 0)
			bi_clrused = 256;

		if (bi_size == 12) {
			for (i = 0; i < bi_clrused; i++) {
				read (fd, &video->pal->b[i], 1);
				read (fd, &video->pal->g[i], 1);
				read (fd, &video->pal->b[i], 1);
			}
		} else {
			for (i = 0; i < bi_clrused; i++) {
				read (fd, &video->pal->b[i], 1);
				read (fd, &video->pal->g[i], 1);
				read (fd, &video->pal->b[i], 1);
				lseek (fd, 1, SEEK_CUR);
			}
		}
	}

	/* Make the target VisVideo ready for use */
	visual_video_set_depth (video, visual_video_depth_enum_from_value (bi_bitcount));
	visual_video_set_dimension (video, bi_width, bi_height);
	visual_video_allocate_buffer (video);

	/* Set to the beginning of image data, note that MickeySoft likes stuff upside down .. */
	lseek (fd, bf_bits, SEEK_SET);

	pad = ((video->pitch % 4) ? (4 - (video->pitch % 4)) : 0);

	data = video->screenbuffer + (video->height * video->pitch);
	while (data > (u_int8_t *) video->screenbuffer) {
		data -= video->pitch;

		/* @todo fix endianess issues. */
		if (read (fd, data, video->pitch) != video->pitch) {
			visual_log (VISUAL_LOG_CRITICAL, "Bitmap data is not complete");
			
			visual_video_free_buffer (video);
			return -1;
		}

		if (pad != 0) {
			for (i = 0; i < pad; i++) {
				read (fd, &temp, 1);
			}
		}
	}

	close (fd);

	return 0;
}

/**
 * Loads a bitmap into a VisVideo and return this, so it's not needed to 
 * allocate a VisVideo before hand.
 *
 * @see visual_bitmap_load
 *
 * @param filename The filename of the bitmap to be loaded.
 *
 * @return The VisVideo containing the bitmap or NULL on error.
 */
VisVideo *visual_bitmap_load_new_video (char *filename)
{
	VisVideo *video;

	video = visual_video_new ();
	
	if (visual_bitmap_load (video, filename) < 0) {
		visual_video_free_with_buffer (video);
		video = NULL;
	}

	return video;
}

/**
 * @}
 */

