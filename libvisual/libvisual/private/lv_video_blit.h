#ifndef _LV_VIDEO_BLIT_H
#define _LV_VIDEO_BLIT_H

#include "lv_video.h"

int blit_overlay_noalpha      (VisVideo *dest, VisVideo *src);
int blit_overlay_alphasrc     (VisVideo *dest, VisVideo *src);
int blit_overlay_colorkey     (VisVideo *dest, VisVideo *src);
int blit_overlay_surfacealpha (VisVideo *dest, VisVideo *src);
int blit_overlay_surfacealphacolorkey (VisVideo *dest, VisVideo *src);



#endif /* _LV_VIDEO_BLIT_H */
