#ifndef LVCPP_THREAD_HPP
#define LVCPP_THREAD_HPP

#include <libvisual/lv_thread.h>
#include <lv_function.hpp>

namespace Lv
{
  class Thread
  {
  public:

      static inline bool init ()
      {
          return visual_thread_initialize ();
      }

      static inline bool is_init ()
      {
          return visual_thread_is_initialized ();
      }

      static inline bool is_supported ()
      {
          return visual_thread_is_supported ();
      }

      static inline void enable (bool enabled)
      {
          visual_thread_enable (enabled ? TRUE : FALSE);
      }

      static inline bool is_enabled ()
      {
          return visual_thread_is_enabled ();
      }

      static inline void yield ()
      {
	  visual_thread_yield ();
      }

      explicit Thread (const Lv::Function<void>& func,
                       bool joinable = true)
          : m_func (func)
      {
	  m_thread = visual_thread_create (invoke_functor, static_cast<void *> (&m_func), joinable);
      }

      ~Thread ()
      {
	  visual_thread_free (m_thread);
      }

      inline void join ()
      {
	  visual_thread_join (m_thread);
      }

  private:

      VisThread *m_thread;
      Lv::Function<void> m_func;

      static void *invoke_functor (void *params);
  };

  class Mutex
  {
  public:

      Mutex ()
      {
	  visual_mutex_init (&m_mutex);
      }

      ~Mutex () 
      {}

      inline int try_lock ()
      {
	  return visual_mutex_trylock (&m_mutex);
      }
      
      inline int lock ()
      {
	  return visual_mutex_lock (&m_mutex);
      }

      inline int unlock ()
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

      explicit ScopedLock (Lock& lock)
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

      explicit ScopedTryLock (Lock& lock)
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

} // namespace Lv

#endif // #ifdef LVCPP_THREAD_HPP
