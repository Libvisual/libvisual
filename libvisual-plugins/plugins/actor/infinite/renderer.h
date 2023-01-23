#ifndef _INF_RENDERER_H
#define _INF_RENDERER_H

#include "main.h"

/* renderer.c */
void _inf_init_renderer(InfinitePrivate *priv);
void _inf_init_display(InfinitePrivate *priv);
void _inf_renderer(InfinitePrivate *priv);
void _inf_close_renderer(InfinitePrivate *priv);

/* file.c */
void _inf_load_random_effect(InfinitePrivate *priv, t_effect *effect);

#endif /* _INF_RENDERER_H */
