#include "config.h"
#include "lv_video_blit.hpp"
#include "lv_video_private.hpp"
#include "lv_common.h"
#include "lv_cpu.h"

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

  void VideoBlit::blit_overlay_noalpha (Video& dest, Video const& src)
  {
      uint8_t* destbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* srcbuf = static_cast<uint8_t const*> (src.get_pixels ());

      /* src and dest are completely equal, do one big mem copy instead of a per line mem copy.
       * Also check if the pitch is equal to it's width * bpp, this is because of subregions. */
      if (visual_video_compare_attrs (dest, src) && (src->pitch == (src->width * src->bpp))) {
          visual_mem_copy (destbuf, srcbuf, visual_video_get_size (dest));
          return;
      }

      for (int y = 0; y < src->height; y++) {
          visual_mem_copy (destbuf, srcbuf, src->width * src->bpp);

          destbuf += dest->pitch;
          srcbuf += src->pitch;
      }
  }

  void VideoBlit::blit_overlay_alphasrc (Video& dest, Video const& src)
  {
      uint8_t* destbuf = dest.get_pixels ();
      uint8_t const* srcbuf  = src.get_pixels ();

      if (visual_cpu_has_mmx ()) {
          blit_overlay_alphasrc_mmx (dest, src);
          return;
      }

      for (int y = 0; y < src->height; y++) {
          for (int x = 0; x < src->width; x++) {
              uint8_t alpha = srcbuf[3];

              destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
              destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
              destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

              destbuf += dest->bpp;
              srcbuf  += src->bpp;
          }

          destbuf += dest->pitch - (dest->width * dest->bpp);
          srcbuf  += src->pitch  - (src->width  * src->bpp);
      }
  }

  void VideoBlit::blit_overlay_colorkey (Video& dest, Video const& src)
  {
      unsigned int pixel_count = dest->width * dest->height;

      if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
          uint8_t *destbuf = dest.get_pixels ();
          uint8_t *srcbuf = src.get_pixels ();
          Palette const* pal = src->pal;

          if (!pal) {
              blit_overlay_noalpha (dest, src);
              return;
          }

          int index = pal->find_color (*src->colorkey);

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (*srcbuf != index)
                  *destbuf = *srcbuf;

              destbuf++;
              srcbuf++;
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
          uint16_t *destbuf = dest.get_pixels ();
          uint16_t *srcbuf = src.get_pixels ();
          uint16_t color = src->colorkey->to_uint16 ();

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (color != *srcbuf)
                  *destbuf = *srcbuf;

              destbuf++;
              srcbuf++;
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
          uint8_t *destbuf = dest.get_pixels ();
          uint8_t *srcbuf = src.get_pixels ();
          uint8_t r = src->colorkey->r;
          uint8_t g = src->colorkey->g;
          uint8_t b = src->colorkey->b;

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (b != srcbuf[0] && g != srcbuf[1] && r != srcbuf[2]) {
                  destbuf[0] = srcbuf[0];
                  destbuf[1] = srcbuf[1];
                  destbuf[2] = srcbuf[2];
              }

              destbuf += 3;
              srcbuf  += 3;
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
          uint32_t *destbuf = dest.get_pixels ();
          uint32_t *srcbuf = src.get_pixels ();
          uint32_t color = src->colorkey->to_uint32 ();

          for (unsigned int i = 0; i < pixel_count; i++) {
              if (color != *srcbuf)
                  *destbuf = *srcbuf;

              destbuf++;
              srcbuf++;
          }
      }
  }

  void VideoBlit::blit_overlay_surfacealpha (Video& dest, Video const& src)
  {
      uint8_t *destbuf = dest.get_pixels ();
      uint8_t *srcbuf = src.get_pixels ();
      uint8_t alpha = src->alpha;

      if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {

          for (int y = 0; y < src->height; y++) {
              for (int x = 0; x < src->width; x++) {
                  *destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

                  destbuf += dest->bpp;
                  srcbuf += src->bpp;
              }

              destbuf += dest->pitch - (dest->width * dest->bpp);
              srcbuf  += src->pitch  - (src->width  * src->bpp);
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {

          for (int y = 0; y < src->height; y++) {
              rgb16_t *destr = (rgb16_t *) destbuf;
              rgb16_t *srcr  = (rgb16_t *) srcbuf;

              for (int x = 0; x < src->width; x++) {
                  destr->r = (alpha * (srcr->r - destr->r) >> 8) + destr->r;
                  destr->g = (alpha * (srcr->g - destr->g) >> 8) + destr->g;
                  destr->b = (alpha * (srcr->b - destr->b) >> 8) + destr->b;

                  destr++;
                  srcr++;
              }

              destbuf += dest->pitch;
              srcbuf += src->pitch;
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {

          for (int y = 0; y < src->height; y++) {
              for (int x = 0; x < src->width; x++) {
                  destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                  destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                  destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

                  destbuf += dest->bpp;
                  srcbuf  += src->bpp;
              }

              destbuf += dest->pitch - (dest->width * dest->bpp);
              srcbuf  += src->pitch  - (src->width  * src->bpp);
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {

          for (int y = 0; y < src->height; y++) {
              for (int x = 0; x < src->width; x++) {
                  destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                  destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                  destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];

                  destbuf += dest->bpp;
                  srcbuf  += src->bpp;
              }

              destbuf += dest->pitch - (dest->width * dest->bpp);
              srcbuf  += src->pitch  - (src->width  * src->bpp);
          }
      }
  }

  void VideoBlit::blit_overlay_surfacealphacolorkey (Video& dest, Video const& src)
  {
      uint8_t *destbuf = static_cast<uint8_t*> (dest.get_pixels ());
      uint8_t const* srcbuf = static_cast<uint8_t const*> (src.get_pixels ());

      uint8_t alpha = src->alpha;

      if (dest->depth == VISUAL_VIDEO_DEPTH_8BIT) {
          Palette *pal = src->pal;

          if (!pal) {
              blit_overlay_noalpha (dest, src);
              return;
          }

          int index = pal->find_color (*src->colorkey);

          for (int y = 0; y < src->height; y++) {
              for (int x = 0; x < src->width; x++) {
                  if (*srcbuf != index)
                      *destbuf = ((alpha * (*srcbuf - *destbuf) >> 8) + *destbuf);

                  destbuf += dest->bpp;
                  srcbuf += src->bpp;
              }

              destbuf += dest->pitch - (dest->width * dest->bpp);
              srcbuf += src->pitch - (src->width * src->bpp);
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_16BIT) {
          uint16_t color = src->colorkey->to_uint16 ();

          for (int y = 0; y < src->height; y++) {
              rgb16_t* destr = reinterpret_cast<rgb16_t*> (destbuf);
              rgb16_t const* srcr  = reinterpret_cast<rgb16_t const*> (srcbuf):

                  for (int x = 0; x < src->width; x++) {
                      if (color != *((uint16_t *) srcr)) {
                          destr->r = (alpha * (srcr->r - destr->r) >> 8) + destr->r;
                          destr->g = (alpha * (srcr->g - destr->g) >> 8) + destr->g;
                          destr->b = (alpha * (srcr->b - destr->b) >> 8) + destr->b;
                      }

                      destr++;
                      srcr++;
                  }

              destbuf += dest->pitch;
              srcbuf += src->pitch;
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_24BIT) {
          uint8_t r = src->colorkey->r;
          uint8_t g = src->colorkey->g;
          uint8_t b = src->colorkey->b;

          for (int y = 0; y < src->height; y++) {
              for (int x = 0; x < src->width; x++) {
                  if (b != srcbuf[0] && g != srcbuf[1] && r != srcbuf[2]) {
                      destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                      destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                      destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];
                  }

                  destbuf += dest->bpp;
                  srcbuf += src->bpp;
              }

              destbuf += dest->pitch - (dest->width * dest->bpp);
              srcbuf += src->pitch - (src->width * src->bpp);
          }

      } else if (dest->depth == VISUAL_VIDEO_DEPTH_32BIT) {
          uint32_t color = src->colorkey->to_uint32 ();

          for (int y = 0; y < src->height; y++) {
              for (int x = 0; x < src->width; x++) {
                  if (color == *reinterpret_cast<uint32_t*> (destbuf)) {
                      destbuf[0] = (alpha * (srcbuf[0] - destbuf[0]) >> 8) + destbuf[0];
                      destbuf[1] = (alpha * (srcbuf[1] - destbuf[1]) >> 8) + destbuf[1];
                      destbuf[2] = (alpha * (srcbuf[2] - destbuf[2]) >> 8) + destbuf[2];
                  }

                  destbuf += dest->bpp;
                  srcbuf += src->bpp;
              }

              destbuf += dest->pitch - (dest->width * dest->bpp);
              srcbuf += src->pitch - (src->width * src->bpp);
          }
      }
  }

} // LV namespace
