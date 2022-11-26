#ifndef NEBULUS_H

#include <math.h>
#include <stdlib.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <libvisual/libvisual.h>

#ifndef PI
#define PI 3.14159265358979323846
#endif
#define PI2 6.28318530717958647692
#define NUM_BANDS 16
#define BEAT_MAX 100
#define T_AVERAGE_SPECTRAL 100
#define FRAME_AVG 30
#define FRAME_BASE 50
#define EFFECT_NUMBER 9

typedef struct nebulus {
  GLuint WIDTH;
  GLuint HEIGHT;
  GLuint effect;
  GLuint beat;
  GLuint old_beat;
  GLuint max_fps;
  GLuint old_max_fps;
  GLfloat energy;
  int paused;
  int fullscreen;
  int mouse;
  int finished;
  int in_thread;
  int init;
  int changement;
  int freeze;
  int config_load;
  int infos;
} nebulus;

typedef struct
{
  char *name;
  int value;
} effect;

typedef struct {
  unsigned int   width;
  unsigned int   height;
  unsigned int   bytes_per_pixel; /* 3:RGB, 4:RGBA */
  unsigned char  pixel_data[64 * 64 * 3 + 1];
} image_s64;

typedef struct {
  unsigned int 	 width;
  unsigned int 	 height;
  unsigned int 	 bytes_per_pixel; /* 3:RGB, 4:RGBA */ 
  unsigned char	 pixel_data[128 * 128 * 3 + 1];
} image_s128;

typedef struct
{
  GLfloat x, y, z;
} glcoord;

extern VisVideo *child_image;
extern VisVideo *energy_image;
extern VisVideo *tentacle_image;
extern VisVideo *tunnel_image;
extern VisVideo *twist_image;
extern VisVideo *background_image;

extern nebulus general;
extern nebulus *point_general;
extern effect my_effect[EFFECT_NUMBER];
extern effect my_effect_old[EFFECT_NUMBER];
extern GLfloat scale, heights[16][16];
extern short pcm_data[512];
extern float framerate;
extern int face_first, tentacles_first, child_first, tunnel_first;
extern int create_knot, create_quadratic;
extern GLuint blurtexture, knotbg, tunnel, tentacle, twist, twistbg;
extern GLuint texchild, childbg, energy;
extern GLuint facedl, cubedl, childdl;
extern GLint *blur_data[128*128*4];
extern GLUquadricObj *myquadratic;
extern GLfloat y_angle, y_speed, y_old_speed;
extern GLfloat x_angle, x_speed;
extern GLfloat z_angle, z_speed;
extern GLfloat speed_bg, old_speed_bg;
extern GLfloat elapsed_time, last_time;
extern char section_name[];

extern void nebulus_config(void);
extern void init_gl(void);
extern void draw_scene(void);
extern void drawblur(GLfloat times, GLfloat inc, GLfloat spost);
extern void draw_knot(void);
extern void draw_spectrum(void);
extern void draw_face(void);
extern void draw_glthreads(void);
extern void draw_tunnel(void);
extern void draw_tentacles(void);
extern void draw_twist(void);
extern void draw_child(void);
extern void draw_energy(void);
extern GLfloat reduce_vsync(GLfloat rotate);
extern GLfloat reduce_vsync_inv(GLfloat rotate);
extern void draw_background(int mode);
extern void precalculate_tunnel(void);
extern void recalc_perspective(void);
extern void viewortho(void);
extern void viewperspective(void);
extern int gen_gl_texture(GLuint texture);
extern void use_gl_texture(GLuint texture);
extern void upload_gl_texture(VisVideo *image);
extern void delete_gl_texture(GLuint texture);
extern void use_twist_texture(void);
extern void use_child_texture(void);
extern void use_energy_texture(void);
extern void use_background_texture(void);
extern void draw_background(int mode);
extern void config_load(void);

#endif
