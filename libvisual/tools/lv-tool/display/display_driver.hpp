#ifndef _LV_TOOL_DISPLAY_DRIVER_HPP
#define _LV_TOOL_DISPLAY_DRIVER_HPP

#include <libvisual/libvisual.h>

class SADisplay;

class SADisplayDriver {
public:

    virtual int create (VisVideoDepth depth, VisVideoAttributeOptions const* vidoptions,
						unsigned int width, unsigned int height, bool resizable) = 0;
    virtual int close  () = 0;
    virtual int lock   () = 0;
    virtual int unlock () = 0;
    virtual int set_fullscreen (bool fullscreen, bool autoscale) = 0;
    virtual int update_rect (LV::Rect const& rect) = 0;
    virtual int drain_events (VisEventQueue& eventqueue) = 0;

    virtual int get_video (VisVideo* screen) = 0;

    virtual ~SADisplayDriver () {}
};

#endif // _LV_TOOL_DISPLAY_DRIVER_HPP
