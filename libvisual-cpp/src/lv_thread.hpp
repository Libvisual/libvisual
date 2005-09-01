// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_thread.hpp,v 1.6 2005-09-01 08:30:13 descender Exp $
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

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
          visual_thread_enable (enabled);
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

#endif // #ifndef LVCPP_THREAD_HPP
