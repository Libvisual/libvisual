#ifndef _LV_SCOPED_PTR_HPP
#define _LV_SCOPED_PTR_HPP

namespace LV {

  template <typename T>
  class ScopedPtr
  {
  public:

	explicit ScopedPtr (T* ptr = 0)
	    : m_ptr (ptr)
	{}

	~ScopedPtr ()
	{
	    checked_delete (m_ptr);
	}

	T& operator* () const
	{
	    return *m_ptr;
	}

	T* operator-> () const
	{
	    return m_ptr;
	}

	T* get () const
	{
	    return m_ptr;
	}

	operator bool() const
	{
	    return m_ptr != 0;
	}

  private:

	T* m_ptr;

	ScopedPtr (ScopedPtr const&);
	ScopedPtr& operator= (ScopedPtr const&);
  };

  template<class T>
  inline void checked_delete (T* x)
  {
      typedef char type_must_be_complete[ sizeof(T) ? 1 : -1 ];
      (void) sizeof (type_must_be_complete);
      delete x;
  }

} // LV namespace

#endif // _LV_SCOPED_PTR_HPP
