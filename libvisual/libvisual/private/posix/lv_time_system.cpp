#define _POSIX_C_SOURCE 200112L

#include "config.h"
#include "private/lv_time_system.hpp"
#include <thread>
#include <time.h>

namespace LV {

  namespace TimeSystem {

    void start ()
    {
        // nothing to do
    }

    void shutdown ()
    {
        // nothing to do
    }

    void usleep (uint64_t usecs)
    {
    #if defined(VISUAL_OS_ANDROID)
        timespec request;
        request.tv_sec  = usecs / VISUAL_USECS_PER_SEC;
        request.tv_nsec = VISUAL_NSECS_PER_USEC * (usecs % VISUAL_USECS_PER_SEC);
        nanosleep (&request, nullptr);
    #else
        std::this_thread::sleep_for (std::chrono::microseconds (usecs));
    #endif
    }

    Time now ()
    {
        struct timespec clock_time;
        clock_gettime (CLOCK_MONOTONIC, &clock_time);

        return Time { clock_time.tv_sec, clock_time.tv_nsec };
    }

  } // TimeSystem

} // LV
