#ifndef _OINK_MISC_H
#define _OINK_MISC_H

int _oink_line_x_next_rad (int x, int length, float rad);
int _oink_line_y_next_rad (int y, int length, float rad);
int _oink_line_length (int x0, int y0, int x1, int y1);
int _oink_line_xory_next_xy (int xory, int length, int x0, int y0, int x1, int y1);

void _oink_pixel_rotate (int *x, int *y, int rot);

#endif /* _OINK_MISC_H */
