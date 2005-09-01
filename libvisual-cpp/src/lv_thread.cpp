// Libvisual-c++ - C++ bindings for Libvisual
// 
// Copyright (C) 2005 Chong Kai Xiong <descender@phreaker.net>
//
// Author: Chong Kai Xiong <descender@phreaker.net>
//
// $Id: lv_thread.cpp,v 1.5 2005-09-01 02:40:40 descender Exp $
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

#include <config.h>
#include <lv_thread.hpp>

namespace Lv
{
  void *Thread::invoke_functor (void *params)
  {
      Lv::Function<void> *m_func = static_cast<Lv::Function<void> *> (params);
      
      (*m_func) ();

      visual_thread_exit (NULL);

      // Used to shut GCC up
      return NULL;
  }
}


#ifdef LVCPP_THREAD_TEST

#include <iostream>
#include <list>
#include <queue>

template <class Lock>
struct DebugLock
{
    Lock *m_lock;
    bool  m_own;

    DebugLock () 
        : m_lock (new Lock),
          m_own (true)
    {}

    ~DebugLock ()
    {
        if (m_own)
            delete m_lock;
    }        

    DebugLock (Lock& lock)
        : m_lock (&lock),
          m_own (false)
    {}

    void lock ()
    {
        std::cout << "DebugLock: locking." << std::endl;
        m_lock->lock ();
    }

    int try_lock ()
    {
        std::cout << "DebugLock: trying to lock." << std::endl;
        return m_lock->try_lock ();
    }    

    void unlock ()
    {
        std::cout << "DebugLock: unlocking." << std::endl;
        m_lock->unlock ();
    }
};

typedef DebugLock<Lv::Mutex> DebugMutex;

template <typename T, class Lock = Lv::Mutex>
class SynchronizedQueue
{
public:
    typedef Lv::ScopedLock<Lock> ScopedLock;

    SynchronizedQueue ()
    {}

    ~SynchronizedQueue ()
    {}

    bool is_empty ()
    {
        ScopedLock lock(m_lock);
        
        return m_queue.empty ();
    }

    void enqueue (const T& item)
    {
        ScopedLock lock(m_lock);

        m_queue.push (item);
    }

    T dequeue ()
    {
        ScopedLock lock(m_lock);

        T item (m_queue.front ());
        m_queue.pop ();

        return item;
    }

private:

    std::queue<T, std::list<T> > m_queue;
    Lock m_lock;

    SynchronizedQueue (SynchronizedQueue&);
};

template <class Queue>
class Consumer
{
public:

    Consumer (int id, Queue& queue, int limit)
        : m_id (id),
          m_queue (queue),
          m_collected (0),
          m_limit (limit)
    {}

    void operator () ()
    {
        int item;

        while (m_collected < m_limit)
        {
            if  (!m_queue.is_empty ())
            {
                item = m_queue.dequeue ();
                m_collected++;

                std::cout << m_id << ": Consumed: " << item << "\n";
            }
        }
    }

private:

    int m_id;
    Queue& m_queue;
    int m_collected;
    int m_limit;
};

template <class Queue>
class Producer
{
public:

    Producer (int id, Queue& queue, int limit)
        : m_id (id),
          m_queue (queue),
          m_limit (limit)
    {}

    void operator () ()
    {
        for (int i = 0; i < m_limit; i++)
        {
            m_queue.enqueue (i);

            std::cout << m_id << ": Produced: " << i << "\n";
            Lv::Thread::yield ();
        }

        std::cout << m_id << ": Finished production.\n";
    }

private:
    
    int m_id;
    Queue& m_queue;
    int m_limit;
};


void mutex_test ()
{
    std::cout << "Lv::Mutex test\n";

    Lv::Mutex actual_lock;
    DebugMutex lock (actual_lock);

    {
        Lv::ScopedLock<DebugMutex> scoped_lock(lock);
        std::cout << "Middle of scope 1\n";
    }
    
    {
        Lv::ScopedTryLock<DebugMutex> scoped_try_lock(lock);
        std::cout << "Middle of scope 2\n";
    }
}

void thread_test ()
{
    typedef SynchronizedQueue<int, Lv::Mutex> Queue;

    std::cout << "Lv::Thread test\n";

    Queue queue;

    Lv::Thread consumer  (Consumer<Queue> (0, queue, 50));
    Lv::Thread producer1 (Producer<Queue> (1, queue, 25));
    Lv::Thread producer2 (Producer<Queue> (2, queue, 25));

    producer1.join ();
    producer2.join ();
    consumer.join ();
}

int main ()
{
    Lv::Thread::init ();

    mutex_test ();
    thread_test ();
}

#endif // #ifdef LVCPP_THREAD_TEST
