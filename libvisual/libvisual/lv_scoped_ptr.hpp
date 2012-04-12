#ifndef _LV_SCOPED_PTR_HPP
#define _LV_SCOPED_PTR_HPP

#include <libvisual/lv_util.hpp>
#include <algorithm>

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

	void reset (T* ptr = 0)
	{
	    ScopedPtr (ptr).swap (*this);
	}

	void swap (ScopedPtr& s)
	{
	    std::swap (m_ptr, s.m_ptr);
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
