#ifndef _OINK_GFX_MISC_H
#define _OINK_GFX_MISC_H

#include "oinksie.h"

inline void _oink_gfx_something_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y, int type, int pixmap);
inline void _oink_gfx_pixmap_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y, int type);
inline int _oink_gfx_pixel_get (OinksiePrivate *priv, uint8_t *buf, int x, int y);
inline void _oink_gfx_pixel_set (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y);
inline void _oink_gfx_line (OinksiePrivate *priv, uint8_t *buf, int color, int x0, int y0, int x1, int y1);
inline void _oink_gfx_hline (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y1, int y2);
inline void _oink_gfx_vline (OinksiePrivate *priv, uint8_t *buf, int color, int x, int y1, int y2);
inline void _oink_gfx_circle_filled (OinksiePrivate *priv, uint8_t *buf, int color, int size, int x, int y);
inline void _oink_gfx_circle (OinksiePrivate *priv, uint8_t *buf, int color, int xsize, int ysize, int x, int y);

#endif /* _OINK_GFX_MISC_H */
