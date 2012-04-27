#ifndef _LV_VIDEO_BLIT_H
#define _LV_VIDEO_BLIT_H

#include "lv_video.h"

LV_BEGIN_DECLS

void blit_overlay_noalpha      (VisVideo *dest, VisVideo *src);
void blit_overlay_alphasrc     (VisVideo *dest, VisVideo *src);
void blit_overlay_colorkey     (VisVideo *dest, VisVideo *src);
void blit_overlay_surfacealpha (VisVideo *dest, VisVideo *src);
void blit_overlay_surfacealphacolorkey (VisVideo *dest, VisVideo *src);

LV_END_DECLS

#endif /* _LV_VIDEO_BLIT_H */
