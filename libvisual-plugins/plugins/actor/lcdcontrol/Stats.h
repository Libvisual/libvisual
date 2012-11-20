#ifndef __STATS_H
#define __STATS_H
#include <sys/time.h>

/* simple stats management */
typedef struct {
    double  renderTime;
    double  frameTime;
} FrameStats;

#define  MAX_FRAME_STATS  200
#define  MAX_PERIOD_MS    1500

typedef struct {
    double  firstTime;
    double  lastTime;
    double  frameTime;

    int         firstFrame;
    int         numFrames;
    FrameStats  frames[ MAX_FRAME_STATS ];
    double avgFrame, maxFrame, minFrame, avgRender, minRender, maxRender;
} Stats;

double now_ms(void);

void stats_init( Stats*  s );
void stats_startFrame( Stats*  s );
void stats_endFrame( Stats*  s );

#endif
