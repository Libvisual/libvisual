#include "config.h"
#include "private/lv_time_system.hpp"
#include <thread>
#include <windows.h>

namespace LV {

  namespace TimeSystem {

    uint64_t counter_freq;

    void start ()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency (&freq);
        counter_freq = freq.QuadPart;

        // Increase timer resolution for fine-grained sleeps
        timeBeginPeriod (1);
    }

    void shutdown ()
    {
        timeEndPeriod (1);
    }

    void usleep (uint64_t usecs)
    {
        Sleep (usecs / VISUAL_USECS_PER_MSEC);
    }

    Time now ()
    {
        LARGE_INTEGER perf_counter;
        QueryPerformanceCounter (&perf_counter);

        return Time { perf_counter.QuadPart / counter_freq,
                      ((perf_counter.QuadPart % counter_freq) * VISUAL_NSECS_PER_SEC) / counter_freq };
    }

  } // TimeSystem

} // LV namespace
