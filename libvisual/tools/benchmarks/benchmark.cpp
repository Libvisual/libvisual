#include "benchmark.hpp"
#include <chrono>
#include <iostream>

namespace LV {
  namespace Tools {

    // Benchmark clock settings
    typedef std::chrono::high_resolution_clock Clock;
    typedef std::chrono::duration<double, std::micro> Duration;

    void run_benchmark (LV::Tools::Benchmark& test, unsigned int max_runs)
    {
        auto start_time = Clock::now ();
        test (max_runs);
        Duration duration = Clock::now () - start_time;

        // Print timings
        std::cout << "-- " << test.get_name () << " --\n"
                  << "Total runs: " << max_runs << "\n"
                  << "Total time: " << duration.count () << "us\n"
                  << "Time / run: " << duration.count () / max_runs << "us\n\n";
    }

  } // Tools namespace
} // LV namespace
