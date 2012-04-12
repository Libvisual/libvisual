#include "nebulus.h"

GLuint cubedl = 0;
GLfloat tentacles_time, speed_tentacles = 50, old_speed_tentacles = 50;
int tentacles_first = TRUE;


static void
drawtentacle(int step, GLfloat angle, GLfloat twistangle)
{
  if (step == 19)
    return;

  glTranslatef(2.25f, 0.0f, 0.0f);
  glRotatef(angle, 0.0f, 1.0f, 0.0f);
  glRotatef(twistangle, 1.0f, 0.0f, 0.0f);
  glCallList(cubedl);
  glScalef(0.9f, 0.9f, 0.9f);
  drawtentacle(step+1, angle, twistangle);
}


static void
drawobject(void)
{
  GLfloat angle, twistangle;

  glCallList(cubedl);
  glScalef(0.9f, 0.9f, 0.9f);
  angle = 25*sin(tentacles_time/800);
  twistangle = 25*sin(tentacles_time/1000);
  glPushMatrix();
  drawtentacle(0, angle, twistangle);
  glPopMatrix();
  angle = 25*cos(tentacles_time/700);
  twistangle  = 25*sin(tentacles_time/1000);
  glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  glPushMatrix();
  drawtentacle(0, angle, -twistangle);
  glPopMatrix();
  angle = 25*sin(tentacles_time/700);
  twistangle = 25*cos(tentacles_time/800);
  glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  glPushMatrix();
  drawtentacle(0, angle, twistangle);
  glPopMatrix();
  angle = 25*sin(tentacles_time/800);
  twistangle = 25*sin(tentacles_time/600);
  glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
  glPushMatrix();
  drawtentacle(0, angle, -twistangle);
  glPopMatrix();
  angle = 25*sin(tentacles_time/800);
  twistangle = 25*cos(tentacles_time/600);
  glRotatef(90.0f, 0.0f, 0.0f, 1.0f);
  glPushMatrix();
  drawtentacle(0, angle, twistangle);
  glPopMatrix();
  angle = 25*sin(tentacles_time/600);
  twistangle = 25*cos(tentacles_time/1000);
  glRotatef(180.0f, 0.0f, 0.0f, 1.0f);
  glPushMatrix();
  drawtentacle(0, -angle, -twistangle);
  glPopMatrix();
}


static void
createtentacles(void)
{
  cubedl = glGenLists(1);
  tentacles_first = FALSE;
  glNewList(cubedl, GL_COMPILE);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
  glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
  glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
  glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1.0,  1.0,  1.0);
  glTexCoord2f(1.0, 0.0); glVertex3f( 1.0,  1.0,  1.0);
  glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(-1.0, -1.0, -1.0);
  glTexCoord2f(0.0, 1.0); glVertex3f( 1.0, -1.0, -1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
  glTexCoord2f(1.0, 0.0); glVertex3f( 1.0, -1.0, -1.0);
  glTexCoord2f(1.0, 1.0); glVertex3f( 1.0,  1.0, -1.0);
  glTexCoord2f(0.0, 1.0); glVertex3f( 1.0,  1.0,  1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f( 1.0, -1.0,  1.0);
  glTexCoord2f(0.0, 0.0); glVertex3f(-1.0, -1.0, -1.0);
  glTexCoord2f(1.0, 0.0); glVertex3f(-1.0, -1.0,  1.0);
  glTexCoord2f(1.0, 1.0); glVertex3f(-1.0,  1.0,  1.0);
  glTexCoord2f(0.0, 1.0); glVertex3f(-1.0,  1.0, -1.0);
  glEnd();
  glEndList();
}


static void
drawtentacles(void)
{
  /* GLfloat angle, twistangle; */

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -1.5f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glBegin(GL_QUADS);
  glVertex3f(-1.0, -1.0, 0.0);
  glVertex3f( 1.0, -1.0, 0.0);
  glVertex3f( 1.0,  1.0, 0.0);
  glVertex3f(-1.0,  1.0, 0.0);
  glEnd();
  glPopMatrix();

  /* angle = 25*sin(tentacles_time/1000); */
  /* twistangle = 25*sin(tentacles_time/1200); */
  glPushMatrix();
  glDisable(GL_TEXTURE_2D);
  glTranslatef(0.0f, 0.0f, -18.0f);
  glRotatef(tentacles_time/20, 1.0f, 0.0f, 0.0f);
  glRotatef(tentacles_time/30, 0.0f, 1.0f, 0.0f);
  glColor3f(0.6f, 0.6f, 0.6f);
  drawobject();
  glColor3f(1.0f, 1.0f, 1.0f);
  glPopMatrix();

  glPushMatrix();
  glColor3f(0.0f, 0.0f, 0.0f);
  glTranslatef(0.0f, 0.0f, -1.0f);
  glBegin(GL_QUADS);
  glVertex3f(-1.0, 0.19, 0.0);
  glVertex3f( 1.0, 0.19, 0.0);
  glVertex3f( 1.0, 0.5, 0.0);
  glVertex3f(-1.0, 0.5, 0.0);
  glVertex3f(-1.0, -0.2, 0.0);
  glVertex3f( 1.0, -0.2, 0.0);
  glVertex3f( 1.0, -0.5, 0.0);
  glVertex3f(-1.0, -0.5, 0.0);
  glEnd();
  glColor3f(1.0f, 1.0f, 1.0f);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -33.0f);
  glRotatef(tentacles_time/20, 1.0f, 0.0f, 0.0f);
  glRotatef(tentacles_time/30, 0.0f, 1.0f, 0.0f);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glColor3f(1.0f, 1.0f, 1.0f);
  drawobject();
  glEnable(GL_BLEND);
  glPopMatrix();
}


static void
render_tentacles(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
  glDisable(GL_BLEND);
  glDisable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
}


void
draw_tentacles(void)
{
  if (tentacles_first)
    createtentacles();
  speed_tentacles = old_speed_tentacles;
  speed_tentacles = reduce_vsync(speed_tentacles);
  tentacles_time += speed_tentacles;
  render_tentacles();
  drawtentacles();
}
