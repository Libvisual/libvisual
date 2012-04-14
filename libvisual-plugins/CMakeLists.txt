CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(libvisual-plugins)

SET(LV_PLUGINS_VERSION_MAJOR  0)
SET(LV_PLUGINS_VERSION_MINOR  5)
SET(LV_PLUGINS_VERSION_MICRO  0)
SET(LV_PLUGINS_VERSION        ${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR}.${LV_PLUGINS_VERSION_MICRO})
SET(LV_PLUGINS_VERSION_SUFFIX ${LV_PLUGINS_VERSION_MAJOR}.${LV_PLUGINS_VERSION_MINOR})

SET(LIBVISUAL_PKG "libvisual-${LV_PLUGINS_VERSION_SUFFIX}")

# Package information

SET(PACKAGE           "libvisual-plugins")
SET(PACKAGE_VERSION   "${LV_PLUGINS_VERSION}")
SET(PACKAGE_BUGREPORT "http://libvisual.sf.net")
SET(PACKAGE_NAME      "${PACKAGE}")
SET(PACKAGE_STRING    "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_TARNAME   "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_URL       "http://libvisual.sf.net")

# Installation Paths
INCLUDE(GNUInstallDirs)
SET(LV_PLUGINS_DATA_DIR "${CMAKE_INSTALL_DATADIR}/libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")
SET(LV_PLUGINS_DATA_DIR_FULL "${CMAKE_INSTALL_FULL_DATADIR}/libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")

# Required versions for dependencies

SET(LV_REQUIRED_VERSION     ${LV_PLUGINS_VERSION})
SET(ESOUND_REQUIRED_VERSION 0.2.28)
SET(JACK_REQUIRED_VERSION   0.98.0)
SET(GTK_REQUIRED_VERSION    2.0)
SET(GST_REQUIRED_VERSION    0.8)
SET(PULSE_REQUIRED_VERSION  1.0)
SET(XMMS2_REQUIRED_VERSION  0.6)

# Check for pkg-config
FIND_PACKAGE(PkgConfig REQUIRED)

# Check for libvisual here so that users get told they have the
# wrong dependencies as early in the process as possible.
PKG_CHECK_MODULES(LIBVISUAL ${LIBVISUAL_PKG}>=${LV_REQUIRED_VERSION} REQUIRED)

