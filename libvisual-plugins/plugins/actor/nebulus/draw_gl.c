#include <libvisual/libvisual.h>

#include "nebulus.h"

GLuint blurtexture = 0, knotbg = 0, tunnel = 0;
GLuint tentacle = 0, twist = 0, twistbg = 0, texchild = 0;
GLuint childbg = 0, energy = 0;
GLint *blur_data[128*128*4];
GLUquadricObj *myquadratic;
GLfloat point[37][37][3];
int create_knot, create_quadratic;
GLfloat y_angle = 45.0, y_speed = 1.0, y_old_speed = 1.0;
GLfloat x_angle = 20.0, x_speed = 0.0;
GLfloat z_angle = 0.0, z_speed = 0.0;
GLfloat speed_bg = 6, old_speed_bg = 6;
GLfloat elapsed_time, last_time;
GLfloat GlobalAmbient[4] = { 0.2f, 0.2f,  0.2f, 1.0f };
GLfloat Light0Pos[4] = { 0.0f, 5.0f, 10.0f, 1.0f };
GLfloat Light0Ambient[4] = { 0.2f, 0.2f,  0.2f, 1.0f };
GLfloat Light0Diffuse[4] = { 0.5f, 0.5f,  0.5f, 1.0f };
GLfloat Light0Specular[4] = { 0.8f, 0.8f,  0.8f, 1.0f };
GLfloat LmodelAmbient[4] = { 0.2f, 0.2f,  0.2f, 1.0f };

void
viewortho(void)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, point_general->WIDTH, point_general->HEIGHT, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
}


void
viewperspective(void)
{
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
}


GLfloat
reduce_vsync(GLfloat rotate)
{
  if (framerate > FRAME_BASE)
    rotate /= (GLfloat)framerate / FRAME_BASE;
  return rotate;
}


GLfloat
reduce_vsync_inv(GLfloat rotate)
{
  if (framerate > FRAME_BASE)
    rotate *= (GLfloat)framerate / FRAME_BASE;
  return rotate;
}


void
drawblur(GLfloat times, GLfloat inc, GLfloat spost)
{
  GLfloat alpha = 0.2f;
  GLfloat alphainc;
  GLint i;

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);

  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);
  glBindTexture(GL_TEXTURE_2D,blurtexture);
  viewortho();

  alphainc = alpha / times;
  glBegin(GL_QUADS);
  for (i = 0; i < times; i++) {
    glColor4f(1.0f, 1.0f, 1.0f, alpha);
    glTexCoord2f(0 + spost, 1 - spost);
    glVertex2f(0,0);

    glTexCoord2f(0 + spost, 0 + spost);
    glVertex2f(0, point_general->HEIGHT);

    glTexCoord2f(1 - spost, 0 + spost);
    glVertex2f(point_general->WIDTH, point_general->HEIGHT);

    glTexCoord2f(1 - spost, 1 - spost);
    glVertex2f(point_general->WIDTH, 0);
    spost += inc;
    alpha -= alphainc;
  }
  glEnd();
  viewperspective();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_BLEND);
}


