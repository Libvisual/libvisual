# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

m4_define([lv_plugins_version_major], [0])
m4_define([lv_plugins_version_minor], [4])
m4_define([lv_plugins_version_micro], [0])
m4_define([lv_plugins_version], [lv_plugins_version_major.lv_plugins_version_minor.lv_plugins_version_micro])
m4_define([lv_plugins_version_suffix], [lv_plugins_version_major.lv_plugins_version_minor])

AC_INIT(Libvisual plugins, lv_plugins_version, ds@nerds-incorporated.org)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

m4_define([libvisual_required_version], [lv_plugins_version])
m4_define([esound_required_version], [0.2.28])
m4_define([jack_required_version], [0.98.0])
m4_define([gtk_required_version], [2.0])
m4_define([gst_required_version], [0.8])

m4_define([libvisual_pkg], [libvisual-lv_plugins_version_suffix])
LIBVISUAL_PKG=libvisual_pkg

dnl Check for pkg-config
PKG_PROG_PKG_CONFIG([0.14])
if test -z "$PKG_CONFIG"; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/])
fi

dnl Check for libvisual here so that users get told they have the
dnl wrong dependencies as early in the process as possible.
PKG_CHECK_MODULES([LIBVISUAL], [libvisual_pkg >= libvisual_required_version],
  [],
  [AC_MSG_ERROR([*** libvisual libvisual_required_version not found.
 		It is needed to build this package.
		You can download libvisual at http://libvisual.sf.net/])]
)

dnl C/C++ compilers
AC_PROG_CC
AC_PROG_CXX

dnl Check for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE

dnl 'Standard' header files.
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h unistd.h stdlib.h string.h sys/time.h])

dnl Library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memmove memset pow sqrt])
AC_CHECK_FUNC([munmap], lv_have_munmap=yes, lv_have_munmap=no)
AC_FUNC_MMAP

dnl Installation tools
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Libtool
AC_DISABLE_STATIC

m4_undefine([AC_PROG_F77])
m4_defun([AC_PROG_F77],[])
AC_PROG_LIBTOOL

dnl Lex/Yacc
AM_PROG_LEX
AC_CHECK_PROGS([YACC], ['bison -y' byacc yacc])
if test -z "$YACC"; then
   AC_MSG_ERROR([Bison or Yacc not found])
fi

dnl Internationalization
GETTEXT_PACKAGE=libvisual-plugins-lv_plugins_version_suffix
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [gettext package name])
AC_DEFINE_UNQUOTED(LOCALEDIR, "${prefix}/share/locale", [String catalog location])

AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.14.1])

dnl Dynamic linking library
DYNAMIC_LOADER_LIBS="-ldl"
AC_SUBST([DYNAMIC_LOADER_LIBS])

dnl Libraries

dnl EsounD
PKG_CHECK_MODULES([LIBESD], [esound >= esound_required_version],
  [HAVE_ESD="yes"], [HAVE_ESD="no"])

if test "$HAVE_ESD" = "yes"; then
  build_input_plugins="$build_input_plugins esd"
