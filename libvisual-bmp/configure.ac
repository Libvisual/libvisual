# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual beep media player plugin, 0.2.0, synap@nerds-incorporated.org, libvisual-bmp)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_PREFIX_DEFAULT(/usr)
AC_PREFIX_PROGRAM(beep-media-player)

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
AC_CHECK_HEADERS([unistd.h])
AC_CHECK_HEADERS([GL/gl.h GL/glx.h GL/glu.h],opengl="yes",opengl="no")
if test x"$opengl" = xno; then
	AC_MSG_ERROR([OpenGL and Glu headers not found, the plugin cannot be build])
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE # reserved for future

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

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.2.0, [libvis="yes"], [libvis="no"])
if test x"$libvis" = xno; then
	AC_MSG_ERROR([libvisual not found, libvisual is needed to build this package,
you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

PKG_CHECK_MODULES(BMP, bmp >= 0.9.7, [bmp="yes"], [bmp="no"])
if test x"$bmp" = xno; then
	AC_MSG_ERROR([beep-media-player not found, beep-media-player is needed to build this package,
you can download beep-media-player at http://beepmp.sf.net/])
fi
AC_SUBST(BMP_LIBS)
AC_SUBST(BMP_CFLAGS)



BMP_PLUGIN_DIR="$prefix/lib/bmp"
BMP_VISUALIZATION_PLUGIN_DIR="$BMP_PLUGIN_DIR/Visualization"
AC_SUBST(BMP_VISUALIZATION_PLUGIN_DIR)

# Internationalization
ALL_LINGUAS="es es_AR pt_BR nl fr ru"
GETTEXT_PACKAGE=libvisual-bmp
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

# Arguments to specify certain features.
AC_ARG_ENABLE([debug],
  AC_HELP_STRING([--enable-debug],
		[Enable debug @<:@default=disabled@:>@]),
		[debug=$enableval],
		[debug=no])
AC_MSG_CHECKING([wheter to enable debug])
if test "$debug" = yes; then
  AC_MSG_RESULT([yes])
  AC_DEFINE([LV_BMP_ENABLE_DEBUG], [1], [Enable debug whithin libvisual beep-media-player plugin])
  DEBUG_CFLAGS="-ggdb3"
else
  AC_MSG_RESULT([no])
  DEBUG_CFLAGS=""
fi
AC_SUBST(DEBUG_CFLAGS)

# cflags, ld flags
libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(BUILD_INPUT, "$build_input")
AC_SUBST(BUILD_ACTOR, "$build_actor")

lv_bmp_datadir="$prefix/share/libvisual-bmp"
AC_DEFINE_UNQUOTED(PACKAGE_DATADIR, "$lv_bmp_datadir", [Libvisual bmp plugin data directory])
AC_SUBST(PACKAGE_DATADIR)

AC_CONFIG_FILES([
	po/Makefile.in
	Makefile
	pixmaps/Makefile
	src/Makefile
	])

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-BMP, BEEP MEDIA PLAYER PLUGIN FOR LIBVISUAL $VERSION
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

