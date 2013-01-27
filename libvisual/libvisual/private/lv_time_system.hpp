#ifndef _LV_TIMING_SYSTEM_HPP
#define _LV_TIMING_SYSTEM_HPP

#include "lv_time.h"

namespace LV {

  namespace TimeSystem {

    void start ();

    void shutdown ();

    Time now ();

    void usleep (uint64_t usecs);

  } // TimeSystem

} // LV

#endif // _LV_TIMING_SYSTEM_HPP
