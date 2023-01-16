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

struct Glyph
{
    FT_Vector pos;
    FT_Glyph  ftGlyph;
};

VisVideo *rasteriseText(FT_Face face, const string &text);

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

  int length = std::min(int(std::strlen(str)), ptsNum/50);

  auto bitmap = rasteriseText(ftFace, std::string(str, length));
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

VisVideo *rasteriseText(FT_Face face, const std::string &text)
{
  // This is a very simple FT2 text rasteriser that supports languages with a one-one character to glyph mapping.
  // Anything else would require a text shaping engine like Harfbuzz.

  FT_Error error;

  bool useKerning = FT_HAS_KERNING(face);

  std::size_t charCount = text.length();

  std::vector<Glyph> glyphs;
  glyphs.reserve(charCount);

  auto ftGlyphSlot = face->glyph;

  // Convert text string into a series of glyphs.

  int penX = 0;
  int penY = 0;
  FT_UInt prevFtGlyphIndex = 0;

  for(auto text_char : text)
    {
      auto ftGlyphIndex = FT_Get_Char_Index(face, text_char);

      error = FT_Load_Glyph(face, ftGlyphIndex, FT_LOAD_DEFAULT);
      if(error)
        continue;

      FT_Glyph ftGlyph = nullptr;

      error = FT_Get_Glyph(ftGlyphSlot, &ftGlyph);
      if(error)
        continue;

      if(useKerning && prevFtGlyphIndex && ftGlyphIndex)
        {
          FT_Vector delta;
          FT_Get_Kerning(face, prevFtGlyphIndex, ftGlyphIndex, FT_KERNING_DEFAULT, &delta);
          penX += delta.x >> 6;
        }

      glyphs.push_back({ FT_Vector { penX, penY }, ftGlyph });

      penX += ftGlyphSlot->advance.x >> 6;

      prevFtGlyphIndex = ftGlyphIndex;
    }

  // Calculate bounding box of text rasterization.
  //
  // NOTE: According to FT2 documentation, FT_Glyph_Get_CBox() does not account
  // for anti-aliasing. We don't really care because the text isn't rendered for
  // display.

  FT_BBox textBBox
    {
      std::numeric_limits<int>::max(),
      std::numeric_limits<int>::max(),
      std::numeric_limits<int>::min(),
      std::numeric_limits<int>::min()
    };

  for(auto const& glyph : glyphs)
    {
      FT_BBox glyphBBox { 0, 0, 0, 0 };
      FT_Glyph_Get_CBox(glyph.ftGlyph, ft_glyph_bbox_pixels, &glyphBBox);

      glyphBBox.xMin += glyph.pos.x;
      glyphBBox.yMin += glyph.pos.y;
      glyphBBox.xMax += glyph.pos.x;
      glyphBBox.yMax += glyph.pos.y;

      textBBox.xMin = std::min(textBBox.xMin, glyphBBox.xMin);
      textBBox.yMin = std::min(textBBox.yMin, glyphBBox.yMin);
      textBBox.xMax = std::max(textBBox.xMax, glyphBBox.xMax);
      textBBox.yMax = std::max(textBBox.yMax, glyphBBox.yMax);
    }

  if(textBBox.xMin > textBBox.xMax)
    return nullptr;

  // Rasterize text.

  int textWidth = textBBox.xMax - textBBox.xMin;
  int textHeight = textBBox.yMax - textBBox.yMin;

  auto textBitmap = visual_video_new_with_buffer(textWidth, textHeight, VISUAL_VIDEO_DEPTH_8BIT);

  for(auto const& glyph : glyphs)
    {
      FT_Vector pen { glyph.pos.x << 6, glyph.pos.y << 6 };

      auto ftGlyph = glyph.ftGlyph;

      error = FT_Glyph_To_Bitmap(&ftGlyph, FT_RENDER_MODE_NORMAL, &pen, 0);
      if(!error)
        {
          auto bitmapFtGlyph = reinterpret_cast<FT_BitmapGlyph>(ftGlyph);
          auto bitmap = bitmapFtGlyph->bitmap;

          auto source = visual_video_new_wrap_buffer(bitmap.buffer,
                                                     false,
                                                     bitmap.width,
                                                     bitmap.rows,
                                                     VISUAL_VIDEO_DEPTH_8BIT,
                                                     bitmap.pitch);

          visual_video_blit(textBitmap, source, bitmapFtGlyph->left, textHeight - bitmapFtGlyph->top, 255);

          visual_video_unref(source);

          FT_Done_Glyph(ftGlyph);
        }
    }

  // Free all glyphs
  for(auto const& glyph : glyphs)
    FT_Done_Glyph(glyph.ftGlyph);

  return textBitmap;
}

