#ifndef _LV_VIDEO_CONVERT_H
#define _LV_VIDEO_CONVERT_H

#include "lv_video.h"

/* Format conversions */
void visual_video_convert_get_smallest (VisVideo *dest, VisVideo *src, int *width, int *height);

void visual_video_index8_to_rgb16  (VisVideo *dest, VisVideo *src);
void visual_video_index8_to_rgb24  (VisVideo *dest, VisVideo *src);
void visual_video_index8_to_argb32 (VisVideo *dest, VisVideo *src);

void visual_video_rgb16_to_index8 (VisVideo *dest, VisVideo *src);
void visual_video_rgb16_to_rgb24  (VisVideo *dest, VisVideo *src);
void visual_video_rgb16_to_argb32 (VisVideo *dest, VisVideo *src);

void visual_video_rgb24_to_index8 (VisVideo *dest, VisVideo *src);
void visual_video_rgb24_to_rgb16  (VisVideo *dest, VisVideo *src);
void visual_video_rgb24_to_argb32 (VisVideo *dest, VisVideo *src);

void visual_video_argb32_to_index8 (VisVideo *dest, VisVideo *src);
void visual_video_argb32_to_rgb16  (VisVideo *dest, VisVideo *src);
void visual_video_argb32_to_rgb24  (VisVideo *dest, VisVideo *src);

/* BGR to RGB conversions */
void visual_video_flip_pixel_bytes_color16 (VisVideo *dest, VisVideo *src);
void visual_video_flip_pixel_bytes_color24 (VisVideo *dest, VisVideo *src);
void visual_video_flip_pixel_bytes_color32 (VisVideo *dest, VisVideo *src);

#endif /* _LV_VIDEO_CONVERT_H */
