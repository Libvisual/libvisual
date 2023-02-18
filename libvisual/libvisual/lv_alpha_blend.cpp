#include "config.h"
#include "lv_alpha_blend.h"
#include "lv_common.h"
#include "lv_cpu.h"
#include "lv_alpha_blend_orc.h"
#include <cassert>

namespace {

  #pragma pack(1)

  struct rgb16_t {
  #if VISUAL_LITTLE_ENDIAN == 1
      std::uint16_t b:5, g:6, r:5;
  #else
      std::uint16_t r:5, g:6, b:5;
  #endif
  };

  #pragma pack()

  static_assert (sizeof (rgb16_t) == 2);

} // anonymous namespace

namespace LV {

  void alpha_blend_8 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha)
  {
      assert (dst.data () != src1.data ());
      assert (dst.data () != src2.data ());
      assert (src1.size () == src2.size ());

      auto size {static_cast<int> (std::min (dst.size (), src1.size ()))};
      ::simd_interpolate_8 (dst.data (), src1.data (), src2.data (), alpha, size);
  }

  void alpha_blend_16 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha)
  {
      assert (dst.data () != src1.data ());
      assert (dst.data () != src2.data ());
      assert (src1.size () == src2.size ());

      auto dst_ptr  = reinterpret_cast<rgb16_t*> (dst.data ());
      auto src1_ptr = reinterpret_cast<rgb16_t const*> (src1.data ());
      auto src2_ptr = reinterpret_cast<rgb16_t const*> (src2.data ());

      auto size {static_cast<int> (std::min (dst.size (), src1.size ()))};

      for (auto i = 0; i < size / sizeof (rgb16_t); i++) {
          dst_ptr[i].r = (alpha * (src2_ptr[i].r - src1_ptr[i].r)) / 255 + src1_ptr[i].r;
          dst_ptr[i].g = (alpha * (src2_ptr[i].g - src1_ptr[i].g)) / 255 + src1_ptr[i].g;
          dst_ptr[i].b = (alpha * (src2_ptr[i].b - src1_ptr[i].b)) / 255 + src1_ptr[i].b;
      }
  }

  void alpha_blend_24 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha)
  {
      assert (dst.data () != src1.data ());
      assert (dst.data () != src2.data ());
      assert (src1.size () == src2.size ());

      auto size {static_cast<int> (std::min (dst.size (), src1.size ()))};
      ::simd_interpolate_8 (dst.data (), src1.data (), src2.data (), alpha, size * 3);
  }

  void alpha_blend_32 (std::span<std::uint8_t> dst, std::span<std::uint8_t const> src1, std::span<std::uint8_t const> src2, std::uint8_t alpha)
  {
      assert (dst.data () != src1.data ());
      assert (dst.data () != src2.data ());
      assert (src1.size () == src2.size ());

      auto size {static_cast<int> (std::min (dst.size (), src1.size ()))};
      ::simd_interpolate_8 (dst.data (), src1.data (), src2.data (), alpha, size * 4);
  }

} // LV namespace


void visual_alpha_blend_8 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha)
{
    simd_interpolate_8 (dest, src1, src2, alpha, (int) size);
}

void visual_alpha_blend_16 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha)
{
    LV::alpha_blend_16 ({dest, size}, {src1, size}, {src2, size}, alpha);
}

void visual_alpha_blend_24 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha)
{
    simd_interpolate_8 (dest, src1, src2, alpha, (int) size * 3);
}

void visual_alpha_blend_32 (uint8_t *LV_RESTRICT dest, const uint8_t *LV_RESTRICT src1, const uint8_t *LV_RESTRICT src2, visual_size_t size, uint8_t alpha)
{
    simd_interpolate_8 (dest, src1, src2, alpha, (int) size * 4);
}
