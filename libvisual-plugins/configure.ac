# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual plugins, 0.3.0, ds@nerds-incorporated.org)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

m4_define([libvisual_required_version], [0.3.0])
m4_define([esound_required_version], [0.2.28])
m4_define([jack_required_version], [0.98.0])
m4_define([gdk_pixbuf_required_version], [2.0])

# Checks for programs.
AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/])
fi

if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_ERROR([*** pkg-config too old; version 0.14 or better required.])
fi

## Initial sanity check, done here so that users get told they 
## have the wrong dependencies as early in the process as possible.
## Later on we actually use the cflags/libs from separate pkg-config
## calls.
PKG_CHECK_MODULES(BASE_DEPENDENCIES, [libvisual = libvisual_required_version])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

# Internationalization
ALL_LINGUAS="es es_AR"
GETTEXT_PACKAGE=libvisual-plugins
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [GETTEXT package name])
AM_GLIB_GNU_GETTEXT

# Checks for libraries.
build_input=""
build_actor=""
build_morph=""

PKG_CHECK_MODULES(LIBESD, esound >= esound_required_version, [libesd="yes"], [libesd="no"])
if test x$libesd = xno; then
	AC_MSG_WARN([*** esound not found or too old.
			The esound input plugin won't be build])
fi
AC_SUBST(LIBESD_LIBS)
AC_SUBST(LIBESD_CFLAGS)
if test x$libesd = xyes; then
  build_input="$build_input esd"
  AC_SUBST([ESD], ['input_esd.la'])
fi

PKG_CHECK_MODULES(LIBJACK, jack >= jack_required_version, [jack="yes"], [jack="no"])
if test x$jack = xno; then
	AC_MSG_WARN([*** libjack is too old. You can download a newer version at
		     	http://jackit.sf.net/. The jackit input plugin won't be build])
fi
AC_SUBST(LIBJACK_LIBS)
AC_SUBST(LIBJACK_CFLAGS)
if test x$jack = xyes; then
  build_input="$build_input jack"
  AC_SUBST([JACK], ['input_jack.la'])
fi

PKG_CHECK_MODULES(LIBVISUAL, libvisual = libvisual_required_version, [libvis="yes"], [libvis="no"])
if test x$libvis = xno; then
	AC_MSG_ERROR([*** libvisual not found.
			libvisual is needed to build this package.
			You can download libvisual at http://libvisual.sf.net/])
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
  AC_SUBST([GOOM2], ['actor_goom2.la'])
fi

PKG_CHECK_MODULES(GDKPIXBUF, gdk-2.0 >= gdk_pixbuf_required_version, [gdkpixbuf="yes"], [gdkpixbuf="no"])
if test x$gdkpixbuf = xno; then
  AC_MSG_WARN([*** GdkPixbuf >= ${gdk_pixbuf_required_version} is not found.
		The libvisual gdkpixbuf image loader plugin won't be build.
		GdkPixbuf is included within gtk-2.0 and newer, which can be
		downloaded at http://www.gtk.org/])
fi
AC_SUBST(GDKPIXBUF_LIBS)
AC_SUBST(GDKPIXBUF_CFLAGS)
if test x$gdkpixbuf = xyes; then
  build_actor="$build_actor gdkpixbuf"
  AC_SUBST([GDKPIXBUF], ['actor_gdkpixbuf.la'])
fi
  
AC_PATH_X

opengl=no
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

    opengl=$have_lib_gl

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
else
    opengl=no
fi

AM_CONDITIONAL(HAVE_X,  test x$have_x = xyes)


# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
# Checks for header files.
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h unistd.h stdlib.h string.h sys/time.h])
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memmove memset pow sqrt])
AC_CHECK_FUNC([munmap], lv_have_munmap=yes, lv_have_munmap=no)
AC_FUNC_MMAP

# Arguments to specify certain features.
LV_ALSA_MIN_VERSION=0.9.0
AC_ARG_ENABLE([alsa],
            AC_HELP_STRING([--disable-alsa],
                           [Do not build Alsa input plugin @<:@default=enabled@:>@]),
	    [alsa=$enableval],
	    [alsa=yes])
AC_MSG_CHECKING([whether to enable Alsa input plugin])
if test x$alsa = xyes; then
  AC_MSG_RESULT([yes])
  build_alsa=yes
  AM_PATH_ALSA($LV_ALSA_MIN_VERSION,[ build_alsa=yes ], [ build_alsa=no ])
  if test x$build_alsa = xyes; then
    build_input="$build_input alsa"
    AC_SUBST([ALSA], ['input_alsa.la'])
    AC_TRY_COMPILE([#include <stdlib.h>
		    #include <alsa/asoundlib.h>],
		   [#if(SND_LIB_MAJOR == 0)
		     exit(0);
		    #else
		     something that fails to compile
		    #endif],
		   [ AC_DEFINE([HAVE_0_9_X_ALSA], [1], [Alsa Library is from 0.9 series]) ],
		   [ AC_DEFINE([HAVE_1_X_X_ALSA], [1], [Alsa Library is from 1 series]) ])
  else
    AC_MSG_WARN([*** Alsa input plugin cannot be built.
		 May be you have an ALSA library older than $LV_ALSA_MIN_VERSION.
		 Take a look at {prefix}/include/alsa/version.h file.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([mplayer],
            AC_HELP_STRING([--disable-mplayer],
                           [Do not build mplayer input plugin @<:@default=enabled@:>@]),
	    [mplayer=$enableval],
	    [mplayer=yes])
AC_MSG_CHECKING([whether to build mplayer input plugin])
if test x$mplayer = xyes; then
  AC_MSG_RESULT([yes])
  # FIXME we must check also for HAVE_MMAP here
  if test x$lv_have_munmap = xyes; then
    build_input="$build_input mplayer"
    AC_SUBST([MPLAYER], ['input_mplayer.la'])
  else
    AC_MSG_WARN([*** There is not a working munmap function available.
		mplayer input plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([gltest],
            AC_HELP_STRING([--disable-gltest],
                           [Do not build lv_gltest plugin @<:@default=enabled@:>@]),
	    [lvgltest=$enableval],
	    [lvgltest=yes])
AC_MSG_CHECKING([whether to build lv_gltest plugin])
if test x$lvgltest = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor lv_gltest"
    AC_SUBST([GLTEST], ['actor_lv_gltest.la'])
  else
    AC_MSG_WARN([*** No OpenGL found.
		lv_gltest plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([madspin],
            AC_HELP_STRING([--disable-madspin],
                           [Do not build Madspin plugin @<:@default=enabled@:>@]),
	    [madspin=$enableval],
	    [madspin=yes])
AC_MSG_CHECKING([whether to build Madspin plugin])
if test x$madspin = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor madspin"
    AC_SUBST([MADSPIN], ['actor_madspin.la'])
  else
    AC_MSG_WARN([*** No OpenGL found.
		Madspin plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([dancingparticles],
            AC_HELP_STRING([--disable-dancingparticles],
                           [Do not build Dancing Particles plugin @<:@default=enabled@:>@]),
	    [dancingparticles=$enableval],
	    [dancingparticles=yes])
AC_MSG_CHECKING([whether to build Dancing Particles plugin])
if test x$dancingparticles = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor dancingparticles"
    AC_SUBST([DANCINGPARTICLES], ['actor_dancingparticles.la'])
  else
    AC_MSG_WARN([*** No OpenGL found.
		Dancing Particles plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([flower],
            AC_HELP_STRING([--disable-flower],
                           [Do not build Pseudotoad Flower plugin @<:@default=enabled@:>@]),
	    [flower=$enableval],
	    [flower=yes])
AC_MSG_CHECKING([whether to build Pseudotoad Flower plugin])
if test x$flower = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor flower"
    AC_SUBST([FLOWER], ['actor_flower.la'])
  else
    AC_MSG_WARN([*** No OpenGL found.
		Pseudotoad Flower plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([nebulus],
            AC_HELP_STRING([--disable-nebulus],
                           [Do not build Nebulus plugin @<:@default=enabled@:>@]),
	    [nebulus=$enableval],
	    [nebulus=yes])
AC_MSG_CHECKING([whether to build Nebulus plugin])
if test x$nebulus = xyes; then
  AC_MSG_RESULT([yes])
  if test x$opengl = xyes; then
    build_actor="$build_actor nebulus"
    AC_SUBST([NEBULUS], ['actor_nebulus.la'])
  else
    AC_MSG_WARN([*** No OpenGL found.
		Nebulus plugin will not be built.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([infinite],
            AC_HELP_STRING([--disable-infinite],
                           [Do not build Infinite plugin @<:@default=enabled@:>@]),
	    [infinite=$enableval],
	    [infinite=yes])
AC_MSG_CHECKING([whether to build Infinite plugin])
if test x$infinite = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor infinite"
  AC_SUBST([INFINITE], ['actor_infinite.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([jess],
            AC_HELP_STRING([--disable-jess],
                           [Do not build JESS plugin @<:@default=enabled@:>@]),
	    [jess=$enableval],
	    [jess=yes])
AC_MSG_CHECKING([whether to build JESS plugin])
if test x$jess = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor JESS"
  AC_SUBST([JESS], ['actor_JESS.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([corona],
            AC_HELP_STRING([--disable-corona],
                           [Do not build corona plugin @<:@default=enabled@:>@]),
	    [corona=$enableval],
	    [corona=yes])
AC_MSG_CHECKING([whether to build corona plugin])
if test x$corona = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor corona"
  AC_SUBST([corona], ['actor_corona.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([analyzer],
            AC_HELP_STRING([--disable-analyzer],
                           [Do not build Analyzer plugin @<:@default=enabled@:>@]),
	    [analyzer=$enableval],
	    [analyzer=yes])
AC_MSG_CHECKING([whether to build Analyzer plugin])
if test x$analyzer = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor lv_analyzer"
  AC_SUBST([ANALYZER], ['actor_lv_analyzer.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([scope],
            AC_HELP_STRING([--disable-scope],
                           [Do not build Scope plugin @<:@default=enabled@:>@]),
	    [scope=$enableval],
	    [scope=yes])
AC_MSG_CHECKING([whether to build Scope plugin])
if test x$scope = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor lv_scope"
  AC_SUBST([SCOPE], ['actor_lv_scope.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([oinksie],
            AC_HELP_STRING([--disable-oinksie],
                           [Do not build Oinksie plugin @<:@default=enabled@:>@]),
	    [oinksie=$enableval],
	    [oinksie=yes])
AC_MSG_CHECKING([whether to build Oinksie plugin])
if test x$oinksie = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor oinksie"
  AC_SUBST([OINKSIE], ['actor_oinksie.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([jakdaw],
            AC_HELP_STRING([--disable-jakdaw],
                           [Do not build Jakdaw plugin @<:@default=enabled@:>@]),
	    [jakdaw=$enableval],
	    [jakdaw=yes])
AC_MSG_CHECKING([whether to build Jakdaw plugin])
if test x$jakdaw = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor jakdaw"
  AC_SUBST([JAKDAW], ['actor_jakdaw.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([plazma],
            AC_HELP_STRING([--disable-plazma],
                           [Do not build plazma plugin @<:@default=enabled@:>@]),
	    [plazma=$enableval],
	    [plazma=yes])
AC_MSG_CHECKING([whether to build plazma plugin])
if test x$plazma = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor plazma"
  AC_SUBST([PLAZMA], ['actor_plazma.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([bumpscope],
            AC_HELP_STRING([--disable-bumpscope],
                           [Do not build bumpscope plugin @<:@default=enabled@:>@]),
	    [bumpscope=$enableval],
	    [bumpscope=yes])
AC_MSG_CHECKING([whether to build bumpscope plugin])
if test x$bumpscope = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor bumpscope"
  AC_SUBST([BUMPSCOPE], ['actor_bumpscope.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([gforce],
            AC_HELP_STRING([--disable-gforce],
                           [Do not build the G-Force plugin @<:@default=enabled@:>@]),
	    [gforce=$enableval],
	    [gforce=yes])
AC_MSG_CHECKING([whether to build the G-Force plugin])
if test x$gforce = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor G-Force"
  AC_SUBST([GFORCE], ['actor_gforce.la'])
  
  # Special flags for G-Force, make this only for G-Force FIXME FIXME!!! 
  GFORCE_CFLAGS="-DUNIX_X -D_REENTRANT"
  AC_SUBST(GFORCE_FLAGS, "${GFORCE_CFLAGS}")
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([alphablend],
            AC_HELP_STRING([--disable-alphablend],
                           [Do not build alphablend morph plugin @<:@default=enabled@:>@]),
	    [alphablend=$enableval],
	    [alphablend=yes])
AC_MSG_CHECKING([whether to build alphablend morph plugin])
if test x$alphablend = xyes; then
  AC_MSG_RESULT([yes])
  build_morph="$build_morph alphablend"
  AC_SUBST([ALPHABLEND], ['morph_alphablend.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([flash],
            AC_HELP_STRING([--disable-flash],
                           [Do not build flash morph plugin @<:@default=enabled@:>@]),
	    [flash=$enableval],
	    [flash=yes])
AC_MSG_CHECKING([whether to build flash morph plugin])
if test x$flash = xyes; then
  AC_MSG_RESULT([yes])
  build_morph="$build_morph flash"
  AC_SUBST([FLASH], ['morph_flash.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([tentacle],
            AC_HELP_STRING([--disable-tentacle],
                           [Do not build tentacle morph plugin @<:@default=enabled@:>@]),
	    [tentacle=$enableval],
	    [tentacle=yes])
AC_MSG_CHECKING([whether to build tentacle morph plugin])
if test x$tentacle = xyes; then
  AC_MSG_RESULT([yes])
  build_morph="$build_morph tentacle"
  AC_SUBST([TENTACLE], ['morph_tentacle.la'])
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([slide],
            AC_HELP_STRING([--disable-slide],
                           [Do not build slide morph plugin @<:@default=enabled@:>@]),
	    [slide=$enableval],
	    [slide=yes])
AC_MSG_CHECKING([whether to build slide morph plugin])
if test x$slide = xyes; then
  AC_MSG_RESULT([yes])
  build_morph="$build_morph slide"
  AC_SUBST([SLIDE], ['morph_slide.la'])
else
  AC_MSG_RESULT([no])
fi

#
# Build paramaters
#

DEBUG_CFLAGS=""

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
AC_MSG_CHECKING([whether to enable debug])
if test "$debug" = yes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall -Wmissing-braces -Wimplicit -Wunused"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-prototypes -Wno-unused-variable"
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

AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${GFORCE_CFLAGS}")
AC_SUBST(CXXFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${GFORCE_CFLAGS}")

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

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(DATADIR, "$datadir")

AC_CONFIG_FILES([
	Makefile
	po/Makefile.in
	plugins/Makefile
	plugins/actor/Makefile
	plugins/actor/lv_analyzer/Makefile
	plugins/actor/lv_scope/Makefile
	plugins/actor/lv_gltest/Makefile
	plugins/actor/madspin/Makefile
	plugins/actor/dancingparticles/Makefile
	plugins/actor/pseudotoad_flower/Makefile
	plugins/actor/nebulus/Makefile
	plugins/actor/oinksie/Makefile
	plugins/actor/jakdaw/Makefile
	plugins/actor/plazma/Makefile
	plugins/actor/bumpscope/Makefile
	plugins/actor/infinite/Makefile
	plugins/actor/JESS/Makefile
	plugins/actor/corona/Makefile
	plugins/actor/goom2/Makefile
	plugins/actor/gdkpixbuf/Makefile
	plugins/actor/G-Force/Makefile
	plugins/actor/G-Force/docs/Makefile
	plugins/actor/G-Force/Common/Makefile
	plugins/actor/G-Force/Common/GeneralTools/Makefile
	plugins/actor/G-Force/Common/GeneralTools/Headers/Makefile
	plugins/actor/G-Force/Common/io/Makefile
	plugins/actor/G-Force/Common/io/Headers/Makefile
	plugins/actor/G-Force/Common/math/Makefile
	plugins/actor/G-Force/Common/math/Headers/Makefile
	plugins/actor/G-Force/Common/UI/Makefile
	plugins/actor/G-Force/Common/UI/Headers/Makefile
	plugins/actor/G-Force/GForceCommon/Makefile
	plugins/actor/G-Force/GForceCommon/Headers/Makefile
	plugins/actor/G-Force/unix/Makefile
	plugins/actor/G-Force/unix/Headers/Makefile
	plugins/actor/G-Force/unix/libmfl/Makefile
	plugins/actor/G-Force/unix/libvisual/Makefile
	plugins/actor/G-Force/GForceColorMaps/Makefile
	plugins/actor/G-Force/GForceDeltaFields/Makefile
	plugins/actor/G-Force/GForceParticles/Makefile
	plugins/actor/G-Force/GForceWaveShapes/Makefile
	plugins/actor/G-Force/NotWorkingWaveShapes/Makefile
	plugins/input/Makefile
	plugins/input/esd/Makefile
	plugins/input/alsa/Makefile
	plugins/input/mplayer/Makefile
	plugins/input/jack/Makefile
	plugins/morph/Makefile
	plugins/morph/alphablend/Makefile
	plugins/morph/tentacle/Makefile
	plugins/morph/flash/Makefile
	plugins/morph/slide/Makefile
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
input:                                : ${build_input}
actor:                                : ${build_actor}
morph:                                : ${build_morph}

Libvisual plugin data dir:            : ${prefix}/share

Build options:
install path                          : $plugins_base_dir
source code location:                 : ${srcdir}
compiler:                             : ${CC}
"

