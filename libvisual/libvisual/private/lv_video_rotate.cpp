#include "config.h"
#include "lv_video_transform.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"

namespace LV {

  void VideoTransform::rotate_90 (Video& dest, Video const& src)
  {
      visual_return_if_fail (dest.m_impl->width == src.m_impl->height);
      visual_return_if_fail (dest.m_impl->height == src.m_impl->width);

      uint8_t const* tsbuf = static_cast<uint8_t*> (src.m_impl->pixel_rows[src.m_impl->height-1]);
      uint8_t const* sbuf = tsbuf;

      for (int y = 0; y < dest.m_impl->height; y++) {
          uint8_t* dbuf = static_cast<uint8_t*> (dest.m_impl->pixel_rows[y]);

          for (int x = 0; x < dest.m_impl->width; x++) {
              for (int i = 0; i < dest.m_impl->bpp; i++) {
                  *(dbuf++) = *(sbuf + i);
              }

              sbuf -= src.m_impl->pitch;
          }

          tsbuf += src.m_impl->bpp;
          sbuf = tsbuf;
      }
  }

  void VideoTransform::rotate_180 (Video& dest, Video const& src)
  {
      int h1 = src.m_impl->height - 1;
      int w1 = (src.m_impl->width - 1) * src.m_impl->bpp;

      visual_return_if_fail (dest.m_impl->width  == src.m_impl->width);
      visual_return_if_fail (dest.m_impl->height == src.m_impl->height);

      for (int y = 0; y < dest.m_impl->height; y++) {
          uint8_t* dbuf = static_cast<uint8_t*> (dest.m_impl->pixel_rows[y]);
          uint8_t const* sbuf = static_cast<uint8_t*> (src.m_impl->pixel_rows[h1 - y]) + w1;

          for (int x = 0; x < dest.m_impl->width; x++) {
              for (int i = 0; i < src.m_impl->bpp; i++) {
                  *(dbuf++) = *(sbuf + i);
              }

              sbuf -= src.m_impl->bpp;
          }
      }
  }

  void VideoTransform::rotate_270 (Video& dest, Video const& src)
  {
      uint8_t const* tsbuf = static_cast<uint8_t*> (src.get_pixels ()) + src.m_impl->pitch - src.m_impl->bpp;
      uint8_t const* sbuf = tsbuf;

      visual_return_if_fail (dest.m_impl->width == src.m_impl->height);
      visual_return_if_fail (dest.m_impl->height == src.m_impl->width);

      for (int y = 0; y < dest.m_impl->height; y++) {
          uint8_t* dbuf = static_cast<uint8_t*> (dest.m_impl->pixel_rows[y]);

          for (int x = 0; x < dest.m_impl->width; x++) {
              for (int i = 0; i < dest.m_impl->bpp; i++) {
                  *(dbuf++) = *(sbuf + i);
              }

              sbuf += src.m_impl->pitch;
          }

          tsbuf -= src.m_impl->bpp;
          sbuf = tsbuf;
      }
  }

  // Mirror functions

  void VideoTransform::mirror_x (Video& dest, Video const& src)
  {
      const int step2 = dest.m_impl->bpp << 1;
      const int w1b = (dest.m_impl->width - 1) * dest.m_impl->bpp;

      for (int y = 0; y < dest.m_impl->height; y++) {
          uint8_t const* sbuf = static_cast<uint8_t*> (src.m_impl->pixel_rows[y]) + w1b;
          uint8_t* dbuf = static_cast<uint8_t*> (dest.m_impl->pixel_rows[y]);

          for (int x = 0; x < dest.m_impl->width; x++) {

              for (int i = 0; i < dest.m_impl->bpp; i++)
                  *(dbuf++) = *(sbuf++);

              sbuf -= step2;
          }
      }
  }

  void VideoTransform::mirror_y (Video& dest, Video const& src)
  {
      int y;

      for (y = 0; y < dest.m_impl->height; y++) {
          visual_mem_copy (dest.m_impl->pixel_rows[y],
                           src.m_impl->pixel_rows[dest.m_impl->height - 1 - y],
                           dest.m_impl->width * dest.m_impl->bpp);
      }
  }

} // LV namespace
