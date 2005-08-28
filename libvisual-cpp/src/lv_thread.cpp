#include <lv_thread.hpp>

namespace Lv
{

  void test ()
  {
      Mutex lock;

      {
	  ScopedLock<Mutex> scoped_lock(lock);
      }

      {
	  ScopedTryLock<Mutex> scoped_try_lock(lock);
      }
  }

}
