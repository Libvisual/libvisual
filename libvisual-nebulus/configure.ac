# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual Nebulus plugin, 0.1.3, [synap@nerds-incorporated.org], [libvisual-nebulus])
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_PREFIX_DEFAULT(/usr)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

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
PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.1.6, [libvis="yes"], [libvis="no"])
if test x"$libvis" = xno; then
  AC_MSG_ERROR([*** libvisual not found or version mismatch.
		libvisual is needed to build this package,
		you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

AC_MSG_CHECKING([for OpenGL support on Libvisual])
AC_TRY_COMPILE([#include <libvisual/libvisual.h>],
	       [#ifdef VISUAL_HAVE_OPENGL
		;
		#else
		something that will fail to compile.
		#endif],
		[opengl=yes], [opengl=no])
if test x"$opengl" = xyes; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
  AC_MSG_ERROR([*** you need OpenGL support in order to run Nebulus plugin])
fi

if test x"$opengl" = xyes; then
  # FIXME: Replace `main' with a function in `-lGL':
  AC_CHECK_LIB([GL], [main])
  # FIXME: Replace `main' with a function in `-lGLU':
  AC_CHECK_LIB([GLU], [main])
else
  :
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.
AC_CHECK_FUNCS([memset sqrt])

# Arguments to specify certain features.
AC_ARG_ENABLE([debug],
            AC_HELP_STRING([--enable-debug],
                           [Enable debug @<:@default=disabled@:>@]),
	    [debug=$enableval],
	    [debug=no])
AC_MSG_CHECKING([wheter to enable debug])
if test "$debug" = yes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="-ggdb3"
else
  AC_MSG_RESULT([no])
  DEBUG_CFLAGS=""
fi
AC_SUBST(DEBUG_CFLAGS)

# Terrible hack, but otherwise the CFLAGS are passed as last arguments
# to compiler, and this way these arguments (probably -g -O2) overrides
# our -O3 flag on src/Makefile.am
AC_SUBST(CFLAGS, "")

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_CONFIG_FILES([
	Makefile
	src/Makefile
])

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-NEBULA, THE NEBULA PLUGIN FOR LIBVISUAL, VERSION $VERSION
=====================================================================

Packages found:
opengl:                               : ${opengl}

Build options:
install path                          : ${prefix}/lib/libvisual/actor
source code location                  : ${srcdir}
compiler                              : ${CC}
"

