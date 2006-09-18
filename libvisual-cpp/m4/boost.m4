#-*- Mode: Autoconf; -*-

dnl LV_BOOST_VARS()
AC_DEFUN([LV_BOOST_VARS],
    [AC_ARG_VAR([BOOST_CFLAGS], [C++ compiler flags for Boost])
     AC_ARG_VAR([BOOST_LIBS], [linker flags for Boost])
    ])

dnl LV_WRAPPED_BOOST_CHECK([BODY])
AC_DEFUN([LV_WRAPPED_BOOST_CHECK],
    [AC_REQUIRE([LV_BOOST_VARS])
     LV_WRAPPED_CHECK(
         [CXXFLAGS="-Wall -ansi -pedantic -Werror $BOOST_CFLAGS"
          LIBS="$BOOST_LIBS"
          $1
         ])
    ])

dnl LV_CHECK_BOOST_VERSION(DISPLAY-VERSION, VERSION, [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([LV_CHECK_BOOST_VERSION],
    [LV_WRAPPED_BOOST_CHECK(
        [AC_MSG_CHECKING([for Boost >= $1])
         AC_TRY_COMPILE(
             [#include <boost/version.hpp>
              #if (BOOST_VERSION < $2)
              #  error
              #endif],
             [],
             [AC_MSG_RESULT([yes])
              $3],
             [AC_MSG_RESULT([not found])
              $4])
        ])
    ])

dnl LV_CHECK_BOOST_FUNCTION([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([LV_CHECK_BOOST_FUNCTION],
    [LV_WRAPPED_BOOST_CHECK(
        [AC_MSG_CHECKING([for Boost.Function])
         AC_TRY_COMPILE(
             [#include <boost/function.hpp>
              #include <boost/function_equal.hpp>
              template class boost::function<void ()>;
              template class boost::function5<void, int, int, int, int, int>;],
             [],
             [AC_MSG_RESULT([yes])
              $1],
             [AC_MSG_RESULT([not found])
              $2])
        ])
    ])

dnl LV_CHECK_BOOST_CALL_TRAITS([ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
AC_DEFUN([LV_CHECK_BOOST_CALL_TRAITS],
    [LV_WRAPPED_BOOST_CHECK(
        [AC_MSG_CHECKING([for Boost Call Traits])
         AC_TRY_COMPILE(
             [#include <boost/call_traits.hpp>
              using boost::call_traits;
              template <typename T>
              struct TestStruct
              {                 
                  typedef typename call_traits<T>::param_type      param_type;
                  typedef typename call_traits<T>::reference       reference;
                  typedef typename call_traits<T>::const_reference const_reference;
                  typedef typename call_traits<T>::value_type      result_type;
              };
              template class TestStruct<int>;],
             [],
             [AC_MSG_RESULT([yes])
              $1],
             [AC_MSG_RESULT([not found])
              $2])
        ])
    ])
