# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual Library, 0.1.5, synap@nerds-incorporated.org, libvisual)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_HEADER(config.h)

AC_PREFIX_DEFAULT(/usr)

# Checks for programs.
AC_PROG_CC
AC_PROG_LN_S

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_WARN([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/.
	       *** You will need pkg-config to compile against Libvisual Library])
fi
if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_WARN([*** pkg-config too old; version 0.14 or better recommended.])
fi

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h unistd.h])

#
# In future, we will set here a flags indicating we have or not OpenGl.
# First, CONFIG_HEADER must be removed.
#
#usable_opengl=0
#AC_CHECK_HEADERS([GL/gl.h GL/glx.h GL/glu.h],opengl="yes",opengl="no")
#if test x"$opengl" = xno; then
#	AC_MSG_WARN([
#*** OpenGl and Glu headers not found, the opengl examples won't be build
#	])
#else
#	usable_opengl=1
#fi
#AC_DEFINE([USABLE_OPENGL], [$usable_opengl])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

AC_MSG_CHECKING(for ISO C99 varargs macros in C)
AC_TRY_COMPILE([],[
		int a(int p1, int p2, int p3);
		#define call_a(...) a(1,__VA_ARGS__)
		call_a(2,3);
		],lv_have_iso_c_varargs=yes,lv_have_iso_c_varargs=no)
AC_MSG_RESULT($lv_have_iso_c_varargs)
if test "x$lv_have_iso_c_varargs" = xyes; then
  AC_DEFINE(LV_HAVE_ISO_VARARGS, 1, [Have ISOC99 varargs macros])
else
  AC_DEFINE(LV_HAVE_ISO_VARARGS, 0, [Have ISOC99 varargs macros])
fi
AC_SUBST(LV_HAVE_ISO_VARARGS)

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset sqrt strdup])
# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main])
# FIXME: Replace `main' with a function in `-lGLU':
AC_CHECK_LIB([GLU], [main])

# Arguments to specify certain features.
build_subdirs="libvisual"
AC_ARG_ENABLE([examples],
            AC_HELP_STRING([--enable-examples],
                           [Build example programs @<:@default=disabled@:>@]),
	    [examples=$enableval],
	    [examples=no])
AC_MSG_CHECKING([wheter to build example programs])
if test "$examples" = yes; then
  AC_MSG_RESULT([yes])
  # Find SDL to build the examples.
  AC_PATH_PROG(SDL_CONFIG, [sdl-config], [no])
  if test x$SDL_CONFIG = xno ; then
    AC_MSG_ERROR([
*** sdl-config not found.
*** If you have installed from binaries, probably you have not 
*** installed SDL development package.
*** See http://www.libsdl.org
    ])
  fi
  # On success, this will set and substitute SDL_CFLAGS and SDL_LIBS
  AM_PATH_SDL(1.2.0,,AC_MSG_ERROR([*** SDL >= 1.2.0 not installed - please install first]))
  build_subdirs="$build_subdirs examples"
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(BUILD_SUBDIRS, "$build_subdirs")

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

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

VISUAL_CFLAGS="-I${includedir}"
VISUAL_LIBS="-L${libdir} -lvisual -lm -ldl"
AC_SUBST(VISUAL_CFLAGS)
AC_SUBST(VISUAL_LIBS)

# Terrible hack, but otherwise the CFLAGS are passed as last arguments
# to compiler, and this way these arguments (probably -g -O2) overrides
# our -O3 flag on libvisual/Makefile.am
AC_SUBST(CFLAGS, "")

AC_CONFIG_FILES([
Makefile
libvisual.pc
libvisual/Makefile
examples/Makefile
])
#tests/Makefile		This are not included in the package

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL, AN AUDIO VISUALISATION ABSTRACTION LIBRARY, VERSION $VERSION
=====================================================================

install path                         : ${prefix}/lib
compiler                             : ${CC}
debug enabled                        : ${debug}
build examples                       : ${examples}
build morph example                  : ${opengl}
"

