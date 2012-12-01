#ifndef __LCDEVENT
#define __LCDEVENT

#include <libvisual/libvisual.h>

typedef void (*LCDEventFunc) (void *data);

namespace LCD 
{
class LCDEvent {
    public:
    LCDEventFunc mFunc;
    void *mData;

    LCDEvent(LCDEventFunc func, void *data)
    {

        mFunc = func;
        mData = data;
    }

    ~LCDEvent()
    {
    }

    void Fire()
    {
        mFunc(mData);
    }
};
};

#endif

