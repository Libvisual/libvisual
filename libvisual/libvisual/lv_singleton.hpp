#ifndef _LV_SINGLETON_HPP
#define _LV_SINGLETON_HPP

namespace LV {

  //! Singleton class template.
  //!
  //! @tparam T Class to make singleton out of
  //!
  //! @note Singleton is implemented using the curiously recurring template pattern (CRTP).
  //!
  template <class T>
  class Singleton
  {
  public:

      Singleton (Singleton const&) = delete;

      Singleton const& operator= (Singleton const&) = delete;

      //! Destructor
      virtual ~Singleton () {}

      //! Returns the singleton instance
      //!
      //! @return singleton instance
      static T* instance () {
          return m_instance;
      }

      //! Destroys the singleton instance
      static void destroy ()
      {
          delete m_instance;
          m_instance = nullptr;
      }

  protected:

      static T* m_instance;

      Singleton () = default;
  };

} // LV namespace

#endif // _LV_SINGLETON_HPP
