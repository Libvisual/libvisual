#ifndef _LV_ALIGNED_ALLOCATOR_HPP
#define _LV_ALIGNED_ALLOCATOR_HPP

#include <libvisual/lv_mem.h>
#include <cstdlib>
#include <limits>
#include <memory>
#include <stdexcept>

namespace LV
{
  //! Aligned memory allocator.
  //!
  //! @tparam alignment alignment boundary
  //! @tparam T         type of object to allocate
  //!
  //! AlignedAllocator is an implementation of the C++ Allocator concept for use with standard library containers.
  //!
  template <std::size_t alignment, typename T = std::byte>
  struct AlignedAllocator
  {
      using value_type = T;

      AlignedAllocator () = default;

      template <typename U>
      constexpr AlignedAllocator (AlignedAllocator<alignment, U> const&) noexcept
      {}

      [[nodiscard]] T* allocate (std::size_t n)
      {
          if (n > std::numeric_limits<std::size_t>::max () / sizeof (T))
              throw std::bad_array_new_length {};

          auto ptr = visual_mem_malloc_aligned (n * sizeof (T), alignment);

          if (!ptr)
              throw std::bad_alloc {};

          return static_cast<T*> (ptr);
      }

      void deallocate (T* ptr, std::size_t) noexcept
      {
          visual_mem_free_aligned (ptr);
      }
  };

  template <typename T, typename U, std::size_t alignment>
  bool operator== (AlignedAllocator<alignment, T> const&, AlignedAllocator<alignment, U> const&)
  {
      return true;
  }

  template <typename T, typename U, std::size_t alignment>
  bool operator!= (AlignedAllocator<alignment, T> const&, AlignedAllocator<alignment, U> const&)
  {
      return false;
  }

} // LV namespace

#endif // LV_ALIGNED_ALLOCATOR_HPP
