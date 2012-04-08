#include "nebulus.h"

extern GLuint childNormals;
extern GLfloat child_normals[6153][3], child_vertices[6153][3];

GLfloat child_time, child_speed = 0.35f, old_child_speed = 0.35f;
GLuint childdl = 0;
int child_first = TRUE;


static void
createchild(void)
{
  int i;

  childdl = glGenLists(1);
  child_first = FALSE;
  glNewList(childdl, GL_COMPILE);
  glBegin(GL_TRIANGLES);
  for (i = 0; i < childNormals; i++) {
    glNormal3f(child_normals[i][0], child_normals[i][1], child_normals[i][2]);
    glVertex3f(child_vertices[i][0], child_vertices[i][1], child_vertices[i][2]);
  }
  glEnd();
  glEndList();
}


static void
drawchild(void)
{
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -4.0f);
  glRotatef(child_time*2.0f, 0.0f, 0.0f, 1.0f);
  glScalef(4.0f, 4.0f, 1.0f);
  use_energy_texture();
  glBegin(GL_QUADS);
  glColor3f(0.75f, 0.65f, 0.55f);
  glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -0.5f);
  glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.5f);
  glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, 1.0f, 0.5f);
  glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, -1.0f, -0.5f);
  glEnd();
  glTranslatef(0.0f, 0.0f, 1.6f);
  glScalef(0.25f, 0.25f, 1.0f);
  glRotatef(child_time*2, 1.f, 0.0f, 0.0f);
  glRotatef(child_time*5, 0.0f, 1.0f ,0.0f);
  glRotatef(child_time*2, 0.0f, 0.0f, 1.0f);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glTexGenf(GL_S,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
  glTexGenf(GL_T,GL_TEXTURE_GEN_MODE,GL_SPHERE_MAP);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  use_child_texture();
  glColor3f(0.45f, 0.45f, 0.25f);
  glDepthMask(GL_FALSE);
  glScalef(0.75f, 0.75f, 0.75f);
  glCallList(childdl);
  glDepthMask(GL_TRUE);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glPopMatrix();
}


static void
render_child(void)
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
draw_child(void)
{
  if (child_first)
    createchild();
  child_speed = old_child_speed;
  child_speed = reduce_vsync(child_speed);
  child_time += child_speed;
  render_child();
  drawchild();
}
