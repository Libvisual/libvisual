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

  VisColor *visual_color_clone (VisColor* src)
  {
      visual_return_val_if_fail (src != nullptr, nullptr);

      return new LV::Color (*src);
  }

  void visual_color_copy (VisColor* dest, VisColor* src)
  {
      visual_return_if_fail (dest != nullptr);
      visual_return_if_fail (src  != nullptr);

      *dest = *src;
  }

  void visual_color_set (VisColor* color, uint8_t r, uint8_t g, uint8_t b)
  {
      visual_return_if_fail (color != nullptr);

      color->set (r, g, b);
  }

  void visual_color_set_rgba (VisColor* color, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
  {
      visual_return_if_fail (color != nullptr);

      color->set (r, g, b, a);
  }

  int visual_color_compare (VisColor* src1, VisColor* src2)
  {
      visual_return_val_if_fail (src1 != nullptr, FALSE);
      visual_return_val_if_fail (src2 != nullptr, FALSE);

      return ( *src1 == *src2 );
  }

  void visual_color_set_hsv (VisColor* color, float h, float s, float v)
  {
      visual_return_if_fail (color != nullptr);

      color->set_hsv (h, s, v);
  }

  void visual_color_get_hsv (VisColor* color, float* h, float* s, float* v)
  {
      visual_return_if_fail (color != nullptr);

      color->get_hsv (*h, *s, *v);
  }

  void visual_color_set_from_uint32 (VisColor* color, uint32_t rgba)
  {
      visual_return_if_fail (color != nullptr);

      color->set_from_uint32 (rgba);
  }

  void visual_color_set_from_uint16 (VisColor* color, uint16_t rgb)
  {
      visual_return_if_fail (color != nullptr);

      color->set_from_uint16 (rgb);
  }

  uint32_t visual_color_to_uint32 (VisColor* color)
  {
      visual_return_val_if_fail (color != nullptr, 0);

      return color->to_uint32 ();
  }

  uint16_t visual_color_to_uint16 (VisColor* color)
  {
      visual_return_val_if_fail (color != nullptr, 0);

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
