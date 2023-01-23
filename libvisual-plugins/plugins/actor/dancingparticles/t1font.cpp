#include "t1font.h"
#include "etoile.h"
#include <t1lib.h>
#include <cstring>

#include <stdio.h>

void draw_bitmap(char * glyph_bits, int width, int height) {
    printf("%dx%d\n", width, height);

    // This prints rotated by 90° clockwise
    for (int y = 0; y < width; y++) {
        for (int x = height - 1; x >= 0; x--) {
            const unsigned char pixel = glyph_bits[x * width + y];
            if (pixel == 0) {
                printf("__ ");
            } else {
                printf("%02x ", pixel);
            }
        }
        printf("\n");
    }
    printf("%dx%d\n", width, height);
}

void loadString(const char *str)
{
  static bool inited = false;
  static int fontID;

  if(!inited)
	{
	  inited = true;
	  T1_InitLib(NO_LOGFILE);
	  T1_AASetBitsPerPixel(8);
	  T1_AASetLevel(T1_AA_LOW);
	  T1_AASetGrayValues( 0,65,127,191,255 );
	  fontID = T1_AddFont(const_cast<char*>(DATA_DIR "/font.pfb"));
	}

  GLYPH* glyph;
  int length = std::strlen(str);
  if(length > (ptsNum/50))
	length = ptsNum/50;

  glyph = T1_AASetString(fontID, const_cast<char*>(str), length, 0, 0, 25, 0);
  if(glyph && glyph->bits)
	{
	  int height_c = glyph->metrics.ascent - glyph->metrics.descent;
	  int width_c = glyph->metrics.rightSideBearing - glyph->metrics.leftSideBearing;
draw_bitmap(glyph->bits, width_c, height_c);
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
printf("[%4d/%4d][%d] x=%5d, y=%5d\n", numPart, numCenters, a / 4, x*4-width_c*2, height_c*8-y*16+a);
					Centers[numPart++]=FloatPoint(x*4-width_c*2,height_c*8-y*16+a,0);
					scoreToGo= (numPart*maxscore)/numCenters;
					a+=4;
				  }
			}
		}


	}
  else
	visual_log (VISUAL_LOG_WARNING, "No glyph");
}
