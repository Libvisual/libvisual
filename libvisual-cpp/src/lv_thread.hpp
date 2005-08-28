#ifndef LVCPP_THREAD_HPP
#define LVCPP_THREAD_HPP

#include <libvisual/lv_thread.h>

namespace Lv
{

  class Mutex
  {
  public:

      Mutex ()
      {
	  visual_mutex_init (&m_mutex);
      }

      ~Mutex () 
      {}

      int try_lock ()
      {
	  return visual_mutex_trylock (&m_mutex);
      }
      
      int lock ()
      {
	  return visual_mutex_lock (&m_mutex);
      }

      int unlock ()
      {
	  return visual_mutex_unlock (&m_mutex);
      }

  private:

      VisMutex m_mutex;

      Mutex (const Mutex& mutex);
  };

  template <typename Lock>
  class ScopedLock
  {
  public:

      ScopedLock (Lock& lock) 
	  : m_lock (lock)
      {
	  m_lock.lock();
      }

      ~ScopedLock ()
      {
	  m_lock.unlock();
      }

  private:

      Lock& m_lock;

      ScopedLock (const ScopedLock& lock);
  };

  template <typename Lock>
  class ScopedTryLock
  {
  public:

      ScopedTryLock (Lock& lock)
	  : m_lock(lock)
      {
	  m_lock.try_lock ();
      }
      
      ~ScopedTryLock ()
      {
	  m_lock.unlock ();
      }

  private:

      Lock& m_lock;

      ScopedTryLock (const ScopedTryLock& lock);
  };

}

#endif
