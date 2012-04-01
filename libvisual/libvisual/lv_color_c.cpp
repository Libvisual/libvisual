#include "config.h"
#include "lv_color.h"
#include "lv_common.h"

extern "C" {

  VisColor* visual_color_new ()
  {
      return new LV::Color ();
  }

  void visual_color_free (VisColor* color)
  {
      delete color;
  }

  int visual_color_set (VisColor* color, uint8_t r, uint8_t g, uint8_t b)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      color->set (r, g, b);

      return VISUAL_OK;
  }

  int visual_color_set_with_alpha (VisColor* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      color->set (r, g, b, a);

      return VISUAL_OK;
  }

  int visual_color_compare (VisColor* src1, VisColor* src2)
  {
      visual_return_val_if_fail (src1 != NULL, -VISUAL_ERROR_COLOR_NULL);
      visual_return_val_if_fail (src2 != NULL, -VISUAL_ERROR_COLOR_NULL);

      return ( *src1 == *src2 );
  }

  int visual_color_from_hsv (VisColor* color, float h, float s, float v)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      color->set_hsv (h, s, v);

      return VISUAL_OK;
  }

  int visual_color_to_hsv (VisColor* color, float* h, float* s, float* v)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      color->get_hsv (*h, *s, *v);

      return VISUAL_OK;
  }

  int visual_color_copy (VisColor* dest, VisColor* src)
  {
      visual_return_val_if_fail (dest != NULL, -VISUAL_ERROR_COLOR_NULL);
      visual_return_val_if_fail (src  != NULL, -VISUAL_ERROR_COLOR_NULL);

      *dest = *src;

      return VISUAL_OK;
  }

  int visual_color_from_uint32 (VisColor* color, uint32_t rgba)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      color->set_from_uint32 (rgba);

      return VISUAL_OK;
  }

  int visual_color_from_uint16 (VisColor* color, uint16_t rgb)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      color->set_from_uint16 (rgb);

      return VISUAL_OK;
  }

  uint32_t visual_color_to_uint32 (VisColor* color)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      return color->to_uint32 ();
  }

  uint16_t visual_color_to_uint16 (VisColor* color)
  {
      visual_return_val_if_fail (color != NULL, -VISUAL_ERROR_COLOR_NULL);

      return color->to_uint16 ();
  }

  VisColor* visual_color_black ()
  {
	  return const_cast<VisColor*> (&LV::Color::black ());
  }

  VisColor* visual_color_white ()
  {
	  return const_cast<VisColor*> (&LV::Color::white ());
  }

} // C extern
