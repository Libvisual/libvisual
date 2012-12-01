#include <libvisual/libvisual.h>
#include <libvisual/lv_aligned_allocator.hpp>
#include "benchmark.hpp"
#include "random.hpp"
#include <vector>
#include <cstdlib>

namespace {

  // Allocator settings
  // FIXME: Use C++11 template aliases when available
#ifdef USE_ALIGNED_ALLOCATOR
  template <typename T> struct Vector { typedef std::vector<T, LV::AlignedAllocator<T, 16>> type; };
#else
  template <typename T> struct Vector { typedef std::vector<T> type; };
#endif

  // Test class for DFT
  class DFTBench
      : public LV::Tools::Benchmark
  {
  public:

      typedef typename Vector<float>::type Input;
      typedef typename Vector<float>::type Output;

      explicit DFTBench (unsigned int data_size)
          : Benchmark ("DFTTest")
          , m_dft     (data_size, data_size)
          , m_input   (LV::Tools::make_random<Input> (0.0, 1.0, data_size))
          , m_output  (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              m_dft.perform (m_output.data (), m_input.data ());
          }
      }

      virtual ~DFTBench ()
      {}

  private:

      LV::DFT m_dft;
      Input   m_input;
      Output  m_output;
  };

} // anonymous

int main (int argc, char** argv)
{
    LV::System::init (argc, argv);

    unsigned int data_size = 1024;
    unsigned int max_runs  = 1000;

    if (argc > 2) {
        data_size = std::atoi (argv[1]);
        max_runs  = std::atoi (argv[2]);
    }

    DFTBench bench (data_size);
    LV::Tools::run_benchmark (bench, max_runs);

    return EXIT_SUCCESS;
}
