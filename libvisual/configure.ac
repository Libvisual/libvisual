# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual Library, 0.1.5, synap@nerds-incorporated.org, libvisual)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_HEADER(config.h)

AC_DISABLE_STATIC

# Checks for programs.
AC_PROG_LIBTOOL
AC_PROG_CC
AC_PROG_LN_S

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h stdint.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memset sqrt strdup])

# Default prefix
AC_PREFIX_DEFAULT(/usr)

#
# Find pkg-config
#
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

# Find SDL to build the examples.
# On future, this must be made optional
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

libs_dynamic_loader="-ldl"
AC_SUBST(CFLAGS, "-I./ -O2 -Wall -rdynamic -ggdb")
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

VISUAL_LIBS="-L${libdir} -lvisual -lm -ldl"
VISUAL_CFLAGS="-I${includedir}"
AC_SUBST(VISUAL_CFLAGS)
AC_SUBST(VISUAL_LIBS)

AC_CONFIG_FILES([
Makefile
libvisual.pc
libvisual/Makefile
examples/Makefile
])
#examples/Makefile	This are not included in the package
#tests/Makefile		This are not included in the package

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL, AN AUDIO VISUALISATION ABSTRACTION LIBRARY $VERSION
=====================================================================

prefix:                               : ${prefix}
source code location:                 : ${srcdir}
compiler:                             : ${CC}
cflags:                               : ${CFLAGS}
"

