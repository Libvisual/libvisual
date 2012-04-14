#ifndef _LV_ALPHA_BLEND_H
#define _LV_ALPHA_BLEND_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

typedef void (*VisAlphaBlendFunc) (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);

LV_BEGIN_DECLS

extern LV_API VisAlphaBlendFunc visual_alpha_blend_8;
extern LV_API VisAlphaBlendFunc visual_alpha_blend_16;
extern LV_API VisAlphaBlendFunc visual_alpha_blend_24;
extern LV_API VisAlphaBlendFunc visual_alpha_blend_32;

LV_API void visual_alpha_blend_initialize (void);

LV_END_DECLS

#endif /* _LV_ALPHA_BLEND_H */
