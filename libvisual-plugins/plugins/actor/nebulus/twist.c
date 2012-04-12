#include "nebulus.h"

#define NR_DIVS 16
#define NR_SEGS 64

double rad[NR_SEGS], sinpos[NR_SEGS], xpos[NR_SEGS];
GLfloat twist_time, twist_speed = 0.04f, old_twist_speed = 0.04f;


static void setvertex(int i, int j)
{
  GLfloat col;

  col = (GLfloat)(1.2*rad[i]*sin(j*PI*2.0/(double)NR_DIVS));
  glColor3f(col, col, col);
  glTexCoord2f((GLfloat)(sinpos[i]+j*(3.0/(double)NR_DIVS)),
              (GLfloat)(i*(3.0/(double)NR_SEGS)));
  j = j % NR_DIVS;
  glVertex3f((GLfloat)(xpos[i]+rad[i]*cos(j*PI*2.0/(double)NR_DIVS)),
            (GLfloat)(i*(5.5/(double)NR_SEGS)),
            (GLfloat)(rad[i]*sin(j*PI*2.0/(double)NR_DIVS)));
}


static void
createtwist(void)
{
  int i, j;

  glBegin(GL_QUADS);
  for( i = 0; i < (NR_SEGS-1); i++) {
    for(j = 0; j < NR_DIVS; j++) {
      setvertex(i, j);
      setvertex(i+1, j);
      setvertex(i+1, j+1);
      setvertex(i, j+1);
    }
  }
  glEnd();
}


static void
drawtwist(void)
{
  int i;

  glPushMatrix();
  glRotatef(twist_time*20, 0, 0, 1.0f);
  glTranslatef(0.0f, -2.75f, -3.15f);
  for(i = 0; i < NR_SEGS; i++) {
    sinpos[i] = 0.53f*sin(twist_time*0.78f+(GLfloat)i/12.0f)+
                0.45f*sin(twist_time*-1.23f+(GLfloat)i/18.0f);
    rad[i] = 0.8f+0.3f*sin(twist_time+(GLfloat)i/8.0f)+
             0.15f*sin(twist_time*-0.8f+(GLfloat)i/3.0f);
    xpos[i] = 0.25f*sin(twist_time*1.23f+(GLfloat)i/5.0f)+
              0.3f*sin(twist_time*0.9+(GLfloat)i/6.0);
  }
  createtwist();
  glPopMatrix();
}


static void
render_twist(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(55.0f, (GLfloat)640/480, 1.0f, 20.0f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glDisable(GL_BLEND);
  glDisable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_TEXTURE_2D);
}


void
draw_twist(void)
{
  twist_speed = old_twist_speed;
  twist_speed = reduce_vsync(twist_speed);
  twist_time += twist_speed;
  speed_bg = old_speed_bg;
  speed_bg = reduce_vsync(speed_bg);
  elapsed_time += speed_bg;
  render_twist();
  use_twist_texture();
  drawtwist();
  use_background_texture();
  draw_background(TRUE);
}
