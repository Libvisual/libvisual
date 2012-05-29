#ifndef _LV_TOOL_DISPLAY_DRIVER_HPP
#define _LV_TOOL_DISPLAY_DRIVER_HPP

#include <string>
#include <libvisual/libvisual.h>

class SADisplay;

class SADisplayDriver {
public:

    virtual LV::VideoPtr create (VisVideoDepth depth,
                                 VisVideoAttrOptions const* vidoptions,
                                 unsigned int width,
                                 unsigned int height,
                                 bool resizable) = 0;

    virtual void close  () = 0;

    virtual void lock   () = 0;

    virtual void unlock () = 0;

    virtual void set_fullscreen (bool fullscreen, bool autoscale) = 0;

    virtual void update_rect (LV::Rect const& rect) = 0;

    virtual void drain_events (VisEventQueue& eventqueue) = 0;

    virtual LV::VideoPtr get_video () const = 0;

    virtual void set_title(std::string const& title) = 0;

    virtual ~SADisplayDriver () {}
};

#endif // _LV_TOOL_DISPLAY_DRIVER_HPP