void
draw_background(int mode)
{
  int x, y;
  GLfloat xf, xf2, yf, yf2, angle;

  glEnable(GL_TEXTURE_2D);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);
  glPushMatrix();
  if (!mode) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(135.0f, 4/3, 0.001f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
    for (x = 0; x < 16; x++) {
      for (y = 0; y < 16; y++) {
          point[x][y][0] = 0;
          point[x][y][1] = 0;
          point[x][y][2] = 0;
      }
    }
  }
  else
    recalc_perspective();
  angle = (GLfloat)(last_time + elapsed_time)/2;
  angle = angle/4;
  last_time = elapsed_time;
  if (mode) {
    for (x = 0; x < 16; x++) {
      for (y = 0; y < 16; y++) {
          point[x][y][0] = x*2 - 16 + sin(angle/60 + x*10*PI/360)*2;
          point[x][y][1] = y*2 - 16 + cos(angle/95 + x*10*2*PI/360)*8;
          point[x][y][2] = sin(angle/180 + x*10*2*PI/360)*2;
      }
    }
  }
  glTranslatef(0.0f, 0.0f, -6.0f);
  glRotatef(-angle/5, 0.0f, 0.0f, 1.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_QUADS);
  for (x = 0; x < 15; x++) {
    xf = (GLfloat)x/16;
    xf2 = (GLfloat)(x+1)/16;
    for (y = 0; y < 15; y++) {
      yf = (GLfloat)y/16;
      yf2 = (GLfloat)(y+1)/16;
      glTexCoord2f(xf, yf);
      glVertex3f(point[x][y][0], point[x][y][1], point[x][y][2]);
      glTexCoord2f(xf, yf2);
      glVertex3f(point[x][y+1][0], point[x][y+1][1], point[x][y+1][2]);
      glTexCoord2f(xf2, yf2);
      glVertex3f(point[x+1][y+1][0], point[x+1][y+1][1], point[x+1][y+1][2]);
      glTexCoord2f(xf2, yf);
      glVertex3f(point[x+1][y][0], point[x+1][y][1], point[x+1][y][2]);
    }
  }    
  glEnd();
  if (!mode) {
    glTranslatef(0, 0, 5);
    glRotatef(angle/1.3f, 1.0f, 0.0f, 0.0f);
    glRotatef(angle/2, 0.0f, 1.0f, 0.0f);
    gluSphere(myquadratic, 1.3f, 32.0f, 32.0f);
  }
  glPopMatrix();
}


int
gen_gl_texture(GLuint texture)
{
  if (texture)
    return FALSE;
  glGenTextures(1, &texture);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  return TRUE;
}


void
delete_gl_texture(GLuint texture)
{
  if (texture)
    glDeleteTextures(1, &texture);
  texture = 0;
}


void
use_gl_texture(GLuint texture)
{
  glBindTexture(GL_TEXTURE_2D, texture);
}

void
upload_gl_texture (VisVideo *image)
{
  glTexImage2D(GL_TEXTURE_2D, 0, 3, visual_video_get_width (image), visual_video_get_height (image),
               0, GL_RGB, GL_UNSIGNED_BYTE, visual_video_get_pixels (image));
}

void
use_twist_texture(void)
{
  if (gen_gl_texture(twist))
    upload_gl_texture(twist_image);
  use_gl_texture(twist);
}


void
use_child_texture(void)
{
  if (gen_gl_texture(texchild))
    upload_gl_texture(child_image);
  use_gl_texture(texchild);
}


void
use_energy_texture(void)
{
  if (gen_gl_texture(childbg))
    upload_gl_texture(energy_image);
  use_gl_texture(childbg);
}


void
use_background_texture(void)
{
  if (gen_gl_texture(twistbg))
    upload_gl_texture(background_image);
  use_gl_texture(twistbg);
}


void
recalc_perspective(void)
{
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)640/480, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
}


