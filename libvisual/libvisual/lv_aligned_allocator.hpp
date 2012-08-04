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
      typedef const void* const_pointer;

      template <typename U>
      struct rebind { typedef AlignedAllocator<U, alignment> other; };
  };

  template <typename T, std::size_t alignment>
  struct AlignedAllocator
  {
      typedef T*             pointer;
      typedef T&             reference;
      typedef const T*       const_pointer;
      typedef const T&       const_reference;
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

      pointer allocate (size_type n, typename AlignedAllocator<void, alignment>::pointer = 0)
      {
          void* ptr = visual_mem_malloc_aligned (n * sizeof (T), alignment);

          if (!ptr)
              throw std::bad_alloc ();

          return pointer (ptr);
      }

      void deallocate (pointer ptr, size_type) throw()
      {
          visual_mem_free_aligned (ptr);
      }

      size_type max_size () const
      {
          return size_type (-1) / sizeof (T);
      }

      void construct (pointer p, const value_type& x)
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
