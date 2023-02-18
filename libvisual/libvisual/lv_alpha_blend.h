#ifndef _LV_ALPHA_BLEND_H
#define _LV_ALPHA_BLEND_H

#include <libvisual/lvconfig.h>
#include <libvisual/lv_defines.h>
#include <libvisual/lv_types.h>

#ifdef __cplusplus

#include <cstdint>
#include <span>

namespace LV {

  LV_API void alpha_blend_8  (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha);
  LV_API void alpha_blend_16 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha);
  LV_API void alpha_blend_24 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha);
  LV_API void alpha_blend_32 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha);

} // LV namespace

#endif /* __cplusplus */

LV_BEGIN_DECLS

LV_API void visual_alpha_blend_8  (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);
LV_API void visual_alpha_blend_16 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);
LV_API void visual_alpha_blend_24 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);
LV_API void visual_alpha_blend_32 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha);

LV_END_DECLS

#endif /* _LV_ALPHA_BLEND_H */
