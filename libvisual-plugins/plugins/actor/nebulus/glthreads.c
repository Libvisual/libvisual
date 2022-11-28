#include "nebulus.h"

#define THREADLENGTH 100
#define MAXTHREADS 50

glcoord threads[MAXTHREADS][THREADLENGTH];
GLfloat xval, xabs;
GLfloat speed_xval = 0.3f, old_speed_xval = 0.3f;
GLfloat speed_xabs = 0.008f, old_speed_xabs = 0.008f;


static void
createglthreads(void)
{
  int i, j;
  glcoord p;
  GLfloat r, g, b;
  short val_pcm;

  val_pcm = pcm_data[0];
  if (val_pcm > 10000) val_pcm = 10000;
  if (val_pcm < -10000) val_pcm = -10000;
  xval += speed_xval;
  xabs += speed_xabs;
  p.x = ((GLfloat)point_general->WIDTH/2)/(GLfloat)
        (point_general->WIDTH/8)-(fabs(sin(xval)*4)*2)+(sin(xabs)/1.5f);
  p.y = -2.0f;
  p.z = (GLfloat)((point_general->HEIGHT/2)+((GLfloat)val_pcm
        *(0.11f*point_general->HEIGHT)/1800))/(GLfloat)
        (point_general->HEIGHT/6)-3.0f;
  for (i = 0; i < MAXTHREADS; i++)
    threads[i][THREADLENGTH-1] = p;
  for (j = 0; j < MAXTHREADS; j++) {
    for (i = 0; i < THREADLENGTH-1; i++) {
      threads[j][i].x = (threads[j][i+1].x*2+threads[j][i].x)/2.9f;
      threads[j][i].y = (threads[j][i+1].y*(2+(GLfloat)j/1000)+threads[j][i].y)/2.9f;
      threads[j][i].z = (threads[j][i+1].z*2+threads[j][i].z)/2.9f;
    }
  }

  for (i = 0; i < MAXTHREADS-1; i++) {
    glBegin(GL_LINE_STRIP);
    for (j = 0; j < THREADLENGTH; j++) {
      r = 0.8f*(0.3f+sin(-i/60))*(1-(GLfloat)j/THREADLENGTH);
      g = 0.8f*(0.3f+sin(-i/60))*(1-(GLfloat)j/THREADLENGTH);
      b = 1.1f*(0.3f+sin(-i/60))*(1-(GLfloat)j/THREADLENGTH);
      if (j > THREADLENGTH-8)
        glColor3f(0.0f, 0.0f, 0.0f);
      else
        glColor3f(r, g, b);
      glVertex3f(threads[i][j].x, threads[i][j].y, threads[i][j].z);
    }
    glEnd();
  }
}


static void
drawglthreads(void)
{
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -7.0f);
  glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
  createglthreads();
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}


static void
render_glthreads(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
  glLineWidth((GLfloat)point_general->WIDTH/160);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_BLEND);
  glDisable(GL_NORMALIZE);
}


void
draw_glthreads(void)
{
  speed_xval = old_speed_xval;
  speed_xabs = old_speed_xabs;
  speed_xval = reduce_vsync_inv(speed_xval);
  speed_xabs = reduce_vsync(speed_xabs);
  render_glthreads();
  drawglthreads();
}