else
  AC_MSG_WARN([*** EsounD not found or too old. The EsounD input plugin won't be built])
fi

dnl JACK
PKG_CHECK_MODULES([LIBJACK], [jack >= jack_required_version], [HAVE_JACK="yes"], [HAVE_JACK="no"])
if test "$HAVE_JACK" = "yes"; then
  build_input_plugins="$build_input_plugins jack"
else
  AC_MSG_WARN([*** libjack is too old. You can download a newer version at
             http://jackit.sf.net/. The jackit input plugin won't be built])
fi

dnl List of plugins to build
build_input_plugins=""
build_actor_plugins=""
build_morph_plugins=""

dnl GdkPixbuf
AC_ARG_ENABLE([gdkpixbuf-plugin],
  AS_HELP_STRING([--disable-gdkpixbuf-plugin],
    [Do not build GdkPixbuf visualization plugin @<:@default=enabled@:>@]),
  [ENABLE_PLUGIN_GDKPIXBUF=$enableval],
  [ENABLE_PLUGIN_GDKPIXBUF=yes])

if test "$ENABLE_PLUGIN_GDKPIXBUF" = "yes"; then
  PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= gtk_required_version],
    [HAVE_GTK="yes"], [HAVE_GTK="no"])

  if test "$HAVE_GTK" = "yes"; then
    build_actor_plugins="$build_actor_plugins gdkpixbuf"
  else
    AC_MSG_WARN([*** GTK+ >= ${gtk_required_version} is not found.
	  	The libvisual GdkPixbuf image loader plugin won't be build.
		GdkPixbuf is included within gtk-2.0 and newer, which can be
		downloaded at http://www.gtk.org/])
  fi
fi

dnl GStreamer
AC_ARG_ENABLE([gstreamer-plugin],
  AS_HELP_STRING([--disable-gstreamer-plugin],
    [Do not build GStreamer visualization plugin @<:@default=enabled@:>@]),
  [ENABLE_GSTREAMER_PLUGIN=$enableval],
  [ENABLE_GSTREAMER_PLUGIN=yes])

if test "$ENABLE_GSTREAMER_PLUGIN" = "yes"; then
  PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.8 >= gst_required_version],
    [HAVE_GSTREAMER="yes"], [HAVE_GSTREAMER="no"])

  if test "$HAVE_GSTREAMER" = "yes"; then
    build_actor_plugins="$build_actor_plugins gstreamer"
  else
    AC_MSG_WARN([*** GStreamer >= ${gst_required_version} is not found.
	  	The libvisual GStreamer viewer plugin won't be build.
		GStreamer can be downloaded from http://www.gstreamer.org/])
  fi
fi

dnl Xlib
AC_PATH_XTRA
AM_CONDITIONAL(HAVE_X, test "$have_x" = "yes")

dnl OpenGL, GLX
HAVE_OPENGL=no

if test "$have_x" = "yes" ; then
  CPPFLAGS_SAVE=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $X_CFLAGS"

  AC_CHECK_HEADERS([GL/gl.h GL/glx.h],
    [AC_CHECK_LIB([GL], [glBegin], [HAVE_OPENGL=yes], [], [$X_LIBS])])

  CPPFLAGS=$CPPFLAGS_SAVE
fi

AM_CONDITIONAL([HAVE_OPENGL], [test "$HAVE_OPENGL" = "yes"])

dnl XF86VidMode
HAVE_XF86VMODE=no
XF86VMODE_LIB=

CPPFLAGS_SAVE=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $X_CFLAGS"

AC_CHECK_HEADERS([X11/extensions/xf86vmode.h],
  [AC_CHECK_LIB([Xxf86vm], [XF86VidModeSwitchToMode], 
    [AC_DEFINE([HAVE_XF86VMODE], [], [XF86VidMode is available])
     XF86VMODE_LIB=-lXxf86vm
     HAVE_XF86VMODE=yes],
    [],
    [$X_LIBS])],
  [],
  [#include <X11/Xlib.h>
  ]
)

CPPFLAGS=$CPPFLAGS_SAVE

AC_SUBST([XF86VMODE_LIB])
AM_CONDITIONAL([HAVE_XF86VMODE], [test "$HAVE_XF86VMODE" = "yes"])

dnl ALSA
LV_ALSA_MIN_VERSION=0.9.0
AC_ARG_ENABLE([alsa],
  AS_HELP_STRING([--disable-alsa],
    [Do not build ALSA input plugin @<:@default=enabled@:>@]),
  [ENABLE_ALSA=$enableval],
  [ENABLE_ALSA=yes])

if test "$ENABLE_ALSA" = "yes"; then
  AM_PATH_ALSA([$LV_ALSA_MIN_VERSION], [HAVE_ALSA=yes], [HAVE_ALSA=no])

  if test "$HAVE_ALSA" = "yes"; then
    build_input_plugins="$build_input_plugins alsa"
    AC_TRY_COMPILE([#include <stdlib.h>
		    #include <alsa/asoundlib.h>
                   ],
		   [#if(SND_LIB_MAJOR == 0)
		    exit(0);
                    #else
		    #error ALSA library version 1.x
		    #endif
                   ],
		   [ AC_DEFINE([HAVE_0_9_X_ALSA], [1], [ALSA Library is from 0.9 series]) ],
		   [ AC_DEFINE([HAVE_1_X_X_ALSA], [1], [ALSA Library is from 1 series]) ])
  else
    AC_MSG_WARN([*** Alsa input plugin cannot be built.
		 May be you have an ALSA library older than $LV_ALSA_MIN_VERSION.
		 Take a look at {prefix}/include/alsa/version.h file.])
  fi
fi

dnl MPlayer
AC_ARG_ENABLE([mplayer],
  AS_HELP_STRING([--disable-mplayer],
    [Do not build mplayer input plugin @<:@default=enabled@:>@]),
  [ENABLE_MPLAYER=$enableval],
  [ENABLE_MPLAYER=yes])

# FIXME: Missing check for MPlayer...
if test "$ENABLE_MPLAYER" = "yes"; then
  # FIXME we must check also for HAVE_MMAP here
  if test "$lv_have_munmap" = "yes"; then
    build_input_plugins="$build_input_plugins mplayer"
  else
    AC_MSG_WARN([*** There is not a working munmap function available.
		mplayer input plugin will not be built.])
  fi
fi

dnl Input debug plugin
AC_ARG_ENABLE([inputdebug],
  AS_HELP_STRING([--disable-inputdebug],
    [Do not build input debug plugin @<:@default=enabled@:>@]),
  [ENABLE_INPUTDEBUG=$enableval],
  [ENABLE_INPUTDEBUG=yes])

if test "$inputdebug" = "yes"; then
  build_input_plugins="$build_input_plugins debug"
fi

dnl OpenGL test plugin
AC_ARG_ENABLE([gltest],
  AS_HELP_STRING([--disable-gltest],
    [Do not build lv_gltest plugin @<:@default=enabled@:>@]),
  [ENABLE_LVGLTEST=$enableval],
  [ENABLE_LVGLTEST=yes])

if test "$ENABLE_LVGLTEST" = "yes"; then
  if test "$HAVE_OPENGL" = "yes"; then
    build_actor_plugins="$build_actor_plugins lv_gltest"
  else
    AC_MSG_WARN([*** No OpenGL found.
		lv_gltest plugin will not be built.])
  fi
fi

dnl NastyFFT
AC_ARG_ENABLE([nastyfft],
  AS_HELP_STRING([--disable-nastyfft],
    [Do not build nastyfft plugin @<:@default=enabled@:>@]),
  [ENABLE_NASTYFFT=$enableval],
  [ENABLE_NASTYFFT=yes])

if test "$ENABLE_NASTYFFT" = "yes"; then
  if test "$HAVE_OPENGL" = "yes"; then
    build_actor_plugins="$build_actor_plugins nastyfft"
  else
    AC_MSG_WARN([*** No OpenGL found.
		nastyfft plugin will not be built.])
  fi
fi

dnl Madspin
AC_ARG_ENABLE([madspin],
  AS_HELP_STRING([--disable-madspin],
    [Do not build Madspin plugin @<:@default=enabled@:>@]),
  [ENABLE_MADSPIN=$enableval],
  [ENABLE_MADSPIN=yes])

if test "$ENABLE_MADSPIN" = "yes"; then
  if test "$HAVE_OPENGL" = "yes"; then
    build_actor_plugins="$build_actor_plugins madspin"
  else
    AC_MSG_WARN([*** No OpenGL found.
		Madspin plugin will not be built.])
  fi
fi

dnl Pseudotoad Flower
AC_ARG_ENABLE([flower],
  AS_HELP_STRING([--disable-flower],
    [Do not build Pseudotoad Flower plugin @<:@default=enabled@:>@]),
  [ENABLE_FLOWER=$enableval],
  [ENABLE_FLOWER=yes])

if test "$ENABLE_FLOWER" = "yes"; then
  if test "$HAVE_OPENGL" = "yes"; then
    build_actor_plugins="$build_actor_plugins pseudotoad_flower"
  else
    AC_MSG_WARN([*** No OpenGL found.
		Pseudotoad Flower plugin will not be built.])
  fi
fi

dnl Infinite
AC_ARG_ENABLE([infinite],
  AS_HELP_STRING([--disable-infinite],
    [Do not build Infinite plugin @<:@default=enabled@:>@]),
  [ENABLE_INFINITE=$enableval],
  [ENABLE_INFINITE=yes])

if test "$ENABLE_INFINITE" = "yes"; then
  build_actor_plugins="$build_actor_plugins infinite"
fi

dnl JESS
AC_ARG_ENABLE([jess],
  AS_HELP_STRING([--disable-jess],
    [Do not build JESS plugin @<:@default=enabled@:>@]),
  [ENABLE_JESS=$enableval],
  [ENABLE_JESS=yes])

if test "$ENABLE_JESS" = "yes"; then
  build_actor_plugins="$build_actor_plugins JESS"
fi

dnl Corona
AC_ARG_ENABLE([corona],
  AS_HELP_STRING([--disable-corona],
    [Do not build corona plugin @<:@default=enabled@:>@]),
  [ENABLE_CORONA=$enableval],
  [ENABLE_CORONA=yes])

if test "$ENABLE_CORONA" = "yes"; then
  build_actor_plugins="$build_actor_plugins corona"
fi

dnl Analyzer
AC_ARG_ENABLE([analyzer],
  AS_HELP_STRING([--disable-analyzer],
    [Do not build Analyzer plugin @<:@default=enabled@:>@]),
  [ENABLE_ANALYZER=$enableval],
  [ENABLE_ANALYZER=yes])

if test "$ENABLE_ANALYZER" = xyes; then
  build_actor_plugins="$build_actor_plugins lv_analyzer"
fi

dnl Scope
AC_ARG_ENABLE([scope],
  AS_HELP_STRING([--disable-scope],
    [Do not build Scope plugin @<:@default=enabled@:>@]),
  [ENABLE_SCOPE=$enableval],
  [ENABLE_SCOPE=yes])

if test "$ENABLE_SCOPE" = "yes"; then
  build_actor_plugins="$build_actor_plugins lv_scope"
fi

dnl Oinksie
AC_ARG_ENABLE([oinksie],
  AS_HELP_STRING([--disable-oinksie],
    [Do not build Oinksie plugin @<:@default=enabled@:>@]),
  [ENABLE_OINKSIE=$enableval],
  [ENABLE_OINKSIE=yes])

if test "$ENABLE_OINKSIE" = "yes"; then
  build_actor_plugins="$build_actor_plugins oinksie"
fi

dnl Jakdaw
AC_ARG_ENABLE([jakdaw],
  AS_HELP_STRING([--disable-jakdaw],
    [Do not build Jakdaw plugin @<:@default=enabled@:>@]),
  [ENABLE_JAKDAW=$enableval],
  [ENABLE_JAKDAW=yes])

if test "$ENABLE_JAKDAW" = "yes"; then
  build_actor_plugins="$build_actor_plugins jakdaw"
fi

dnl Bumpscope
AC_ARG_ENABLE([bumpscope],
  AS_HELP_STRING([--disable-bumpscope],
    [Do not build bumpscope plugin @<:@default=enabled@:>@]),
  [ENABLE_BUMPSCOPE=$enableval],
  [ENABLE_BUMPSCOPE=yes])

if test "$ENABLE_BUMPSCOPE" = "yes"; then
  build_actor_plugins="$build_actor_plugins bumpscope"
fi

dnl G-Force
AC_ARG_ENABLE([gforce],
  AS_HELP_STRING([--disable-gforce],
    [Do not build the G-Force plugin @<:@default=enabled@:>@]),
    [ENABLE_GFORCE=$enableval],
    [ENABLE_GFORCE=yes])

if test "$ENABLE_GFORCE" = "yes"; then
  build_actor_plugins="$build_actor_plugins G-Force"
  
  # Special flags for G-Force, make this only for G-Force FIXME FIXME!!! 
  GFORCE_CFLAGS="-DUNIX_X -D_REENTRANT"
  AC_SUBST([GFORCE_FLAGS])
fi

dnl Alphabend
AC_ARG_ENABLE([alphablend],
  AS_HELP_STRING([--disable-alphablend],
    [Do not build alphablend morph plugin @<:@default=enabled@:>@]),
  [ENABLE_ALPHABLEND=$enableval],
  [ENABLE_ALPHABLEND=yes])

if test "$ENABLE_ALPHABLEND" = "yes"; then
  build_morph_plugins="$build_morph_plugins alphablend"
fi

dnl Flash
AC_ARG_ENABLE([flash],
  AS_HELP_STRING([--disable-flash],
    [Do not build flash morph plugin @<:@default=enabled@:>@]),
  [ENABLE_FLASH=$enableval],
  [ENABLE_FLASH=yes])

if test "$ENABLE_FLASH" = "yes"; then
  build_morph_plugins="$build_morph_plugins flash"
fi

dnl Tentacle
AC_ARG_ENABLE([tentacle],
  AS_HELP_STRING([--disable-tentacle],
    [Do not build tentacle morph plugin @<:@default=enabled@:>@]),
  [ENABLE_TENTACLE=$enableval],
  [ENABLE_TENTACLE=yes])

if test "$ENABLE_TENTACLE" = "yes"; then
  build_morph_plugins="$build_morph_plugins tentacle"
fi

dnl Slide
AC_ARG_ENABLE([slide],
  AS_HELP_STRING([--disable-slide],
     [Do not build slide morph plugin @<:@default=enabled@:>@]),
  [ENABLE_SLIDE=$enableval],
  [ENABLE_SLIDE=yes])

if test "$ENABLE_SLIDE" = "yes"; then
  build_morph_plugins="$build_morph_plugins slide"
fi

dnl Build parameters

DEBUG_CFLAGS=

dnl Profiling
AC_ARG_ENABLE([profiling],
  AS_HELP_STRING([--enable-profiling],
		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
		[ENABLE_PROFILING=$enableval],
		[ENABLE_PROFILING=no])

if test "$ENABLE_PROFILING" = "yes"; then
  DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
fi

dnl Debug build
AC_ARG_ENABLE([debug],
  AS_HELP_STRING([--enable-debug],
    [Enable debug @<:@default=disabled@:>@]),
  [ENABLE_DEBUG=$enableval],
  [ENABLE_DEBUG=no])

if test "$ENABLE_DEBUG" = "yes"; then
  DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall -Wmissing-braces -Wimplicit -Wunused"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-prototypes -Wno-unused-variable"
fi
AC_SUBST(DEBUG_CFLAGS)

dnl Optimization
AC_ARG_ENABLE([extra-optimization],
  AS_HELP_STRING([--enable-extra-optimization],
    [Enable extra optimizations @<:@default=disabled@:>@]),
    [ENABLE_EXTRA_OPT=$enableval],
    [ENABLE_EXTRA_OPT=no])

OPT_CFLAGS=
if test "$ENABLE_EXTRA_OPT" = "yes"; then
  OPT_CFLAGS="-O3 -fexpensive-optimizations"
else
  if test "$debug" = "yes"; then
    OPT_CFLAGS="-O0"
  fi
fi
AC_SUBST(OPT_CFLAGS)

AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${GFORCE_CFLAGS}")
AC_SUBST(CXXFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${GFORCE_CFLAGS}")

dnl Plugin installation paths

AC_ARG_WITH([plugins-base-dir],
  AS_HELP_STRING([--with-plugins-base-dir=DIR],
    [Install plugins to this base directory. Defaults to Libvisual's base dir]),
  [plugins_base_dir=$withval],
  [plugins_base_dir=`$PKG_CONFIG --variable=pluginsbasedir $LIBVISUAL_PKG`])

LIBVISUAL_PLUGINS_BASE_DIR="$plugins_base_dir"
AC_SUBST([LIBVISUAL_PLUGINS_BASE_DIR])
AC_SUBST([actor_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/actor])
AC_SUBST([input_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/input])
AC_SUBST([morph_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/morph])

AC_SUBST(LIBVISUAL_PLUGINS_DATA_DIR, [$datadir/libvisual-plugins-lv_plugins_version_suffix])

dnl Plugins to build
AC_SUBST([build_actor_plugins])
AC_SUBST([build_input_plugins])
AC_SUBST([build_morph_plugins])

AC_CONFIG_FILES([
	Makefile
	po/Makevars
	po/Makefile.in
	plugins/Makefile
	plugins/actor/Makefile
	plugins/actor/bumpscope/Makefile
	plugins/actor/corona/Makefile
	plugins/actor/gdkpixbuf/Makefile
	plugins/actor/gstreamer/Makefile
	plugins/actor/infinite/Makefile
	plugins/actor/jakdaw/Makefile
	plugins/actor/JESS/Makefile
	plugins/actor/lv_analyzer/Makefile
	plugins/actor/lv_gltest/Makefile
	plugins/actor/lv_scope/Makefile
	plugins/actor/madspin/Makefile
	plugins/actor/nastyfft/Makefile
	plugins/actor/oinksie/Makefile
	plugins/actor/pseudotoad_flower/Makefile
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
	plugins/input/debug/Makefile
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
libesd                   : $HAVE_ESD
ALSA                     : $HAVE_ALSA
OpenGL                   : $HAVE_OPENGL
GTK+                     : $HAVE_GTK

Plugins being build
-------------------
Input plugins            :${build_input_plugins}
Actor plugins            :${build_actor_plugins}
Morph plugins            :${build_morph_plugins}

Libvisual plugin data dir: ${prefix}/share

Build options
-------------
install path             : $plugins_base_dir
source code location     : ${srcdir}
compiler                 : ${CC}

Compiler options
----------------
Extra optimization       : $ENABLE_EXTRA_OPT
Debug build              : $ENABLE_DEBUG
Profiling support        : $ENABLE_PROFILING
"
