cmake_minimum_required(VERSION 2.6)

# PROJECT SETTINGS
project(libvisual)

set(LIBRARY_NAME "Libvisual plugins")
set(LIBRARY_BUGREPORT "synap@nerds-incorporated.org")
set(LV_PLUGINS_VERSION_MAJOR 0)
set(LV_PLUGINS_VERSION_MINOR 4)
set(LV_PLUGINS_VERSION_MICRO 99)
set(LV_PLUGINS_VERSION "${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR}.${LV_PLUGINS_VERSION_MICRO}")
set(LIBRARY_SOVERSION "0")
string(REGEX REPLACE "\\$[a-zA-Z]+: ([0-9]+) \\$" "\\1" GENERIC_SVN_VERSION "$Revision: 0 $")

# TODO: add to README/wiki or something
# commandline options that might be of interest:
# -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_FLAGS="-Wall -g -O0" -DCMAKE_VERBOSE_MAKEFILE=True

# BUILD OPTIONS
option(UNIT_TESTING "Build with unit tests" ON)
option(ENABLE_NLS "Translation of program messages to the user's native language" ON)

# DEPENDENCIES
include(FindThreads)

if(NOT Threads_FOUND)
  message (FATAL_ERROR "Couldn't find a suitable threading library.")
endif(NOT Threads_FOUND)

include(FindPkgConfig)

pkg_search_module(LIBVISUAL REQUIRED  libvisual==${LV_PLUGINS_VERSION})

# TODO: move plugin-specific checks into plugin's CMakeLists.txt
# TODO: check REQUIRED
pkg_search_module(ESOUND # REQUIRED
  esound>=0.2.28)
pkg_search_module(JACK # REQUIRED
  jack>=0.98.0)
pkg_search_module(GTK # REQUIRED
  gtk>=2.0)
pkg_search_module(GST # REQUIRED
  gst>=0.8)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_BINARY_DIR}/
    ${LIBVISUAL_INCLUDE_DIRS}
)

# List of plugins to build
set(build_input_plugins )
set(build_actor_plugins )
set(build_morph_plugins )


if(NOT ESOUND_FOUND)
  # TODO
#   build_input_plugins="$build_input_plugins esd"
# else
#   AC_MSG_WARN([*** EsounD not found or too old. The EsounD input plugin won't be built])
endif(NOT ESOUND_FOUND)

if(NOT JACK_FOUND)
# if test "$HAVE_JACK" = "yes"; then
#   build_input_plugins="$build_input_plugins jack"
# else
#   AC_MSG_WARN([*** libjack is too old. You can download a newer version at
#              http://jackit.sf.net/. The jackit input plugin won't be built])
# fi
endif(NOT JACK_FOUND)


if(ENABLE_NLS)
  include(FindGettext)            # defines var GETTEXT_FOUND

  if(NOT GETTEXT_FOUND)
    message (FATAL_ERROR "Couldn't find gettext.")
  endif(NOT GETTEXT_FOUND)

  message (STATUS "Found gettext for NLS: ${GETTEXT_FOUND}")
  set(HAVE_GETTEXT "1")
endif(ENABLE_NLS)

# from old libvisual-buildsystems
# TODO: review

set (LIBVISUAL_VERSION "${LIBRARY_VERSION}")
set (LV_PLUGINS_VERSION_SUFFIX "${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR}")

