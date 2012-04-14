#ifndef _LV_ALPHA_BLEND_H
#define _LV_ALPHA_BLEND_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

VISUAL_BEGIN_DECLS

typedef void (*VisAlphaBlendFunc) (uint8_t *dest, uint8_t *src1, uint8_t *src2, visual_size_t size, uint8_t alpha);

extern LV_DLL_PUBLIC VisAlphaBlendFunc visual_alpha_blend_8;
extern LV_DLL_PUBLIC VisAlphaBlendFunc visual_alpha_blend_16;
extern LV_DLL_PUBLIC VisAlphaBlendFunc visual_alpha_blend_24;
extern LV_DLL_PUBLIC VisAlphaBlendFunc visual_alpha_blend_32;

LV_DLL_PUBLIC void visual_alpha_blend_initialize (void);

VISUAL_END_DECLS

#endif /* _LV_ALPHA_BLEND_H */
