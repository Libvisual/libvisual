#include "config.h"
#include "gl.h"
#include "etoile.h"

#ifdef USE_OPENGL_ES
#include <GLES/gl.h>
#include "common/GL/glu.h"
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <pthread.h>
#include <sched.h>

GLfloat heights[NUM_BANDS];
extern pthread_mutex_t height_mutex;
extern bool paused;
GLuint texture[2];

void LoadTexture()
{
  static GLubyte imageData[64][64][3];
  for(int i=0;i<64;i++){
    for(int j=0;j<64;j++){
      float x= 255*fastsqrt((i-32)*(i-32)+(j-32)*(j-32))/32;

      // several formulas have been tested, the better is the "hyperbole"
      //x= 200*(sin(x/56)/(x/56)+0.23);
      //x= 35000*(1/(x+100)-0.0025);
      x=15000*(1/(x+50)-0.003);
      if(x<0)
		x=0;
      imageData[i][j][0]= (GLubyte)x;
      imageData[i][j][1]= (GLubyte)x;
      imageData[i][j][2]= (GLubyte)x;

    }
  }
  // Create Texture
  glGenTextures(1, &texture[0]);
  glBindTexture(GL_TEXTURE_2D, texture[0]);   // 2d texture (x and y size)

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); // scale linearly when image bigger than texture
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // scale linearly when image smalled than texture

  // 2d texture, level of detail 0 (normal), 3 components (red, green, blue), x size from image, y size from image,
  // border 0 (normal), rgb color data, unsigned byte data, and finally the data itself.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE,&imageData[0][0][0] );
}

void init_gl(void)
{
  LoadTexture();

  glViewport(0, 0, 640, 480);
  glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping
  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);	// Clear The Background Color To Blue
#ifdef USE_OPENGL_ES
  glClearDepthf(1.0);
#else
  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
#endif
  glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();				// Reset The Projection Matrix

  gluPerspective(45.0f,(GLfloat)640/(GLfloat)480,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window

  glMatrixMode(GL_MODELVIEW);
  etoileinit();
}

extern int *newline;
extern int numCenters;
extern int gloudness;
extern float spin_x ;
extern float spin_y ;

extern int gloudnesses[200];
extern int curloudness ;
extern int maxgloudness ;
extern int avgloudness ;
extern int mingloudness ;

void draw_gl(void)
{
  static float SIZE=1;

  float Size =(p.size + p.sizeloudness * gloudness)/2;
  SIZE = (SIZE +3*Size) /4;

  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
#ifdef USE_OPENGL_ES
  glFrustumf(-1, 1, -1, 1, 1.5, 40);
#else
  glFrustum(-1, 1, -1, 1, 1.5, 40);
#endif
  gluLookAt(0, 0, 6,
			0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // BEGIN DRAWING
  glDisable(GL_TEXTURE_2D);

#if DEBUG_BEAT
  if(beat>0)
    {
      beat--;
	  glBegin(GL_TRIANGLE_STRIP);
      glColor3f(0.1 , 0.9 , 0.1 );// subliminal beat..
      glVertex3f(2,2,0);
      glVertex3f(-2,2,0);
      glVertex3f(-2,-2,0);
      glVertex3f(2,-2,0);
	  glEnd();
    }
  glColor3f(1 , 1 , .3 );// subliminal beat..
  glBegin(GL_LINES);
  for(int i = 0; i < 200; i++)
	{
	  double x = (i-100);
	  double y = gloudnesses[(200+i+curloudness)%200];
	  int beat = 0;
	  if(y <0)
		{
		  y = -y;
		  beat = 1;
		}
	  y = 4*(y-100)/200;
      glVertex3f(x/30,y,0);
	  if(beat)
		{
		  glVertex3f(x/30,-1.9,0);
		  glVertex3f(x/30,-2,0);
		}
	  if(i>0 && i <199)
		glVertex3f(x/30,y,0);

	}
  double y = maxgloudness;
  y = 4*(y-100)/200;
  glVertex3f(-3,y,0);
  glVertex3f(3,y,0);

  y = mingloudness;
  y = 4*(y-100)/200;
  glVertex3f(-3,y,0);
  glVertex3f(3,y,0);

  y = avgloudness;
  y = 4*(y-100)/200;
  glVertex3f(-3,y,0);
  glVertex3f(3,y,0);

  glEnd();
#else
  glDisable(GL_DEPTH_TEST);
  glDepthMask(false);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  for(int i=0;i<ptsNum;i++)
    {
      GLfloat colors[][3]={
         {1.0,0.2,0.2},
         {0.2,1.0,0.2},
         {0.4,0.4,1.0}
      };

      if(p.mode < 50)
		{
		  static const GLfloat vertices[4][2] = {
			{ -1.0, -1.0 },
			{  1.0, -1.0 },
			{  1.0,	 1.0 },
			{ -1.0,	 1.0 }
		  };

		  static const GLfloat tex_coords[4][2] = {
			{ 0.0, 0.0 },
			{ 1.0, 0.0 },
			{ 1.0, 1.0 },
			{ 0.0, 1.0 }
		  };

		  glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.
		  glEnable(GL_TEXTURE_2D);
		  glPushMatrix();
		  glTranslatef(pts[i][0]/100,pts[i][1]/100,pts[i][2]/100);
		  glScalef(SIZE, SIZE, 1.0);

		  int c = i%3;
		  glColor4f(colors[c][0], colors[c][1], colors[c][2], 1.0f);

		  glEnableClientState(GL_VERTEX_ARRAY);
		  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		  glVertexPointer(2, GL_FLOAT, 0, vertices);
		  glTexCoordPointer(2, GL_FLOAT, 0, tex_coords);
		  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		  glDisableClientState(GL_VERTEX_ARRAY);

		  glPopMatrix();
		}
      else
		{
		  //float SIZE2=SIZE*10;
		  glDisable(GL_TEXTURE_2D);
		  if(pts[(i+1)%ptsNum][0]>pts[i][0])
			{
			  /*	      if(speed[i][0]>0)
						  {
						  glColor4f(colors[0][0], colors[0][1], colors[0][2], 1.0f);
						  glBegin(GL_QUADS);
						  glVertex3f(pts[i][0]/100,1,pts[i][2]/100);
						  glVertex3f(pts[i][0]/100+SIZE2,1,pts[i][2]/100);
						  glVertex3f(pts[i][0]/100+SIZE2, 1+SIZE2, pts[(i+1)%ptsNum][2]/100);
						  glVertex3f(pts[i][0]/100,1+SIZE2,pts[(i+1)%ptsNum][2]/100);
						  glEnd();
						  }
			  */
			  glColor4f(colors[2][0], colors[2][1], colors[2][2], 1.0f);

			  GLfloat vertices[4][2] = {
				{ pts[i][0]/100,pts[i][1]/100 },
				{ pts[i][0]/100+SIZE,pts[i][1]/100 },
				{ pts[(i+1)%ptsNum][0]/100+SIZE, pts[(i+1)%ptsNum][1]/100 },
				{ pts[(i+1)%ptsNum][0]/100,pts[(i+1)%ptsNum][1]/100 }
			  };

			  glEnableClientState(GL_VERTEX_ARRAY);
			  glVertexPointer(2,GL_FLOAT,0,vertices);
			  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			  glDisableClientState(GL_VERTEX_ARRAY);
			}
		}
    }

#endif
}
