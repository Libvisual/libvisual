#ifndef _LV_VIDEO_SCALE_H
#define _LV_VIDEO_SCALE_H

#include "lv_video.h"

void visual_video_zoom_color8  (VisVideo *dest, VisVideo *src);
void visual_video_zoom_color16 (VisVideo *dest, VisVideo *src);
void visual_video_zoom_color24 (VisVideo *dest, VisVideo *src);
void visual_video_zoom_color32 (VisVideo *dest, VisVideo *src);

void visual_video_scale_nearest_color8  (VisVideo *dest, VisVideo *src);
void visual_video_scale_nearest_color16 (VisVideo *dest, VisVideo *src);
void visual_video_scale_nearest_color24 (VisVideo *dest, VisVideo *src);
void visual_video_scale_nearest_color32 (VisVideo *dest, VisVideo *src);

void visual_video_scale_bilinear_color8  (VisVideo *dest, VisVideo *src);
void visual_video_scale_bilinear_color16 (VisVideo *dest, VisVideo *src);
void visual_video_scale_bilinear_color24 (VisVideo *dest, VisVideo *src);
void visual_video_scale_bilinear_color32 (VisVideo *dest, VisVideo *src);

#endif /* _LV_VIDEO_SCALE_H */
