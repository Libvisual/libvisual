CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(libvisual-plugins)

SET(LV_PLUGINS_VERSION_MAJOR  0)
SET(LV_PLUGINS_VERSION_MINOR  5)
SET(LV_PLUGINS_VERSION_MICRO  0)
SET(LV_PLUGINS_VERSION        ${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR}.${LV_PLUGINS_VERSION_MICRO})
SET(LV_PLUGINS_VERSION_SUFFIX ${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR})

SET(PACKAGE           "libvisual-plugins")
SET(PACKAGE_VERSION   "${LV_PLUGINS_VERSION}")
SET(PACKAGE_BUGREPORT "http://libvisual.sf.net")
SET(PACKAGE_NAME      "${PACKAGE}")
SET(PACKAGE_STRING    "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_TARNAME   "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_URL       "http://libvisual.sf.net")

# Installation Paths
SET(EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
SET(DATA_DIR    "${CMAKE_INSTALL_PREFIX}/share")
SET(LOCALE_DIR  "${DATA_DIR}/locale")
SET(LV_PLUGINS_DATA_DIR "${DATA_DIR}/libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")

SET(LIBVISUAL_REQUIRED_VERSION ${LV_PLUGINS_VERSION})
SET(ESOUND_REQUIRED_VERSION 0.2.28)
SET(JACK_REQUIRED_VERSION 0.98.0)
SET(GTK_REQUIRED_VERSION 2.0)
SET(GST_REQUIRED_VERSION 0.8)

SET(LIBVISUAL_PKG "libvisual-${LV_PLUGINS_VERSION_SUFFIX}")

# Check for pkg-config
FIND_PACKAGE(PkgConfig REQUIRED)

# Check for libvisual here so that users get told they have the
# wrong dependencies as early in the process as possible.
PKG_CHECK_MODULES(LIBVISUAL ${LIBVISUAL_PKG}>=${LIBVISUAL_REQUIRED_VERSION} REQUIRED)

# Check for typedefs, structures, and compiler characteristics.
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckCCompiler)
CHECK_C_COMPILER_SUPPORTS_CONST(HAVE_C_CONST)
CHECK_C_COMPILER_SUPPORTS_INLINE(INLINE HAVE_C_INLINE)

# 'Standard' header files.
INCLUDE(CheckStdCHeaders)
IF(NOT STDC_HEADERS)
  MESSAGE(FATAL ERROR "Libvisual-plugins requires standard C library headers to build")
ENDIF(NOT STDC_HEADERS)

# Library functions.
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(floor HAVE_FLOOR)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(pow HAVE_POW)
CHECK_FUNCTION_EXISTS(sqrt HAVE_SQRT)
CHECK_FUNCTION_EXISTS(munmap HAVE_MUNMAP)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)
#AC_FUNC_MMAP

# Assembly
#AM_PROG_AS

# Lex/Yacc
FIND_PACKAGE(FLEX REQUIRED)
FIND_PACKAGE(BISON REQUIRED)

# Internationalization
SET(GETTEXT_PACKAGE "libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")

# Dynamic linking library
SET(DYNAMIC_LOADER_LIBS "dl")

# Libraries

# EsounD
OPTION(ENABLE_ESD "Build esound input plugin (default=enabled)" yes)
IF(ENABLE_ESD)
  PKG_CHECK_MODULES(ESD esound>=${ESOUND_REQUIRED_VERSION})
  IF(NOT ESD_FOUND)
    MESSAGE(WARNING "Esound >= ${ESOUND_REQUIRED_VERSION} not found.")
    SET(ENABLE_ESD no)
  ENDIF(NOT ESD_FOUND)
ENDIF(ENABLE_ESD)

# JACK
OPTION(ENABLE_JACK "Build jack input plugin (default=enabled)" yes)
IF(ENABLE_JACK)
  PKG_CHECK_MODULES(JACK jack>=${JACK_REQUIRED_VERSION})
  IF(NOT JACK_FOUND)
    MESSAGE(WARNING "JACK >= ${JACK_REQUIRED_VERSION} not found.")
    SET(ENABLE_JACK no)
  ENDIF(NOT JACK_FOUND)
ENDIF(ENABLE_JACK)

