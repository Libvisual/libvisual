#include "config.h"
#include "lv_rectangle.h"
#include "lv_common.h"

extern "C" {

  VisRectangle *visual_rectangle_new (int x, int y, int width, int height)
  {
      return new LV::Rect (x, y, width, height);
  }

  VisRectangle *visual_rectangle_new_empty ()
  {
      return new LV::Rect;
  }

  void visual_rectangle_free (VisRectangle *self)
  {
      delete self;
  }

  void visual_rectangle_set (VisRectangle *self, int x, int y, int width, int height)
  {
      (void)x;
      (void)y;
      (void)width;
      (void)height;

      visual_return_if_fail (self != nullptr);
  }

  void visual_rectangle_copy (VisRectangle *self, VisRectangle *src)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (src  != nullptr);

      *self = *src;
  }

  VisRectangle *visual_rectangle_clone (VisRectangle *self)
  {
      visual_return_val_if_fail (self != nullptr, nullptr);

      return visual_rectangle_new (self->x, self->y, self->width, self->height);
  }

  void visual_rectangle_set_x (VisRectangle *self, int x)
  {
      visual_return_if_fail (self != nullptr);

      self->x = x;
  }

  int visual_rectangle_get_x (VisRectangle *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->x;
  }

  void visual_rectangle_set_y (VisRectangle *self, int y)
  {
      visual_return_if_fail (self != nullptr);

      self->y = y;
  }

  int visual_rectangle_get_y (VisRectangle *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->y;
  }

  void visual_rectangle_set_width (VisRectangle *self, int width)
  {
      visual_return_if_fail (self != nullptr);

      self->width = width;
  }

  int visual_rectangle_get_width  (VisRectangle *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->width;
  }

  void visual_rectangle_set_height (VisRectangle *self, int height)
  {
      visual_return_if_fail (self != nullptr);

      self->height = height;
  }

  int visual_rectangle_get_height (VisRectangle *self)
  {
      visual_return_val_if_fail (self != nullptr, 0);

      return self->height;
  }

  int visual_rectangle_is_empty (VisRectangle *self)
  {
      visual_return_val_if_fail (self != nullptr, TRUE);

      return self->empty ();
  }

  int visual_rectangle_intersects (VisRectangle *self, VisRectangle *src)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);
      visual_return_val_if_fail (src  != nullptr, FALSE);

      return self->intersects (*src);
  }

  int visual_rectangle_contains_point (VisRectangle *self, int x, int y)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);

      return self->contains (x, y);
  }

  int visual_rectangle_contains_rect (VisRectangle *self, VisRectangle *src)
  {
      visual_return_val_if_fail (self != nullptr, FALSE);
      visual_return_val_if_fail (src  != nullptr, FALSE);

      return self->contains (*src);
  }

  void visual_rectangle_clip (VisRectangle *result, VisRectangle *bounds, VisRectangle *src)
  {
      visual_return_if_fail (result != nullptr);
      visual_return_if_fail (bounds != nullptr);
      visual_return_if_fail (src    != nullptr);

      *result = bounds->clip (*src);
  }

  void visual_rectangle_normalize (VisRectangle *self)
  {
      visual_return_if_fail (self != nullptr);

      self->normalize ();
  }

  void visual_rectangle_normalize_to (VisRectangle *self, VisRectangle *src)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (src  != nullptr);

      self->normalize_to (*src);
  }

  void visual_rectangle_denormalize_point (VisRectangle *self, float fx, float fy, int32_t *x, int32_t *y)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (x != nullptr);
      visual_return_if_fail (y != nullptr);

      self->denormalize_point (fx, fy, *x, *y);
  }

  void visual_rectangle_denormalize_point_neg (VisRectangle *self, float fx, float fy, int32_t *x, int32_t *y)
  {
      visual_return_if_fail (self != nullptr);
      visual_return_if_fail (x != nullptr);
      visual_return_if_fail (y != nullptr);

      self->denormalize_point_neg (fx, fy, *x, *y);
  }

  void visual_rectangle_denormalize_points (VisRectangle *self, const float *fxlist, const float *fylist, int32_t *xlist, int32_t *ylist, unsigned int size)
  {
      visual_return_if_fail (self != nullptr);

      self->denormalize_points (fxlist, fylist, xlist, ylist, size);
  }

  void visual_rectangle_denormalise_points_neg (VisRectangle *self, const float *fxlist, const float *fylist, int32_t *xlist, int32_t *ylist, unsigned int size)
  {
      visual_return_if_fail (self != nullptr);

      self->denormalize_points_neg (fxlist, fylist, xlist, ylist, size);
  }

} // extern C
