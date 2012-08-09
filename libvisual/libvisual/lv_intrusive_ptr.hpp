#ifndef _LV_INTRUSIVE_HPP
#define _LV_INTRUSIVE_HPP

namespace LV
{

  // An implementation of boost::intrusive_ptr

  template <typename T>
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

      IntrusivePtr (IntrusivePtr const& rhs)
          : m_ptr (rhs.m_ptr)
      {
          if (m_ptr)
              intrusive_ptr_add_ref (m_ptr);
      }

      IntrusivePtr (IntrusivePtr&& rhs)
          : m_ptr (rhs.m_ptr)
      {
          rhs.m_ptr = nullptr;
      }

      ~IntrusivePtr ()
      {
          if (m_ptr)
              intrusive_ptr_release (m_ptr);
      }

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

      //! Reset to pointer to null
      void reset ()
      {
          IntrusivePtr ().swap (*this);
      }

      //
      void reset (T* rhs)
      {
          IntrusivePtr (rhs).swap (*this);
      }

      T* get () const
      {
          return m_ptr;
      }

      T& operator* () const
      {
          return *m_ptr;
      }

      T* operator-> () const
      {
          return m_ptr;
      }

      explicit operator bool () const
      {
          return m_ptr != nullptr;
      }

      void swap (IntrusivePtr& rhs)
      {
          T* tmp = m_ptr;
          m_ptr = rhs.m_ptr;
          rhs.m_ptr = tmp;
      }

  private:

      T* m_ptr;
  };

} // LV namespace

namespace std {

  // std::swap() overload for efficiently swapping IntrusivePtrs
  template <class T>
  void swap (LV::IntrusivePtr<T>& lhs, LV::IntrusivePtr<T>& rhs)
  {
      lhs.swap (rhs);
  }

} // std namespace

#endif // _LV_INTRUSIVE_HPP
