#include <iostream>
#include "etoile.h"
#include <stdio.h>
#include <stdlib.h>
#include "t1font.h"
// or why use lex when..


int nump=0;
parameters *ps=0;

parameters p;

char *configfilename = "";
int IKnowAboutConfigFile;
parameters p_parsed;
static  int newconfig = 1;

enum { INT, STR, FLOAT, CHAR };
struct param_params{ char *pname; int type; void * where; };
param_params pp[] = 
  { { "name", STR,  &p_parsed.name },
    { "key", CHAR,  &p_parsed.key },
    { "mode", INT,  &p_parsed.mode },
    { "k", FLOAT,  &p_parsed.k },
    { "d0min", FLOAT,  &p_parsed.d0min },
    { "d1", FLOAT,  &p_parsed.d1 },
    { "ag", FLOAT,  &p_parsed.ag },
    { "agnumparts", INT,  &p_parsed.agnumparts },
    { "antigorder", INT,  &p_parsed.antigorder },
    { "maxantig", FLOAT,  &p_parsed.maxantig },
    { "noagexplosion", INT,  &p_parsed.noagexplosion },
    { "dancingpart", FLOAT,  &p_parsed.dancingpart },
    { "dancingpartk", INT,  &p_parsed.dancingpartk },
    { "velocity", FLOAT,  &p_parsed.velocity },
    { "numfrq", INT,  &p_parsed.numfrq },
    { "visc", FLOAT,  &p_parsed.visc },
    { "rotspeed1", FLOAT,  &p_parsed.rotspeed1 },
    { "rotspeed2", FLOAT,  &p_parsed.rotspeed2 },
    { "strombo", FLOAT,  &p_parsed.strombo },
    { "numpart", INT,  &p_parsed.numpart},
    { "size", FLOAT,  &p_parsed.size},
    { "sizeloudness", FLOAT,  &p_parsed.sizeloudness},
    { "chance", INT,  &p_parsed.chance },
    { "duration_f", INT,  &p_parsed.duration_f },
    { "duration_b", INT,  &p_parsed.duration_b },


    { "MaxParticles", INT,  &ptsNumMax },
    { "IKnowAboutConfigFile", INT,  &IKnowAboutConfigFile },
  };
int numpp= sizeof(pp)/sizeof(pp[0]);

char *nextword(char * &ptr)
{
  char *deb;
  while ( *ptr==' ' || *ptr=='\t')
    ptr++;
  deb = ptr;
  while ( *ptr!=' ' && *ptr!='\t' && *ptr!='\0' && *ptr!='\n')
    ptr++;
  if(*ptr!=0)
    *ptr++ = 0;
  return deb;

}

void freeparams()
{
  for(int i=0;i<nump;i++)
    {
      if(ps[i].name)
	free(ps[i].name);
    }
  free(ps);
  nump=0;
}
void changep();
// a very tolerant parser...

int load_parameters(char * filename);
void init_parameters()
{
  if(!load_parameters(configfilename))
    if(!load_parameters("/usr/local/share/dancingparticles/dancingparticles.conf"))
      {
      }
}
int load_parameters(char * filename)
{
  FILE *f = fopen(filename,"r");
  if(f==NULL)
    return 0;
  char buf[1024];
  char * ptr, *word, *err;
  int line = 1, mode = 0;
  
  char *oldlocal = setlocale (LC_NUMERIC, "C");


  if(ps)
    freeparams();
  p_parsed.name = 0;

  fgets(buf,1024, f);

  while(!feof(f))
    {
      ptr = buf;
      word = nextword(ptr);
      if(word[0]!= '#')
	{
	  switch (mode)
	    {
	    case 0: //Begin
	      if(strcmp(word, "Begin") == 0)
		{
		  word = nextword(ptr);
		  if(strcmp(word, "Effect") == 0)
		    {
		      mode = 1;
		      word = nextword(ptr);
		      p_parsed.name = 0;
		      if(strcmp(word, "inherits") == 0)
			{
			  word = nextword(ptr);
			  for(int i=0;i<nump;i++)
			    {
			      if(strcmp(ps[i].name, word)==0)
				{
				  p_parsed = ps[i];
				  p_parsed.name = strdup(p_parsed.name);
				  break;
				}
			    }
			}
		    }
		  else
		    {
		      err= "unknown Begin";
		      goto error;
		    }
	      
		}
	 
	    case 1:
	      if(strcmp(word, "End") == 0)
			{
			  word = nextword(ptr);
	      
			  if(strcmp(word, "Effect") == 0)
				{
				  if(p_parsed.name==0)
					{
					  err = "no name for this effect";
					  goto error;
					}
				  mode = 0;
				  if(nump++==0)
					ps = (parameters *) malloc(nump* sizeof(parameters));
				  else
					ps = (parameters *) realloc(ps,nump* sizeof(parameters));
				  ps[nump-1] = p_parsed;
				  
				}
	     
			}
	      else
		{
		  char *var,  *egal, *val; 
		  int i;
		  var = word;
		  egal = nextword(ptr);
		  val = nextword(ptr);
		  for(i=0;i<numpp;i++)
		    {
		      if(strcmp(var, pp[i].pname)==0)
			{
			  switch (pp[i].type)
			    {
			    case INT:
			      sscanf(val, "%d", (int *)pp[i].where);
			      break;
			    case FLOAT:
				  float val2 ;
				  val2 = atof(val);
				  *((float *)pp[i].where) = val2;
			      break;
			    case CHAR:
			      sscanf(val, "%c", (char *)pp[i].where);
			      break;
			    case STR:
			      if(*(char **)pp[i].where)
				free(*(char **)pp[i].where);
			      *(char **)pp[i].where = strdup(val);
			      break;
			    }
			  break;
			}
		    }
		  if(i==numpp && *egal == '=')
		    {
		      err= "unknown parameter";
		      goto error;
		    }
		}
	    }
	}
      fgets(buf,1024, f);line++;
    }
  if(mode == 1)
    {
      err = " missing End Effect";
      goto error;
    }
  setlocale (LC_NUMERIC, oldlocal);


  fclose(f);
  newconfig = 1;
  allocParts();
  changep();  
  return 1;
 error:
  setlocale (LC_NUMERIC, oldlocal);
  fclose(f);
  return 0;
}

