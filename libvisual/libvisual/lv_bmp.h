#ifndef _LV_BMP_H
#define _LV_BMP_H

#include <libvisual/lv_video.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int visual_bitmap_load (VisVideo *video, const char *filename);
VisVideo *visual_bitmap_load_new_video (const char *filename);
	
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LV_BMP_H */
