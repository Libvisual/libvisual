#ifndef _OINK_GFX_BLUR_H
#define _OINK_GFX_BLUR_H

#include "oinksie.h"

void _oink_gfx_blur_fade (OinksiePrivate *priv, uint8_t *buf, int fade);
void _oink_gfx_blur_simple (OinksiePrivate *priv, uint8_t *buf);
void _oink_gfx_blur_middle (OinksiePrivate *priv, uint8_t *buf);
void _oink_gfx_blur_midstrange (OinksiePrivate *priv, uint8_t *buf);

#endif /* _OINK_GFX_BLUR_H */
