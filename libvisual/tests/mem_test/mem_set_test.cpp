#include "test.h"
#include <cstdint>
#include <libvisual/libvisual.h>
#include <libvisual/lv_mem.h>
#include <cstdint>
#include <array>
#include <algorithm>
#include <vector>

namespace {

  void test_mem_set16 ()
  {
      constexpr std::uint16_t test_pattern = 0x1234;

      constexpr auto test {[test_pattern] (auto x) {
                               return x == test_pattern;
                           }};

      for (auto test_size = 1; test_size <= 128; test_size++) {
          // Allocate extra elements in front and behind of test memory area.
          std::vector<std::uint16_t> vec (test_size + 2);

          visual_mem_set16 (&vec[1], test_pattern, test_size);

          // Test for buffer overruns
          LV_TEST_ASSERT (vec.front () == 0);
          LV_TEST_ASSERT (vec.back () == 0);

          // Test data area
          LV_TEST_ASSERT (std::all_of (++begin (vec), --end (vec), test));
      }
  }

  void test_mem_set32 ()
  {
      constexpr std::uint32_t test_pattern = 0x12345678;

      constexpr auto test {[test_pattern] (auto x) {
                               return x == test_pattern;
                           }};

      for (auto test_size = 1; test_size <= 128; test_size++) {
          // Allocate extra elements in front and behind of test memory area.
          std::vector<std::uint32_t> vec (test_size + 2);

          visual_mem_set32 (&vec[1], test_pattern, test_size);

          // Test for buffer overruns
          LV_TEST_ASSERT (vec.front () == 0);
          LV_TEST_ASSERT (vec.back () == 0);

          // Test data area
          LV_TEST_ASSERT (std::all_of (++begin (vec), --end (vec), test));
      }
  }

} // anonymous namespace

int main (int argc, char* argv[])
{
    LV::System::init (argc, argv);

    test_mem_set16 ();
    test_mem_set32 ();

    LV::System::destroy ();
}
