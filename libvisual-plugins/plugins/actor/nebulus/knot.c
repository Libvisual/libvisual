#include "nebulus.h"

GLfloat knot_time, old_knot_time;

typedef struct
{
  GLfloat r, g, b;
} glcolors;

typedef struct
{
  int numfaces, numverts, numsides;
  GLuint faces[16*64*4*4];
  glcoord vertices[16*64];
  glcolors colors[16*64];
} tknotobject;

tknotobject knotobject;


static void
initknot(int scaling_factor1, int scaling_factor2, GLfloat radius1, GLfloat radius2, GLfloat radius3)
{
  int count1, count2;
  GLfloat alpha, beta, distance, mindistance, rotation;
  GLfloat x, y, z, dx, dy, dz;
  GLfloat value, modulus, dist;
  int index1, index2;

  knotobject.numsides = 4;
  knotobject.numverts = 0;
  alpha = 0;
  for (count2 = 0; count2 < scaling_factor2; count2++) {
    alpha = alpha + 2*PI / scaling_factor2;
    x = radius2 * cos(2 * alpha) + radius1 * sin(alpha);
    y = radius2 * sin(2 * alpha) + radius1 * cos(alpha);
    z = radius2 * cos(3 * alpha);
    dx = -2 * radius2 * sin(2 * alpha) + radius1 * cos(alpha);
    dy = 2 * radius2 * cos(2 * alpha) - radius1 * sin(alpha);
    dz = -3 * radius2 * sin(3 * alpha);
    value = sqrt(dx * dx + dz * dz);
    modulus = sqrt(dx * dx + dy * dy + dz * dz);

    beta = 0;
    for (count1 = 0; count1 < scaling_factor1; count1++) {
      beta = beta + 2 * PI / scaling_factor1;

      knotobject.vertices[knotobject.numverts].x = x - radius3 * (cos(beta) * dz - sin(beta) * dx * dy / modulus) / value;
      knotobject.vertices[knotobject.numverts].y = y - radius3 * sin(beta) * value / modulus;
      knotobject.vertices[knotobject.numverts].z = z + radius3 * (cos(beta) * dx + sin(beta) * dy * dz / modulus) / value;


      dist = sqrt(knotobject.vertices[knotobject.numverts].x*knotobject.vertices[knotobject.numverts].x +
                   knotobject.vertices[knotobject.numverts].y*knotobject.vertices[knotobject.numverts].y +
                   knotobject.vertices[knotobject.numverts].z*knotobject.vertices[knotobject.numverts].z);

      knotobject.colors[knotobject.numverts].r = ((2/dist)+(0.5*sin(beta)+0.4))/2.0;
      knotobject.colors[knotobject.numverts].g = ((2/dist)+(0.5*sin(beta)+0.4))/2.0;
      knotobject.colors[knotobject.numverts].b = ((2/dist)+(0.5*sin(beta)+0.4))/2.0;

      knotobject.numverts++;
    }
  }

  for (count1 = 0; count1 < scaling_factor2; count1++) {
    index1 = count1 * scaling_factor1;
    index2 = index1 + scaling_factor1;
    index2 = index2 % knotobject.numverts;
    rotation = 0;
    mindistance = (knotobject.vertices[index1].x - knotobject.vertices[index2].x) * (knotobject.vertices[index1].x - knotobject.vertices[index2].x) +
                   (knotobject.vertices[index1].y - knotobject.vertices[index2].y) * (knotobject.vertices[index1].y - knotobject.vertices[index2].y) +
                   (knotobject.vertices[index1].z - knotobject.vertices[index2].z) * (knotobject.vertices[index1].z - knotobject.vertices[index2].z);
    for (count2 = 1; count2 < scaling_factor1; count2++) {
      index2 = count2 + index1 + scaling_factor1;
      if (count1 == scaling_factor2 - 1)
        index2 = count2;
      distance = (knotobject.vertices[index1].x - knotobject.vertices[index2].x) * (knotobject.vertices[index1].x - knotobject.vertices[index2].x) +
                  (knotobject.vertices[index1].y - knotobject.vertices[index2].y) * (knotobject.vertices[index1].y - knotobject.vertices[index2].y) +
                  (knotobject.vertices[index1].z - knotobject.vertices[index2].z) * (knotobject.vertices[index1].z - knotobject.vertices[index2].z);
      if (distance < mindistance) {
        mindistance = distance;
        rotation = count2;
      }
    }

    for (count2 = 0; count2 < scaling_factor1; count2++) {
      knotobject.faces[4*(index1+count2)+0] = index1 + count2;

      index2 = count2 + 1;
      index2 = index2 % scaling_factor1;
      knotobject.faces[4*(index1+count2)+1] = index1 + index2;

      index2 = (count2 + rotation + 1);
      index2 = index2 % scaling_factor1;
      knotobject.faces[4*(index1+count2)+2] = (index1 + index2 + scaling_factor1) % knotobject.numverts;

      index2 = (count2 + rotation);
      index2 = index2 % scaling_factor1;

      knotobject.faces[4*(index1+count2)+3] = (index1 + index2 + scaling_factor1) % knotobject.numverts;
      knotobject.numfaces++;
    }
  }
}


