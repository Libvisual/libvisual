#ifndef _OINK_GFX_BACKGROUND_H
#define _OINK_GFX_BACKGROUND_H

void _oink_gfx_background_circles_sine (OinksiePrivate *priv,
		uint8_t *buf, int color, int rotate, int scroll, int stretch, int size);
void _oink_gfx_background_circles_filled (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int number, int distance, int turn, int x, int y);
void _oink_gfx_background_floaters (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int number, int xturn, int yturn, int x, int badd1, int badd2);
void _oink_gfx_background_ball_shooting (OinksiePrivate *priv,
		uint8_t *buf, int color, int distance, int xb, int yb, int x, int y);
void _oink_gfx_background_ball_whirling (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int dia, int rot, int x, int y);
void _oink_gfx_background_dots (OinksiePrivate *priv,
		uint8_t *buf, int color, int number);
void _oink_gfx_background_fill (OinksiePrivate *priv,
		uint8_t *buf, int color);
void _oink_gfx_background_circles_star (OinksiePrivate *priv,
		uint8_t *buf, int color, int size, int tentnr, int ballnr, int badd, int turn, int x, int y);

#endif /* _OINK_GFX_BACKGROUND_H */
