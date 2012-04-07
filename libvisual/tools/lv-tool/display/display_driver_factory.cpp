#include "config.h"
#include "display_driver_factory.hpp"
#include <map>

#include "stdout_driver.hpp"

#if HAVE_SDL
#include "sdl_driver.hpp"
#endif

#if HAVE_GLX
#include "glx_driver.hpp"
#endif


typedef std::map<std::string, DisplayDriverCreator> CreatorMap;


class DisplayDriverFactory::Impl
{
public:

    CreatorMap creators;
};

DisplayDriverFactory::DisplayDriverFactory ()
    : m_impl (new Impl)
{
    add ("stdout", stdout_driver_new);
#if defined(HAVE_SDL)
    add ("sdl", sdl_driver_new);
#endif
#if defined(HAVE_GLX)
    add ("glx", glx_driver_new);
#endif
}

DisplayDriverFactory::~DisplayDriverFactory ()
{
    // nothing to do
}

void DisplayDriverFactory::add (std::string const& name, Creator creator)
{
    m_impl->creators[name] = creator;
}

SADisplayDriver* DisplayDriverFactory::make (std::string const& name, SADisplay& display)
{
    CreatorMap::const_iterator entry = m_impl->creators.find (name);

    if (entry == m_impl->creators.end())
        return 0;

    return (*entry->second) (display);
}