static void
recalcknot(int scaling_factor1, int scaling_factor2, GLfloat radius1, GLfloat radius2, GLfloat radius3)
{
  int count1, count2;
  GLfloat alpha, beta;
  GLfloat x, y, z, dx, dy, dz;
  GLfloat value, modulus;
  int index1, index2;
  GLfloat distance, Mindistance, rotation;

  knotobject.numverts = 0;
  alpha = 0;
  for (count2 = 0; count2 < scaling_factor2; count2++) {
    alpha = alpha + 2*PI / scaling_factor2;
    x = radius2 * cos(2 * alpha) + radius1 * sin(alpha);
    y = radius2 * sin(2 * alpha) + radius1 * cos(alpha);
    z = radius2 * cos(3 * alpha);
    dx = -2 * radius2 * sin(2 * alpha) + radius1 * cos(alpha);
    dy = 2 * radius2 * cos(2 * alpha) - radius1 * sin(alpha);
    dz = -3 * radius2 * sin(3 * alpha);
    value = sqrt(dx * dx + dz * dz);
    modulus = sqrt(dx * dx + dy * dy + dz * dz);

    beta = 0;
    for (count1 = 0; count1 < scaling_factor1; count1++) {
      beta = beta + 2 * PI / scaling_factor1;

      knotobject.vertices[knotobject.numverts].x = x - radius3 * (cos(beta) * dz - sin(beta) * dx*dy/modulus) / value;
      knotobject.vertices[knotobject.numverts].y = y - radius3 *  sin(beta) * value / modulus;
      knotobject.vertices[knotobject.numverts].z = z + radius3 * (cos(beta) * dx + sin(beta) * dy*dz/modulus) / value;

      knotobject.numverts++;
    }
  }

  knotobject.numfaces = 0;
  for (count1 = 0; count1 < scaling_factor2; count1++) {
    index1 = count1 * scaling_factor1;
    index2 = index1 + scaling_factor1;
    index2 = index2 % knotobject.numverts;
    rotation = 0;
    Mindistance = (knotobject.vertices[index1].x - knotobject.vertices[index2].x) * (knotobject.vertices[index1].x - knotobject.vertices[index2].x) +
                   (knotobject.vertices[index1].y - knotobject.vertices[index2].y) * (knotobject.vertices[index1].y - knotobject.vertices[index2].y) +
                   (knotobject.vertices[index1].z - knotobject.vertices[index2].z) * (knotobject.vertices[index1].z - knotobject.vertices[index2].z);
    for (count2  = 1; count2 < scaling_factor1; count2++) {
      index2 = count2 + index1 + scaling_factor1;
      if (count1 == scaling_factor2 - 1)
        index2 = count2;
      distance = (knotobject.vertices[index1].x - knotobject.vertices[index2].x) * (knotobject.vertices[index1].x - knotobject.vertices[index2].x) +
                  (knotobject.vertices[index1].y - knotobject.vertices[index2].y) * (knotobject.vertices[index1].y - knotobject.vertices[index2].y) +
                  (knotobject.vertices[index1].z - knotobject.vertices[index2].z) * (knotobject.vertices[index1].z - knotobject.vertices[index2].z);
      if (distance < Mindistance) {
        Mindistance = distance;
        rotation = count2;
      }
    }

    for (count2 = 0; count2 < scaling_factor1; count2++) {
      knotobject.faces[4*(index1+count2)+0] = index1 + count2;

      index2 = index2 % scaling_factor1;

      knotobject.faces[4*(index1+count2)+2] = (index1 + index2 + scaling_factor1) %  knotobject.numverts;

      index2 = count2 + rotation;
      index2 = index2 % scaling_factor1;

      knotobject.faces[4*(index1+count2)+3] = (index1 + index2 + scaling_factor1) % knotobject.numverts;
      knotobject.numfaces++;
    }
  }
}


