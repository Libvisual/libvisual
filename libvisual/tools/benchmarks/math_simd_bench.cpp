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

  // Bench class for visual_math_simd_mul_floats_floats()
  class MulFloatsFloatsBench
      : public LV::Tools::Benchmark
  {
  public:

      typedef typename Vector<float>::type Input1;
      typedef typename Vector<float>::type Input2;
      typedef typename Vector<float>::type Output;

      explicit MulFloatsFloatsBench (unsigned int data_size)
          : Benchmark ("MulFloatsFloats")
          , m_input1  (LV::Tools::make_random<Input1> (-2.0, 2.0, data_size))
          , m_input2  (LV::Tools::make_random<Input2> (-2.0, 2.0, data_size))
          , m_output  (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_mul_floats_floats (m_output.data (), m_input1.data (), m_input2.data (), m_output.size ());
          }
      }

      virtual ~MulFloatsFloatsBench ()
      {}

  private:

      Input1 m_input1;
      Input2 m_input2;
      Output m_output;
  };

  // Bench class for visual_math_simd_denorm_neg_floats_int32s()
  class DenormNegBench
      : public LV::Tools::Benchmark
  {
  public:

      typedef typename Vector<float>::type   Input;
      typedef typename Vector<int32_t>::type Output;

      explicit DenormNegBench (unsigned int data_size)
          : Benchmark ("DenormNegBench")
          , m_input   (LV::Tools::make_random<Input> (-2.0, 2.0, data_size))
          , m_output  (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_denorm_neg_floats_to_int32s (m_output.data (), m_input.data (), 100.0, m_output.size ());
          }
      }

      virtual ~DenormNegBench ()
      {}

  private:

      Input  m_input;
      Output m_output;
  };

  // Bench class for visual_math_simd_complex_norm()
  class ComplexNormBench
      : public LV::Tools::Benchmark
  {
  public:

      typedef typename Vector<float>::type Input1;
      typedef typename Vector<float>::type Input2;
      typedef typename Vector<float>::type Output;

      explicit ComplexNormBench (unsigned int data_size)
          : Benchmark ("ComplexNormBench")
          , m_input1  (LV::Tools::make_random<Input1> (0.0, 1.0, data_size))
          , m_input2  (LV::Tools::make_random<Input2> (0.0, 1.0, data_size))
          , m_output  (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_complex_norm (m_output.data (), m_input1.data (), m_input2.data (), m_output.size ());
          }
      }

      virtual ~ComplexNormBench ()
      {}

  private:

      Input1 m_input1;
      Input2 m_input2;
      Output m_output;
  };

  // Bench class for visual_math_simd_complex_scaled_norm()
  class ComplexScaledNormBench
      : public LV::Tools::Benchmark
  {
  public:

      typedef typename Vector<float>::type Input1;
      typedef typename Vector<float>::type Input2;
      typedef typename Vector<float>::type Output;

      explicit ComplexScaledNormBench (unsigned int data_size)
          : Benchmark ("ComplexScaledNormBench")
          , m_input1  (LV::Tools::make_random<Input1> (0.0, 1.0, data_size))
          , m_input2  (LV::Tools::make_random<Input2> (0.0, 1.0, data_size))
          , m_output  (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_complex_scaled_norm (m_output.data (), m_input1.data (), m_input2.data (), 100.0, m_output.size ());
          }
      }

      virtual ~ComplexScaledNormBench ()
      {}

  private:

      Input1 m_input1;
      Input2 m_input2;
      Output m_output;
  };

} // anonymous

int main (int argc, char** argv)
{
    LV::System::init (argc, argv);

    unsigned int data_size = 100000;
    unsigned int max_runs  = 10000;

    if (argc > 2) {
        data_size = std::atoi (argv[1]);
        max_runs  = std::atoi (argv[2]);
    }

    MulFloatsFloatsBench test1 (data_size);
    LV::Tools::run_benchmark (test1, max_runs);

    DenormNegBench test2 (data_size);
    LV::Tools::run_benchmark (test2, max_runs);

    ComplexNormBench test3 (data_size);
    LV::Tools::run_benchmark (test3, max_runs);

    ComplexScaledNormBench test4 (data_size);
    LV::Tools::run_benchmark (test4, max_runs);

    return EXIT_SUCCESS;
}