static void
init_effect(void)
{
  viewperspective();
  glClearDepth(1.0f);
  glDepthFunc(GL_LESS);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);

  if (!glIsEnabled(GL_LIGHTING)) {
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LmodelAmbient);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, GlobalAmbient);
    glLightfv(GL_LIGHT0, GL_POSITION, Light0Pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, Light0Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Light0Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Light0Specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
  }

  if (gen_gl_texture(blurtexture))
    glTexImage2D(GL_TEXTURE_2D, 0, 4, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, blur_data);
  use_gl_texture(blurtexture);
  glShadeModel(GL_SMOOTH);
  glMateriali(GL_FRONT, GL_SHININESS, 128);
  if (!point_general->effect) {
    if (gen_gl_texture(knotbg))
      upload_gl_texture(background_image);
    use_gl_texture(knotbg);
  }
  if (point_general->effect == 4) {
    if (gen_gl_texture(tunnel))
      upload_gl_texture(tunnel_image);
    use_gl_texture(tunnel);
  }
  if (point_general->effect == 5) {
    if (gen_gl_texture(tentacle))
      upload_gl_texture(tentacle_image);
    use_gl_texture(tentacle);
  }
  if (point_general->effect == 6) {
    if (gen_gl_texture(twist))
      upload_gl_texture(twist_image);
    if (gen_gl_texture(twistbg))
      upload_gl_texture(background_image);
    use_gl_texture(twist);
  }
  if (point_general->effect == 7) {
    if (gen_gl_texture(texchild))
      upload_gl_texture(child_image);
    if (gen_gl_texture(childbg))
      upload_gl_texture(energy_image);
    use_gl_texture(texchild);
  }
  if (point_general->effect == 8) {
    if (gen_gl_texture(energy))
      upload_gl_texture(energy_image);
    use_gl_texture(energy);
  }
  point_general->init = TRUE;
}

void
init_gl(void)
{
  /* Initialize bitmaps */

  child_image      = visual_video_load_from_file (BITMAP_DIR "/child_texture.bmp");
  energy_image     = visual_video_load_from_file (BITMAP_DIR "/energy_texture.bmp");
  tentacle_image   = visual_video_load_from_file (BITMAP_DIR "/tentacle_texture.bmp");
  tunnel_image     = visual_video_load_from_file (BITMAP_DIR "/tunnel_texture.bmp");
  twist_image      = visual_video_load_from_file (BITMAP_DIR "/twist_texture.bmp");
  background_image = visual_video_load_from_file (BITMAP_DIR "/background_texture.bmp");

  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
  glEnable(GL_TEXTURE_2D);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClearDepth(1.0f);
  glShadeModel(GL_SMOOTH);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, (GLfloat)640/480, 0.1f, 100.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glEnable(GL_BLEND);
}

void
draw_scene(void)
{
  if (point_general->changement) {
    recalc_perspective();
    if (point_general->effect > EFFECT_NUMBER - 1) {
      point_general->effect = 0;
      if (gen_gl_texture(knotbg))
        upload_gl_texture(background_image);
      use_gl_texture(knotbg);
    }
    if (point_general->effect == 4) {
      if (gen_gl_texture(tunnel))
        upload_gl_texture(tunnel_image);
      use_gl_texture(tunnel);
    }
    if (point_general->effect == 5) {
      if (gen_gl_texture(tentacle))
        upload_gl_texture(tentacle_image);
      use_gl_texture(tentacle);
    }
    if (point_general->effect == 6) {
      if (gen_gl_texture(twist))
        upload_gl_texture(twist_image);
      if (gen_gl_texture(twistbg))
		upload_gl_texture(background_image);
      use_gl_texture(twist);
    }
    if (point_general->effect == 7) {
      if (gen_gl_texture(texchild))
        upload_gl_texture(child_image);
      if (gen_gl_texture(childbg))
        upload_gl_texture(energy_image);
      use_gl_texture(texchild);
    }
    if (point_general->effect == 8) {
      if (gen_gl_texture(energy))
        upload_gl_texture(energy_image);
      use_gl_texture(energy);
    }
    point_general->changement = FALSE;
  }
  switch (point_general->effect) {
    case 0:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_knot();
      break;
    case 1:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_spectrum();
      break;
    case 2:
      if (!point_general->init)
        init_effect();
      glEnable(GL_LIGHTING);
      draw_face();
      break;
    case 3:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_glthreads();
      break;
    case 4:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_tunnel();
      break;
    case 5:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_tentacles();
      break;
    case 6:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_twist();
      break;
    case 7:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_child();
      break;
    case 8:
      if (!point_general->init)
        init_effect();
      glDisable(GL_LIGHTING);
      draw_energy();
      break;
    default:
      break;
  }
}
