#include "nebulus.h"

extern GLuint numVertices, numNormals, numTriangles;
extern GLfloat face[806][6], vertices[473][3], normals[537][3];

GLfloat face_time, face_rotate = 27, old_face_rotate = 27;
GLuint facedl = 0;
int face_first = TRUE;


static void
createvertex(GLuint face0, GLuint face1)
{
  glNormal3f(normals[face1][0], normals[face1][1], normals[face1][2]);
  glVertex3f(vertices[face0][0], vertices[face0][1], vertices[face0][2]);
}


static void
createface(void)
{
  int i;

  facedl = glGenLists(1);
  face_first = FALSE;
  glNewList(facedl, GL_COMPILE);
  glRotatef(-90, 1, 0, 0);
  glBegin(GL_TRIANGLES);
  for (i = 0; i < numTriangles; i++) {
    createvertex(face[i][0], face[i][3]);
    createvertex(face[i][1], face[i][4]);
    createvertex(face[i][2], face[i][5]);
  }
  glEnd();
  glEndList();
}


static void
drawface(void)
{
  GLfloat MaterialColor[4] = { 0.4f, 0.2f, 0.8f, 1.0f };
  GLfloat Specular[4] = { 1, 1, 1, 1 };

  glDisable(GL_TEXTURE_2D);
  glPushMatrix();
  glTranslatef(0.0f, 0.0f, -3.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glRotatef(60*sin(face_time/600), 0, 1, 0);
  glRotatef(30*sin(face_time/1100), 1, 0, 0);
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, MaterialColor);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Specular);
  glScalef(2, 2, 2);

  glCallList(facedl);
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
}


static void
render_face(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_BLEND);
  glEnable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glViewport(0, 0, 128, 128);
  drawface();
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, 0, 0, 128, 128, 0);
  glClearColor(0.0f, 0.2f, 0.4f, 0.4f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
}


void
draw_face(void)
{
  if (face_first)
    createface();
  face_rotate = old_face_rotate;
  face_rotate = reduce_vsync(face_rotate);
  face_time += face_rotate;
  render_face();
  drawface();
  drawblur(5, 0.01f, 0.4f);
}
