#ifndef _LV_INTRUSIVE_HPP
#define _LV_INTRUSIVE_HPP

namespace LV
{
  namespace Impl
  {
    // Checks that the required overloads for LV::IntrusivePtr<> are met.
    // NOTE: Only used for concept checking.
    template <typename T>
    void check_intrusive_ref_countable (T* a)
    {
        intrusive_ptr_add_ref (a);
        intrusive_ptr_release (a);
    }
  }

  //! Concept for reference countable types that can be used with LV::IntrusivePtr.
  template <typename T>
  concept IntrusiveRefCountable = requires (T* a)
  {
      Impl::check_intrusive_ref_countable (a);
  };

  //! Intrusive smart pointer class template.
  //!
  //! @tparam T Class to reference count
  //!
  //! @note This is an implementation of boost::intrusive_ptr.
  //!
  //! The type T must have two functions overloaded:
  //!
  //! * void intrusive_ptr_add_ref(T* object) -- _Called to add a reference_
  //! * void intrusive_ptr_release(T* object) -- _Called to remove a reference and destroy the object when not longer used_
  //!
  template <IntrusiveRefCountable T>
  class IntrusivePtr
  {
  public:

      typedef T element_type;

      IntrusivePtr ()
          : m_ptr (nullptr)
      {}

      IntrusivePtr (T* ptr, bool add_ref = true)
          : m_ptr (ptr)
      {
          if (ptr && add_ref)
              intrusive_ptr_add_ref (m_ptr);
      }

      template <typename U>
      IntrusivePtr (IntrusivePtr<U> const& rhs)
          : m_ptr (rhs.get ())
      {
          if (m_ptr)
              intrusive_ptr_add_ref (m_ptr);
      }

      //! Copy constructor
      IntrusivePtr (IntrusivePtr const& rhs)
          : m_ptr (rhs.m_ptr)
      {
          if (m_ptr)
              intrusive_ptr_add_ref (m_ptr);
      }

      //! Move constructor
      IntrusivePtr (IntrusivePtr&& rhs)
          : m_ptr (rhs.m_ptr)
      {
          rhs.m_ptr = nullptr;
      }

      //! Destructor
      ~IntrusivePtr ()
      {
          if (m_ptr)
              intrusive_ptr_release (m_ptr);
      }

      //! Assignment operator
      template <class U>
      IntrusivePtr& operator= (IntrusivePtr<U> const& rhs)
      {
          IntrusivePtr (rhs).swap (*this);
          return *this;
      }

      //! Copy assignment operator
      IntrusivePtr& operator= (IntrusivePtr const& rhs)
      {
          IntrusivePtr (rhs).swap (*this);
          return *this;
      }

      //! Raw-pointer assignment operator
      IntrusivePtr& operator= (T* rhs)
      {
          IntrusivePtr (rhs).swap (*this);
          return *this;
      }

      //! Move assignment
      IntrusivePtr& operator= (IntrusivePtr&& rhs)
      {
          IntrusivePtr (rhs).swap (*this);
          return *this;
      }

      //! Resets pointer to null
      void reset ()
      {
          IntrusivePtr ().swap (*this);
      }

      void reset (T* rhs)
      {
          IntrusivePtr (rhs).swap (*this);
      }

      //! Returns the memory managed pointer
      T* get () const
      {
          return m_ptr;
      }

      //! Dereference operator
      T& operator* () const
      {
          return *m_ptr;
      }

      //! Member dereference operator
      T* operator-> () const
      {
          return m_ptr;
      }

      //! Bool conversion operator
      explicit operator bool () const
      {
          return m_ptr != nullptr;
      }

      //! Swaps pointer with another
      void swap (IntrusivePtr& rhs)
      {
          T* tmp = m_ptr;
          m_ptr = rhs.m_ptr;
          rhs.m_ptr = tmp;
      }

      //! Swaps two intrusive pointers.
      friend void swap (LV::IntrusivePtr<T>& lhs, LV::IntrusivePtr<T>& rhs)
      {
          lhs.swap (rhs);
      }

  private:

      T* m_ptr;
  };

} // LV namespace

#endif // _LV_INTRUSIVE_HPP
