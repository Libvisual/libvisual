#include <config.h>
#include "lv_alpha_blend.h"
#include "lv_common.h"
#include "lv_cpu.h"
#include "lv_alpha_blend_orc.h"

#pragma pack(1)

typedef struct {
#if VISUAL_LITTLE_ENDIAN == 1
	uint16_t b:5, g:6, r:5;
#else
	uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

void visual_alpha_blend_8 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t n, uint8_t alpha)
{
    simd_interpolate_8 (dest, src1, src2, alpha, (int) n);
}

void visual_alpha_blend_16 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t n, uint8_t alpha)
{
    rgb16_t *destr = (rgb16_t *) dest;
    rgb16_t *src1r = (rgb16_t *) src1;
    rgb16_t *src2r = (rgb16_t *) src2;

    for (visual_size_t i = 0; i < n; i++) {
        destr[i].r = (alpha * (src2r[i].r - src1r[i].r)) / 255 + src1r[i].r;
        destr[i].g = (alpha * (src2r[i].g - src1r[i].g)) / 255 + src1r[i].g;
        destr[i].b = (alpha * (src2r[i].b - src1r[i].b)) / 255 + src1r[i].b;
    }
}

void visual_alpha_blend_24 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t n, uint8_t alpha)
{
    simd_interpolate_8 (dest, src1, src2, alpha, (int) n * 3);
}

void visual_alpha_blend_32 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t n, uint8_t alpha)
{
    simd_interpolate_8 (dest, src1, src2, alpha, (int) n * 4);
}