static void
draw_the_knot(void)
{
  int i, j, num;

  glBegin(GL_QUADS);
  j = knotobject.numfaces*4;
  for (i = 0; i < j; i++) {
    num = knotobject.faces[i];
    glColor3f(knotobject.colors[num].r*2.5, knotobject.colors[num].g*2, knotobject.colors[num].b*1.5);
    glVertex3f(knotobject.vertices[num].x, knotobject.vertices[num].y,  knotobject.vertices[num].z);
  }
  glEnd();
}


static void
createknot(void)
{
  recalcknot(16, 64, 3.4*sin(0.02*knot_time)-1, 4*cos(0.006*knot_time)-2, 1.1);
  glTranslatef(0.0f, 0.2f, -30.0f);
  glRotatef(x_angle, 1.0f, 0.0f, 0.0f);
  glRotatef(y_angle, 0.0f, 1.0f, 0.0f);
  glRotatef(z_angle, 0.0f, 0.0f, 1.0f);
  draw_the_knot();
}


static void
drawknot(void)
{
  glPushMatrix();
  createknot();
  glPopMatrix();
}


static void
render_knot(void)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(0.9f, 0.9f, 0.9f);
  glDisable(GL_BLEND);
  glDisable(GL_NORMALIZE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  if (!create_quadratic) {
    myquadratic = gluNewQuadric();
    gluQuadricNormals(myquadratic, GLU_SMOOTH);
    gluQuadricTexture(myquadratic, GL_TRUE);
    create_quadratic = TRUE;
  }
  recalc_perspective();
  glViewport(0, 0, point_general->WIDTH, point_general->HEIGHT);
}


void
draw_knot(void)
{
  if (!create_knot) {
    initknot(16, 64, 2, 2.0, 1.0);
    create_knot = TRUE;
  }
  y_speed = y_old_speed;
  y_speed = reduce_vsync(y_speed);
  x_angle += x_speed;
  if(x_angle >= 360.0) x_angle -= 360.0;
  y_angle += y_speed;
  if(y_angle >= 360.0) y_angle -= 360.0;
  z_angle += z_speed;
  if(z_angle >= 360.0) z_angle -= 360.0;
  old_knot_time = knot_time;
  speed_bg = old_speed_bg;
  speed_bg = reduce_vsync(speed_bg);
  elapsed_time += speed_bg;
  point_general->energy = reduce_vsync(point_general->energy);
  knot_time += point_general->energy;
  if (knot_time < old_knot_time)
    knot_time = old_knot_time;
  render_knot();
  drawknot();
  draw_background(FALSE);
}
