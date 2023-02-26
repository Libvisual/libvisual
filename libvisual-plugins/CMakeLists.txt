CMAKE_MINIMUM_REQUIRED(VERSION 3.20 FATAL_ERROR)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(libvisual-plugins
  VERSION 0.5.0
  DESCRIPTION "Core plugins for Libvisual"
  HOMEPAGE_URL "http://libvisual.org"
  LANGUAGES C CXX)

SET(LV_PLUGINS_VERSION_MAJOR  ${PROJECT_VERSION_MAJOR})
SET(LV_PLUGINS_VERSION_MINOR  ${PROJECT_VERSION_MINOR})
SET(LV_PLUGINS_VERSION_MICRO  ${PROJECT_VERSION_PATCH})
SET(LV_PLUGINS_VERSION        ${PROJECT_VERSION})
SET(LV_PLUGINS_VERSION_SUFFIX ${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR})

# Package information

SET(PACKAGE           "libvisual-plugins")
SET(PACKAGE_VERSION   "${LV_PLUGINS_VERSION}")
SET(PACKAGE_BUGREPORT "http://github.com/Libvisual/libvisual/issues")
SET(PACKAGE_NAME      "${PACKAGE}")
SET(PACKAGE_STRING    "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_VENDOR    "Libvisual team")
SET(PACKAGE_URL       ${PROJECT_HOMEPAGE_URL})
SET(PACKAGE_CONTACT   ${PROJECT_HOMEPAGE_URL})
SET(PACKAGE_SUMMARY   ${PROJECT_DESCRIPTION})

# Required versions for dependencies
SET(LV_REQUIRED_VERSION     ${LV_PLUGINS_VERSION})
SET(JACK_REQUIRED_VERSION   0.98.0)
SET(GTK_REQUIRED_VERSION    3.0)
SET(GST_REQUIRED_VERSION    1.0)
SET(PULSE_REQUIRED_VERSION  1.0)
SET(XMMS2_REQUIRED_VERSION  0.6)
SET(LUAJIT_REQUIRED_VERSION 0.26)

# Set C/C++ standards
SET(CMAKE_C_STANDARD 99)
SET(CMAKE_C_STANDARD_REQUIRED on)
SET(CMAKE_C_EXTENSIONS off)
SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED on)
SET(CMAKE_CXX_EXTENSIONS off)

# Check for libvisual early so that users get told they have the wrong
# dependencies as early in the process as possible.
FIND_PACKAGE(Libvisual 0.5 REQUIRED)
MESSAGE(STATUS "Plugins will be installed into ${LV_PLUGIN_DIR}")
MESSAGE(STATUS "Plugin data will be installed into ${LV_PLUGIN_DATA_DIR}")

# Check for pkg-config
FIND_PACKAGE(PkgConfig REQUIRED)

# Check for threading implementation
FIND_PACKAGE(Threads REQUIRED)

# Check for OpenMP support.
FIND_PACKAGE(OpenMP)
SET(HAVE_OPENMP ${OPENMP_FOUND})

# 'Standard' header files.
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckStdCHeaders)
IF(NOT STDC_HEADERS)
  MESSAGE(FATAL ERROR "Libvisual-plugins requires standard C library headers to build")
ENDIF()

# Library functions.
INCLUDE(CheckFunctionExists)
CHECK_FUNCTION_EXISTS(floor HAVE_FLOOR)
CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(pow HAVE_POW)
CHECK_FUNCTION_EXISTS(sqrt HAVE_SQRT)
CHECK_FUNCTION_EXISTS(munmap HAVE_MUNMAP)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)
CHECK_FUNCTION_EXISTS(mremap HAVE_MREMAP)
#AC_FUNC_MMAP

# Assembly
#AM_PROG_AS

# Resolve all external symbols when linking SOs.
OPTION(ENABLE_SO_UNRESOLVED_SYMBOL_CHECKS "Enable resolution of external symbols when linking SOs" yes)

# Internationalization
SET(GETTEXT_PACKAGE "libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")
OPTION(ENABLE_NLS "Enable internationalization" yes)
IF(ENABLE_NLS)
  FIND_PACKAGE(Gettext REQUIRED)
  SET(HAVE_GETTEXT ${GETTEXT_FOUND})
  SET(LOCALE_DIR ${LV_PLUGIN_LOCALE_DIR})
  ADD_SUBDIRECTORY(po)
ENDIF()

# Check for system architecture

