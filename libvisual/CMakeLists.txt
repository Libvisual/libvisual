CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

PROJECT(libvisual)

# Version numbers
SET(LV_VERSION_MAJOR  0)
SET(LV_VERSION_MINOR  5)
SET(LV_VERSION_MICRO  0)
SET(LV_VERSION        "${LV_VERSION_MAJOR}.${LV_VERSION_MINOR}.${LV_VERSION_MICRO}")
SET(LV_VERSION_SUFFIX "${LV_VERSION_MAJOR}.${LV_VERSION_MINOR}")

# Package information
SET(PACKAGE           "libvisual")
SET(PACKAGE_VERSION   "${LV_VERSION}")
SET(PACKAGE_BUGREPORT "http://github.com/Libvisual/libvisual/issues")
SET(PACKAGE_NAME      "${PACKAGE}")
SET(PACKAGE_STRING    "${PACKAGE}-${PACKAGE_VERSION}")
SET(PACKAGE_VENDOR    "Libvisual team")
SET(PACKAGE_URL       "http://libvisual.org")
SET(PACKAGE_CONTACT   "http://libvisual.org")
SET(PACKAGE_SUMMARY   "Libvisual visualization framework")

# Installation Paths
INCLUDE(GNUInstallDirs)
SET(LV_INCLUDE_DIR      "${CMAKE_INSTALL_INCLUDEDIR}/libvisual-${LV_VERSION_SUFFIX}")
SET(LV_INCLUDE_DIR_FULL "${CMAKE_INSTALL_FULL_INCLUDEDIR}/libvisual-${LV_VERSION_SUFFIX}")
SET(LV_PLUGIN_DIR       "${CMAKE_INSTALL_LIBDIR}/libvisual-${LV_VERSION_SUFFIX}")
SET(LV_PLUGIN_DIR_FULL  "${CMAKE_INSTALL_FULL_LIBDIR}/libvisual-${LV_VERSION_SUFFIX}")

# Include basic CMake modules
INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckTypeSize)
FIND_PACKAGE(PkgConfig REQUIRED)

INCLUDE(LVCheckTargetEnv)

# Check for C++11 features
INCLUDE(CheckCXX11Features)
FOREACH(FEATURE AUTO NULLPTR LAMBDA STATIC_ASSERT RVALUE_REFERENCES VARIADIC_TEMPLATES DECLTYPE CSTDINT_H)
  IF(NOT HAS_CXX11_${FEATURE})
    MESSAGE(FATAL_ERROR "Required C++11 feature support not found (${FEATURE})\nRecommended C++ compilers: gcc >= 4.6, clang >= 3.1")
  ENDIF()
ENDFOREACH()

