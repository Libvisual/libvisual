#include "config.h"
#include "display_driver_factory.hpp"
#include "stdout_driver.hpp"

#if HAVE_SDL
#include "sdl_driver.hpp"
#endif

#if HAVE_GLX
#include "glx_driver.hpp"
#endif

#include <algorithm>
#include <iterator>
#include <map>


typedef std::map<std::string, DisplayDriverCreator> CreatorMap;


class DisplayDriverFactory::Impl
{
public:

    CreatorMap creators;
};

DisplayDriverFactory::DisplayDriverFactory ()
    : m_impl (new Impl)
{
    add_driver ("stdout", stdout_driver_new);
#if defined(HAVE_SDL)
    add_driver ("sdl", sdl_driver_new);
#endif
}

DisplayDriverFactory::~DisplayDriverFactory ()
{
    // nothing to do
}

void DisplayDriverFactory::add_driver (std::string const& name, Creator creator)
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

bool DisplayDriverFactory::has_driver (std::string const& name) const
{
    return (m_impl->creators.find (name) != m_impl->creators.end ());
}

void DisplayDriverFactory::get_driver_list (DisplayDriverList& list) const
{
    typedef std::back_insert_iterator<DisplayDriverList> BackInserter;

    list.clear ();
    list.reserve (m_impl->creators.size ());

    std::transform (m_impl->creators.begin (),
                    m_impl->creators.end (),
                    BackInserter (list),
                    LV::select1st<CreatorMap::value_type>);
}
