//#include "t1font.h"
#include "etoile.h"
//#include  <t1lib.h> 
#include <cstdio>
#include <cstdlib>

void loadString(char *str)
{
#if 0
  static bool inited = false;
  static int fontID;
  if(!inited)
	{
	  inited = true; 
	  T1_InitLib(NO_LOGFILE);
	  T1_AASetBitsPerPixel(8);
	  T1_AASetLevel(T1_AA_LOW);
	  T1_AASetGrayValues( 0,65,127,191,255 );
	  fontID = T1_AddFont("/usr/local/share/dancingparticles/font.pfb");
	}
  GLYPH* glyph;
  int length =  strlen(str);
  if(length > (ptsNum/50))length = ptsNum/50;
  glyph = T1_AASetString(  fontID, str,length, 
						 0, 0, 
						 25, 0);
  if(glyph && glyph->bits)
	{
	  int height_c = glyph->metrics.ascent - glyph->metrics.descent;
	  int width_c = glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
	  int maxscore = 0;
	  for(int y = 0; y < height_c ; y++)
		{
		  for(int x = 0; x < width_c ; x++)
			{
			  unsigned char c = glyph->bits[y*width_c+x];
			  maxscore +=c;
			}

		}
	  numCenters = numCenters2 = ptsNum;
	  int curscore = 0;
	  int lastscore = 0;
	  int numPart = 0;
	  int scoreToGo = 1;
	  for(int y = 0; y < height_c ; y++)
		{
		  for(int x = 0; x < width_c ; x++)
			{
			  unsigned char c = glyph->bits[y*width_c+x];
			  int a = 0;
			  lastscore = curscore;
			  curscore +=c;
			  if(c!=0)
				while(lastscore < scoreToGo  && curscore >= scoreToGo) 
				  {
					Centers[numPart++]=FloatPoint(x*4-width_c*2,height_c*8-y*16+a,0);
					scoreToGo= (numPart*maxscore)/numCenters;
					a+=4;
				  }
			}
		}
	  
	
	}
  else
	std::cout <<"no glyph!!\n";

#endif
}