# Check for Unix specific functions
# TODO: Generate error when a required header is missing
CHECK_INCLUDE_FILE(unistd.h    HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(dlfcn.h     HAVE_DLFCN_H)
CHECK_INCLUDE_FILE(dirent.h    HAVE_DIRENT_H)
CHECK_FUNCTION_EXISTS(sysconf HAVE_SYSCONF)

# Check threading implementation
FIND_PACKAGE(Threads)

# Check for dynamic linking library
CHECK_LIBRARY_EXISTS(dl dlsym "" HAVE_DLFCN)
IF(VISUAL_OS_POSIX AND NOT HAVE_DLFCN)
  MESSAGE(FATAL_ERROR "Libvisual requires dlfcn to build")
ENDIF()

# Check for standard math library
CHECK_LIBRARY_EXISTS(m sqrt "" HAVE_STDC_MATH)
IF(NOT HAVE_STDC_MATH)
  MESSAGE(FATAL_ERROR "Libvisual requires libm to build")
ENDIF()

# Check for libpng
FIND_PACKAGE(PNG REQUIRED)

# Internationalization
SET(GETTEXT_PACKAGE "libvisual-${LV_VERSION_SUFFIX}")
OPTION(ENABLE_NLS "Enable internationalization" yes)
IF(ENABLE_NLS)
  FIND_PACKAGE(Gettext REQUIRED)
  SET(HAVE_GETTEXT ${GETTEXT_FOUND})
  IF(NOT HAVE_GETTEXT)
    MESSAGE(FATAL_ERROR "Internationalization requires GNU Gettext to work")
  ENDIF()
  SET(LOCALE_DIR ${CMAKE_INSTALL_FULL_LOCALEDIR})
  ADD_SUBDIRECTORY(po)
ENDIF()

# Check for type sizes
CHECK_TYPE_SIZE(short  SIZEOF_SHORT)
CHECK_TYPE_SIZE(int    SIZEOF_INT)
CHECK_TYPE_SIZE(long   SIZEOF_LONG)
CHECK_TYPE_SIZE(size_t SIZEOF_SIZE_T)

# Try to figure out whether visual_size should be long or int
MESSAGE(STATUS "Checking for the appropriate definition for size_t")
IF(SIZEOF_SIZE_T EQUAL SIZEOF_SHORT)
  SET(VISUAL_SIZE_TYPE short)
  SET(VISUAL_SIZE_T_FORMAT "hu")
ELSEIF(SIZEOF_SIZE_T EQUAL SIZEOF_INT)
  SET(VISUAL_SIZE_TYPE int)
  SET(VISUAL_SIZE_T_FORMAT "u")
ELSEIF(SIZEOF_SIZE_T EQUAL SIZEOF_LONG)
  SET(VISUAL_SIZE_TYPE long)
  SET(VISUAL_SIZE_T_FORMAT "lu")
ELSE()
  MESSAGE(FATAL_ERROR "No type matching size_t in size")
ENDIF()
MESSAGE(STATUS "Size of size_t is ${VISUAL_SIZE_TYPE}")

# Add RPATHs where necessary
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_FULL_LIBDIR}" IS_SYSTEM_DIR)
IF(IS_SYSTEM_DIR STREQUAL "-1")
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_FULL_LIBDIR}")
ENDIF()

# Compiler optimizations
OPTION(ENABLE_EXTRA_OPTIMIZATIONS "Enable extra optimizations" no)
IF(ENABLE_EXTRA_OPTIMIZATIONS)
  SET(OPT_C_FLAGS "-O3 -fexpensive-optimizations")
ELSEIF(CMAKE_BUILD_TYPE STREQUAL Debug)
  SET(OPT_C_FLAGS "-O0")
ENDIF()

# Profiling
OPTION(ENABLE_PROFILING "Enable profiling" no)
IF(ENABLE_PROFILING)
  SET(PROFILE_C_FLAGS "${EXTRA_CFLAGS} -pg")
ENDIF()

# Build profiles
# Standard
SET(CMAKE_C_FLAGS "${OPT_C_FLAGS} ${PROFILE_C_FLAGS} -std=c99 -Wall -Wunused -Werror -fstrict-aliasing")
SET(CMAKE_CXX_FLAGS "${OPT_C_FLAGS} ${PROFILE_C_FLAGS} ${CXX11_FLAGS} -Wall -Wunused -Werror -fstrict-aliasing")
# Debug
IF(CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_C_FLAGS_DEBUG "-ggdb3")
  SET(CMAKE_CXX_FLAGS_DEBUG "-ggdb3")
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  SET(CMAKE_C_FLAGS_DEBUG "-g")
  SET(CMAKE_CXX_FLAGS_DEBUG "-g")
ENDIF()

# Miscellaneous optimizations
OPTION(ENABLE_FAST_FP_RNG "Enable faster random floating point generator" ${ENABLE_EXTRA_OPTIMIZATIONS})
SET(VISUAL_RANDOM_FAST_FP_RNG ${ENABLE_FAST_FP_RNG})

# ORC
FIND_PACKAGE(ORC REQUIRED)

