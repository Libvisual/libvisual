#include "test.h"
#include <libvisual/libvisual.h>
#include <libvisual/lv_aligned_allocator.hpp>
#include <string>
#include <vector>
#include <cstdint>

namespace
{
  inline bool ptr_is_aligned (void* ptr, std::size_t alignment)
  {
      return (reinterpret_cast<std::intptr_t> (ptr) % alignment) == 0;
  }

  void test_allocation ()
  {
      std::array<std::size_t, 6> constexpr alignments { 4, 8, 16, 32, 64, 128 };

      for (auto alignment = alignments.begin (); alignment != alignments.end(); ++alignment) {
          std::size_t const test_block_size = *alignment * 2 + 1;

          void* ptr = visual_mem_malloc_aligned (test_block_size, *alignment);
          LV_TEST_ASSERT (ptr_is_aligned (ptr, *alignment));

          visual_mem_free_aligned (ptr);
      }
  }

  template <std::size_t alignment>
  void test_cxx_allocator ()
  {
      constexpr size_t test_count = 3;

      LV::AlignedAllocator<alignment, std::string> allocator;
      using allocator_traits = std::allocator_traits<decltype(allocator)>;

      std::string* strings = allocator_traits::allocate (allocator, test_count);

      LV_TEST_ASSERT (ptr_is_aligned (strings, alignment));

      allocator_traits::deallocate (allocator, strings, test_count);
  }

  void test_cxx_allocators ()
  {
      test_cxx_allocator<4> ();
      test_cxx_allocator<8> ();
      test_cxx_allocator<16> ();
      test_cxx_allocator<32> ();
      test_cxx_allocator<64> ();
      test_cxx_allocator<128> ();
  }
}

int main (int argc, char* argv[])
{
    LV::System::init (argc, argv);

    test_allocation ();
    test_cxx_allocators ();

    LV::System::destroy ();
}
