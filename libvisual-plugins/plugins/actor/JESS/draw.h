#include "jess.h"

void analyser(JessPrivate *priv, uint8_t * buffer);
void fusee(JessPrivate *priv, uint8_t * buffer, int new);
void super_spectral(JessPrivate *priv, uint8_t * buffer);
void super_spectral_balls(JessPrivate *priv, uint8_t * buffer);
void grille_3d(JessPrivate *priv, uint8_t *buffer, short data[2][512],float alpha, float beta, float gamma, int persp, int dist_cam);
void l2_grilles_3d(JessPrivate *priv, uint8_t *buffer, short data[2][512],float alpha, float beta, float gamma, int persp, int dist_cam);
void burn_3d(JessPrivate *priv, uint8_t *buffer, short data[2][512],float alpha, float beta, float gamma, int persp, int dist_cam, int mode);
void courbes(JessPrivate *priv, uint8_t *buffer, short data[2][512],uint8_t color, int type);
void sinus_rect(JessPrivate *priv, uint8_t *buffer,int k);
void stars_create_state(JessPrivate *priv, float pos[3][STARS_MAX], int mode);
void stars_manage(JessPrivate *priv, uint8_t *buffer, int new,  float alpha, float beta, float gamma, int persp, int dist_cam);