set(LIBVISUAL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/libvisual")
set(LIBVISUAL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(LIBVISUAL_PLUGINS_BASE_DIR "${LIBVISUAL_LIB_DIR}/libvisual-${LIBVISUAL_VERSION_SUFFIX}")

# Internationalization
set(GETTEXT_PACKAGE "libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")
set(LOCALEDIR "${CMAKE_INSTALL_PREFIX}/share/locale") # String catalog location
# TODO: GETTEXT_CREATE_TRANSLATIONS ( outputFile [ALL] file1 ... fileN )

ADD_DEFINITIONS(-DLINUX)

# TODO write a macro for this kind of stuff, which also adds these defines to config.h
include(CheckIncludeFiles)
check_include_files(fcntl.h HAVE_FCNTL_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(sched.h HAVE_SCHED_H)
check_include_files(sys/sched.h HAVE_SYS_SCHED_H)
check_include_files(sys/socket.h HAVE_SYS_SOCKET_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)
check_include_files(stddef.h HAVE_STDDEF_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(strings.h HAVE_STRINGS_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/dir.h HAVE_SYS_DIR_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)

set(LV_HAVE_STDINT_H ${HAVE_STDINT_H})
set(LV_HAVE_INTTYPES_H ${HAVE_INTTYPES_H})

include(CheckFunctionExists)
check_function_exists(select HAVE_SELECT)
check_function_exists(usleep HAVE_USLEEP)
check_function_exists(nanosleep HAVE_NANOSLEEP)

check_function_exists(iconv HAVE_ICONV)

check_function_exists(malloc HAVE_MALLOC)
check_function_exists(realloc HAVE_REALLOC)
check_function_exists(select_argtypes HAVE_SELECT_ARGTYPES)

check_function_exists(floor HAVE_FLOOR) #todo add to config.h
check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(memmove HAVE_MEMMOVE) #todo add to config.h
check_function_exists(pow HAVE_POW) #todo add to config.h
check_function_exists(sqrt HAVE_SQRT)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strndup HAVE_STRNDUP)
check_function_exists(sysconf HAVE_SYSCONF)
check_function_exists(mmap HAVE_MMAP) #todo add to config.h
check_function_exists(munmap HAVE_MUNMAP) #todo add to config.h

check_function_exists(dcgettext HAVE_DCGETTEXT)

# MacOS X
check_function_exists(CFLocaleCopyCurrent HAVE_CFLOCALECOPYCURRENT)
check_function_exists(CFPreferencesCopyAppValue HAVE_CFPREFERENCESCOPYAPPVALUE)

# TODO: define VISUAL_OS_WIN32


# INCLUDE(CheckSymbolExists)
# CHECK_SYMBOL_EXISTS(malloc header HAVE_MALLOC)
# ...

# TODO: add/fix ENABLE_NLS in config file

# AC_CHECK_LIB([dl], [dlsym], [LIBS="-ldl $LIBS"],
# 	     AC_MSG_WARN([*** You need dlsym() to make use of Libvisual.]))
# AC_CHECK_LIB([m], [sqrt], [LIBS="-lm $LIBS"],
# 	     AC_MSG_ERROR([*** You need the Standard C Math Library to build Libvisual.]))

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)
CHECK_TYPE_SIZE(int   SIZEOF_INT)
CHECK_TYPE_SIZE(long  SIZEOF_LONG)
CHECK_TYPE_SIZE("long long"  SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE(__int64  SIZEOF_INT_64)
CHECK_TYPE_SIZE(size_t  SIZEOF_SIZE_T)

set(VISUAL_LIBS "${LIBS} -lvisual-${LIBVISUAL_VERSION_SUFFIX}")

# INSTALLING (static files)
# copy input-file over to build-dir, (limited to replacing @varname@ placeholders via @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  IMMEDIATE @ONLY)

# BUILDING SUBDIRECTORIES
# TODO: Add subdirectories of plugins that may be built
# add_subdirectory(plugins)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# UNIT TESTING; disabled as of now
# if (UNIT_TESTING)
#   SET (MEMORYCHECK_COMMAND "/usr/bin/valgrind")
#   SET (MEMORYCHECK_COMMAND_OPTIONS "-q --tool=memcheck --leak-check=yes --show-reachable=yes --workaround-gcc296-bugs=yes --num-callers=50")

#   include(CTest)
#   add_subdirectory(tests)
# endif (UNIT_TESTING)

# PACKAGING
include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_cpack.cmake")


# things from old autoconf/automake, maybe to be enabled again

# TODO: PORT...

# m4_define([libvisual_pkg], [libvisual-lv_plugins_version_suffix])
# LIBVISUAL_PKG=libvisual_pkg

# dnl Check for typedefs, structures, and compiler characteristics.
# AC_C_CONST
# AC_C_INLINE

# dnl 'Standard' header files.
# AC_HEADER_TIME
# AC_CHECK_HEADERS([fcntl.h unistd.h stdlib.h string.h sys/time.h])

# dnl Lex/Yacc
# AM_PROG_LEX
# AC_CHECK_PROGS([YACC], ['bison -y' byacc yacc])
# if test -z "$YACC"; then
#    AC_MSG_ERROR([Bison or Yacc not found])
# fi

# dnl GdkPixbuf
# AC_ARG_ENABLE([gdkpixbuf-plugin],
#   AS_HELP_STRING([--disable-gdkpixbuf-plugin],
#     [Do not build GdkPixbuf visualization plugin @<:@default=enabled@:>@]),
#   [ENABLE_PLUGIN_GDKPIXBUF=$enableval],
#   [ENABLE_PLUGIN_GDKPIXBUF=yes])

# if test "$ENABLE_PLUGIN_GDKPIXBUF" = "yes"; then
#   PKG_CHECK_MODULES([GTK], [gtk+-2.0 >= gtk_required_version],
#     [HAVE_GTK="yes"], [HAVE_GTK="no"])

#   if test "$HAVE_GTK" = "yes"; then
#     build_actor_plugins="$build_actor_plugins gdkpixbuf"
#   else
#     AC_MSG_WARN([*** GTK+ >= gtk_required_version is not found.
# 	  	The libvisual GdkPixbuf image loader plugin won't be build.
# 		GdkPixbuf is included within gtk-2.0 and newer, which can be
# 		downloaded at http://www.gtk.org/])
#   fi
# fi

# dnl GStreamer
# AC_ARG_ENABLE([gstreamer-plugin],
#   AS_HELP_STRING([--disable-gstreamer-plugin],
#     [Do not build GStreamer visualization plugin @<:@default=enabled@:>@]),
#   [ENABLE_GSTREAMER_PLUGIN=$enableval],
#   [ENABLE_GSTREAMER_PLUGIN=yes])

# if test "$ENABLE_GSTREAMER_PLUGIN" = "yes"; then
#   PKG_CHECK_MODULES([GSTREAMER], [gstreamer-0.8 >= gst_required_version],
#     [HAVE_GSTREAMER="yes"], [HAVE_GSTREAMER="no"])

#   if test "$HAVE_GSTREAMER" = "yes"; then
#     build_actor_plugins="$build_actor_plugins gstreamer"
#   else
#     AC_MSG_WARN([*** GStreamer >= gst_required_version is not found.
# 	  	The libvisual GStreamer viewer plugin won't be build.
# 		GStreamer can be downloaded from http://www.gstreamer.org/])
#   fi
# fi

# dnl Xlib
# AC_PATH_XTRA
# AM_CONDITIONAL(HAVE_X, test "$have_x" = "yes")

# dnl OpenGL, GLX
# HAVE_OPENGL=no

# if test "$have_x" = "yes" ; then
#   CPPFLAGS_SAVE=$CPPFLAGS
#   CPPFLAGS="$CPPFLAGS $X_CFLAGS"

#   AC_CHECK_HEADERS([GL/gl.h GL/glx.h],
#     [AC_CHECK_LIB([GL], [glBegin], [HAVE_OPENGL=yes], [], [$X_LIBS])])

#   CPPFLAGS=$CPPFLAGS_SAVE
# fi

# AM_CONDITIONAL([HAVE_OPENGL], [test "$HAVE_OPENGL" = "yes"])

# dnl XF86VidMode
# HAVE_XF86VMODE=no
# XF86VMODE_LIB=

# CPPFLAGS_SAVE=$CPPFLAGS
# CPPFLAGS="$CPPFLAGS $X_CFLAGS"

# AC_CHECK_HEADERS([X11/extensions/xf86vmode.h],
#   [AC_CHECK_LIB([Xxf86vm], [XF86VidModeSwitchToMode], 
#     [AC_DEFINE([HAVE_XF86VMODE], [], [XF86VidMode is available])
#      XF86VMODE_LIB=-lXxf86vm
#      HAVE_XF86VMODE=yes],
#     [],
#     [$X_LIBS])],
#   [],
#   [#include <X11/Xlib.h>
#   ]
# )

# CPPFLAGS=$CPPFLAGS_SAVE

# AC_SUBST([XF86VMODE_LIB])
# AM_CONDITIONAL([HAVE_XF86VMODE], [test "$HAVE_XF86VMODE" = "yes"])

# dnl x86
# AM_CONDITIONAL(ARCH_X86,test "$ARCH_TYPE" = "i386" || test "$ARCH_TYPE" = "i486" || test "$ARCH_TYPE" =
# "i586" || test "$ARCH_TYPE" = "i686" || test "$ARCH_TYPE" = "i86pc")

# dnl ALSA
# LV_ALSA_MIN_VERSION=0.9.0
# AC_ARG_ENABLE([alsa],
#   AS_HELP_STRING([--disable-alsa],
#     [Do not build ALSA input plugin @<:@default=enabled@:>@]),
#   [ENABLE_ALSA=$enableval],
#   [ENABLE_ALSA=yes])

# if test "$ENABLE_ALSA" = "yes"; then
#   AM_PATH_ALSA([$LV_ALSA_MIN_VERSION], [HAVE_ALSA=yes], [HAVE_ALSA=no])

#   if test "$HAVE_ALSA" = "yes"; then
#     build_input_plugins="$build_input_plugins alsa"
#     AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <stdlib.h>
# 		    #include <alsa/asoundlib.h>
#                    ]], [[#if(SND_LIB_MAJOR == 0)
# 		    exit(0);
#                     #else
# 		    #error ALSA library version 1.x
# 		    #endif
#                    ]])],[ AC_DEFINE([HAVE_0_9_X_ALSA], [1], [ALSA Library is from 0.9 series]) ],[ AC_DEFINE([HAVE_1_X_X_ALSA], [1], [ALSA Library is from 1 series]) ])
#   else
#     AC_MSG_WARN([*** Alsa input plugin cannot be built.
# 		 May be you have an ALSA library older than $LV_ALSA_MIN_VERSION.
# 		 Take a look at {prefix}/include/alsa/version.h file.])
#   fi
# fi

# dnl MPlayer
# AC_ARG_ENABLE([mplayer],
#   AS_HELP_STRING([--disable-mplayer],
#     [Do not build mplayer input plugin @<:@default=enabled@:>@]),
#   [ENABLE_MPLAYER=$enableval],
#   [ENABLE_MPLAYER=yes])

# # FIXME: Missing check for MPlayer...
# if test "$ENABLE_MPLAYER" = "yes"; then
#   # FIXME we must check also for HAVE_MMAP here
#   if test "$lv_have_munmap" = "yes"; then
#     build_input_plugins="$build_input_plugins mplayer"
#   else
#     AC_MSG_WARN([*** There is not a working munmap function available.
# 		mplayer input plugin will not be built.])
#   fi
# fi

# dnl Xmms2
# AC_ARG_ENABLE([xmms2],
#   AS_HELP_STRING([--disable-xmms2],
#     [Do not build xmms2 input plugin @<:@default=enabled@:>@]),
#   [ENABLE_XMMS2=$enableval],
#   [ENABLE_XMMS2=yes])

# if test "$ENABLE_XMMS2" = "yes"; then
#   PKG_CHECK_MODULES(XMMS2, xmms2-client >= 0.6, [HAVE_XMMS2=yes], [HAVE_XMMS2=no])
#   if test "$HAVE_XMMS2" = "yes"; then
#     AC_SUBST(XMMS2_CFLAGS)
#     AC_SUBST(XMMS2_LIBS)
#     build_input_plugins="$build_input_plugins xmms2"
#   else
#     AC_MSG_WARN([*** xmms2 not found. Plugin will not be built.])
#   fi
# fi

# dnl Input debug plugin
# AC_ARG_ENABLE([inputdebug],
#   AS_HELP_STRING([--disable-inputdebug],
#     [Do not build input debug plugin @<:@default=enabled@:>@]),
#   [ENABLE_INPUTDEBUG=$enableval],
#   [ENABLE_INPUTDEBUG=yes])

# if test "$inputdebug" = "yes"; then
#   build_input_plugins="$build_input_plugins debug"
# fi

# dnl OpenGL test plugin
# AC_ARG_ENABLE([gltest],
#   AS_HELP_STRING([--disable-gltest],
#     [Do not build lv_gltest plugin @<:@default=enabled@:>@]),
#   [ENABLE_LVGLTEST=$enableval],
#   [ENABLE_LVGLTEST=yes])

# if test "$ENABLE_LVGLTEST" = "yes"; then
#   if test "$HAVE_OPENGL" = "yes"; then
#     build_actor_plugins="$build_actor_plugins lv_gltest"
#   else
#     AC_MSG_WARN([*** No OpenGL found.
# 		lv_gltest plugin will not be built.])
#   fi
# fi

# dnl NastyFFT
# AC_ARG_ENABLE([nastyfft],
#   AS_HELP_STRING([--disable-nastyfft],
#     [Do not build nastyfft plugin @<:@default=enabled@:>@]),
#   [ENABLE_NASTYFFT=$enableval],
#   [ENABLE_NASTYFFT=yes])

# if test "$ENABLE_NASTYFFT" = "yes"; then
#   if test "$HAVE_OPENGL" = "yes"; then
#     build_actor_plugins="$build_actor_plugins nastyfft"
#   else
#     AC_MSG_WARN([*** No OpenGL found.
# 		nastyfft plugin will not be built.])
#   fi
# fi

# dnl Madspin
# AC_ARG_ENABLE([madspin],
#   AS_HELP_STRING([--disable-madspin],
#     [Do not build Madspin plugin @<:@default=enabled@:>@]),
#   [ENABLE_MADSPIN=$enableval],
#   [ENABLE_MADSPIN=yes])

# if test "$ENABLE_MADSPIN" = "yes"; then
#   if test "$HAVE_OPENGL" = "yes"; then
#     build_actor_plugins="$build_actor_plugins madspin"
#   else
#     AC_MSG_WARN([*** No OpenGL found.
# 		Madspin plugin will not be built.])
#   fi
# fi

# dnl Dancing Particles
# AC_ARG_ENABLE([dancingparticles],
#   AS_HELP_STRING([--disable-dancingparticles],
#     [Do not build Dancing Particles plugin @<:@default=enabled@:>@]),
#   [ENABLE_DANCINGPARTICLES=$enableval],
#   [ENABLE_DANCINGPARTICLES=yes])

# if test "$ENABLE_DANCINGPARTICLES" = "yes"; then
#   if test "$HAVE_OPENGL" = "yes"; then
#     build_actor_plugins="$build_actor_plugins dancingparticles"
#   else
#     AC_MSG_WARN([*** No OpenGL found.
# 		Dancing Particles plugin will not be built.])
#   fi
# fi

# dnl Pseudotoad Flower
# AC_ARG_ENABLE([flower],
#   AS_HELP_STRING([--disable-flower],
#     [Do not build Pseudotoad Flower plugin @<:@default=enabled@:>@]),
#   [ENABLE_FLOWER=$enableval],
#   [ENABLE_FLOWER=yes])

# if test "$ENABLE_FLOWER" = "yes"; then
#   if test "$HAVE_OPENGL" = "yes"; then
#     build_actor_plugins="$build_actor_plugins pseudotoad_flower"
#   else
#     AC_MSG_WARN([*** No OpenGL found.
# 		Pseudotoad Flower plugin will not be built.])
#   fi
# fi

# dnl Nebulus
# AC_ARG_ENABLE([nebulus],
#   AS_HELP_STRING([--disable-nebulus],
#     [Do not build Nebulus plugin @<:@default=enabled@:>@]),
#   [ENABLE_NEBULUS=$enableval],
#   [ENABLE_NEBULUS=yes])

# if test "$ENABLE_NEBULUS" = "yes"; then
#   if test "$HAVE_OPENGL" = "yes"; then
#     build_actor_plugins="$build_actor_plugins nebulus"
#   else
#     AC_MSG_WARN([*** No OpenGL found.
# 		Nebulus plugin will not be built.])
#   fi
# fi

# dnl Infinite
# AC_ARG_ENABLE([infinite],
#   AS_HELP_STRING([--disable-infinite],
#     [Do not build Infinite plugin @<:@default=enabled@:>@]),
#   [ENABLE_INFINITE=$enableval],
#   [ENABLE_INFINITE=yes])

# if test "$ENABLE_INFINITE" = "yes"; then
#   build_actor_plugins="$build_actor_plugins infinite"
# fi

# dnl JESS
# AC_ARG_ENABLE([jess],
#   AS_HELP_STRING([--disable-jess],
#     [Do not build JESS plugin @<:@default=enabled@:>@]),
#   [ENABLE_JESS=$enableval],
#   [ENABLE_JESS=yes])

# if test "$ENABLE_JESS" = "yes"; then
#   build_actor_plugins="$build_actor_plugins JESS"
# fi

# dnl Corona
# AC_ARG_ENABLE([corona],
#   AS_HELP_STRING([--disable-corona],
#     [Do not build corona plugin @<:@default=enabled@:>@]),
#   [ENABLE_CORONA=$enableval],
#   [ENABLE_CORONA=yes])

# if test "$ENABLE_CORONA" = "yes"; then
#   build_actor_plugins="$build_actor_plugins corona"
# fi

# dnl Analyzer
# AC_ARG_ENABLE([analyzer],
#   AS_HELP_STRING([--disable-analyzer],
#     [Do not build Analyzer plugin @<:@default=enabled@:>@]),
#   [ENABLE_ANALYZER=$enableval],
#   [ENABLE_ANALYZER=yes])

# if test "$ENABLE_ANALYZER" = xyes; then
#   build_actor_plugins="$build_actor_plugins lv_analyzer"
# fi

# dnl Finespectrum
# AC_ARG_ENABLE([finespectrum],
#   AS_HELP_STRING([--disable-finespectrum],
#     [Do not build Finespectrum plugin @<:@default=enabled@:>@]),
#   [ENABLE_FINESPECTRUM=$enableval],
#   [ENABLE_FINESPECTRUM=yes])

# if test "$ENABLE_FINESPECTRUM" = "yes"; then
#   build_actor_plugins="$build_actor_plugins finespectrum"
# fi

# dnl Scope
# AC_ARG_ENABLE([scope],
#   AS_HELP_STRING([--disable-scope],
#     [Do not build Scope plugin @<:@default=enabled@:>@]),
#   [ENABLE_SCOPE=$enableval],
#   [ENABLE_SCOPE=yes])

# if test "$ENABLE_SCOPE" = "yes"; then
#   build_actor_plugins="$build_actor_plugins lv_scope"
# fi

# dnl Oinksie
# AC_ARG_ENABLE([oinksie],
#   AS_HELP_STRING([--disable-oinksie],
#     [Do not build Oinksie plugin @<:@default=enabled@:>@]),
#   [ENABLE_OINKSIE=$enableval],
#   [ENABLE_OINKSIE=yes])

# if test "$ENABLE_OINKSIE" = "yes"; then
#   build_actor_plugins="$build_actor_plugins oinksie"
# fi

# dnl Jakdaw
# AC_ARG_ENABLE([jakdaw],
#   AS_HELP_STRING([--disable-jakdaw],
#     [Do not build Jakdaw plugin @<:@default=enabled@:>@]),
#   [ENABLE_JAKDAW=$enableval],
#   [ENABLE_JAKDAW=yes])

# if test "$ENABLE_JAKDAW" = "yes"; then
#   build_actor_plugins="$build_actor_plugins jakdaw"
# fi

# dnl Plazma
# AC_ARG_ENABLE([plazma],
#   AS_HELP_STRING([--disable-plazma],
#     [Do not build plazma plugin @<:@default=enabled@:>@]),
#   [ENABLE_PLAZMA=$enableval],
#   [ENABLE_PLAZMA=yes])

# if test "$ENABLE_PLAZMA" = "yes"; then
#   build_actor_plugins="$build_actor_plugins plazma"
# fi

# dnl Bumpscope
# AC_ARG_ENABLE([bumpscope],
#   AS_HELP_STRING([--disable-bumpscope],
#     [Do not build bumpscope plugin @<:@default=enabled@:>@]),
#   [ENABLE_BUMPSCOPE=$enableval],
#   [ENABLE_BUMPSCOPE=yes])

# if test "$ENABLE_BUMPSCOPE" = "yes"; then
#   build_actor_plugins="$build_actor_plugins bumpscope"
# fi

# dnl G-Force
# AC_ARG_ENABLE([gforce],
#   AS_HELP_STRING([--disable-gforce],
#     [Do not build the G-Force plugin @<:@default=enabled@:>@]),
#     [ENABLE_GFORCE=$enableval],
#     [ENABLE_GFORCE=yes])

# if test "$ENABLE_GFORCE" = "yes"; then
#   build_actor_plugins="$build_actor_plugins G-Force"
  
#   # Special flags for G-Force, make this only for G-Force FIXME FIXME!!! 
#   GFORCE_CFLAGS="-DUNIX_X -D_REENTRANT"
#   AC_SUBST([GFORCE_FLAGS])
# fi

# dnl goom2k4
# AC_ARG_ENABLE([goom2k4],
#   AS_HELP_STRING([--disable-goom2k4],
#     [Do not build goom2k4 plugin @<:@default=enabled@:>@]),
#     [ENABLE_GOOM2K4=$enableval],
#     [ENABLE_GOOM2K4=yes])

# if test "$ENABLE_GOOM2K4" = "yes"; then
#   build_actor_plugins="$build_actor_plugins goom2k4"
# fi

# dnl blursk
# AC_ARG_ENABLE([blursk],
#   AS_HELP_STRING([--disable-blursk],
#   [Do not build blursk plugin @<:@default=enabled@:>@]),
#   [ENABLE_BLURSK=$enableval],
#   [ENABLE_BLURSK=yes])

# if test "$ENABLE_BLURSK" = "yes"; then
#   build_actor_plugins="$build_actor_plugins blursk"
# fi

# dnl Alphabend
# AC_ARG_ENABLE([alphablend],
#   AS_HELP_STRING([--disable-alphablend],
#     [Do not build alphablend morph plugin @<:@default=enabled@:>@]),
#   [ENABLE_ALPHABLEND=$enableval],
#   [ENABLE_ALPHABLEND=yes])

# if test "$ENABLE_ALPHABLEND" = "yes"; then
#   build_morph_plugins="$build_morph_plugins alphablend"
# fi

# dnl Flash
# AC_ARG_ENABLE([flash],
#   AS_HELP_STRING([--disable-flash],
#     [Do not build flash morph plugin @<:@default=enabled@:>@]),
#   [ENABLE_FLASH=$enableval],
#   [ENABLE_FLASH=yes])

# if test "$ENABLE_FLASH" = "yes"; then
#   build_morph_plugins="$build_morph_plugins flash"
# fi

# dnl Tentacle
# AC_ARG_ENABLE([tentacle],
#   AS_HELP_STRING([--disable-tentacle],
#     [Do not build tentacle morph plugin @<:@default=enabled@:>@]),
#   [ENABLE_TENTACLE=$enableval],
#   [ENABLE_TENTACLE=yes])

# if test "$ENABLE_TENTACLE" = "yes"; then
#   build_morph_plugins="$build_morph_plugins tentacle"
# fi

# dnl Slide
# AC_ARG_ENABLE([slide],
#   AS_HELP_STRING([--disable-slide],
#      [Do not build slide morph plugin @<:@default=enabled@:>@]),
#   [ENABLE_SLIDE=$enableval],
#   [ENABLE_SLIDE=yes])

# if test "$ENABLE_SLIDE" = "yes"; then
#   build_morph_plugins="$build_morph_plugins slide"
# fi

# dnl Build parameters

# DEBUG_CFLAGS=

# dnl Profiling
# AC_ARG_ENABLE([profiling],
#   AS_HELP_STRING([--enable-profiling],
# 		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
# 		[ENABLE_PROFILING=$enableval],
# 		[ENABLE_PROFILING=no])

# if test "$ENABLE_PROFILING" = "yes"; then
#   DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
# fi

# dnl Debug build
# AC_ARG_ENABLE([debug],
#   AS_HELP_STRING([--enable-debug],
#     [Enable debug @<:@default=disabled@:>@]),
#   [ENABLE_DEBUG=$enableval],
#   [ENABLE_DEBUG=no])

# if test "$ENABLE_DEBUG" = "yes"; then
#   DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3"
#   DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall -Wmissing-braces -Wimplicit -Wunused"
#   DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-prototypes -Wno-unused-variable"
# fi
# AC_SUBST(DEBUG_CFLAGS)

# dnl Optimization
# AC_ARG_ENABLE([extra-optimization],
#   AS_HELP_STRING([--enable-extra-optimization],
#     [Enable extra optimizations @<:@default=disabled@:>@]),
#     [ENABLE_EXTRA_OPT=$enableval],
#     [ENABLE_EXTRA_OPT=no])

# OPT_CFLAGS=
# if test "$ENABLE_EXTRA_OPT" = "yes"; then
#   OPT_CFLAGS="-O3 -fexpensive-optimizations"
# else
#   if test "$debug" = "yes"; then
#     OPT_CFLAGS="-O0"
#   fi
# fi
# AC_SUBST(OPT_CFLAGS)

# AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${GFORCE_CFLAGS}")
# AC_SUBST(CXXFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${GFORCE_CFLAGS}")

# dnl Plugin installation paths

# AC_ARG_WITH([plugins-base-dir],
#   AS_HELP_STRING([--with-plugins-base-dir=DIR],
#     [Install plugins to this base directory. Defaults to Libvisual's base dir]),
#   [plugins_base_dir=$withval],
#   [plugins_base_dir=`$PKG_CONFIG --variable=pluginsbasedir $LIBVISUAL_PKG`])

# LIBVISUAL_PLUGINS_BASE_DIR="$plugins_base_dir"
# AC_SUBST([LIBVISUAL_PLUGINS_BASE_DIR])
# AC_SUBST([actor_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/actor])
# AC_SUBST([input_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/input])
# AC_SUBST([morph_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/morph])

# AC_SUBST(LIBVISUAL_PLUGINS_DATA_DIR, [$datadir/libvisual-plugins-lv_plugins_version_suffix])


# AC_OUTPUT

# echo "
# ================================================
# LIBVISUAL-PLUGINS, PLUGINS FOR LIBVISUAL $VERSION
# ================================================

# Packages found:
# libesd                   : $HAVE_ESD
# ALSA                     : $HAVE_ALSA
# OpenGL                   : $HAVE_OPENGL
# GTK+                     : $HAVE_GTK

# Plugins being build
# -------------------
# Input plugins            :${build_input_plugins}
# Actor plugins            :${build_actor_plugins}
# Morph plugins            :${build_morph_plugins}

# Libvisual plugin data dir: ${prefix}/share

# Build options
# -------------
# install path             : $plugins_base_dir
# source code location     : ${srcdir}
# compiler                 : ${CC}

# Compiler options
# ----------------
# Extra optimization       : $ENABLE_EXTRA_OPT
# Debug build              : $ENABLE_DEBUG
# Profiling support        : $ENABLE_PROFILING
# "
