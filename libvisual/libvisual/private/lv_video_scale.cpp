#include "config.h"
#include "lv_video_transform.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"
#include "lv_cpu.h"

#pragma pack(1)

// NOTE: Although the following struct members are named 'r', 'g' and 'b', the scale functions are very much pixel
// format agnostic. They only assume the number of components.

typedef struct {
    uint16_t r:5, g:6, b:5;
} color16_t;

typedef struct {
    uint8_t r, g, b;
} color24_t;

#pragma pack()

namespace LV {

  void VideoTransform::scale_nearest_color8 (Video& dest, Video const& src)
  {
      uint8_t* dest_pixel = static_cast<uint8_t*> (dest.get_pixels ());

      uint32_t du = (src.m_impl->width  << 16) / dest.m_impl->width;
      uint32_t dv = (src.m_impl->height << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = 0; y < dest.m_impl->height; y++, v += dv) {
          uint8_t const* src_pixel_row = static_cast<uint8_t const*> (src.m_impl->pixel_rows[v >> 16]);

          if (v >> 16 >= (unsigned int) src.m_impl->height)
              v -= 0x10000;

          uint32_t u = 0;
          for (int x = 0; x < dest.m_impl->width; x++, u += du)
              *dest_pixel++ = src_pixel_row[u >> 16];

          dest_pixel += dest.m_impl->pitch - dest.m_impl->width;
      }
  }

  void VideoTransform::scale_nearest_color16 (Video& dest, Video const& src)
  {
      uint16_t* dest_pixel = static_cast<uint16_t*> (dest.get_pixels ());

      uint32_t du = (src.m_impl->width  << 16) / dest.m_impl->width;
      uint32_t dv = (src.m_impl->height << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = 0; y < dest.m_impl->height; y++, v += dv) {
          uint16_t const* src_pixel_row = static_cast<uint16_t const*> (src.m_impl->pixel_rows[v >> 16]);

          if (v >> 16 >= (unsigned int) src.m_impl->height)
              v -= 0x10000;

          uint32_t u = 0;
          for (int x = 0; x < dest.m_impl->width; x++, u += du)
              *dest_pixel++ = src_pixel_row[u >> 16];

          dest_pixel += (dest.m_impl->pitch / dest.m_impl->bpp) - dest.m_impl->width;
      }
  }

  void VideoTransform::scale_nearest_color24 (Video& dest, Video const& src)
  {
      color24_t* dest_pixel = static_cast<color24_t*> (dest.get_pixels ());

      uint32_t du = (src.m_impl->width << 16)  / dest.m_impl->width;
      uint32_t dv = (src.m_impl->height << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = 0; y < dest.m_impl->height; y++, v += dv) {
          color24_t const* src_pixel_row = static_cast<color24_t const*> (src.m_impl->pixel_rows[v >> 16]);

          if (v >> 16 >= (unsigned int) src.m_impl->height)
              v -= 0x10000;

          uint32_t u = 0;
          for (int x = 0; x < dest.m_impl->width; x++, u += du)
              *dest_pixel++ = src_pixel_row[u >> 16];

          dest_pixel += (dest.m_impl->pitch / dest.m_impl->bpp) - dest.m_impl->width;
      }
  }

  void VideoTransform::scale_nearest_color32 (Video& dest, Video const& src)
  {
      uint32_t* dest_pixel = static_cast<uint32_t*> (dest.get_pixels ());

      uint32_t du = (src.m_impl->width  << 16) / dest.m_impl->width;
      uint32_t dv = (src.m_impl->height << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = 0; y < dest.m_impl->height; y++, v += dv) {
          uint32_t const* src_pixel_row = static_cast<uint32_t const*> (src.m_impl->pixel_rows[v >> 16]);

          if (v >> 16 >= (unsigned int) src.m_impl->height)
              v -= 0x10000;

          uint32_t u = 0;
          for (int x = 0; x < dest.m_impl->width; x++, u += du)
              *dest_pixel++ = src_pixel_row[u >> 16];

          dest_pixel += (dest.m_impl->pitch / dest.m_impl->bpp) - dest.m_impl->width;
      }
  }

  void VideoTransform::scale_bilinear_color8 (Video& dest, Video const& src)
  {
      uint8_t* dest_pixel = static_cast<uint8_t*> (dest.get_pixels ());

      uint32_t du = ((src.m_impl->width  - 1) << 16) / dest.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = dest.m_impl->height; y--; v += dv) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          uint8_t const* src_pixel_rowu = static_cast<uint8_t const*> (src.m_impl->pixel_rows[v >> 16]);
          uint8_t const* src_pixel_rowl = static_cast<uint8_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          for (int x = dest.m_impl->width - 1; x--; u += du) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU  = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              uint8_t cul = src_pixel_rowu[u >> 16];
              uint8_t cll = src_pixel_rowl[u >> 16];
              uint8_t cur = src_pixel_rowu[(u >> 16) + 1];
              uint8_t clr = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul;
              b0 += ll * cll;
              b0 += ur * cur;
              b0 += lr * clr;

