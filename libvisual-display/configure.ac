#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(libvisual-display, 0.0.0, Libvisual-devel@lists.sourceforge.net)
AC_CONFIG_SRCDIR([lvdisplay/lv_display.h])
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/])
fi

if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_ERROR([*** pkg-config too old; version 0.14 or better required.])
fi


# Checks for libraries.

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.2.0, [libvis="yes"], [libvis="no"])
if test x$libvis = xno; then
	AC_MSG_ERROR([*** libvisual not found.
			libvisual is needed to build this package.
			You can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

AC_ARG_ENABLE([profiling],
  AC_HELP_STRING([--enable-profiling],
		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
		[profiling=$enableval],
		[profiling=no])
AC_MSG_CHECKING([wheter to enable profiling])
if test x$profiling = xyes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([debug],
            AC_HELP_STRING([--enable-debug],
                           [Enable debug @<:@default=disabled@:>@]),
	    [debug=$enableval],
	    [debug=no])
AC_MSG_CHECKING([wheter to enable debug])
if test x$debug = xyes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall -Wmissing-braces -Wimplicit -Wunused"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wno-unused-variable"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Werror"
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(DEBUG_CFLAGS)

AC_ARG_ENABLE([extra-optimization],
            AC_HELP_STRING([--enable-extra-optimization],
                           [Enable extra optimizations @<:@default=disabled@:>@]),
	    [extra_opt=$enableval],
	    [extra_opt=no])
AC_MSG_CHECKING([wheter to enable extra optimizations])
if test x$extra_opt = xyes; then
  AC_MSG_RESULT([yes])
  OPT_CFLAGS="-O3 -fexpensive-optimizations"
else
  AC_MSG_RESULT([no])
  if test x$debug = xyes; then
    OPT_CFLAGS="-O0"
  else
    OPT_CFLAGS=""
  fi
fi
AC_SUBST(OPT_CFLAGS)

AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS}")

AC_ARG_WITH([plugins_base_dir],
            AC_HELP_STRING([--with-plugins_base_dir="dir"],
                           [Install plugins to this base directory. Defaults to Libvisual's base dir ]),
dnl ' # fix string coloring ;)
	    [plugins_base_dir=$withval],
	    [plugins_base_dir=])

AC_MSG_CHECKING([where to install plugins])

if test -z "$plugins_base_dir"; then
    plugins_base_dir=`$PKG_CONFIG --variable=pluginsbasedir libvisual`
    AC_MSG_RESULT([$plugins_base_dir])
else
    if test -d "$plugins_base_dir"; then
        AC_MSG_RESULT([$plugins_base_dir])
    else
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Specified plugins base directory "$plugins_base_dir" is not accesible!])
        exit 1
    fi
fi

LIBVISUAL_PLUGINS_BASE_DIR="$plugins_base_dir"
AC_SUBST(LIBVISUAL_PLUGINS_BASE_DIR)

# Checks for header files.
AC_PATH_X

if test x$have_x = xyes ; then

    LDFLAGS="$LDFLAGS -Wl,-rpath,$x_libraries -L$x_libraries"
    CPPFLAGS="$CPPFLAGS -I$x_includes"

    AC_CHECK_HEADERS([\
	X11/keysym.h	\
	X11/X.h	\
	X11/Xlib.h	\
	X11/Xutil.h], [], [
	    AC_MSG_ERROR([Required X11 headers not found.])
	])

    AC_CHECK_HEADERS([\
	GL/gl.h \
	GL/glx.h], [have_gl_headers=yes], [have_gl_headers=no])

    if test x$have_gl_headers = xyes ; then
	AC_CHECK_LIB([GL], [glBegin], [have_lib_gl=yes], [have_lib_gl=no])
    else
	have_lib_gl=no
    fi

    AM_CONDITIONAL(HAVE_LIB_GL,  test x$have_lib_gl = xyes)

    XF86VM_LIB=
    AC_CHECK_HEADERS([\
	X11/extensions/xf86vmode.h], [
	    AC_CHECK_LIB([Xxf86vm], [XF86VidModeSwitchToMode], [
		AC_DEFINE(HAVE_XF86VMODE, [], [XF86VidMode is available])
		XF86VM_LIB=-lXxf86vm
		xf86vm=yes
	    ], [xf86vm=no]) ], [],
	    [[#include <X11/Xlib.h>
	    ]])

    AC_SUBST(XF86VM_LIB)
    AM_CONDITIONAL(HAVE_XF86VMODE,  test x$xf86vm = xyes)

fi

AM_CONDITIONAL(HAVE_X,  test x$have_x = xyes)

AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset])

AC_CONFIG_FILES([Makefile
		libvisual-display.pc
                 drivers/Makefile
                 drivers/glx/Makefile
                 drivers/x11/Makefile
                 drivers/null/Makefile
                 example/Makefile
                 example/plugins/Makefile
                 lvdisplay/Makefile])
AC_OUTPUT
