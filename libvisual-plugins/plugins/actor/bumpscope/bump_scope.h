#include "actor_bumpscope.h"

void __bumpscope_generate_palette (BumpscopePrivate *priv, VisColor *col);
void __bumpscope_generate_phongdat (BumpscopePrivate *priv);
void __bumpscope_render_pcm (BumpscopePrivate *priv, short data[3][512]);
void __bumpscope_init (BumpscopePrivate *priv);
void __bumpscope_cleanup (BumpscopePrivate *priv);

