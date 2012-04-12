#include "nebulus.h"


static void draw_rectangle(GLfloat x1, GLfloat y1, GLfloat z1, GLfloat x2, GLfloat y2, GLfloat z2)
{
  if(y1 == y2) {
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z1);
    glVertex3f(x2, y2, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z2);
    glVertex3f(x1, y1, z1);
  }
  else {
    glVertex3f(x1, y1, z1);
    glVertex3f(x2, y1, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x2, y2, z2);
    glVertex3f(x1, y2, z1);
    glVertex3f(x1, y1, z1);
  }
}


static void
draw_bar(GLfloat x_offset, GLfloat z_offset, GLfloat height, GLfloat red, GLfloat green, GLfloat blue )
{
  GLfloat width = 0.08f;
  glColor3f(red - 0.1f, 0.0f, blue);
  draw_rectangle(x_offset, height, z_offset, x_offset + width, height, z_offset + 0.1);
  draw_rectangle(x_offset, 0, z_offset, x_offset + width, 0, z_offset + 0.1);
  glColor3f(0.5 * red, 0.0f, 0.6 * blue);
  draw_rectangle(x_offset, 0.0, z_offset + 0.1, x_offset + width, height, z_offset + 0.1);
  draw_rectangle(x_offset, 0.0, z_offset, x_offset + width, height, z_offset );
  glColor3f(0.25 * red, 0.0f, 0.25 * blue);
  draw_rectangle(x_offset, 0.0, z_offset , x_offset, height, z_offset + 0.1);
  draw_rectangle(x_offset + width, 0.0, z_offset , x_offset + width, height, z_offset + 0.1);
}


static void
createbars(void)
{
  int x, y;
  GLfloat x_offset, z_offset, r_base, b_base;
  glBegin(GL_TRIANGLES);
  for(y = 0; y < 16; y++) {
    z_offset = -1.6 + ((15 - y) * 0.2);
    b_base = y * (1.0 / 14);
    r_base = 1.0 - b_base;
    for(x = 0; x < 16; x++) {
      x_offset = -1.6 + (x * 0.2);
      draw_bar(x_offset, z_offset, heights[y][x], r_base, y * (r_base/10.0f), b_base);
    }
  }
  glEnd();
}


static void
drawbars(void)
{
  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -10.4f);
  glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
  glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
  glRotatef(z_angle, 0.0f, 0.0f, 1.0f);

  glScalef(2, 2, 2);
  createbars();
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
}


static void
render_spectrum(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_BLEND);
  glDisable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, 128, 128);
  drawbars();
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, 128, 128, 0);
  glClearColor(0.0f, 0.2f, 0.4f, 0.4f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
}


void
draw_spectrum(void)
{
  scale = 1.0 / log(64.0);
  y_speed = y_old_speed;
  y_speed = reduce_vsync(y_speed);
  x_angle += x_speed;
  if(x_angle >= 360.0) x_angle -= 360.0;
  y_angle += y_speed;
  if(y_angle >= 360.0) y_angle -= 360.0;
  z_angle += z_speed;
  if(z_angle >= 360.0) z_angle -= 360.0;
  render_spectrum();
  drawbars();
  drawblur(1, 0.01f, 0.4f);
}