# GdkPixbuf
OPTION(ENABLE_GDKPIXBUF "Build GdkPixbuf visualization plugin (default=enabled)" yes)
IF(ENABLE_GDKPIXBUF)
  PKG_CHECK_MODULES(GTK gtk+-2.0>=${GTK_REQUIRED_VERSION})
  IF(NOT GTK_FOUND)
    MESSAGE(WARNING "GTK+ >= ${GTK_REQUIRED_VERSION} is not found. The GdkPixbuf image loader plugin won't be built. GdkPixbuf is included within gtk-2.0 and newer, which can be downloaded at http://www.gtk.org/")
    SET(ENABLE_GDKPIXBUF no)
  ENDIF(NOT GTK_FOUND)
ENDIF(ENABLE_GDKPIXBUF)

# GStreamer
OPTION(ENABLE_GSTREAMER "Build GStreamer visualization plugin (default=enabled)" yes)
IF(ENABLE_GSTREAMER)
  PKG_CHECK_MODULES(GSTREAMER gstreamer-0.8>=${GST_REQUIRED_VERSION})
  IF(NOT GSTREAMER_FOUND)
    MESSAGE(WARNING "GStreamer >= ${GST_REQUIRED_VERSION} is not found. The GStreamer viewer plugin won't be built. GStreamer can be downloaded from http://www.gstreamer.org/")
    SET(ENABLE_GSTREAMER no)
  ENDIF(NOT GSTREAMER_FOUND)
ENDIF(ENABLE_GSTREAMER)

FIND_PACKAGE(X11)
SET(HAVE_X ${X11_FOUND})

FIND_PACKAGE(OpenGL)
SET(HAVE_OPENGL ${OPENGL_FOUND})
SET(HAVE_XF86VMODE ${X11_xf86vmode_FOUND})

# x86
IF(CMAKE_SYSTEM_PROCESSOR MATCHES "^(i.86|x86|x86_64)$")
  SET(ARCH_X86 yes)
ENDIF()

# ALSA
SET(LV_ALSA_MIN_VERSION 0.9.0)
OPTION(ENABLE_ALSA "Build the ALSA input plugin (default=enabled)" yes)
IF(ENABLE_ALSA)
  FIND_PACKAGE(ALSA)
  IF(NOT ALSA_FOUND)
    MESSAGE(WARNING "Alsa input plugin cannot be built. May be you have an ALSA library older than ${LV_ALSA_MIN_VERSION}. Take a look at {prefix}/include/alsa/version.h file.")
    SET(ENABLE_ALSA no)
  ENDIF(NOT ALSA_FOUND)
ENDIF(ENABLE_ALSA)

# MPlayer
OPTION(ENABLE_MPLAYER "Build the mplayer input plugin (default=enabled)" yes)
IF(ENABLE_MPLAYER)
  # FIXME: Missing check for MPlayer...
  # FIXME: we must check also for HAVE_MMAP here
  IF(NOT LV_HAVE_MUNMAP)
    MESSAGE(WARNING "There is not a working munmap function available. mplayer input plugin will not be built.")
    SET(ENABLE_MPLAYER no)
  ENDIF(NOT LV_HAVE_MUNMAP)
ENDIF(ENABLE_MPLAYER)

# XMMS2
OPTION(ENABLE_XMMS2 "Do not build xmms2 input plugin (default=enabled)" yes)
IF(ENABLE_XMMS2)
  PKG_CHECK_MODULES(XMMS2 xmms2-client>=0.6)
  IF(NOT XMMS2_FOUND)
    MESSAGE(WARNING "xmms2 not found. Plugin will not be built.")
    SET(ENABLE_XMMS2 no)
  ENDIF(NOT XMMS2_FOUND)
ENDIF(ENABLE_XMMS2)

# Input debug plugin
OPTION(ENABLE_INPUT_DEBUG "Build the input debug plugin (default=enabled)" yes)

# OpenGL test plugin
OPTION(ENABLE_LV_GLTEST "Build the lv_gltest plugin (default=enabled)" yes)
IF(ENABLE_LV_GLTEST)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. lv_gltest plugin will not be built.")
    SET(ENABLE_LV_GLTEST no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_LV_GLTEST)

# NastyFFT
OPTION(ENABLE_NASTYFFT "Do not build nastyfft plugin (default=enabled)" yes)
IF(ENABLE_NASTYFFT)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. nastyfft plugin will not be built.")
    SET(ENABLE_NASTYFFT no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_NASTYFFT)

# Madspin
OPTION(madspin "Build the Madspin plugin (default=enabled)" yes)
IF(ENABLE_MADSPIN)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. Madspin plugin will not be built.")
    SET(ENABLE_MADSPIN no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_MADSPIN)

# Pseudotoad Flower
OPTION(ENABLE_FLOWER "Build the Pseudotoad Flower plugin (default=enabled)" yes)
IF(ENABLE_FLOWER)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. Pseudotoad Flower plugin will not be built.")
    SET(ENABLE_FLOWER no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_FLOWER)

