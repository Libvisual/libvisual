# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual plugins, 0.1.7, synap@nerds-incorporated.org)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

AC_PREFIX_DEFAULT(/usr)

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
build_input="mplayer"
build_actor=""
build_morph="alphablend tentacle flash"

# FIXME: Replace `main' with a function in `-lGL':
AC_CHECK_LIB([GL], [main])
# FIXME: Replace `main' with a function in `-lGLU':
AC_CHECK_LIB([GLU], [main])

PKG_CHECK_MODULES(LIBESD, esound >= 0.2.28, [libesd="yes"], [libesd="no"])
if test x$libesd = xno; then
	AC_MSG_WARN([*** esound not found or too old.
			The esound plugin won't be build])
fi
AC_SUBST(LIBESD_LIBS)
AC_SUBST(LIBESD_CFLAGS)
if test x$libesd = xyes; then
	build_input="esd $build_input"
fi

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.1.7, [libvis="yes"], [libvis="no"])
if test x$libvis = xno; then
	AC_MSG_ERROR([*** libvisual not found.
			libvisual is needed to build this package.
			you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)
	
PKG_CHECK_MODULES(LIBGOOM2, libgoom2 = 2k4, [libgoom="yes"], [libgoom="no"])
if test x$libgoom = xno; then
	AC_MSG_WARN([*** libgoom 2k4 not found.
			The goom plugin won't be build.
			you can download goom at http://www.ios-software.com/])
fi
AC_SUBST(LIBGOOM2_LIBS)
AC_SUBST(LIBGOOM2_CFLAGS)
if test x$libgoom = xyes; then
  build_actor="$build_actor goom2"
fi

#FIXME We actually don't want to depend on gtk here..... 
PKG_CHECK_MODULES(GDKPIXBUF, gdk-2.0 >= 2.0, [gdkpixbuf="yes"], [gdkpixbuf="no"])
if test x$gdkpixbuf = xno; then
  AC_MSG_WARN([*** GdkPixbuf >= 2.0 is not found.
		The libvisual gdkpixbuf image loader plugin won't be build.
		GdkPixbuf is included within gtk-2.0 and newer, which can be
		downloaded at http://www.gtk.org/])
fi
AC_SUBST(GDKPIXBUF_LIBS)
AC_SUBST(GDKPIXBUF_CFLAGS)
if test x$gdkpixbuf = xyes; then
  build_actor="$build_actor gdkpixbuf"
fi
  
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
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
# Checks for header files.
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h unistd.h])
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memset pow sqrt])

# Arguments to specify certain features.
LV_ALSA_MIN_VERSION=1.0.0
AC_ARG_ENABLE([alsa],
            AC_HELP_STRING([--disable-alsa],
                           [Do not build Alsa input plugin @<:@default=enabled@:>@]),
	    [alsa=$enableval],
	    [alsa=yes])
AC_MSG_CHECKING([wheter to enable Alsa input plugin])
if test x$alsa = xyes; then
  AC_MSG_RESULT([yes])
  build_alsa=yes
  AM_PATH_ALSA($LV_ALSA_MIN_VERSION,[ build_alsa=yes ], [ build_alsa=no ])
  if test x$build_alsa = xyes; then
    build_input="$build_input alsa"
  else
    AC_MSG_WARN([*** Alsa input plugin cannot be built.
		 May be you have an ALSA library older than $LV_ALSA_MIN_VERSION.
		 Take a look at {prefix}/include/alsa/version.h file.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([dna],
            AC_HELP_STRING([--disable-dna],
                           [Do not build dna plugin @<:@default=enabled@:>@]),
	    [dna=$enableval],
	    [dna=yes])
AC_MSG_CHECKING([wheter to build dna plugin])
if test x$dna = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor dna"
  else
    AC_MSG_WARN([*** Libvisual doesn't have OpenGL support.
		dna plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([gltest],
            AC_HELP_STRING([--disable-gltest],
                           [Do not build lv_gltest plugin @<:@default=enabled@:>@]),
	    [lvgltest=$enableval],
	    [lvgltest=yes])
AC_MSG_CHECKING([wheter to build lv_gltest plugin])
if test x$lvgltest = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor lv_gltest"
  else
    AC_MSG_WARN([*** Libvisual doesn't have OpenGL support.
		lv_gltest plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([madspin],
            AC_HELP_STRING([--disable-madspin],
                           [Do not build madspin plugin @<:@default=enabled@:>@]),
	    [madspin=$enableval],
	    [madspin=yes])
AC_MSG_CHECKING([wheter to build madspin plugin])
if test x$madspin = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor madspin"
  else
    AC_MSG_WARN([*** Libvisual doesn't have OpenGL support.
		madspin plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([infinite],
            AC_HELP_STRING([--disable-infinite],
                           [Do not build Infinite plugin @<:@default=enabled@:>@]),
	    [infinite=$enableval],
	    [infinite=yes])
AC_MSG_CHECKING([wheter to build Infinite plugin])
if test x$infinite = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor infinite"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([jess],
            AC_HELP_STRING([--disable-jess],
                           [Do not build JESS plugin @<:@default=enabled@:>@]),
	    [jess=$enableval],
	    [jess=yes])
AC_MSG_CHECKING([wheter to build JESS plugin])
if test x$jess = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor JESS"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([analyzer],
            AC_HELP_STRING([--disable-analyzer],
                           [Do not build Analyzer plugin @<:@default=enabled@:>@]),
	    [analyzer=$enableval],
	    [analyzer=yes])
AC_MSG_CHECKING([wheter to build Analyzer plugin])
if test x$analyzer = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor lv_analyzer"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([scope],
            AC_HELP_STRING([--disable-scope],
                           [Do not build Scope plugin @<:@default=enabled@:>@]),
	    [scope=$enableval],
	    [scope=yes])
AC_MSG_CHECKING([wheter to build Scope plugin])
if test x$scope = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor lv_scope"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([oinksie],
            AC_HELP_STRING([--disable-oinksie],
                           [Do not build Oinksie plugin @<:@default=enabled@:>@]),
	    [oinksie=$enableval],
	    [oinksie=yes])
AC_MSG_CHECKING([wheter to build Oinksie plugin])
if test x$oinksie = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor oinksie"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([jakdaw],
            AC_HELP_STRING([--disable-jakdaw],
                           [Do not build Jakdaw plugin @<:@default=enabled@:>@]),
	    [jakdaw=$enableval],
	    [jakdaw=yes])
AC_MSG_CHECKING([wheter to build Jakdaw plugin])
if test x$jakdaw = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor jakdaw"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([plazma],
            AC_HELP_STRING([--disable-plazma],
                           [Do not build plazma plugin @<:@default=enabled@:>@]),
	    [plazma=$enableval],
	    [plazma=yes])
AC_MSG_CHECKING([wheter to build plazma plugin])
if test x$plazma = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor plazma"
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([bumpscope],
            AC_HELP_STRING([--disable-bumpscope],
                           [Do not build bumpscope plugin @<:@default=enabled@:>@]),
	    [bumpscope=$enableval],
	    [bumpscope=yes])
AC_MSG_CHECKING([wheter to build bumpscope plugin])
if test x$bumpscope = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor bumpscope"
else
  AC_MSG_RESULT([no])
fi

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

AC_SUBST(DATADIR, "$datadir")

AC_SUBST(BUILD_INPUT, "$build_input")
AC_SUBST(BUILD_ACTOR, "$build_actor")
AC_SUBST(BUILD_MORPH, "$build_morph")

AC_CONFIG_FILES([
	Makefile
	plugins/Makefile
	plugins/actor/Makefile
	plugins/actor/lv_analyzer/Makefile
	plugins/actor/lv_scope/Makefile
	plugins/actor/lv_gltest/Makefile
	plugins/actor/madspin/Makefile
	plugins/actor/dna/Makefile
	plugins/actor/oinksie/Makefile
	plugins/actor/jakdaw/Makefile
	plugins/actor/plazma/Makefile
	plugins/actor/bumpscope/Makefile
	plugins/actor/infinite/Makefile
	plugins/actor/JESS/Makefile
	plugins/actor/goom2/Makefile
	plugins/actor/gdkpixbuf/Makefile
	plugins/input/Makefile
	plugins/input/esd/Makefile
	plugins/input/alsa/Makefile
	plugins/input/mplayer/Makefile
	plugins/morph/Makefile
	plugins/morph/alphablend/Makefile
	plugins/morph/tentacle/Makefile
	plugins/morph/flash/Makefile
])


AC_OUTPUT

echo "
================================================
LIBVISUAL-PLUGINS, PLUGINS FOR LIBVISUAL $VERSION
================================================

Packages found:
libvisual:                            : ${libvis}
libgoom2:                             : ${libgoom}
libesd:                               : ${libesd}
alsa:                                 : ${alsa_found}
opengl:                               : ${opengl}
gdkpixbuf:                            : ${gdkpixbuf}

Plugins being build:
input:                                : ${BUILD_INPUT}
actor:                                : ${BUILD_ACTOR}
morph:                                : ${BUILD_MORPH}

Libvisual plugin data dir:            : ${prefix}/share

Build options:
install path                          : ${prefix}/libvisual
source code location:                 : ${srcdir}
compiler:                             : ${CC}
"

