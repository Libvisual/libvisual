#ifndef _LV_VIDEO_FILL_H
#define _LV_VIDEO_FILL_H

#include "lv_video.h"
#include "lv_color.h"

LV_BEGIN_DECLS

/* Color fill functions */
void visual_video_fill_color_index8 (VisVideo *video, VisColor *color);
void visual_video_fill_color_rgb16  (VisVideo *video, VisColor *color);
void visual_video_fill_color_rgb24  (VisVideo *video, VisColor *color);
void visual_video_fill_color_argb32 (VisVideo *video, VisColor *color);

LV_END_DECLS

#endif /* _LV_VIDEO_FILL_H */
