#ifndef _OINK_GFX_PALETTE_H
#define _OINK_GFX_PALETTE_H

#include "oinksie.h"

void _oink_gfx_palette_init (OinksiePrivate *priv);
void _oink_gfx_palette_save_old (OinksiePrivate *priv);
void _oink_gfx_palette_transform (OinksiePrivate *priv);
void _oink_gfx_palette_build (OinksiePrivate *priv, uint8_t funky);
void _oink_gfx_palette_color (OinksiePrivate *priv, int color, int red, int green, int blue);
uint8_t _oink_gfx_palette_gradient_gen (OinksiePrivate *priv, uint8_t i, int mode);
void _oink_gfx_palette_build_gradient (OinksiePrivate *priv, uint8_t funky);

#endif /* _OINK_GFX_PALETTE_H */
