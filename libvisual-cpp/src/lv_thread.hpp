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

      ~Mutex ()
      {}

  private:

      VisMutex m_mutex;

      Mutex (const Mutex& mutex);
  };

  class ScopeMutex
  {
  public:

      ScopeMutex ()
      {
	  m_mutex.lock();
      }

      ~ScopeMutex ()
      {
	  m_mutex.unlock();
      }

  private:

      Mutex m_mutex;

      ScopeMutex (const Mutex& mutex);
  };

}

#endif
