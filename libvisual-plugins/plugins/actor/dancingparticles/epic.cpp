#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include "etoile.h"


static float totLength;



void addpoints(char *p)
{
  int x,y;
  char *a,*b;
  FloatPoint t;
  while(1)
    {
      while(*p && *p!='(')p++;
      if(*p==0)
	return;
      p++;
      a=p;
      while(*p && *p!=',')p++;
      if(*p==0)
	return;

      *p=0;
      p++;
      b=p;
      while(*p && *p!=')')p++;
      if(*p==0)
	return;

      *p=0;
      p++;
      x=atoi(a);
      y=atoi(b);
      t=FloatPoint(x/15 - 300,y/15-200,0);
      if(!newline2[numCenters2])
        totLength+=(t-Centers2[numCenters2-1]).length();

      Centers2[numCenters2++]=t;
      newline2[numCenters2]=0;
    }


}

// we should change this hack..
void loadepic2(const char *file)
{
  FILE *f = fopen(file,"r");
  int mode=0;
  char buf[255];

  if(!f)
    {
      printf("unable to open the file %s \n", file);
      exit(1);
    }
  numCenters2 = 0;
  totLength=0;

  while(fgets(buf,sizeof(buf),f))
    {

      if(buf[0]=='\\')
	mode = 0;
      switch(mode)
	{
	case 0:
	  if(memcmp(buf,"\\drawline",9)==0)
	  {
	    char *p=buf+9;
	    newline2[numCenters2]=1;
	    addpoints(p);
	    mode++;
	  }
	  break;
	case 1:
	  addpoints(buf);
	  break;


	}
    }
  fclose(f);
  if(!numCenters2)
    printf("pas de points\n");

}

void loadepic(const char *filename)
{
  loadepic2(filename);
  numCenters=0;
  Centers[numCenters++]=Centers2[0];
  for(int i=1;i<numCenters2;i++)
  {
    if(!newline2[i])
    {
        int nump = (int)( ptsNum*((Centers2[i]-Centers2[i-1]).length()/totLength));
        for(int j=1;j<=nump;j++)
        {
            if(numCenters<=ptsNum)
                Centers[numCenters++] = (Centers2[i-1]*((nump)-j) + Centers2[i]*j )/(nump);
        }
    }
    else
        if(numCenters<=ptsNum)
            Centers[numCenters++] = Centers2[i];
  }

}
