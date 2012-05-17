
#include "LCDTimer.h"

using namespace LCD;

LCDTimer::LCDTimer(LCDEventFunc func, void *data, int duration, bool repeating)
{

    mDuration = duration;
    mRepeating = repeating;
    mStartTime = 0;
    mTimer = visual_timer_new();

    mEvent = new LCDEvent(func, data);
}

LCDTimer::~LCDTimer()
{
    delete mEvent;
}

void LCDTimer::Start()
{
    visual_timer_start(mTimer);
}

void LCDTimer::Start(int duration)
{
    mDuration = duration;
    Start();
}

void LCDTimer::Start(int duration, void *data)
{
    mDuration = duration;
    mEvent->mData = data;
}

void LCDTimer::Start(int duration, void *data, LCDEventFunc func)
{
    mEvent->mData = data;
    mEvent->mFunc = func;
    mDuration = duration;
    Start();
}

void LCDTimer::Stop()
{
    visual_timer_stop(mTimer);
}

void LCDTimer::Tick()
{
    if(visual_timer_elapsed_msecs(mTimer) > mDuration)
    {
        mEvent->Fire();
        if(mRepeating)
        {
            visual_timer_reset(mTimer);
            visual_timer_start(mTimer);
        }
        else
        {
            visual_timer_stop(mTimer);
        }
    }
}

LCDTimerBin::LCDTimerBin() : mTimers(0), mEvents()
{
}

LCDTimerBin::~LCDTimerBin()
{
}

LCDTimer *LCDTimerBin::AddTimer(LCDEventFunc func, void *data, int duration, bool repeating)
{
    LCDTimer *timer = new LCDTimer(func, data, duration, repeating);

    mTimers.push_back(timer);

    return timer;
}

void LCDTimerBin::Tick()
{
    unsigned int i;
    for(i = 0; i < mTimers.size(); i++)
    {
        mTimers[i]->Tick();
    }
}

void LCDTimerBin::Stop()
{
    unsigned int i;
    for(i = 0; i < mTimers.size(); i++)
    {
        //(mTimers[i])->Stop();
    }
}

