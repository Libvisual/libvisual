# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual XMMS plugin, 0.1.5, synap@nerds-incorporated.org, libvisual-xmms)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S

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

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])
AC_CHECK_HEADERS([GL/gl.h GL/glx.h GL/glu.h],opengl="yes",opengl="no")
if test x"$opengl" = xno; then
	AC_MSG_ERROR([OpenGL and Glu headers not found, the plugin cannot be build])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE # reserved for future

# Check for libraries
AM_PATH_GLIB(1.2.2,,AC_MSG_ERROR([*** GLIB >= 1.2.2 not installed - please install first ***]))
AM_PATH_GTK(1.2.2,,AC_MSG_ERROR([*** GTK+ >= 1.2.2 not installed - please install first ***]),gthread)

AC_PATH_PROG(SDL_CONFIG, [sdl-config], [no])
if test x$SDL_CONFIG = xno ; then
  AC_MSG_ERROR([
*** sdl-config not found.
*** If you have installed from binaries, probably you have not 
*** installed SDL development package.
*** See http://www.libsdl.org
])
fi
AM_PATH_SDL(1.2.5, , AC_MSG_ERROR([*** SDL >= 1.2.5 not installed - please install first ***]))

AC_PATH_PROG(XMMS_CONFIG, [xmms-config], [no])
if test x$XMMS_CONFIG = xno ; then
  AC_MSG_ERROR([
*** xmms-config not found.
*** If you have installed from binaries, probably you have not
*** installed XMMS development package.
*** See http://www.xmms.org
])
fi
AM_PATH_XMMS(1.2.0,,AC_MSG_ERROR([*** XMMS >= 1.0.0 not installed - please install first]))

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.1.5, [libvis="yes"], [libvis="no"])
if test x"$libvis" = xno; then
	AC_MSG_ERROR([libvisual not found, libvisual is needed to build this package,
you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

# Default prefix
AC_PREFIX_DEFAULT(/usr)
AC_PREFIX_PROGRAM(xmms)

XMMS_PLUGIN_DIR="$prefix/lib/xmms"
XMMS_VISUALIZATION_PLUGIN_DIR="$XMMS_PLUGIN_DIR/Visualization"
AC_SUBST(XMMS_VISUALIZATION_PLUGIN_DIR)

# cflags, ld flags
libs_dynamic_loader="-ldl"
AC_SUBST(CFLAGS, "-I./ -O3 -rdynamic -ggdb $LIBVISUAL_CFLAGS $GTK_CFLAGS -I$prefix/include/xmms $SDL_CFLAGS")
AC_SUBST(LIBS, "$GTK_LIBS -lxmms $SDL_LIBS $LIBVISUAL_LIBS")
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(BUILD_INPUT, "$build_input")
AC_SUBST(BUILD_ACTOR, "$build_actor")

AC_CONFIG_FILES([
	Makefile
	src/Makefile
	])

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-XMMS, XMMS PLUGIN FOR LIBVISUAL $VERSION
=====================================================================

Packages found:
libvisual:                            : ${libvis}
opengl:                               : ${opengl}

Build options:
prefix:                               : ${prefix}
source code location:                 : ${srcdir}
compiler:                             : ${CC}
cflags:                               : ${CFLAGS}
"

