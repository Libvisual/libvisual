#ifndef _OINK_GFX_SCOPE_H
#define _OINK_GFX_SCOPE_H

#include "oinksie.h"

void _oink_gfx_scope_balls (OinksiePrivate *priv, uint8_t *buf, int color, int height, int space);
void _oink_gfx_scope_stereo (OinksiePrivate *priv, uint8_t *buf, int color1, int color2, int height, int space, int rotate);
void _oink_gfx_scope_bulbous (OinksiePrivate *priv, uint8_t *buf, int color, int height, int mode);
void _oink_gfx_scope_normal (OinksiePrivate *priv, uint8_t *buf, int color, int height);
void _oink_gfx_scope_circle (OinksiePrivate *priv, uint8_t *buf, int color, int size, int x, int y);

#endif /* _OINK_GFX_SCOPE_H */
