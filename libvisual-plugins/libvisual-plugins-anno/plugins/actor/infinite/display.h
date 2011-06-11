#ifndef _INF_DISPLAY_H
#define _INF_DISPLAY_H

#define NB_PALETTES 5

#include "main.h"

void _inf_display (InfinitePrivate *priv, uint8_t *surf, int pitch);

void _inf_generate_colors(InfinitePrivate *priv);
void _inf_change_color(InfinitePrivate *priv, int old_p,int p,int w);
void _inf_blur(InfinitePrivate *priv, t_interpol* vector_field);
void _inf_spectral(InfinitePrivate *priv, t_effect* current_effect, float data[2][512]);
void _inf_curve(InfinitePrivate *priv, t_effect* current_effect);
void _inf_init_display(InfinitePrivate *priv);

#endif /* _INF_DISPLAY_H */