IF(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86")
  SET(TARGET_ARCH_X86 yes)
ENDIF()

# Check for libraries

# OpenGL
OPTION(USE_OPENGL_ES1 "Force use of OpenGL ES 1.x (for development only)" no)
IF(NOT USE_OPENGL_ES1)
  FIND_PACKAGE(OpenGL)
  SET(HAVE_OPENGL ${OPENGL_FOUND})
  SET(HAVE_OPENGL_GLU ${OPENGL_GLU_FOUND})
ELSE()
  MESSAGE(STATUS "Forcing use of OpenGL ES 1.x")
  FIND_PACKAGE(OpenGLES1)
  SET(HAVE_OPENGL ${OpenGLES1_FOUND})
  IF(OpenGLES1_FOUND)
    ADD_LIBRARY(OpenGL::GL ALIAS OpenGLES1::GL)

    # OpenGL ES does not have GLU. We use miniGLU in place to cover some basic functions.
    ADD_COMPILE_DEFINITIONS(USE_OPENGL_ES)
    ADD_SUBDIRECTORY(common/GL)
    ADD_LIBRARY(OpenGL::GLU ALIAS miniGLU)
    SET(HAVE_OPENGL_GLU yes)
  ELSE()
    MESSAGE(WARNING "No OpenGL ES 1.x implementation found, OpenGL support is now disabled")
  ENDIF()
ENDIF()
IF(NOT HAVE_OPENGL)
  MESSAGE(WARNING "OpenGL is required to build OpenGL actors")
ENDIF()

# Plugin build options
OPTION(ENABLE_ALPHABLEND  "Build the Alphablend morph plugin" yes)
OPTION(ENABLE_ALSA        "Build the ALSA input plugin" yes)
OPTION(ENABLE_BLURSK      "Build the Blursk plugin" yes)
OPTION(ENABLE_BUMPSCOPE   "Build the Bumpscope plugin" yes)
OPTION(ENABLE_CORONA      "Build the Corona plugin" yes)
OPTION(ENABLE_DANCINGPARTICLES "Build the Dancing Particles plugin" yes)
OPTION(ENABLE_FLASH       "Build the Flash morph plugin" yes)
OPTION(ENABLE_FLOWER      "Build the Pseudotoad Flower plugin" yes)
OPTION(ENABLE_GDKPIXBUF   "Build the GdkPixbuf plugin" yes)
OPTION(ENABLE_GFORCE      "Build the G-Force plugin" yes)
OPTION(ENABLE_GOOM2K4     "Build the Goom2K4 plugin" yes)
OPTION(ENABLE_GSTREAMER   "Build the GStreamer visualization plugin" yes)
OPTION(ENABLE_INFINITE    "Build the Infinite plugin" yes)
OPTION(ENABLE_INPUT_DEBUG "Build the input debug plugin" yes)
OPTION(ENABLE_JACK        "Build the JACK input plugin" yes)
OPTION(ENABLE_JAKDAW      "build the Jakdaw plugin" yes)
OPTION(ENABLE_JESS        "Build the JESS plugin" yes)
OPTION(ENABLE_LV_ANALYZER "Build the LV Analyzer plugin" yes)
OPTION(ENABLE_LV_DUMP     "Build the LV Data Dump plugin" yes)
OPTION(ENABLE_LV_GLTEST   "Build the LV GL Test plugin" yes)
OPTION(ENABLE_LV_SCOPE    "Build the LV Scope plugin" yes)
OPTION(ENABLE_MADSPIN     "Build the Madspin plugin" yes)
OPTION(ENABLE_NEBULUS     "Build the Nebulus plugin" yes)
OPTION(ENABLE_MPLAYER     "Build the MPlayer input plugin" yes)
OPTION(ENABLE_NASTYFFT    "Build the NastyFFT plugin" yes)
OPTION(ENABLE_PLAZMA      "Build the Plazma plugin" yes)
OPTION(ENABLE_PORTAUDIO   "Build the PortAudio input plugin" yes)
OPTION(ENABLE_PULSEAUDIO  "Build the PulseAudio input plugin" yes)
OPTION(ENABLE_OINKSIE     "Build the Oinksie plugin" yes)
OPTION(ENABLE_SLIDE       "Build the Slide morph plugin" yes)
OPTION(ENABLE_TENTACLE    "Build the Tentacle morph plugin" yes)
OPTION(ENABLE_WAVEIN      "Build the Windows WaveIn plugin" yes)
OPTION(ENABLE_CHECKERS    "Build the checkers morph plugin" yes)
OPTION(ENABLE_LCDCONTROL  "Build the LCDControl actor plugin" yes)


# Check for plugin dependencies

IF(ENABLE_ALSA)
  FIND_PACKAGE(ALSA)
  IF(NOT ALSA_FOUND)
    MESSAGE(WARNING "ALSA library not found. The ALSA input plugin will not be built.")
    SET(ENABLE_ALSA no)
  ENDIF()
ENDIF()

IF(ENABLE_DANCINGPARTICLES)
  FIND_PACKAGE(Freetype)
  IF(NOT FREETYPE_FOUND)
    MESSAGE(WARNING "FreeType not found. The Dancing Particles plugin will not be built.")
    SET(ENABLE_DANCINGPARTICLES no)
  ENDIF()

  IF(NOT HAVE_OPENGL OR NOT HAVE_OPENGL_GLU)
    MESSAGE(WARNING "OpenGL or GLU not found. The Dancing Particles plugin will not be built.")
    SET(ENABLE_DANCINGPARTICLES no)
  ENDIF()
ENDIF()

IF(ENABLE_FLOWER)
  IF(NOT HAVE_OPENGL OR NOT HAVE_OPENGL_GLU)
    MESSAGE(WARNING "OpenGL or GLU not found. The Pseudotoad Flower plugin will not be built.")
    SET(ENABLE_FLOWER no)
  ENDIF()
ENDIF()

IF(ENABLE_GDKPIXBUF)
  PKG_CHECK_MODULES(GTK gtk+-3.0>=${GTK_REQUIRED_VERSION} IMPORTED_TARGET)
  IF(NOT GTK_FOUND)
    MESSAGE(WARNING "GTK+ >= ${GTK_REQUIRED_VERSION} is not found. The GdkPixbuf image loader plugin will not be built.")
    SET(ENABLE_GDKPIXBUF no)
  ENDIF()
ENDIF()

IF(ENABLE_GOOM2K4)
  FIND_PACKAGE(BISON)
  IF(NOT BISON_FOUND)
    MESSAGE(WARNING "Bison/Yacc is not found. The Goom2K4 plugin will not be built.")
    SET(ENABLE_GOOM2K4 no)
  ENDIF()

  FIND_PACKAGE(FLEX)
  IF(NOT FLEX_FOUND)
    MESSAGE(WARNING "Flex/Lex is not found. The Goom2K4 plugin will not be built.")
    SET(ENABLE_GOOM2K4 no)
  ENDIF()
ENDIF()

IF(ENABLE_GSTREAMER)
  PKG_CHECK_MODULES(GSTREAMER gstreamer-1.0>=${GST_REQUIRED_VERSION} IMPORTED_TARGET)
  IF(NOT GSTREAMER_FOUND)
    MESSAGE(WARNING "GStreamer >= ${GST_REQUIRED_VERSION} is not found. The GStreamer viewer plugin will not be built.")
    SET(ENABLE_GSTREAMER no)
  ENDIF()
ENDIF()

IF(ENABLE_JACK)
  PKG_CHECK_MODULES(JACK jack>=${JACK_REQUIRED_VERSION} IMPORTED_TARGET)
  IF(NOT JACK_FOUND)
    MESSAGE(WARNING "JACK >= ${JACK_REQUIRED_VERSION} not found. The JACK plugin will not be built")
    SET(ENABLE_JACK no)
  ENDIF()
ENDIF()

IF(ENABLE_LV_GLTEST)
  IF(NOT HAVE_OPENGL OR NOT HAVE_OPENGL_GLU)
    MESSAGE(WARNING "OpenGL or GLU not found. The lv_gltest plugin will not be built.")
    SET(ENABLE_LV_GLTEST no)
  ENDIF()
ENDIF()

IF(ENABLE_MADSPIN)
  IF(NOT HAVE_OPENGL OR NOT HAVE_OPENGL_GLU)
    MESSAGE(WARNING "OpenGL or GLU not found. The Madspin plugin will not be built.")
    SET(ENABLE_MADSPIN no)
  ENDIF()
ENDIF()

IF(ENABLE_MPLAYER)
  IF(NOT HAVE_MREMAP)
    MESSAGE(WARNING "No working mremap() function available. The MPlayer input plugin will not be built.")
    SET(ENABLE_MPLAYER no)
  ENDIF()
ENDIF()

IF(ENABLE_NASTYFFT)
  IF(NOT HAVE_OPENGL OR NOT HAVE_OPENGL_GLU)
    MESSAGE(WARNING "OpenGL or GLU not found. The nastyfft plugin will not be built.")
    SET(ENABLE_NASTYFFT no)
  ENDIF()
ENDIF()

IF(ENABLE_NEBULUS)
  IF(NOT HAVE_OPENGL OR NOT HAVE_OPENGL_GLU)
    MESSAGE(WARNING "OpenGL or GLU not found. The Nebulus plugin will not be built.")
    SET(ENABLE_NEBULUS no)
  ENDIF()
ENDIF()

IF(ENABLE_PORTAUDIO)
  PKG_CHECK_MODULES(PORTAUDIO portaudio-2.0 IMPORTED_TARGET)
  IF(NOT PORTAUDIO_FOUND)
    MESSAGE(WARNING "PortAudio not found. The PortAudio input plugin will not be built.")
    SET(ENABLE_PORTAUDIO no)
  ENDIF()
ENDIF()

IF(ENABLE_PULSEAUDIO)
  PKG_CHECK_MODULES(PULSE libpulse>=${PULSE_REQUIRED_VERSION} IMPORTED_TARGET)
  IF(NOT PULSE_FOUND)
    MESSAGE(WARNING "PulseAudio >= ${PULSE_REQUIRED_VERSION} not found. The PulseAudio input plugin will not be built.")
    SET(ENABLE_PULSEAUDIO no)
  ENDIF()
ENDIF()

IF(ENABLE_WAVEIN)
  IF(NOT WIN32)
    MESSAGE(WARNING "The WaveIn input plugin is only supported on Windows")
    SET(ENABLE_WAVEIN no)
  ENDIF()
ENDIF()

IF(ENABLE_LCDCONTROL)
  PKG_CHECK_MODULES(LUAJIT luajit>=${LUAJIT_REQUIRED_VERSION} IMPORTED_TARGET)
  IF(NOT LUAJIT_FOUND)
    MESSAGE(WARNING "LuaJIT >= ${LUAJIT_REQUIRED_VERSION} was not found. The LCDControl plugin will not be built.")
    SET(ENABLE_LCDCONTROL no)
  ENDIF(NOT LUAJIT_FOUND)
ENDIF(ENABLE_LCDCONTROL)

# Build parameters

OPTION(ENABLE_PROFILING "Enable profiling" no)
OPTION(ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations" no)

IF(ENABLE_EXTRA_OPTIMIZATIONS)
  ADD_COMPILE_OPTIONS(-O3 -fexpensive-optimizations)
ELSEIF(CMAKE_BUILD_TYPE STREQUAL Debug)
  ADD_COMPILE_OPTIONS(-O0)
ENDIF()

IF(ENABLE_PROFILING)
  ADD_COMPILE_OPTIONS(-pg --coverage)
ENDIF()

# Fatal warnings
OPTION(ENABLE_FATAL_WARNINGS "Compile with -Werror" no)
IF(ENABLE_FATAL_WARNINGS)
  ADD_COMPILE_OPTIONS(-Werror)
ENDIF()

ADD_COMPILE_OPTIONS(
  -fvisibility=hidden -Wall -Wunused -fstrict-aliasing
  "$<$<COMPILE_LANGUAGE:C>:-Wmissing-prototypes>"
)

IF(CMAKE_COMPILER_IS_GNUCXX)
  ADD_COMPILE_OPTIONS(
    "$<$<CONFIG:Debug>:-ggdb3>"
  )
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  ADD_COMPILE_OPTIONS(
    # Get rid of warnings about comparisons of unsigned integers being smaller than 0
    -Wno-tautological-compare
    "$<$<CONFIG:Debug>:-g>"
  )
ENDIF()

# Pedantic checks

OPTION(ENABLE_PEDANTIC_CHECKS "Enable pedantic checks (program immediately aborts if errors occur" no)
IF(ENABLE_PEDANTIC_CHECKS)
  ADD_COMPILE_DEFINITIONS(VISUAL_PEDANTIC_CHECKS)
ENDIF()

# Generate config header

CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)

# Add RPATH where necessary

SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# Build plugins
ADD_SUBDIRECTORY(plugins)

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

# Packaging

SET(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE}-${PACKAGE_VERSION})

SET(CPACK_PACKAGE_VERSION_MAJOR ${LV_PLUGINS_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${LV_PLUGINS_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${LV_PLUGINS_VERSION_MICRO})
SET(CPACK_PACKAGE_VENDOR ${PACKAGE_VENDOR})
SET(CPACK_PACKAGE_CONTACT ${PACKAGE_CONTACT})
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY ${PACKAGE_SUMMARY})
SET(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/COPYING)
SET(CPACK_RESOURCE_FILE_README  ${PROJECT_SOURCE_DIR}/README)

SET(CPACK_GENERATOR TGZ TBZ2)
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
  ~$
)

INCLUDE(CPack)
