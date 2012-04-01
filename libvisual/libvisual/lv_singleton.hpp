#ifndef _LV_SINGLETON_HPP
#define _LV_SINGLETON_HPP

namespace LV {

  template <class T>
  class Singleton
  {
  public:

      static void init ()
      {
          if (!m_instance)
              m_instance = new T;
      }

      static void deinit ()
      {
          delete m_instance;
          m_instance = 0;
      }

      static T* instance () {
          return m_instance;
      }

  protected:

      static T* m_instance;
  };

  template <class T>
  T* Singleton<T>::m_instance = 0;

} // LV namespace

#endif // _LV_SINGLETON_HPP