# Determine plugin installation paths
MARK_AS_ADVANCED(LV_PLUGIN_DIR)
IF(NOT DEFINED LV_PLUGIN_DIR)
  EXECUTE_PROCESS(
    COMMAND ${PKG_CONFIG_EXECUTABLE} --variable=pluginsbasedir ${LIBVISUAL_PKG}
    OUTPUT_VARIABLE PLUGIN_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  SET(LV_PLUGIN_DIR "${PLUGIN_DIR}" CACHE PATH "Plugin installation directory")
ENDIF(NOT DEFINED LV_PLUGIN_DIR)
MESSAGE(STATUS "Plugins will be installed into ${LV_PLUGIN_DIR}")

SET(LV_ACTOR_PLUGIN_DIR "${LV_PLUGIN_DIR}/actor")
SET(LV_INPUT_PLUGIN_DIR "${LV_PLUGIN_DIR}/input")
SET(LV_MORPH_PLUGIN_DIR "${LV_PLUGIN_DIR}/morph")

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
CHECK_FUNCTION_EXISTS(mremap HAVE_MREMAP)
#AC_FUNC_MMAP

# Assembly
#AM_PROG_AS

# Internationalization
SET(GETTEXT_PACKAGE "libvisual-plugins-${LV_PLUGINS_VERSION_SUFFIX}")

# Check for system architecture

IF(CMAKE_SYSTEM_PROCESSOR MATCHES "i.86")
  SET(TARGET_ARCH_X86 yes)
ENDIF()

# Check for libraries

# Xlib
FIND_PACKAGE(X11)
SET(HAVE_X ${X11_FOUND})

# OpenGL
FIND_PACKAGE(OpenGL)
SET(HAVE_OPENGL ${OPENGL_FOUND})
SET(HAVE_XF86VMODE ${X11_xf86vmode_FOUND})

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
OPTION(ENABLE_LV_GLTEST   "Build the LV GL Test plugin" yes)
OPTION(ENABLE_LV_SCOPE    "Build the LV Scope plugin" yes)
OPTION(ENABLE_MADSPIN     "Build the Madspin plugin" yes)
OPTION(ENABLE_NEBULUS     "Build the Nebulus plugin" yes)
OPTION(ENABLE_MPLAYER     "Build the MPlayer input plugin" yes)
OPTION(ENABLE_NASTYFFT    "Build the NastyFFT plugin" yes)
OPTION(ENABLE_PLAZMA      "Build the Plazma plugin" yes)
OPTION(ENABLE_PULSEAUDIO  "Build the PulseAudio input plugin" yes)
OPTION(ENABLE_OINKSIE     "Build the Oinksie plugin" yes)
OPTION(ENABLE_SLIDE       "Build the Slide morph plugin" yes)
OPTION(ENABLE_TENTACLE    "Build the Tentacle morph plugin" yes)
OPTION(ENABLE_WAVEIN      "Build the Windows WaveIn plugin" yes)


# Check for plugin dependencies

IF(ENABLE_ALSA)
  FIND_PACKAGE(ALSA)
  IF(NOT ALSA_FOUND)
    MESSAGE(WARNING "ALSA library not found. The ALSA input plugin will not be built.")
    SET(ENABLE_ALSA no)
  ENDIF(NOT ALSA_FOUND)
ENDIF(ENABLE_ALSA)

IF(ENABLE_DANCINGPARTICLES)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. The Dancing Particles plugin will not be built.")
    SET(ENABLE_DANCINGPARTICLES no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_DANCINGPARTICLES)

IF(ENABLE_FLOWER)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. The Pseudotoad Flower plugin will not be built.")
    SET(ENABLE_FLOWER no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_FLOWER)

IF(ENABLE_GDKPIXBUF)
  PKG_CHECK_MODULES(GTK gtk+-2.0>=${GTK_REQUIRED_VERSION})
  IF(NOT GTK_FOUND)
    MESSAGE(WARNING "GTK+ >= ${GTK_REQUIRED_VERSION} is not found. The GdkPixbuf image loader plugin will not be built.")
    SET(ENABLE_GDKPIXBUF no)
  ENDIF(NOT GTK_FOUND)
ENDIF(ENABLE_GDKPIXBUF)

IF(ENABLE_GOOM2K4)
  FIND_PACKAGE(BISON)
  IF(NOT BISON_FOUND)
    MESSAGE(WARNING "Bison/Yacc is not found. The Goom2K4 plugin will not be built.")
    SET(ENABLE_GOOM2K4 no)
  ENDIF(NOT BISON_FOUND)

  FIND_PACKAGE(FLEX)
  IF(NOT FLEX_FOUND)
    MESSAGE(WARNING "Flex/Lex is not found. The Goom2K4 plugin will not be built.")
    SET(ENABLE_GOOM2K4 no)
  ENDIF(NOT FLEX_FOUND)
ENDIF(ENABLE_GOOM2K4)

IF(ENABLE_GSTREAMER)
  PKG_CHECK_MODULES(GSTREAMER gstreamer-0.8>=${GST_REQUIRED_VERSION})
  IF(NOT GSTREAMER_FOUND)
    MESSAGE(WARNING "GStreamer >= ${GST_REQUIRED_VERSION} is not found. The GStreamer viewer plugin will not be built.")
    SET(ENABLE_GSTREAMER no)
  ENDIF(NOT GSTREAMER_FOUND)
ENDIF(ENABLE_GSTREAMER)

IF(ENABLE_JACK)
  PKG_CHECK_MODULES(JACK jack>=${JACK_REQUIRED_VERSION})
  IF(NOT JACK_FOUND)
    MESSAGE(WARNING "JACK >= ${JACK_REQUIRED_VERSION} not found.")
    SET(ENABLE_JACK no)
  ENDIF(NOT JACK_FOUND)
ENDIF(ENABLE_JACK)

IF(ENABLE_LV_GLTEST)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. The lv_gltest plugin will not be built.")
    SET(ENABLE_LV_GLTEST no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_LV_GLTEST)

IF(ENABLE_MADSPIN)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. The Madspin plugin will not be built.")
    SET(ENABLE_MADSPIN no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_MADSPIN)

IF(ENABLE_MPLAYER)
  # FIXME: Missing check for MPlayer...
  IF(NOT HAVE_MREMAP)
    MESSAGE(WARNING "There is no working mremap() function available. The MPlayer input plugin will not be built.")
    SET(ENABLE_MPLAYER no)
  ENDIF(NOT HAVE_MREMAP)
ENDIF(ENABLE_MPLAYER)

IF(ENABLE_NASTYFFT)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. The nastyfft plugin will not be built.")
    SET(ENABLE_NASTYFFT no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_NASTYFFT)

IF(ENABLE_NEBULUS)
  IF(NOT HAVE_OPENGL)
    MESSAGE(WARNING "No OpenGL found. The Nebulus plugin will not be built.")
    SET(ENABLE_NEBULUS no)
  ENDIF(NOT HAVE_OPENGL)
ENDIF(ENABLE_NEBULUS)

IF(ENABLE_PULSEAUDIO)
  PKG_CHECK_MODULES(PULSE libpulse>=${PULSE_REQUIRED_VERSION} libpulse-simple>=${PULSE_REQUIRED_VERSION})
  IF(NOT PULSE_FOUND)
    MESSAGE(WARNING "No PulseAudio found. The PulseAudio input plugin will not be built.")
    SET(ENABLE_PULSEAUDIO no)
  ENDIF(NOT PULSE_FOUND)
ENDIF(ENABLE_PULSEAUDIO)

IF(ENABLE_WAVEIN)
  IF(NOT WIN32)
    MESSAGE(WARNING "The WaveIn input plugin is only supported on Windows")
    SET(ENABLE_WAVEIN no)
  ENDIF(NOT WIN32)
ENDIF(ENABLE_WAVEIN)

# Build parameters

OPTION(ENABLE_PROFILING "Enable profiling" no)
OPTION(ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations" no)

IF(ENABLE_EXTRA_OPTIMIZATIONS)
  SET(OPT_C_FLAGS "-O3 -fexpensive-optimizations")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(OPT_C_FLAGS "-O0")
ENDIF(ENABLE_EXTRA_OPTIMIZATIONS)

IF(ENABLE_PROFILING)
  SET(PROFILE_C_FLAGS "${EXTRA_CFLAGS} -pg --coverage")
ENDIF(ENABLE_PROFILING)

# Add -Werror to debug builds?
SET(CMAKE_C_FLAGS         "${OPT_C_FLAGS} ${PROFILE_C_FLAGS} -fvisibility=hidden -std=c99 -Wall -Wunused -Wmissing-prototypes")
SET(CMAKE_CXX_FLAGS       "${OPT_C_FLAGS} ${PROFILE_C_FLAGS} -fvisibility=hidden -std=c++98 -Wall -Wunused -Wno-variadic-macros")
SET(CMAKE_C_FLAGS_DEBUG   "-ggdb3")
SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")

# Pedantic checks

OPTION(ENABLE_PEDANTIC_CHECKS "Enable pedantic checks (program immediately aborts if errors occur" no)
IF(ENABLE_PEDANTIC_CHECKS)
  ADD_DEFINITIONS(-DVISUAL_PEDANTIC_CHECKS)
ENDIF(ENABLE_PEDANTIC_CHECKS)

# Generate config header
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)

# Build plugins

ADD_SUBDIRECTORY(plugins/actor)
ADD_SUBDIRECTORY(plugins/input)
ADD_SUBDIRECTORY(plugins/morph)

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

# Source packaging

SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${PACKAGE_TARNAME}")

SET(CPACK_PACKAGE_VERSION_MAJOR ${LV_PLUGINS_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${LV_PLUGINS_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${LV_PLUGINS_VERSION_MICRO})
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
