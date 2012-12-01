
#ifndef __LCDPRIV__
#define __LCDPRIV__

#include <pthread.h>

namespace LCD {

class LCDControl;

#define PCM_SIZE	1024

typedef struct {
    VisPalette *pal;
	//VisBuffer	pcm;
    //VisThread *thread;
    pthread_attr_t attr;
    pthread_t id;
    LCDControl *control;
    uint32_t *pixels;
} LCDPrivate;

}

#endif
