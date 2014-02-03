
#ifndef __LCDTIMER
#define __LCDTIMER

#include <string>
#include <vector>

#include <libvisual/libvisual.h>
#include "LCDEvent.h"

namespace LCD 
{

class LCDTimer
{
    private:
    LCDEvent *mEvent;
    
    unsigned int mDuration;
    int mStartTime;
    bool mRepeating;
    bool mActive;
    std::string mName;
    VisTimer *mTimer;
    
    public:

    LCDTimer(LCDEventFunc func, void *data, int duration, bool repeating);
    ~LCDTimer();
    void Start(int duration, void *data, LCDEventFunc func);
    void Start(int duration, void *data);
    void Start(int duration);
    void Start();
    void Stop();
    void Tick();
};

class LCDTimerBin {
    private:
    std::vector<LCDTimer *> mTimers;

    public:
    LCDTimerBin();
    ~LCDTimerBin();
    LCDTimer *AddTimer(LCDEventFunc func, void *data, int duration, bool repeating);
    void Tick();

    void Stop();
};

}; // End namespace
#endif
