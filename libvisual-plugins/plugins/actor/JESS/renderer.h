#include "jess.h"

int nice(int inc);  
void *renderer(JessPrivate *priv);
void fade(float variable, uint8_t * dim);
void draw_mode(JessPrivate *priv, int mode);
void on_beat(JessPrivate *priv, int beat);
void on_reprise(JessPrivate *priv);
void copy_and_fade(JessPrivate *priv, float factor);
void render_deformation(JessPrivate *priv, int defmode);
void render_blur(JessPrivate *priv, int blur);
void manage_dynamic_and_states_open(JessPrivate *priv);
void manage_states_close(JessPrivate *priv);
