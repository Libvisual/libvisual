#include "config.h"
#include "lv_video_convert.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"
#include <algorithm>

#pragma pack(1)

typedef struct {
#if VISUAL_LITTLE_ENDIAN == 1
    uint16_t b:5, g:6, r:5;
#else
    uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

namespace LV {

  void VideoConvert::convert_get_smallest (Video& dest, Video const& src, int& width, int& height)
  {
      width  = std::min (dest.m_impl->width,  src.m_impl->width);
      height = std::min (dest.m_impl->height, src.m_impl->height);
  }

  void VideoConvert::index8_to_rgb16 (Video& dest, Video const& src)
  {
      rgb16_t* dbuf = static_cast<rgb16_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      rgb16_t colors[256];

      std::vector<Color> const& src_colors = src.m_impl->palette.colors;

      for(int i = 0; i < 256; i++) {
          colors[i].r = src_colors[i].r >> 3;
          colors[i].g = src_colors[i].g >> 2;
          colors[i].b = src_colors[i].b >> 3;
      }

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = (dest.m_impl->pitch / dest.m_impl->bpp) - w;
      int sdiff = src.m_impl->pitch - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
              *(dbuf++) = colors[*(sbuf++)];
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::index8_to_rgb24 (Video& dest, Video const& src)
  {
      uint8_t* dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      std::vector<Color> const& src_colors = src.m_impl->palette.colors;

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = src.m_impl->pitch - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              *(dbuf++) = src_colors[*(sbuf)].b;
              *(dbuf++) = src_colors[*(sbuf)].g;
              *(dbuf++) = src_colors[*(sbuf)].r;
#else
              *(dbuf++) = src_colors[*(sbuf)].r;
              *(dbuf++) = src_colors[*(sbuf)].g;
              *(dbuf++) = src_colors[*(sbuf)].b;
#endif // VISUAL_LITTLE_ENDIAN
              sbuf++;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::index8_to_argb32 (Video& dest, Video const& src)
  {
      uint32_t *dbuf = static_cast<uint32_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      uint32_t colors[256];

      std::vector<Color> const& src_colors = src.m_impl->palette.colors;

      for (int i = 0; i < 256; ++i) {
          colors[i] =
              255 << 24 |
              src_colors[i].r << 16 |
              src_colors[i].g << 8 |
              src_colors[i].b;
      }

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = (dest.m_impl->pitch >> 2) - w;
      int sdiff = src.m_impl->pitch - w;

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
              *(dbuf++) = colors[*(sbuf++)];
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::rgb16_to_index8 (Video& dest, Video const& src)
  {
      uint8_t *dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      rgb16_t const* sbuf = static_cast<rgb16_t const*> (src.get_pixels ());

      std::vector<Color>& dest_colors = dest.m_impl->palette.colors;

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = (src.m_impl->pitch  / src.m_impl->bpp) - w;

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
              uint8_t r = sbuf->r << 3;
              uint8_t g = sbuf->g << 2;
              uint8_t b = sbuf->b << 3;
              sbuf++;

              uint8_t col = (r + g + b) / 3;

              dest_colors[col].r = r;
              dest_colors[col].g = g;
              dest_colors[col].b = b;

              *(dbuf++) = col;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::rgb16_to_rgb24 (Video& dest, Video const& src)
  {
      uint8_t *dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      rgb16_t const* sbuf = static_cast<rgb16_t const*> (src.get_pixels ());

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = (src.m_impl->pitch  / src.m_impl->bpp) - w;

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              *(dbuf++) = sbuf->b << 3;
              *(dbuf++) = sbuf->g << 2;
              *(dbuf++) = sbuf->r << 3;
#else
              *(dbuf++) = sbuf->r << 3;
              *(dbuf++) = sbuf->g << 2;
              *(dbuf++) = sbuf->b << 3;
#endif /* VISUAL_LITTLE_ENDIAN */
              sbuf++;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::rgb16_to_argb32 (Video& dest, Video const& src)
  {
      uint8_t* dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      rgb16_t const* sbuf = static_cast<rgb16_t const*> (src.get_pixels ());

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = (src.m_impl->pitch  / src.m_impl->bpp) - w;

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              *(dbuf++) = sbuf->b << 3;
              *(dbuf++) = sbuf->g << 2;
              *(dbuf++) = sbuf->r << 3;
              *(dbuf++) = 255;
#else
              *(dbuf++) = 255;
              *(dbuf++) = sbuf->r << 3;
              *(dbuf++) = sbuf->g << 2;
              *(dbuf++) = sbuf->b << 3;
#endif /* VISUAL_LITTLE_ENDIAN */

              sbuf++;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::rgb24_to_index8 (Video& dest, Video const& src)
  {
      uint8_t* dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t*> (src.get_pixels ());

      std::vector<Color>& dest_colors = dest.m_impl->palette.colors;

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = src.m_impl->pitch - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              uint8_t b = *(sbuf++);
              uint8_t g = *(sbuf++);
              uint8_t r = *(sbuf++);
#else
              uint8_t r = *(sbuf++);
              uint8_t g = *(sbuf++);
              uint8_t b = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */

              uint8_t col = (b + g + r) / 3;

              dest_colors[col].r = r;
              dest_colors[col].g = g;
              dest_colors[col].b = b;

              *(dbuf++) = col;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::rgb24_to_rgb16 (Video& dest, Video const& src)
  {
      rgb16_t* dbuf = static_cast<rgb16_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = (src.m_impl->pitch / src.m_impl->bpp) - w;

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              dbuf->b = *(sbuf++) >> 3;
              dbuf->g = *(sbuf++) >> 2;
              dbuf->r = *(sbuf++) >> 3;
#else
              dbuf->r = *(sbuf++) >> 3;
              dbuf->g = *(sbuf++) >> 2;
              dbuf->b = *(sbuf++) >> 3;
#endif
              dbuf++;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::rgb24_to_argb32 (Video& dest, Video const& src)
  {
      uint8_t* dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = src.m_impl->pitch - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = 255;
#else
              *(dbuf++) = 255;
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::argb32_to_index8 (Video& dest, Video const& src)
  {
      uint8_t* dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      std::vector<Color>& dest_colors = dest.m_impl->palette.colors;

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = src.m_impl->pitch - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              uint8_t b = *(sbuf++);
              uint8_t g = *(sbuf++);
              uint8_t r = *(sbuf++);
              sbuf++;
#else
              sbuf++;
              uint8_t r = *(sbuf++);
              uint8_t g = *(sbuf++);
              uint8_t b = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */

              uint8_t col = (r + g + b) / 3;

              dest_colors[col].r = r;
              dest_colors[col].g = g;
              dest_colors[col].b = b;

              *(dbuf++) = col;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::argb32_to_rgb16 (Video& dest, Video const& src)
  {
      rgb16_t* dbuf = static_cast<rgb16_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = (dest.m_impl->pitch / dest.m_impl->bpp) - w;
      int sdiff = src.m_impl->pitch - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              dbuf->b = *(sbuf++) >> 3;
              dbuf->g = *(sbuf++) >> 2;
              dbuf->r = *(sbuf++) >> 3;
              sbuf++;
#else
              sbuf++;
              dbuf->r = *(sbuf++) >> 3;
              dbuf->g = *(sbuf++) >> 2;
              dbuf->b = *(sbuf++) >> 3;
#endif /* VISUAL_LITTLE_ENDIAN */

              dbuf++;
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::argb32_to_rgb24 (Video& dest, Video const& src)
  {
      uint8_t* dbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* sbuf = static_cast<uint8_t const*> (src.get_pixels ());

      int w, h;
      convert_get_smallest (dest, src, w, h);

      int ddiff = dest.m_impl->pitch - (w * dest.m_impl->bpp);
      int sdiff = src.m_impl->pitch  - (w * src.m_impl->bpp);

      for (int y = 0; y < h; y++) {
          for (int x = 0; x < w; x++) {
#if VISUAL_LITTLE_ENDIAN == 1
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
              sbuf++;
#else
              sbuf++;
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
              *(dbuf++) = *(sbuf++);
#endif /* VISUAL_LITTLE_ENDIAN */
          }

          dbuf += ddiff;
          sbuf += sdiff;
      }
  }

  void VideoConvert::flip_pixel_bytes_color16 (Video& dest, Video const& src)
  {
      rgb16_t* destbuf = static_cast<rgb16_t*> (dest.get_pixels ());
      rgb16_t const* srcbuf = static_cast<rgb16_t const*> (src.get_pixels ());

      int pitchdiff = (dest.m_impl->pitch - (dest.m_impl->width * dest.m_impl->bpp)) >> 1;

      for (int y = 0; y < dest.m_impl->height; y++) {
          for (int x = 0; x < dest.m_impl->width; x++) {
              destbuf->b = srcbuf->r;
              destbuf->g = srcbuf->g;
              destbuf->r = srcbuf->b;

              destbuf++;
              srcbuf++;
          }

          destbuf += pitchdiff;
      }
  }

  void VideoConvert::flip_pixel_bytes_color24 (Video& dest, Video const& src)
  {
      uint8_t* destbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* srcbuf = static_cast<uint8_t const*> (src.get_pixels ());

      int pitchdiff = dest.m_impl->pitch - (dest.m_impl->width * dest.m_impl->bpp);

      for (int y = 0; y < dest.m_impl->height; y++) {
          for (int x = 0; x < dest.m_impl->width; x++) {
              destbuf[2] = srcbuf[0];
              destbuf[1] = srcbuf[1];
              destbuf[0] = srcbuf[2];

              destbuf += 3;
              srcbuf  += 3;
          }

          destbuf += pitchdiff;
      }
  }

  void VideoConvert::flip_pixel_bytes_color32 (Video& dest, Video const& src)
  {
      uint8_t* destbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* srcbuf = static_cast<uint8_t const*> (src.get_pixels ());

      int pitchdiff = dest.m_impl->pitch - (dest.m_impl->width * dest.m_impl->bpp);

      for (int y = 0; y < dest.m_impl->height; y++) {
          for (int x = 0; x < dest.m_impl->width; x++) {
              destbuf[0] = srcbuf[3];
              destbuf[1] = srcbuf[2];
              destbuf[2] = srcbuf[1];
              destbuf[3] = srcbuf[0];

              destbuf += 4;
              srcbuf  += 4;
          }

          destbuf += pitchdiff;
      }
  }

} // LV namespace
