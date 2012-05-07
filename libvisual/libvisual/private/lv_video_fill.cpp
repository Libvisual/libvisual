#include "config.h"
#include "lv_video_fill.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"

#pragma pack(1)

typedef struct {
#ifdef VISUAL_LITTLE_ENDIAN
    uint16_t b:5, g:6, r:5;
#else
    uint16_t r:5, g:6, b:5;
#endif
} rgb16_t;

#pragma pack()

namespace LV {

  void VideoFill::fill_color_index8 (Video& video, Color const& color)
  {
      uint8_t* buf = static_cast<uint8_t*> (video.get_pixels ());

      int8_t col = ((color.r + color.g + color.b) / 3);

      for (int y = 0; y < video.m_impl->height; y++) {
          visual_mem_set (buf, col, video.m_impl->width);

          buf += video.m_impl->pitch;
      }
  }

  void VideoFill::fill_color_rgb16 (Video& video, Color const& color)
  {
      uint16_t* buf = static_cast<uint16_t*> (video.get_pixels ());

      int16_t col;

      rgb16_t *col16 = reinterpret_cast<rgb16_t*> (&col);
      col16->r = color.r >> 3;
      col16->g = color.g >> 2;
      col16->b = color.b >> 3;

      for (int y = 0; y < video.m_impl->height; y++) {
          visual_mem_set16 (buf, col, video.m_impl->width);

          buf += (video.m_impl->pitch / video.m_impl->bpp);
      }
  }

  void VideoFill::fill_color_rgb24 (Video& video, Color const& color)
  {
      uint8_t* rbuf = static_cast<uint8_t*> (video.get_pixels ());

      int32_t cola =
          (color.b << 24) |
          (color.g << 16) |
          (color.r << 8) |
          (color.b);
      int32_t colb =
          (color.g << 24) |
          (color.r << 16) |
          (color.b << 8) |
          (color.g);
      int32_t colc =
          (color.r << 24) |
          (color.b << 16) |
          (color.g << 8) |
          (color.r);

      for (int y = 0; y < video.m_impl->height; y++) {
          uint32_t* buf = reinterpret_cast<uint32_t*> (rbuf);

          for (int x = video.m_impl->width; x >= video.m_impl->bpp; x -= video.m_impl->bpp) {
              *(buf++) = cola;
              *(buf++) = colb;
              *(buf++) = colc;
          }

          uint8_t *buf8 = reinterpret_cast<uint8_t*> (buf);
          *(buf8++) = color.b;
          *(buf8++) = color.g;
          *(buf8++) = color.r;

          rbuf += video.m_impl->pitch;
      }
  }

  void VideoFill::fill_color_argb32 (Video& video, Color const& color)
  {
      uint32_t *buf = static_cast<uint32_t*> (video.get_pixels ());

      uint32_t col = (color.a << 24) | (color.r << 16) | (color.g << 8) | color.b;

      for (int y = 0; y < video.m_impl->height; y++) {
          visual_mem_set32 (buf, col, video.m_impl->width);

          buf += (video.m_impl->pitch / video.m_impl->bpp);
      }
  }

} // LV namespace
