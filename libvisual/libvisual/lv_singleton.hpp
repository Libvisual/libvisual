#ifndef _LV_SINGLETON_HPP
#define _LV_SINGLETON_HPP

#include <libvisual/lv_defines.h>
#include <memory>

namespace LV {

  //! Singleton class template.
  //!
  //! @tparam T Class to make singleton out of
  //!
  //! @note Singleton is implemented using the curiously recurring template pattern (CRTP).
  //!
  template <class T>
  class LV_API Singleton
  {
  public:

      Singleton (Singleton const&) = delete;

      Singleton const& operator= (Singleton const&) = delete;

      //! Destructor
      virtual ~Singleton () {}

      //! Returns the singleton instance
      //!
      //! @return singleton instance
      static T* instance ()
      {
          return m_instance.get ();
      }

      //! Returns the singleton instance as const.
      //!
      //! @return Singleton instance.
      static T const* const_instance ()
      {
          return m_instance.get ();
      }

      //! Destroys the singleton instance
      static void destroy ()
      {
          m_instance.reset ();
      }

  protected:

      inline static std::unique_ptr<T> m_instance {};

      Singleton () = default;
  };

} // LV namespace

#endif // _LV_SINGLETON_HPP
