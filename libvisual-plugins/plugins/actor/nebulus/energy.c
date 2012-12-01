#include "nebulus.h"

GLfloat energy_time, energy_speed = 0.04f, old_energy_speed = 0.04f;

typedef struct
{
  GLfloat fu, fv;
} ctexel;


static void
put_quad(ctexel cuvpos)
{
  glTexCoord2f(0 + cuvpos.fu, 0 + cuvpos.fv);
  glVertex3f(-300, 300, 0);
  glTexCoord2f(1 + cuvpos.fu, 0 + cuvpos.fv);
  glVertex3f(300, 300, 0);
  glTexCoord2f(1 + cuvpos.fu, 1 + cuvpos.fv);
  glVertex3f(300, -300, 0);
  glTexCoord2f(0 + cuvpos.fu, 1 + cuvpos.fv);
  glVertex3f(-300, -300, 0);
}


static void
drawenergy(GLfloat ftime)
{
  ctexel cuvpos;
  int i;
  GLfloat fog_color[3] = { 0.0f, 0.0f, 0.0f };

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-0.6f, 0.6f, -0.45f, 0.45f, 1.0f, 1000);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  glTranslatef(0.0f, 0.0f, -300.0f);
  glRotatef(ftime*30.0f, 1.0f, 0.0f, 0.0f);
  glRotatef(30*sin(ftime/3.0f) + 10.0f, 0.0f, 0.0f, 1.0f);
  glRotatef(180.0f, 0.0f, 1.0f, 0.0f);

  glFogf(GL_FOG_START, 200);
  glFogf(GL_FOG_END, 500);
  glFogfv(GL_FOG_COLOR, fog_color);
  glFogf(GL_FOG_DENSITY, 0);
  glFogf(GL_FOG_MODE, GL_LINEAR);
  glEnable(GL_FOG);
  glPushMatrix();

  cuvpos.fu = 0.1f*sin(ftime);
  cuvpos.fv = ftime/5.0f;

  glColor3f(1.0f, 1.0f, 1.0f);
  glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  for (i = 0; i != 8; i++) {
    glBegin(GL_QUADS);
    put_quad(cuvpos);
    glEnd();
    glRotatef(180/7.0f, 1.0f, 0.0f, 0.0f);
  }

  glPopMatrix();
  glDisable(GL_FOG);
}


static void
render_energy(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
  glEnable(GL_BLEND);
  glDisable(GL_NORMALIZE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
}


void
draw_energy(void)
{
  energy_speed = old_energy_speed;
  energy_speed = reduce_vsync(energy_speed);
  energy_time += energy_speed;
  if (energy_time > 29.96f)
    energy_time = 0.0f;
  render_energy();
  drawenergy(energy_time);
}