# API Documentation
OPTION(ENABLE_DOCS "Enable the creation of API documentation" no)
IF(ENABLE_DOCS)
  FIND_PACKAGE(Doxygen)
  IF(DOXYGEN_FOUND)
    IF(NOT DOXYGEN_DOT_FOUND)
      MESSAGE(WARNING "Graphviz Dot tool not found. API documentation will not have diagrams")
    ENDIF()
    CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/Doxyfile.in ${PROJECT_BINARY_DIR}/Doxyfile)
    # FIXME/TODO: Figure out how to generate the docs in the first make run
    ADD_CUSTOM_TARGET(docs
      COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
      COMMENT "Generating API documentation.."
      VERBATIM
    )
  ELSE()
    MESSAGE(WARNING "Doxygen not found. No API documentation will be generated")
    SET(ENABLE_DOCS no)
  ENDIF()
ENDIF()

# Pedantic checks
OPTION(ENABLE_PEDANTIC_CHECKS "Enable pedantic checks (program immediately aborts if errors occur)" no)
IF(ENABLE_PEDANTIC_CHECKS)
  ADD_DEFINITIONS(-DVISUAL_PEDANTIC_CHECKS)
ENDIF()

# Git revision
FIND_PACKAGE(Git)
CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/update-version.sh.in"
  "${PROJECT_BINARY_DIR}/update-version.sh"
  @ONLY
)
ADD_CUSTOM_TARGET(update-version ALL
  COMMAND /bin/sh ${PROJECT_BINARY_DIR}/update-version.sh
)

# Tools
OPTION(ENABLE_TOOLS "Build Libvisual tools" yes)
IF(ENABLE_TOOLS)
  FIND_PACKAGE(SDL)
  SET(HAVE_SDL ${SDL_FOUND})

  FIND_PACKAGE(OpenGL)
  SET(HAVE_GL ${OPENGL_FOUND})

  # Give users heads up about needing SDL and OpenGL for visual display
  IF(NOT HAVE_SDL)
    MESSAGE(WARNING "SDL is required for lv-tool to support visual display")
  ENDIF()
  IF(NOT HAVE_GL)
    MESSAGE(WARNING "OpenGL is required for lv-tool to support OpenGL visualisations")
  ENDIF()

  ADD_SUBDIRECTORY(tools/lv-tool)
  ADD_SUBDIRECTORY(tools/benchmarks)
ENDIF()

# Tests
OPTION(ENABLE_TESTS "Build tests" no)
IF(ENABLE_TESTS)
  ENABLE_TESTING()
  ADD_SUBDIRECTORY(tests)
ENDIF()

# Examples
OPTION(ENABLE_EXAMPLES "Build examples" yes)
IF(ENABLE_EXAMPLES)
  ADD_SUBDIRECTORY(examples)
ENDIF()

# Generate pkg-config file
SET(LV_PKG_CONFIG_NAME   "libvisual-${LV_VERSION_SUFFIX}")
SET(LV_PKG_CONFIG_LIBS   "-lvisual-${LV_VERSION_SUFFIX} ${CMAKE_THREAD_LIBS_INIT}")
SET(LV_PKG_CONFIG_CFLAGS "")
SET(LV_PKG_CONFIG_DEPS   "")
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/libvisual.pc.in ${PROJECT_BINARY_DIR}/${LV_PKG_CONFIG_NAME}.pc @ONLY)
INSTALL(FILES ${PROJECT_BINARY_DIR}/${LV_PKG_CONFIG_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

# Generate config headers
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/config.h.in ${PROJECT_BINARY_DIR}/config.h)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/libvisual/lvconfig.h.in ${PROJECT_BINARY_DIR}/libvisual/lvconfig.h)

# Build libvisual
ADD_SUBDIRECTORY(libvisual)

# Uninstallation (script copied from CMake FAQ)

CONFIGURE_FILE(
  "${PROJECT_SOURCE_DIR}/cmake_uninstall.cmake.in"
  "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
  @ONLY
)

ADD_CUSTOM_TARGET(uninstall
  "${CMAKE_COMMAND}" -P "${PROJECT_BINARY_DIR}/cmake_uninstall.cmake"
)

# Packaging

SET(CPACK_SOURCE_PACKAGE_FILE_NAME ${PACKAGE}-${PACKAGE_VERSION})

SET(CPACK_PACKAGE_VERSION_MAJOR ${LV_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${LV_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${LV_VERSION_MICRO})
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
