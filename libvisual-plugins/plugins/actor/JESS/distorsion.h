#include "jess.h"

void create_tables(JessPrivate *priv);
void rot_hyperbolic_radial(float *n_fx,float *n_fy,float d_alpha, float rad_factor, float cx, float cy);
void rot_cos_radial( float *n_fx,float *n_fy,float d_alpha, float rad_factor, float cx, float cy);
void homothetie_hyperbolic(float *n_fx,float *n_fy, float rad_factor, float cx, float cy);
void homothetie_cos_radial(float *n_fx,float *n_fy, float rad_factor, float cx, float cy);
void noize(float *n_fx,float *n_fy, float intensity);
