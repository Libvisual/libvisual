#ifndef _LV_ALIGNED_ALLOCATOR_HPP
#define _LV_ALIGNED_ALLOCATOR_HPP

#include <cstddef>
#include <cstdlib>
#include <new>
#include <libvisual/lv_mem.h>

namespace LV {

  template <typename T, std::size_t alignment>
  struct AlignedAllocator;

  template <std::size_t alignment>
  struct AlignedAllocator<void, alignment>
  {
      typedef void        value_type;
      typedef void*       pointer;
      typedef void const* const_pointer;

      template <typename U>
      struct rebind { typedef AlignedAllocator<U, alignment> other; };
  };

  //! Aligned memory allocator.
  //!
  //! @tparam T         type of object to allocate
  //! @tparam alignment alignment boundary
  //!
  //! AlignedAllocator is an implementation of the C++ Allocator concept for use with standard library containers.
  //!
  template <typename T, std::size_t alignment>
  struct AlignedAllocator
  {
      typedef T*             pointer;
      typedef T&             reference;
      typedef T const*       const_pointer;
      typedef T const&       const_reference;
      typedef T              value_type;
      typedef std::size_t    size_type;
      typedef std::ptrdiff_t difference_type;

      template <typename U>
      struct rebind { typedef AlignedAllocator<U, alignment> other; };

      AlignedAllocator ()
      {}

      template <typename U>
      AlignedAllocator (AlignedAllocator<U, alignment>&)
      {}

      pointer address (reference x) const
      {
          return &x;
      }

      const_pointer address (const_reference x) const
      {
          return &x;
      }

      pointer allocate (size_type n, typename AlignedAllocator<void, alignment>::pointer = nullptr)
      {
          void* ptr = visual_mem_malloc_aligned (n * sizeof (T), alignment);

          if (!ptr)
              throw std::bad_alloc ();

          return pointer (ptr);
      }

      void deallocate (pointer ptr, size_type) noexcept
      {
          visual_mem_free_aligned (ptr);
      }

      size_type max_size () const
      {
          return size_type (-1) / sizeof (T);
      }

      void construct (pointer p, const_reference x)
      {
          new (p) T (x);
      }

      void destroy (pointer p)
      {
          p->~T ();
      }
  };

  template <typename T, typename U, std::size_t alignment>
  bool operator== (AlignedAllocator<T, alignment> const&, AlignedAllocator<U, alignment> const&)
  {
      return true;
  }

  template <typename T, typename U, std::size_t alignment>
  bool operator!= (AlignedAllocator<T, alignment> const&, AlignedAllocator<U, alignment> const&)
  {
      return false;
  }

} // LV namespace

#endif // LV_ALIGNED_ALLOCATOR_HPP
