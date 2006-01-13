AC_INIT([libvisual-cpp], [0.0.1])
AC_PREREQ(2.5)

LVCPP_ABI_VERSION="1:0:0"
AC_SUBST([LVCPP_ABI_VERSION])

AC_CANONICAL_BUILD
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([1.8])

AC_CONFIG_HEADERS([lv_build_config.hpp])

dnl C++ build environment
AC_LANG([C++])
CXXFLAGS="$CXXFLAGS -Wall -ansi"

dnl Installation tools

AC_PROG_INSTALL
AC_LN_S
AC_PROG_MAKE_SET

dnl Libtool

dnl Libtool hack to remove F77 checks
m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

dnl Check for libvisual

LV_MAJOR_VERSION=0
LV_MINOR_VERSION=3
LV_MICRO_VERSION=0
LV_VERSION="${LV_MAJOR_VERSION}.${LV_MINOR_VERSION}.${LV_MICRO_VERSION}"

PKG_CHECK_MODULES([LV], [libvisual >= $LV_VERSION], [], 
    [AC_MSG_ERROR([Cannot find libvisual $LV_VERSION development files])]
)

dnl HTML documentation

AC_ARG_WITH(html-docs,
  [AS_HELP_STRING([--with-html-docs],
                 [HTML documentation (requires Doxygen, default no)])],
  [WITH_HTML_DOCS=$withval],
  [WITH_HTML_DOCS=no]
)

HAVE_DOXYGEN=no
HAVE_DOT=no

if test "$WITH_HTML_DOCS" = "yes"; then
  dnl Check for Doxygen and Graphviz
  AC_PATH_PROG([DOXYGEN], [doxygen])
  AC_PATH_PROG([DOT], [dot])

  test -n "$DOXYGEN" && HAVE_DOXYGEN=yes
  test -n "$DOT" && HAVE_DOT=yes

  if test "$HAVE_DOXYGEN" = "no"; then
    AC_MSG_WARN([Doxygen not found, skipping HTML documentation generation])
    WITH_HTML_DOCS=no
  else
    if test "$HAVE_DOT" = "no"; then
      AC_MSG_WARN([Graphviz not found, HTML documentation will not have diagrams])
    fi
  fi
fi

AM_CONDITIONAL([WITH_HTML_DOCS], [test "$WITH_HTML_DOCS" = "yes"])
AC_SUBST([HAVE_DOT])
AC_SUBST([HAVE_DOXYGEN])

dnl Profiling
AC_ARG_ENABLE([profile],
    AS_HELP_STRING([--enable-profile], [enable profiling (default=no)]),
    [enable_profile="$enableval"],
    [enable_profile="no"]
)

if test "$enable_profile" = "yes"; then
    CXXFLAGS="$CXXFLAGS -pg"
fi

dnl Debugging info
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [build debugging information (default=no)]),
    [enable_debug="$enableval"],
    [enable_debug="no"]
)

if test "$enable_debug" = "yes"; then
    CXXFLAGS="$CXXFLAGS -g"
fi

dnl Print configured options

echo
echo "Configured options: "
echo "Generate HTML docs: $WITH_HTML_DOCS"
echo

AC_CONFIG_FILES([
    Doxyfile
    Makefile
    libvisual-cpp/Makefile
    tests/Makefile
])

AC_OUTPUT
