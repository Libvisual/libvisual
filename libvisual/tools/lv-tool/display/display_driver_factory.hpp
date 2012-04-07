#ifndef _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP
#define _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP

#include "display_driver.hpp"
#include <libvisual/lv_scoped_ptr.hpp>

typedef SADisplayDriver* (*DisplayDriverCreator)(SADisplay& display);

typedef std::vector<std::string> DisplayDriverList;

class DisplayDriverFactory
{
public:

    typedef DisplayDriverCreator Creator;

    static DisplayDriverFactory& instance ()
    {
	    static DisplayDriverFactory m_instance;
	    return m_instance;
    }

    SADisplayDriver* make (std::string const& name, SADisplay& display);

    void add_driver (std::string const& name, Creator creator);

    bool has_driver (std::string const& name) const;

    void get_driver_list (DisplayDriverList& list) const;

private:

    class Impl;

    LV::ScopedPtr<Impl> m_impl;

    DisplayDriverFactory ();

    ~DisplayDriverFactory ();
};

#endif // _LV_TOOL_DISPLAY_DRIVER_FACTORY_HPP
