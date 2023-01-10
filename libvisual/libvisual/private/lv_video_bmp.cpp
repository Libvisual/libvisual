/* Libvisual - The audio visualisation framework.
 *
 * Copyright (C) 2012      Libvisual team
 *               2004-2006 Dennis Smit
 *
 * Authors: Dennis Smit <ds@nerds-incorporated.org>
 * 	        Sepp Wijnands <sw@nerds-incorporated.org>
 *          Chong Kai Xiong <kaixiong@codeleft.sg>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "config.h"
#include "lv_video_bmp.hpp"
#include "lv_common.h"
#include "lv_bits.h"

#include <memory>
#include <istream>
#include <cstring>

#define BI_RGB  0
#define BI_RLE8 1
#define BI_RLE4 2

namespace LV {

  namespace {

    #if VISUAL_BIG_ENDIAN == 1
    void flip_byte_order (VideoPtr const& video)
    {
        auto pixel = static_cast<uint8_t*> (video->get_pixels ());
        unsigned int pixel_count = video->get_width () * video->get_height ();

        for (unsigned int i = 0; i < pixel_count; i++) {
            pixel[0] = pixel[2];
            pixel[2] = pixel[0];
            pixel += 3;
        }
    }
    #endif // VISUAL_BIG_ENDIAN

    bool load_uncompressed (std::istream& fp, VideoPtr const& video, int depth)
    {
        auto video_pixels = static_cast<uint8_t*> (video->get_pixels ());
        int video_pitch  = video->get_pitch ();
        int video_height = video->get_height ();

        int pad = (4 - (video_pitch & 3)) & 3;
        auto data = video_pixels + video_height * video_pitch;

        switch (depth) {
            case 24:
            case 8:
                while (data > video_pixels) {
                    data -= video_pitch;

                    if (!fp.read (reinterpret_cast<char*> (data), video_pitch))
                        goto err;

                    if (pad)
                        fp.seekg (pad, std::ios::cur);
                }
                break;

            case 4:
                while (data > video_pixels) {
                    // Unpack 4 bpp pixels aka 2 pixels per byte
                    auto col = data - video_pitch;
                    auto end = reinterpret_cast<uint8_t*> ((intptr_t)data & ~1);
                    data = col;

                    while (col < end) {
                        uint8_t p = fp.get ();
                        *col++ = p >> 4;
                        *col++ = p & 0xf;
                    }

                    if (video_pitch & 1)
                        *col++ = fp.get () >> 4;

                    if (pad)
                        fp.seekg (pad, std::ios::cur);
                }
                break;

            case 1:
                while (data > video_pixels) {
                    /* Unpack 1 bpp pixels aka 8 pixels per byte */
                    auto col = data - video_pitch;
                    auto end = reinterpret_cast<uint8_t*> ((intptr_t)data & ~7);
                    data = col;

                    while (col < end) {
                        uint8_t p = fp.get ();
                        for (int i = 0; i < 8; i++) {
                            *col++ = p >> 7;
                            p <<= 1;
                        }
                    }

                    if (video_pitch & 7) {
                        uint8_t p = fp.get ();
                        uint8_t count = video_pitch & 7;
                        for (int i = 0; i < count; i++) {
                            *col++ = p >> 7;
                            p <<= 1;
                        }
                    }

                    if (pad)
                        fp.seekg (pad, std::ios::cur);
                }
                break;
        }

        return true;

    err:
        visual_log (VISUAL_LOG_ERROR, "Bitmap data is not complete");

        return false;
    }

    bool load_rle (std::istream& fp, VideoPtr const& video, int mode)
    {
        auto video_pixels = static_cast<uint8_t*> (video->get_pixels ());
        int video_pitch  = video->get_pitch ();
        int video_height = video->get_height ();

        auto end = video_pixels + video_height * video_pitch;
        auto col = end - video_pitch;
        int y = video_height - 1;

        bool processing = true;

        do {
            int c = fp.get();

            if (c == EOF)
                goto err;

            if (c) {
                if (y < 0)
                    goto err;

                /* Encoded mode */
                uint8_t p = fp.get (); /* Color */
                if (mode == BI_RLE8) {
                    while (c-- && col < end)
                        *col++ = p;
                } else {
                    int k = c >> 1; /* Even count */
                    while (k-- && col < end - 1) {
                        *col++ = p >> 4;
                        *col++ = p & 0xf;
                    }

                    if (c & 1 && col < end)
                        *col++ = p >> 4;
                }
                continue;
            }

            /* Escape sequence */
            c = fp.get ();
            switch (c) {
                case EOF:
                    goto err;

                case 0: /* End of line */
                    y--;
                    col = video_pixels + video_pitch * y;

                    /* Normally we would error here if y < 0.
                     * However, some encoders apparently emit an
                     * End-Of-Line sequence at the very end of a bitmap.
                     */
                    break;

                case 1: /* End of bitmap */
                    processing = false;
                    break;

                case 2: /* Delta */
                    /* X Delta */
                    col += (uint8_t) fp.get ();

                    /* Y Delta */
                    c = (uint8_t) fp.get ();
                    col -= c * video_pitch;
                    y -= c;

                    if (col < video_pixels)
                        goto err;

                    break;

                default: /* Absolute mode: 3 - 255 */
                    int pad;
                    if (mode == BI_RLE8) {
                        pad = c & 1;
                        while (c-- && col < end)
                            *col++ = fp.get ();
                    } else {
                        pad = ((c + 1) >> 1) & 1;
                        int k = c >> 1; /* Even count */
                        while (k-- && col < end - 1) {
                            uint8_t p = fp.get ();
                            *col++ = p >> 4;
                            *col++ = p & 0xf;
                        }

                        if (c & 1 && col < end)
                            *col++ = fp.get () >> 4;
                    }

                    if (pad)
                        fp.get ();
                    break;

            }
        } while (processing);

        return true;

    err:
        visual_log (VISUAL_LOG_ERROR, "Bitmap data is not complete");

        return false;
    }

  } // anonymous namespace

  VideoPtr bitmap_load_bmp (std::istream& fp)
  {
      auto saved_stream_pos = fp.tellg ();

      /* The win32 BMP header */
      uint32_t bf_size = 0;
      uint32_t bf_bits = 0;

      /* The win32 BITMAPINFOHEADER */
      int32_t bi_size = 0;
      int32_t bi_width = 0;
      int32_t bi_height = 0;
      int16_t bi_bitcount = 0;
      uint32_t bi_compression;
      uint32_t bi_clrused;

      /* Worker vars */
      uint8_t depth = 24;

      std::unique_ptr<Palette> palette;

      /* Read the magic string */
      char magic[2];
      fp.read (magic, 2);
      if (std::strncmp (magic, "BM", 2) != 0) {
          visual_log (VISUAL_LOG_WARNING, "Not a bitmap file");
          fp.seekg (saved_stream_pos);
          return nullptr;
      }

      /* Read the file size */
      fp.read (reinterpret_cast<char*> (&bf_size), 4);
      bf_size = VISUAL_ENDIAN_LEI32 (bf_size);

      /* Skip past the reserved bits */
      fp.seekg (4, std::ios::cur);

      /* Read the offset bits */
      fp.read (reinterpret_cast<char*> (&bf_bits), 4);
      bf_bits = VISUAL_ENDIAN_LEI32 (bf_bits);

      /* Read the info structure size */
      fp.read (reinterpret_cast<char*> (&bi_size), 4);
      bi_size = VISUAL_ENDIAN_LEI32 (bi_size);

      if (bi_size == 12) {
          /* And read the width, height */
          fp.read (reinterpret_cast<char*> (&bi_width), 2);
          fp.read (reinterpret_cast<char*> (&bi_height), 2);
          bi_width = VISUAL_ENDIAN_LEI16 (bi_width);
          bi_height = VISUAL_ENDIAN_LEI16 (bi_height);

          /* Skip over the planet */
          fp.seekg (2, std::ios::cur);

          /* Read the bits per pixel */
          fp.read (reinterpret_cast<char*> (&bi_bitcount), 2);
          bi_bitcount = VISUAL_ENDIAN_LEI16 (bi_bitcount);
          bi_compression = BI_RGB;
          bi_clrused = 0;
      } else {
          /* And read the width, height */
          fp.read (reinterpret_cast<char*> (&bi_width), 4);
          fp.read (reinterpret_cast<char*> (&bi_height), 4);
          bi_width = VISUAL_ENDIAN_LEI32 (bi_width);
          bi_height = VISUAL_ENDIAN_LEI32 (bi_height);

          /* Skip over the planet */
          fp.seekg (2, std::ios::cur);

          /* Read the bits per pixel */
          fp.read (reinterpret_cast<char*> (&bi_bitcount), 2);
          bi_bitcount = VISUAL_ENDIAN_LEI16 (bi_bitcount);

          /* Read the compression flag */
          fp.read (reinterpret_cast<char*> (&bi_compression), 4);
          bi_compression = VISUAL_ENDIAN_LEI32 (bi_compression);

          /* Skip over the nonsense we don't want to know */
          fp.seekg (12, std::ios::cur);

          /* Number of colors in palette */
          fp.read (reinterpret_cast<char*> (&bi_clrused), 4);
          bi_clrused = VISUAL_ENDIAN_LEI32 (bi_clrused);

          /* Skip over the other nonsense */
          fp.seekg (4, std::ios::cur);
      }

      /* Check if we can handle it */
      if (bi_bitcount != 1 && bi_bitcount != 4 && bi_bitcount != 8 && bi_bitcount != 24) {
          visual_log (VISUAL_LOG_ERROR, "Only bitmaps with 1, 4, 8 or 24 bits per pixel are supported");
          fp.seekg (saved_stream_pos);
          return nullptr;
      }

      if (bi_compression > 3) {
          visual_log (VISUAL_LOG_ERROR, "Bitmap uses an invalid or unsupported compression scheme");
          fp.seekg (saved_stream_pos);
          return nullptr;
      }

      /* Load the palette */
      if (bi_bitcount < 24) {
          if (bi_clrused == 0) {
              /* When the colors used variable is zero, use the
               * maximum number of palette colors allowed for the specified depth. */
              bi_clrused = 1 << bi_bitcount;
          }

          /* Always allocate 256 palette entries.
           * Depth transformation depends on this */
          palette = std::make_unique<LV::Palette> (256);

          if (bi_size == 12) {
              for (uint32_t i = 0; i < bi_clrused; i++) {
                  palette->colors[i].b = fp.get ();
                  palette->colors[i].g = fp.get ();
                  palette->colors[i].r = fp.get ();
              }
          } else {
              for (uint32_t i = 0; i < bi_clrused; i++) {
                  palette->colors[i].b = fp.get ();
                  palette->colors[i].g = fp.get ();
                  palette->colors[i].r = fp.get ();
                  fp.seekg (1, std::ios::cur);
              }
          }
      }

      /* Use 8 bpp for all bit depths under 24 bits */
      if (bi_bitcount < 24)
          depth = 8;

      /* Make the target VisVideo ready for use */
      auto video = Video::create (bi_width, bi_height, visual_video_depth_from_bpp (depth));

      if (palette)
          video->set_palette (*palette);

      /* Set to the beginning of image data, note that MickeySoft likes stuff upside down .. */
      fp.seekg (bf_bits, std::ios::beg);

      /* Load image data */
      bool result = false;
      switch (bi_compression) {
          case BI_RGB:
              result = load_uncompressed (fp, video, bi_bitcount);
              #if VISUAL_BIG_ENDIAN == 1
              if (error == VISUAL_OK)
                  flip_byte_order (video);
              #endif
              break;

          case BI_RLE4:
              result = load_rle (fp, video, BI_RLE4);
              break;

          case BI_RLE8:
              result = load_rle (fp, video, BI_RLE8);
              break;
      }

      if (!result) {
          fp.seekg (saved_stream_pos);
          return nullptr;
      }

      return video;
  }

} // LV namespace
