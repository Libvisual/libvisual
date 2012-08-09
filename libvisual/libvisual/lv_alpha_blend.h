#ifndef _LV_ALPHA_BLEND_H
#define _LV_ALPHA_BLEND_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

LV_BEGIN_DECLS

LV_API void visual_alpha_blend_8  (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);
LV_API void visual_alpha_blend_16 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);
LV_API void visual_alpha_blend_24 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);
LV_API void visual_alpha_blend_32 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);

LV_END_DECLS

#endif /* _LV_ALPHA_BLEND_H */
