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
	    delete m_ptr;
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

} // LV namespace

#endif // _LV_SCOPED_PTR_HPP
