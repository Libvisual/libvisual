
#include <pthread.h>
#include <sched.h>


#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "gl.h"
#include "etoile.h"



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
  glTexImage2D(GL_TEXTURE_2D, 0, 3, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE,&imageData[0][0][0] );

}
void init_gl(void)
{
 // glViewport(0, 0, 640, 480);
  LoadTexture();
//  glEnable(GL_TEXTURE_2D);			// Enable Texture Mapping
//  glClearColor(0.0f, 0.0f, 1.0f, 0.0f);	// Clear The Background Color To Blue 
//  glClearDepth(1.0);				// Enables Clearing Of The Depth Buffer
//  glDepthFunc(GL_LESS);			// The Type Of Depth Test To Do
//  glEnable(GL_DEPTH_TEST);			// Enables Depth Testing
//  glShadeModel(GL_SMOOTH);			// Enables Smooth Color Shading
    
//  glMatrixMode(GL_PROJECTION);
//  glLoadIdentity();				// Reset The Projection Matrix
    
    
//  gluPerspective(45.0f,(GLfloat)640/(GLfloat)480,0.1f,100.0f);	// Calculate The Aspect Ratio Of The Window
    
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
  static float r=0, SIZE=1;

  float Size =(p.size + p.sizeloudness * gloudness)/2;
  SIZE = (SIZE +3*Size) /4;
  r++;
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(-1, 1, -1, 1, 1.5, 40);
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
	  glBegin(GL_QUADS);
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
        {1.,0.2,0.2   }
        ,{0.2,1.,0.2}
        ,{0.4,0.4,1}};
        
      if(p.mode < 50)
		{
		  glBindTexture(GL_TEXTURE_2D, texture[0]);   // choose the texture to use.
		  glEnable(GL_TEXTURE_2D);
		  glPushMatrix();
		  glTranslatef(pts[i][0]/100,pts[i][1]/100,pts[i][2]/100);
		  glBegin(GL_QUADS);
		  glColor3fv(colors[i%3] );
        
		  //cout<<i<<  " :"<<pts[i][0] << " " <<pts[i][1] << " " << pts[i][2] << endl;
		  glTexCoord2f(0,0);        glVertex3f(-SIZE, -SIZE, 0);
		  glTexCoord2f(1,0);        glVertex3f(SIZE,-SIZE, 0);
		  glTexCoord2f(1,1);        glVertex3f(SIZE, SIZE, 0);
		  glTexCoord2f(0,1);        glVertex3f(-SIZE, SIZE, 0);
        
		  glEnd();
		  glPopMatrix();
		}
      else
		{
		  //float SIZE2=SIZE*10;
		  glDisable(GL_TEXTURE_2D);
		  if(pts[(i+1)%ptsNum][0]>pts[i][0])
			{
			  glBegin(GL_QUADS);
			  /*	      if(speed[i][0]>0)
						  {
						  //		  glColor3fv(colors[0] );
						  glVertex3f(pts[i][0]/100,1,pts[i][2]/100);
						  glVertex3f(pts[i][0]/100+SIZE2,1,pts[i][2]/100);
						  glVertex3f(pts[i][0]/100+SIZE2, 1+SIZE2, pts[(i+1)%ptsNum][2]/100);
						  glVertex3f(pts[i][0]/100,1+SIZE2,pts[(i+1)%ptsNum][2]/100);
						  glEnd();

						  glBegin(GL_QUADS);
						  //		  cout <<"tptp\n";
						  }
			  */
			  glColor3fv(colors[2] );
			  glVertex3f(pts[i][0]/100,pts[i][1]/100,0);
			  glVertex3f(pts[i][0]/100+SIZE,pts[i][1]/100,0);
			  glVertex3f(pts[(i+1)%ptsNum][0]/100+SIZE, pts[(i+1)%ptsNum][1]/100,0);
			  glVertex3f(pts[(i+1)%ptsNum][0]/100,pts[(i+1)%ptsNum][1]/100,0);
			  glEnd();
			}
		}
    }
  
#endif
}



