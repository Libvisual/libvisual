#include "jess.h"

void boule_random (JessPrivate *priv, uint8_t * buffer, int x, int y, int r, uint8_t color);
void cercle(JessPrivate *priv, uint8_t *buffer, int h, int k, int y, uint8_t color);
void cercle_32(JessPrivate *priv, uint8_t *buffer, int h, int k, int y, uint8_t color);
void boule(JessPrivate *priv, uint8_t *buffer, int x, int y, int r, uint8_t color);
void droite(JessPrivate *priv, uint8_t *buffer,int x1, int y1, int x2, int y2, uint8_t color);
void tracer_point_add(JessPrivate *priv, uint8_t *buffer, int x, int y, uint8_t color);
void tracer_point_add_32(JessPrivate *priv, uint8_t *buffer, int x, int y, uint8_t color);

void ball_init(JessPrivate *priv);
void boule_no_add(uint8_t *buffer, int x, int y, int r, uint8_t color);
void cercle_no_add(uint8_t *buffer, int h, int k, int y, uint8_t color);
void tracer_point_no_add(uint8_t *buffer, int x, int y, uint8_t color);
void ball(JessPrivate *priv, uint8_t *buffer, int x, int y, int r, uint8_t color);
