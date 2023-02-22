#ifndef _LV_TOOLS_BENCHMARK_HPP
#define _LV_TOOLS_BENCHMARK_HPP

#include <string>
#include <string_view>

namespace LV {
  namespace Tools {

    // Test interface
    class Benchmark
    {
    public:

        std::string const& get_name () const
        {
            return m_name;
        }

        virtual void operator() (unsigned int max_runs) = 0;

        virtual ~Benchmark () {}

    protected:

        explicit Benchmark (std::string_view name)
            : m_name {name}
        {}

    private:

        std::string m_name;
    };

    void run_benchmark (Benchmark& benchmark, unsigned int max_runs);

  } // Tools namespace
} // LV namespace

#endif // _LV_TOOLS_BENCHMARK_HPP
