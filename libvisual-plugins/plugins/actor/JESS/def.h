#ifndef _JESS_DEF_H
#define _JESS_DEF_H

#define PI 3.1416
#define RESX 640 /* start resolution */
#define RESY 300 
#define RESX_D 640 /* rerence resolution */
#define RESY_D 300 

#define SCREENFLAG SDL_HWSURFACE|SDL_HWPALETTE|SDL_DOUBLEBUF|SDL_ASYNCBLIT|SDL_HWACCEL|SDL_RLEACCEL

#define RESFACTX(par1) ( (int) ( (float) resx*(par1)/RESX_D ))
#define RESFACTY(par1) ( (int) ( (float) resy*(par1)/RESY_D ))
#define RESFACTXF(par1) ( (float) resx*(par1)/RESX_D )
#define RESFACTYF(par1) ( (float) resy*(par1)/RESY_D )

#define FUSEE_MAX 10
#define FUSEE_VIE 5
#define FUSEE_RAYON 210
#define FUSEE_COLOR 250

#define LINE_MAX 10
#define LINE_VIE 60

#define MANAGE 0
#define NEW 1
#define REINIT 1
#define NEW_SESSION 2
#define DEDT_FACTOR 10000 /* speed fading control*/

#define STARS_MAX 256
#define MORPH_SPEED 0.5

#ifndef NULL
#define NULL 0
#endif

#endif
