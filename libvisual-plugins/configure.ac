# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual plugins, 0.1.5, synap@nerds-incorporated.org)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
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

# Checks for libraries.
build_input=""
build_actor="infinite JESS lv_scope oinksie"
build_morph="alphablend tentacle flash"

# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main])
# FIXME: Replace `main' with a function in `-lGLU':
AC_CHECK_LIB([GLU], [main])

PKG_CHECK_MODULES(LIBESD, esound >= 0.2.29, [libesd="yes"], [libesd="no"])
if test x$libesd = xno; then
	AC_MSG_WARN([
*** esound not found or too old, the esound plugin won't be build
	])
fi
AC_SUBST(LIBESD_LIBS)
AC_SUBST(LIBESD_CFLAGS)
if test x$libesd = xyes; then
	build_input="esd $build_input"
fi

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.1.5, [libvis="yes"], [libvis="no"])
if test x$libvis = xno; then
	AC_MSG_ERROR([
*** libvisual not found, libvisual is needed to build this package,
*** you can download libvisual at http://libvisual.sf.net/
	])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)
	
PKG_CHECK_MODULES(LIBGOOM2, libgoom2 = 2k4, [libgoom="yes"], [libgoom="no"])
if test x$libgoom = xno; then
	AC_MSG_WARN([
*** libgoom 2k4 not found, the goom plugin won't be build
*** you can download goom at http://www.ios-software.com/
	])
fi
AC_SUBST(LIBGOOM2_LIBS)
AC_SUBST(LIBGOOM2_CFLAGS)
if test x"$libgoom" = xyes; then
	build_actor="goom2 $build_actor"
fi

# Checks for header files.
AC_HEADER_STDC
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h unistd.h])
AC_CHECK_HEADERS([GL/gl.h GL/glx.h GL/glu.h],opengl="yes",opengl="no")
if test x"$opengl" = xno; then
	AC_MSG_WARN([
*** OpenGl and Glu headers not found, the opengl plugins won't be build
	])
fi
if test x"$opengl" = xyes; then
	build_actor="lv_gltest dna madspin $build_actor"
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memset pow sqrt])

# Default prefix
AC_PREFIX_DEFAULT(/usr)
prefix=$prefix

# check plugin data dir
#datadir = pkg-config libvisual --variable=datadir

# cflags, ld flags
libs_dynamic_loader="-ldl"
AC_SUBST(CFLAGS, "-I./ -O3 -Wall -rdynamic -ggdb $LIBVISUAL_CFLAGS")
AC_SUBST(LV_LIBS, "$LIBVISUAL_LIBS")
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(DATADIR, "$datadir")

AC_SUBST(BUILD_INPUT, "$build_input")
AC_SUBST(BUILD_ACTOR, "$build_actor")
AC_SUBST(BUILD_MORPH, "$build_morph")

AC_CONFIG_FILES([
	Makefile
	plugins/Makefile
	plugins/input/Makefile
	plugins/input/esd/Makefile
	plugins/actor/Makefile
	plugins/actor/lv_scope/Makefile
	plugins/actor/lv_gltest/Makefile
	plugins/actor/madspin/Makefile
	plugins/actor/dna/Makefile
	plugins/actor/oinksie/Makefile
	plugins/actor/infinite/Makefile
	plugins/actor/JESS/Makefile
	plugins/actor/goom2/Makefile
	plugins/morph/Makefile
	plugins/morph/alphablend/Makefile
	plugins/morph/tentacle/Makefile
	plugins/morph/flash/Makefile
])

#plugins/actor/lv_analyzer/Makefile
#plugins/actor/video_test/Makefile

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-PLUGINS, PLUGINS FOR LIBVISUAL $VERSION
=====================================================================

Packages found:
libvisual:                            : ${libvis}
libgoom2:                             : ${libgoom}
libesd:                               : ${libesd}
opengl:                               : ${opengl}

Plugins being build:
input:                                : ${BUILD_INPUT}
actor:                                : ${BUILD_ACTOR}
morph:                                : ${BUILD_MORPH}

Libvisual plugin data dir:            : ${DATADIR}

Build options:
prefix:                               : ${prefix}
source code location:                 : ${srcdir}
compiler:                             : ${CC}
cflags:                               : ${CFLAGS}
"

