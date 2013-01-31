#include <stdlib.h>
#include <math.h>
#include <iostream>
#include "etoile.h"
#include <cstdio>
#include <cstring>

Rect	windRect;

FloatPoint *pts=0;
FloatPoint *speed=0;
FloatPoint *Centers=0;
int         *newline=0;
FloatPoint *Centers2=0;
int         *newline2=0;
int numCenters;
int numCenters2;

FloatPoint Center(200.,200.,0);
int ptsNum=2000, ptsNumMax=2000;

unsigned short *gfrq[256];
int beat;
int beatcnt;
int thick, quiet;

int frames = 0;

matrix rot;
extern int gloudness;
void affloudness(int i)
{
  static int curi =0;

  if(i== 0)
    {
      curi++;
      curi%=(ptsNum/3);
    }
  if(i/3 == curi)
    {
      //      gloudness=0;
      Centers[i][0]=400- (i%3)*(800.0/(myfloat)(ptsNum/3));
      Centers[i][1]=gloudness/6-300;
      Centers[i][2]=0;
    }
  else
    {
      Centers[i][0]-=(800.0/(myfloat)(ptsNum/3));
      Centers[i][2]= (Centers[i][0])/30;
      Centers[i][2]*=  -Centers[i][2]*gloudness/400;

    }
}
void afffilter(int i)
{
  Centers[i][0]= -400+ i*(800.0/(myfloat)(ptsNum));
  Centers[i][1]= detector.filter[i]/2 -300;
  Centers[i][2]=0;
}
void afffilter2(int i)
{
  Centers[i][0]= -400+ i*(800.0/(myfloat)(ptsNum));
  Centers[i][1]= detector.filter2[i]/2 -300;
  Centers[i][2]=0;
}
extern int titleHasChanged;
void etoileLoop(void)
{
  int frames2=frames;
  static int nextframes;
  static int nextbeat;
   if(p.size<1E-6)
	 {
	   init_parameters();
	   visual_log (VISUAL_LOG_INFO, "Size is too small, possible bug with strtof()");
	   //p.size=.2;
	 }
  frames++;
  if(titleHasChanged || (beatcnt >nextbeat  ) || frames > nextframes)
    {
      changep();
      nextframes = frames + p.duration_f;
      nextbeat = beatcnt +  p.duration_b;
    }
  if(beat==2)
    beat--;

  Center=eSetCenter(frames);

  rot=matrix(p.rotspeed1,FloatPoint(cos(((double)frames2)/120.)*2,cos(((double)frames2)/54.)*2,cos(((double)frames2)/231.)*2));

  for(int i=0;i<ptsNum;i++)
    {
      switch(p.mode)
		{
		case 1:
		  Elastantig(i,Center);
		  Tourne(i);
		  break;
		case 2:
		case 3:
		case 4:
		case 5:
		  Elastantig(i,Centers[i%numCenters]);
		  Tourne(i);
		  break;
		  //case 5:
		  //affloudness(i);
		  //Elastantig(i,Centers[i]);
		  break;
		case 6:
		  afffilter(i);
		  Elastantig(i,Centers[i]);
		  break;
		case 7:
		  afffilter2(i);
		  Elastantig(i,Centers[i]);
		  break;
		default:
		  Elastantig(i,Center);
		  Tourne(i);
		  break;
		}
    }

}
void Elastantig(short i , FloatPoint & ctr)
{
  FloatPoint dist = pts[i]-ctr;
  float d=(float)dist.length();
  FloatPoint vectunite=(dist / d);

  // antigravitation  try not to make it n^2
  for(int k=0;k<p.agnumparts;k++)
    {
      int j = (frames + k + i )%ptsNum;
      /*      if((i==0) &&(k==0))
			  cout << j << " ";
      */
      if(i!=j)
		{
		  FloatPoint vect=pts[i]-pts[j];
		  float d1=(float)vect.length();
		  float d2=d1;
		  if(d2!=0)
			{
			  // d include the normalisation
			  for(int k=0; k< p.antigorder;k++)
				d2=d2*d1;
			  d2=(10*p.ag)/d2;

			  if(d2>p.maxantig)
				d2=p.maxantig;
			  vect=(vect * d2);
			  if(p.noagexplosion)// just to make a sphere
				vect= vect - ( vectunite * (vect % vectunite));

			  speed[i]+= vect ;
			}
		  else
			{
			  pts[i][0]+=0.01;
			}
		}
    }

  float d0=p.d0min+ (((p.dancingpartk*i +frames)%ptsNum<p.dancingpart*ptsNum)?//is it dancing?
					 (heights[(p.dancingpartk*i)%p.numfrq]*p.velocity)
					 :0);
  float f=-p.k*(d-d0);
  vectunite = vectunite* f;
  speed[i]+= vectunite ;


  speed[i]= speed[i]  /p.visc;
  pts[i]+= speed[i];// /1000;
  if(p.mode==5)
	{
	  ctr[2]-=50;
	}
}

void Tourne(short i)
{
	rot.ApplyMatrix	(pts[i],Center);
}


FloatPoint eSetCenter(long time)
{
  static FloatPoint a(p.d1,0,0),c(0,0,0);
  matrix rot;
  if(p.mode>1)
    {
      rot=matrix(p.rotspeed2,FloatPoint(cos(((double)time)/40.)*5,cos(((double)time)/124.)*5,cos(((double)time)/231.)*5));
      for(int i=0;i<numCenters;i++)
	{
	    rot.ApplyMatrix (Centers[i],c);
	}
    }
  else
    {
      rot=matrix(p.rotspeed2,FloatPoint(cos(((double)time)/40.)*5,cos(((double)time)/124.)*5,cos(((double)time)/231.)*5));
      rot.ApplyMatrix (a,c);

      FloatPoint dist=a;
      float d=(float)dist.length();
      float d0=p.d1;
      FloatPoint vectunite=(dist / d);
      float f=-1*(d-d0);

      vectunite = vectunite* f;
      a+= vectunite ;
    }

  return a;//FloatPoint(200+(LV::rand()%(width-200)),200+(LV::rand()%(height-200)),0)
}
