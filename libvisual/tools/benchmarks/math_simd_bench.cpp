#include <libvisual/libvisual.h>
#include <libvisual/lv_aligned_allocator.hpp>
#include <vector>
#include <chrono>
#include <random>
#include <iostream>
#include <type_traits>
#include <cstdlib>

namespace {

  // Benchmark clock settings
  typedef std::chrono::high_resolution_clock PerfClock;
  typedef std::chrono::duration<double, std::micro> Duration;

  // Random number generator settings
  typedef std::mt19937_64 RandomNumGen;
  std::random_device rd;
  RandomNumGen rng { rd () };

  // Allocator settings
  // FIXME: Use C++11 template aliases when available
#ifdef USE_ALIGNED_ALLOCATOR
  template <typename T> struct Vector { typedef std::vector<T, LV::AlignedAllocator<T, 16>> type; };
#else
  template <typename T> struct Vector { typedef std::vector<T> type; };
#endif

  // Creates a vector filled with random values, each in the interval [min, max]
  template <class Container>
  Container random (typename Container::value_type min, typename Container::value_type max, typename Container::size_type size)
  {
      typedef typename Container::value_type T;
      typedef typename std::conditional<
                           std::is_floating_point<T>::value,
                           std::uniform_real_distribution<T>,
                           std::uniform_int_distribution<T>
                       >::type Distribution;

      Distribution distrib (min, max);

      Container vec (size);
      for (auto& i : vec) {
          i = distrib (rng);
      }

      return vec;
  }

  // Test interface
  class Test
  {
  public:

      std::string const& get_name () const
      {
          return m_name;
      }

      virtual void operator() (unsigned int max_runs) = 0;

      virtual ~Test () {}

  protected:

      Test (std::string const& name)
          : m_name (name)
      {}

  private:

      std::string m_name;
  };

  // Test class for visual_math_simd_mul_floats_floats()
  class MulFloatsFloatsTest
      : public Test
  {
  public:

      typedef typename Vector<float>::type Input1;
      typedef typename Vector<float>::type Input2;
      typedef typename Vector<float>::type Output;

      explicit MulFloatsFloatsTest (unsigned int data_size)
          : Test     ("MulFloatsFloats")
          , m_input1 (random<Input1> (-2.0, 2.0, data_size))
          , m_input2 (random<Input2> (-2.0, 2.0, data_size))
          , m_output (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_mul_floats_floats (m_output.data (), m_input1.data (), m_input2.data (), m_output.size ());
          }
      }

      virtual ~MulFloatsFloatsTest ()
      {}

  private:

      Input1 m_input1;
      Input2 m_input2;
      Output m_output;
  };

  // Test class for visual_math_simd_denorm_neg_floats_int32s()
  class DenormNegTest
      : public Test
  {
  public:

      typedef typename Vector<float>::type   Input;
      typedef typename Vector<int32_t>::type Output;

      explicit DenormNegTest (unsigned int data_size)
          : Test     ("DenormNegTest")
          , m_input  (random<Input> (-2.0, 2.0, data_size))
          , m_output (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_denorm_neg_floats_to_int32s (m_output.data (), m_input.data (), 100.0, m_output.size ());
          }
      }

      virtual ~DenormNegTest ()
      {}

  private:

      Input  m_input;
      Output m_output;
  };

  // Test class for visual_math_simd_complex_norm()
  class ComplexNormTest
      : public Test
  {
  public:

      typedef typename Vector<float>::type Input1;
      typedef typename Vector<float>::type Input2;
      typedef typename Vector<float>::type Output;

      explicit ComplexNormTest (unsigned int data_size)
          : Test     ("ComplexNormTest")
          , m_input1 (random<Input1> (0.0, 1.0, data_size))
          , m_input2 (random<Input2> (0.0, 1.0, data_size))
          , m_output (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_complex_norm (m_output.data (), m_input1.data (), m_input2.data (), m_output.size ());
          }
      }

      virtual ~ComplexNormTest ()
      {}

  private:

      Input1 m_input1;
      Input2 m_input2;
      Output m_output;
  };

  // Test class for visual_math_simd_complex_scaled_norm()
  class ComplexScaledNormTest
      : public Test
  {
  public:

      typedef typename Vector<float>::type Input1;
      typedef typename Vector<float>::type Input2;
      typedef typename Vector<float>::type Output;

      explicit ComplexScaledNormTest (unsigned int data_size)
          : Test     ("ComplexScaledNormTest")
          , m_input1 (random<Input1> (0.0, 1.0, data_size))
          , m_input2 (random<Input2> (0.0, 1.0, data_size))
          , m_output (data_size)
      {}

      virtual void operator() (unsigned int max_runs)
      {
          for (unsigned int i = 0; i < max_runs; i++) {
              visual_math_simd_complex_scaled_norm (m_output.data (), m_input1.data (), m_input2.data (), 100.0, m_output.size ());
          }
      }

      virtual ~ComplexScaledNormTest ()
      {}

  private:

      Input1 m_input1;
      Input2 m_input2;
      Output m_output;
  };

  void run_test (Test& test, unsigned int max_runs)
  {
      auto start_time = PerfClock::now ();
      test (max_runs);
      Duration duration = PerfClock::now () - start_time;

      // Print timings
      std::cout << "-- " << test.get_name () << " --\n"
                << "Total runs: " << max_runs << "\n"
                << "Total time: " << duration.count () << "us\n"
                << "Time / run: " << duration.count () / max_runs << "us\n\n";
  }

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

    MulFloatsFloatsTest test1 (data_size);
    run_test (test1, max_runs);

    DenormNegTest test2 (data_size);
    run_test (test2, max_runs);

    ComplexNormTest test3 (data_size);
    run_test (test3, max_runs);

    ComplexScaledNormTest test4 (data_size);
    run_test (test4, max_runs);

    return EXIT_SUCCESS;
}
