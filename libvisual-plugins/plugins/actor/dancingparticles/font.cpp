#include "font.h"
#include "etoile.h"
#include "libvisual/lv_log.h"
#include "libvisual/lv_video.h"
#include <cstring>
#include <algorithm>
#include <limits>
#include <vector>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

const char *fontPath = DATA_DIR "/font.pfb";
const int   fontSize = 25; // Does this match the T1lib size parameter?

static FT_Library ftLibrary = nullptr;
static FT_Face ftFace = nullptr;

VisVideo *rasteriseText(FT_Face face, const char *text);

bool initFontRasterizer()
{
  FT_Error error = FT_Init_FreeType(&ftLibrary);
  if(error)
    {
      visual_log(VISUAL_LOG_WARNING, "Unable to initialize FreeType2.");
      return false;
    }

  error = FT_New_Face(ftLibrary, fontPath, 0, &ftFace);
  if(error)
    {
      visual_log(VISUAL_LOG_WARNING, "Unable to load font (%s).", fontPath);
      return false;
    }

  FT_Set_Pixel_Sizes(ftFace, 0, fontSize);

  return true;
}

void destroyFontRasterizer()
{
  if (ftLibrary)
    {
      FT_Done_FreeType(ftLibrary);
    }
}

void loadString(const char *str)
{
  if(!ftLibrary || !ftFace)
      return;

  int length = std::strlen(str);
  if(length > (ptsNum/50))
	length = ptsNum/50;

  auto bitmap = rasteriseText(ftFace, str);
  auto pixels = static_cast<uint8_t const*>(visual_video_get_pixels(bitmap));

  int width = visual_video_get_width(bitmap);
  int height = visual_video_get_height(bitmap);
  int pitch = visual_video_get_pitch(bitmap);

  int maxscore = 0;
  for(int y = 0; y < height ; y++)
    {
      for(int x = 0; x < width ; x++)
        {
          unsigned char c = pixels[y*pitch+x];
          maxscore +=c;
        }
    }

  numCenters = numCenters2 = ptsNum;
  int curscore = 0;
  int lastscore = 0;
  int numPart = 0;
  int scoreToGo = 1;
  for(int y = 0; y < height ; y++)
    {
      for(int x = 0; x < width ; x++)
        {
          unsigned char c = pixels[y*pitch+x];
          int a = 0;
          lastscore = curscore;
          curscore +=c;
          if(c!=0)
            {
              while(lastscore < scoreToGo  && curscore >= scoreToGo)
                {
                  Centers[numPart++]=FloatPoint(x*4-width*2,height*8-y*16+a,0);
                  scoreToGo= (numPart*maxscore)/numCenters;
                  a+=4;
                }
            }
        }
    }
}

VisVideo *rasteriseText(FT_Face face, const char *text)
{
  // This is a very simple FT2 text rasteriser that supports languages with a one-one character to glyph mapping.
  // Anything else would require a text shaping engine like Harfbuzz.

  if(!text)
      return nullptr;

  FT_Error error;

  bool useKerning = FT_HAS_KERNING(face);

  std::size_t charCount = std::strlen(text);

  std::vector<FT_Vector> glyphPos;
  glyphPos.reserve(charCount);

  std::vector<FT_Glyph> glyphs;
  glyphs.reserve(charCount);

  auto glyphSlot = face->glyph;

  // Convert text string into a series of glyphs.

  int penX = 0;
  int penY = 0;
  FT_UInt prevGlyphIndex;

  for(std::size_t i = 0; i < charCount; i++)
    {
      auto glyphIndex = FT_Get_Char_Index(face, text[i]);

      if(useKerning && prevGlyphIndex && glyphIndex)
        {
          FT_Vector delta;
          FT_Get_Kerning(face, prevGlyphIndex, glyphIndex, FT_KERNING_DEFAULT, &delta);
          penX += delta.x >> 6;
        }

      glyphPos.push_back({ penX, penY });

      error = FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT);
      if(error)
          continue;

      glyphs.push_back(nullptr);

      error = FT_Get_Glyph(glyphSlot, &glyphs.back());
      if(error)
          continue;

      penX += glyphSlot->advance.x >> 6;

      prevGlyphIndex = glyphIndex;
    }

  // Calculate bounding box of text rasterization.
  //
  // NOTE: According to FT2 documentation, FT_Glyph_Get_CBox() does not account
  // for anti-aliasing. We don't really care because the text isn't rendered for
  // display.

  FT_BBox textBbox =
    {
      std::numeric_limits<int>::max(),
      std::numeric_limits<int>::max(),
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::min()
    };

  for(std::size_t i = 0; i < glyphs.size(); i++)
    {
      FT_BBox glyphBbox;
      FT_Glyph_Get_CBox(glyphs[i], ft_glyph_bbox_pixels, &glyphBbox);

      glyphBbox.xMin += glyphPos[i].x;
      glyphBbox.yMin += glyphPos[i].y;
      glyphBbox.xMax += glyphPos[i].x;
      glyphBbox.yMax += glyphPos[i].y;

      textBbox.xMin = std::min(textBbox.xMin, glyphBbox.xMin);
      textBbox.yMin = std::min(textBbox.yMin, glyphBbox.yMin);
      textBbox.xMax = std::max(textBbox.xMax, glyphBbox.xMax);
      textBbox.yMax = std::max(textBbox.yMax, glyphBbox.yMax);
    }

  if(textBbox.xMin > textBbox.xMax)
      return nullptr;

  // Rasterize text.

  int textWidth = textBbox.xMax - textBbox.xMin;
  int textHeight = textBbox.yMax - textBbox.yMin;

  auto textBitmap = visual_video_new_with_buffer(textWidth, textHeight, VISUAL_VIDEO_DEPTH_8BIT);

  for(std::size_t i = 0; i < glyphs.size(); i++)
    {
      FT_Vector pen { glyphPos[i].x << 6, glyphPos[i].y << 6 };

      error = FT_Glyph_To_Bitmap(&glyphs[i], FT_RENDER_MODE_NORMAL, &pen, 0);
      if(!error)
        {
          auto bitmapGlyph = reinterpret_cast<FT_BitmapGlyph>(glyphs[i]);
          auto bitmap = bitmapGlyph->bitmap;

          auto source = visual_video_new_wrap_buffer(bitmap.buffer,
                                                     false,
                                                     bitmap.width,
                                                     bitmap.rows,
                                                     VISUAL_VIDEO_DEPTH_8BIT,
                                                     bitmap.pitch);

          visual_video_blit(textBitmap, source, bitmapGlyph->left, textHeight - bitmapGlyph->top, 255);

          visual_video_unref(source);

          FT_Done_Glyph(glyphs[i]);
        }
    }

  return textBitmap;
}