void allocParts()
{
  static int hasallocated = 0; // you cannot change the MaxParticles dynamicaly
  
  if(!hasallocated)
    {
      hasallocated = 1;
      if(pts)
		free(pts);
      pts = (FloatPoint *) malloc(ptsNumMax * sizeof(FloatPoint));
      if(speed)
		free(speed);
      speed = (FloatPoint *) malloc(ptsNumMax * sizeof(FloatPoint));
	  if(Centers)
		free(Centers);
      Centers = (FloatPoint *) malloc(ptsNumMax * sizeof(FloatPoint));
      if(newline)
		free(newline);
      newline = (int *) malloc(ptsNumMax * sizeof(int));
      if(Centers2)
		free(Centers2);
      Centers2 = (FloatPoint *) malloc(ptsNumMax * sizeof(FloatPoint));
      if(newline2)
		free(newline2);
      newline2 = (int *) malloc(ptsNumMax * sizeof(int));

      for(int i=0;i<ptsNumMax;i++)
		{
		  pts[i][0]=rand()%300;
		  pts[i][1]=rand()%400;
		  pts[i][2]=rand()%400;
		  speed[i][0] = 0;
		  speed[i][1] = 0;
		  speed[i][2] = 0;
		  Centers[i][0] = 0;
		  Centers[i][1] = 0;
		  Centers[i][2] = 0;

		  //pts[i]=pts[i-1]+diff; 
		}
    }
}
extern char *curtitle;

extern int titleHasChanged;
void  init_parameters();
void changep()
{
  int i;
  int tot=0;
  int r;
  static int lasti=-1;

  for(i=0;i<nump;i++)
    tot = tot + ps[i].chance;
  r=rand()%tot;
  for(i=-1;r>=0;r-=ps[i].chance)i++;

  if(titleHasChanged)
	for(int j=0;j< nump;j++)
	  if(ps[j].mode==5)
		{
		  i=j;
		  lasti = -1;//force change!
		}
  titleHasChanged = 0;

  p = ps[i];
  if(newconfig || lasti!=i)
    {
      newconfig = 0;
      lasti = i;
      ptsNum = (p.numpart < ptsNumMax) ? p.numpart : ptsNumMax;
      p.agnumparts = (p.numpart < p.agnumparts) ? p.numpart :p.agnumparts ;
      if(p.mode==2)
	{
	  int i=0,j;
	  for(j=i;i<j+ptsNum/8;i++)
	    Centers[i]= FloatPoint( -400 + i*800/(ptsNum/8),0,0);
	  for(j=i;i<j+ptsNum/8;i++)
	    Centers[i]= FloatPoint(0, -400 + (i-j)*800/(ptsNum/8),0);
	  for(j=i;i<j+ptsNum/8;i++)
	    Centers[i]= FloatPoint( 0,0,-400 + (i-j)*800/(ptsNum/8));
	  for(j=i;i<j+ptsNum/8;i++)
	    Centers[i]= FloatPoint( -400 + (i-j)*800/(ptsNum/8),-400 + (i-j)*800/(ptsNum/8),0);
	  numCenters=i;
	  
	}
      if(p.mode==3)
	{
	  loadepic("dance.epic");
	}
      if(p.mode==4)
	{
	  loadepic("xmms.epic");
	}
      if(p.mode == 5)
      {
		loadString(curtitle);
      }
    }
}


void etoileinit(void)
{
//  init_parameters();
	allocParts();
  frames=0;
//  changep();
  return ;	
}

