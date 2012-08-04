#include <libvisual/libvisual.h>
#include <libvisual/lv_aligned_allocator.hpp>
#include <vector>
#include <chrono>
#include <random>
#include <iostream>

typedef std::mt19937_64 RandomNumGen;
typedef std::chrono::system_clock SystemClock;
typedef std::chrono::high_resolution_clock PerfClock;
typedef std::chrono::duration<double, std::micro> Duration;

int main (int argc, char** argv)
{
    LV::System::init (argc, argv);

    unsigned int data_size = 100000;
    unsigned int max_runs  = 10000;

    if (argc > 2) {
        data_size = std::atoi (argv[1]);
        max_runs  = std::atoi (argv[2]);
    }

#ifdef USE_ALIGNED_ALLOCATOR
    typedef std::vector<float, LV::AlignedAllocator<float, 16> > FloatVector;
#else
    typedef std::vector<float> FloatVector;
#endif

    FloatVector input1 (data_size);
    FloatVector input2 (data_size);
    FloatVector output (data_size);

    std::cout << "Array addresses: "
              << std::hex
              << intptr_t (input1.data ()) << ", "
              << intptr_t (input2.data ()) << ", "
              << intptr_t (output.data ())
              << std::endl;

    // Put random stuff
    RandomNumGen rng { SystemClock::to_time_t (SystemClock::now ()) };
    for (unsigned int i = 0; i < input1.size (); i++) {
        input1[i] = std::generate_canonical<float, 32> (rng);
        input2[i] = std::generate_canonical<float, 32> (rng);
    }

    auto start_time = PerfClock::now ();

    // Run function repeatedly
    for (unsigned int i = 0; i < max_runs; i++) {
        visual_math_simd_complex_scaled_norm (output.data (), input1.data (), input2.data (), 100.0, input1.size ());
    }

    auto end_time = PerfClock::now ();

    Duration duration = end_time - start_time;

    // Print timings
    std::cout << "Total time: " << duration.count () << "us\n"
              << "Avg time per run: " << duration.count () / max_runs << "us\n"
              << "Avg time per 1,000 items: " << duration.count () / max_runs / (data_size / 1000) << "us\n";
}