# Infinite
OPTION(ENABLE_INFINITE "Build the Infinite plugin (default=enabled)" yes)

# JESS
OPTION(ENABLE_JESS "Do not build JESS plugin (default=enabled)" yes)

# Corona
OPTION(ENABLE_CORONA "Build the Corona plugin (default=enabled)" yes)

# Analyzer
OPTION(ENABLE_ANALYZER "Build the Analyzer plugin (default=enabled)" yes)

# Scope
OPTION(ENABLE_SCOPE "Build the Scope plugin (default=enabled)" yes)

# Oinksie
OPTION(ENABLE_OINKSIE "Build the Oinksie plugin (default=enabled)" yes)

# Jakdaw
OPTION(ENABLE_JAKDAW "build the Jakdaw plugin (default=enabled)" yes)

# Bumpscope
OPTION(ENABLE_BUMPSCOPE "Build the bumpscope plugin (default=enabled)" yes)

# G-Force
OPTION(ENABLE_GFORCE "Build the G-Force plugin (default=enabled)" yes)

# Blursk
OPTION(ENABLE_BLURSK "Build the Blursk plugin (default=enabled)" yes)

# Alphabend
OPTION(ENABLE_ALPHABLEND "Build the alphablend morph plugin (default=enabled)" yes)

# Flash
OPTION(ENABLE_FLASH "Build the flash morph plugin (default=enabled)" yes)

# Tentacle
OPTION(ENABLE_TENTACLE "Build the tentacle morph plugin (default=enabled)" yes)

# Slide
OPTION(ENABLE_SLIDE "Build the slide morph plugin (default=enabled)" yes)

# Build parameters
SET(CMAKE_C_FLAGS "-Wall -O2")
SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

# Debug build
SET(CMAKE_C_FLAGS_DEBUG   "-O0 -ggdb3 -Wall -Wmissing-braces -Wimplicit -Wunused -Wmissing-prototypes -Wno-unused-variable")
SET(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})

# Extra optimization build
SET(CMAKE_C_FLAGS_EXTRA_OPTIMIZATION "-O3 -fexpensive-optimizations")
SET(CMAKE_CXX_FLAGS_EXTRA_OPTIMIZATION ${CMAKE_C_FLAGS_EXTRA_OPTIMIZATION})

# Profiling build
SET(CMAKE_C_FLAGS_PROFILE "-pg")
SET(CMAKE_CXX_FLAGS_PROFILE ${CMAKE_C_FLAGS_PROFILE})

# Plugin installation paths
IF(NOT DEFINED LV_PLUGIN_DIR)
  EXECUTE_PROCESS(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=pluginsbasedir ${LIBVISUAL_PKG}
    OUTPUT_VARIABLE LV_PLUGIN_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
ENDIF(NOT DEFINED LV_PLUGIN_DIR)
MESSAGE(STATUS "Plugins will be installed into \"${LV_PLUGIN_DIR}\"")

SET(LV_ACTOR_PLUGIN_DIR "${LV_PLUGIN_DIR}/actor")
SET(LV_INPUT_PLUGIN_DIR "${LV_PLUGIN_DIR}/input")
SET(LV_MORPH_PLUGIN_DIR "${LV_PLUGIN_DIR}/morph")

ADD_SUBDIRECTORY(plugins/actor)
ADD_SUBDIRECTORY(plugins/input)
ADD_SUBDIRECTORY(plugins/morph)

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.cmake ${PROJECT_BINARY_DIR}/config.h)

# Uninstallation
# Script copied from CMake FAQ

CONFIGURE_FILE(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
)

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE_TARNAME}")
SET(CPACK_SOURCE_GENERATOR TGZ TBZ2)

SET(CPACK_SOURCE_IGNORE_FILES
  /\\\\.git/
  /\\\\.gitignore$
  /_CPack_Packages/
  /CMakeCache.txt$
  CMakeFiles/
  /CPackConfig.cmake$
  /CPackSourceConfig.cmake$
  cmake_install.cmake$
  cmake_uninstall.cmake$
  Makefile$
  /install_manifest.txt$
  \\\\.service$
  \\\\.desktop$
  \\\\.pc$
  \\\\.o$
  \\\\.a$
  \\\\.so$
  \\\\.so\\\\..*$
  \\\\.mo$
  \\\\.tar\\\\.gz$
  \\\\.tar\\\\.bz2$
)

INCLUDE(CPack)