              *dest_pixel++ = b0 >> 16;
          }

          dest_pixel += dest.m_impl->pitch - (dest.m_impl->width - 1);

      }
  }

  void VideoTransform::scale_bilinear_color16 (Video& dest, Video const& src)
  {
      color16_t* dest_pixel = static_cast<color16_t*> (dest.get_pixels ());

      uint32_t du = ((src.m_impl->width - 1)  << 16) / dest.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = dest.m_impl->height; y--; v += dv) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          color16_t const* src_pixel_rowu = static_cast<color16_t const*> (src.m_impl->pixel_rows[v >> 16]);
          color16_t const* src_pixel_rowl = static_cast<color16_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          for (int x = dest.m_impl->width - 1; x--; u += du) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              color16_t cul = src_pixel_rowu[u >> 16];
              color16_t cll = src_pixel_rowl[u >> 16];
              color16_t cur = src_pixel_rowu[(u >> 16) + 1];
              color16_t clr = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul.r;
              uint32_t b1 = ul * cul.g;
              uint32_t b2 = ul * cul.b;

              b0 += ll * cll.r;
              b1 += ll * cll.g;
              b2 += ll * cll.b;

              b0 += ur * cur.r;
              b1 += ur * cur.g;
              b2 += ur * cur.b;

              b0 += lr * clr.r;
              b1 += lr * clr.g;
              b2 += lr * clr.b;

              color16_t b;
              b.r = b0 >> 16;
              b.g = b1 >> 16;
              b.b = b2 >> 16;

              *dest_pixel++ = b;
          }

          dest_pixel += (dest.m_impl->pitch / dest.m_impl->bpp) - ((dest.m_impl->width - 1));
      }
  }

  void VideoTransform::scale_bilinear_color24 (Video& dest, Video const& src)
  {
      color24_t* dest_pixel = static_cast<color24_t*> (dest.get_pixels ());

      uint32_t du = ((src.m_impl->width  - 1) << 16) / dest.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = dest.m_impl->height; y--; v += dv) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          color24_t const* src_pixel_rowu = static_cast<color24_t const*> (src.m_impl->pixel_rows[v >> 16]);
          color24_t const* src_pixel_rowl = static_cast<color24_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          for (int x = dest.m_impl->width - 1; x--; u += du) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU  = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              color24_t cul = src_pixel_rowu[u >> 16];
              color24_t cll = src_pixel_rowl[u >> 16];
              color24_t cur = src_pixel_rowu[(u >> 16) + 1];
              color24_t clr = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul.r;
              uint32_t b1 = ul * cul.g;
              uint32_t b2 = ul * cul.b;

              b0 += ll * cll.r;
              b1 += ll * cll.g;
              b2 += ll * cll.b;

              b0 += ur * cur.r;
              b1 += ur * cur.g;
              b2 += ur * cur.b;

              b0 += lr * clr.r;
              b1 += lr * clr.g;
              b2 += lr * clr.b;

              color24_t b;
              b.r = b0 >> 16;
              b.g = b1 >> 16;
              b.b = b2 >> 16;

              *dest_pixel++ = b;
          }

          dest_pixel += (dest.m_impl->pitch / dest.m_impl->bpp) - ((dest.m_impl->width - 1));
      }
  }

  void VideoTransform::scale_bilinear_color32 (Video& dest, Video const& src)
  {
      if (visual_cpu_has_mmx ()) {
          scale_bilinear_color32_mmx (dest, src);
          return;
      }

      uint32_t* dest_pixel = static_cast<uint32_t*> (dest.get_pixels ());

      uint32_t du = ((src.m_impl->width  - 1) << 16) / dest.m_impl->width;
      uint32_t dv = ((src.m_impl->height - 1) << 16) / dest.m_impl->height;

      uint32_t v = 0;

      for (int y = dest.m_impl->height; y--; v += dv) {
          if (v >> 16 >= (unsigned int) (src.m_impl->height - 1))
              v -= 0x10000;

          uint32_t const* src_pixel_rowu = static_cast<uint32_t const*> (src.m_impl->pixel_rows[v >> 16]);
          uint32_t const* src_pixel_rowl = static_cast<uint32_t const*> (src.m_impl->pixel_rows[(v >> 16) + 1]);

          /* fracV = frac(v) = v & 0xffff */
          /* fixed point format convertion: fracV >>= 8) */
          uint32_t fracV = (v & 0xffff) >> 8;
          uint32_t u = 0;

          for (int x = dest.m_impl->width - 1; x--; u += du) {
              /* fracU = frac(u) = u & 0xffff */
              /* fixed point format convertion: fracU >>= 8) */
              uint32_t fracU = (u & 0xffff) >> 8;

              /* notice 0x100 = 1.0 (fixed point 24.8) */
              uint32_t ul = (0x100 - fracU) * (0x100 - fracV);
              uint32_t ll = (0x100 - fracU) * fracV;
              uint32_t ur = fracU * (0x100 - fracV);
              uint32_t lr = fracU * fracV;

              union {
                  uint8_t  c8[4];
                  uint32_t c32;
              } cul, cll, cur, clr, b;

              cul.c32 = src_pixel_rowu[u >> 16];
              cll.c32 = src_pixel_rowl[u >> 16];
              cur.c32 = src_pixel_rowu[(u >> 16) + 1];
              clr.c32 = src_pixel_rowl[(u >> 16) + 1];

              uint32_t b0 = ul * cul.c8[0];
              uint32_t b1 = ul * cul.c8[1];
              uint32_t b2 = ul * cul.c8[2];
              uint32_t b3 = ul * cul.c8[3];

              b0 += ll * cll.c8[0];
              b1 += ll * cll.c8[1];
              b2 += ll * cll.c8[2];
              b3 += ll * cll.c8[3];

              b0 += ur * cur.c8[0];
              b1 += ur * cur.c8[1];
              b2 += ur * cur.c8[2];
              b3 += ur * cur.c8[3];

              b0 += lr * clr.c8[0];
              b1 += lr * clr.c8[1];
              b2 += lr * clr.c8[2];
              b3 += lr * clr.c8[3];

              b.c8[0] = b0 >> 16;
              b.c8[1] = b1 >> 16;
              b.c8[2] = b2 >> 16;
              b.c8[3] = b3 >> 16;

              *dest_pixel++ = b.c32;
          }

          dest_pixel += (dest.m_impl->pitch / dest.m_impl->bpp) - ((dest.m_impl->width - 1));

      }
  }

} // LV namespace
