#include "nebulus.h"

#define COUNT 50
#define DEPTH 60

GLfloat tunnel_time, tunnel_speed = 1.8f, old_tunnel_speed = 1.8f;

typedef struct
{
  GLfloat radius[COUNT];
} ring_type;

ring_type ring[DEPTH];
GLfloat itime = 0.0f, total_time = 0.0f;
GLfloat tex_rad = 1.0f, tex_len = 1.0f, len_fact = 0.50f, kot;
int start_ring = 0;
int tunnel_first = TRUE;


void
precalculate_tunnel(void)
{
  int i, j, k, uring;
  GLfloat ckot0;
  GLfloat x, y, r;
  GLfloat rx, ry;
  glcoord normal = { 0.0f, 0.0f, 0.0f };

  tunnel_time = 0.6f;
  for (k = 0; k < DEPTH; k++) {
    itime += tunnel_time * tunnel_speed;
    total_time += tunnel_time * tunnel_speed * 0.2f;
    while (itime > 1.0f) {
      itime -= 1.0f;
      for (i = 0; i < COUNT; i++) {
        kot = (GLfloat)i / COUNT * PI2;
        ring[start_ring].radius[i] = 100 + 20 * sinf(kot*3 + total_time)
                                         + 20 * sinf(kot*2 + total_time*2 + 50)
                                         + 15 * sinf(kot*4 - total_time*3 + 60);
        ring[start_ring].radius[i] *= 0.01f;
      }
      start_ring = (start_ring + 1) % DEPTH;
    }
    uring = start_ring;
    for (i = 0; i < DEPTH; i++) {
      for (j = 0; j < COUNT + 1; j++) {
        kot = (GLfloat)j / COUNT * PI2;
        ckot0 = ((GLfloat)i - itime) * 0.1f;
        rx = 0.3f * cosf(ckot0 + total_time * 3);
        ry = 0.3f * sinf(ckot0 + total_time * 4);
        r = ring[uring].radius[j % COUNT];
        x = rx + r * cosf(kot);
        y = ry + r * sinf(kot);
        normal.x = -x;
        normal.y = -y;
        normal.z = 1.0f - r;
        normal.x = normal.x * sqrt(normal.x * normal.x + normal.y*normal.y + 
                                   normal.z * normal.z);
        normal.y = normal.y * sqrt(normal.x * normal.x + normal.y*normal.y + 
                                   normal.z * normal.z);
        normal.z = normal.z * sqrt(normal.x * normal.x + normal.y*normal.y + 
                                   normal.z * normal.z);
        ckot0 += 0.1f;
        rx = 0.3f * cosf(ckot0 + total_time * 3);
        ry = 0.3f * sinf(ckot0 + total_time * 4);
        r = ring[(uring + 1) % DEPTH].radius[j % COUNT];
        x = rx + r * cosf(kot);
        y = ry + r * sinf(kot);
        normal.x = -x;
        normal.y = -y;
        normal.z = 1.0f - r;
        normal.x = normal.x * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                   normal.z * normal.z);
        normal.y = normal.y * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                   normal.z * normal.z);
        normal.z = normal.z * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                   normal.z * normal.z);
      }
      uring = (uring + 1) % DEPTH;
    }
  }
  tunnel_first = FALSE;
}


static void
updatetunnel(void)
{
  int i;

  tunnel_time = 0.06f;
  itime += tunnel_time * tunnel_speed;
  total_time += tunnel_time * tunnel_speed * 0.2f;
  while (itime > 1.0f) {
    itime -= 1.0f;
    for (i = 0; i < COUNT; i++) {
      kot = (GLfloat)i / COUNT * PI2;
      ring[start_ring].radius[i] = 100 + 20 * sinf(kot*3 + total_time)
                                       + 20 * sinf(kot*2 + total_time*2 + 50)
                                       + 15 * sinf(kot*4 - total_time*3 + 60);
      ring[start_ring].radius[i] *= 0.01f;
    }
    start_ring = (start_ring + 1) % DEPTH;
  }
}


static void
drawtunnel(void)
{
  int i, j, uring;
  GLfloat ckot0;
  GLfloat x, y, z, r;
  GLfloat rx, ry;
  GLfloat u, v;
  GLfloat fog_color[3] = { 0.0f, 0.0f, 0.0f };
  glcoord normal = { 0.0f, 0.0f, 0.0f };

  glPushMatrix();
  glTranslatef(0.0f, 0.0f, 0.0f);
  glColor3f(1.0f, 1.0f, 1.0f);
  glFogfv(GL_FOG_COLOR, fog_color);
  glFogf(GL_FOG_DENSITY, 0.04f);
  glFogi(GL_FOG_MODE, GL_EXP2);
  glEnable(GL_FOG);
  uring = start_ring;

  for (i = 0; i < DEPTH; i++) {
    glBegin(GL_QUAD_STRIP);
    for (j = 0; j < COUNT + 1; j++) {
      v = tex_rad * (GLfloat)j / COUNT;
      u = tex_len * (GLfloat)(i + total_time * 5.0f - itime) / DEPTH;
      kot = (GLfloat)j / COUNT * PI2;
      ckot0 = ((GLfloat)i - itime) * 0.1f;
      rx = 0.3f * cosf(ckot0 + total_time * 3);
      ry = 0.3f * sinf(ckot0 + total_time * 4);
      r = ring[uring].radius[j % COUNT];
      x = rx + r * cosf(kot);
      y = ry + r * sinf(kot);
      z = -((GLfloat)i - itime) * len_fact;
      normal.x = -x;
      normal.y = -y;
      normal.z = 1.0f - r;
      normal.x = normal.x * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                 normal.z * normal.z);
      normal.y = normal.y * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                 normal.z * normal.z);
      normal.z = normal.z * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                 normal.z * normal.z);
      glTexCoord2f(u, v);
      glVertex3f(x, y, z);
      ckot0 += 0.1f;
      rx = 0.3f * cosf(ckot0 + total_time * 3);
      ry = 0.3f * sinf(ckot0 + total_time * 4);
      u = tex_len * (GLfloat)(i + 1 + total_time * 5.0f - itime) / DEPTH;
      r = ring[(uring + 1) % DEPTH].radius[j % COUNT];
      x = rx + r * cosf(kot);
      y = ry + r * sinf(kot);
      z = -((GLfloat)i - itime + 1.0f) * len_fact;
      normal.x = -x;
      normal.y = -y;
      normal.z = 1.0f - r;
      normal.x = normal.x * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                 normal.z * normal.z);
      normal.y = normal.y * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                 normal.z * normal.z);
      normal.z = normal.z * sqrt(normal.x * normal.x + normal.y * normal.y + 
                                 normal.z * normal.z);
      glTexCoord2f(u, v);
      glVertex3f(x, y, z);
    }
    glEnd();
    uring = (uring + 1) % DEPTH;
  }
  glDisable(GL_FOG);
  glPopMatrix();
}


static void
render_tunnel(void)
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
draw_tunnel(void)
{
  tunnel_speed = old_tunnel_speed;
  tunnel_speed = reduce_vsync(tunnel_speed);
  render_tunnel();
  updatetunnel();
  drawtunnel();
}
