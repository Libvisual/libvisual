cmake_minimum_required(VERSION 2.6)

# PROJECT SETTINGS
project(libvisual)

set(LIBRARY_NAME "libvisual")
set(LIBRARY_BUGREPORT "synap@nerds-incorporated.org")
set(GENERIC_MAJOR_VERSION 0)
set(GENERIC_MINOR_VERSION 4)
set(GENERIC_MICRO_VERSION 99)
set(LIBRARY_VERSION "${GENERIC_MAJOR_VERSION}.${GENERIC_MINOR_VERSION}.${GENERIC_MICRO_VERSION}")
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

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/src/
    ${CMAKE_CURRENT_BINARY_DIR}/
)

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
set (LIBVISUAL_VERSION_SUFFIX "${GENERIC_MAJOR_VERSION}.${GENERIC_MINOR_VERSION}")

set(LIBVISUAL_INCLUDE_DIR "${CMAKE_INSTALL_PREFIX}/include/libvisual")
set(LIBVISUAL_LIB_DIR "${CMAKE_INSTALL_PREFIX}/lib")
set(LIBVISUAL_PLUGINS_BASE_DIR "${LIBVISUAL_LIB_DIR}/libvisual-${LIBVISUAL_VERSION_SUFFIX}")

# Internationalization
set(GETTEXT_PACKAGE "libvisual-${LIBVISUAL_VERSION_SUFFIX}")
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

check_function_exists(gettimeofday HAVE_GETTIMEOFDAY)
check_function_exists(memset HAVE_MEMSET)
check_function_exists(sqrt HAVE_SQRT)
check_function_exists(strdup HAVE_STRDUP)
check_function_exists(strndup HAVE_STRNDUP)
check_function_exists(sysconf HAVE_SYSCONF)

check_function_exists(dcgettext HAVE_DCGETTEXT)

# MacOS X
check_function_exists(CFLocaleCopyCurrent HAVE_CFLOCALECOPYCURRENT)
check_function_exists(CFPreferencesCopyAppValue HAVE_CFPREFERENCESCOPYAPPVALUE)

if(WIN32)
  set(VISUAL_OS_WIN32 "1")
endif(WIN32)

INCLUDE(CheckTypeSize)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)
CHECK_TYPE_SIZE(int   SIZEOF_INT)
CHECK_TYPE_SIZE(long  SIZEOF_LONG)
CHECK_TYPE_SIZE("long long"  SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE(__int64  SIZEOF_INT_64)
CHECK_TYPE_SIZE(size_t  SIZEOF_SIZE_T)

# dnl Try to figure out whether visual_size should be long or int
# AC_MSG_CHECKING([for the appropriate definition for size_t])
                                                                                                               
# case $ac_cv_sizeof_size_t in
# $ac_cv_sizeof_short)
#   visual_size_type=short
#   ;;
# $ac_cv_sizeof_int)
#   visual_size_type=int
#   ;;
# $ac_cv_sizeof_long)
#   visual_size_type=long
#   ;;
# *)  AC_MSG_ERROR([No type matching size_t in size])
# 	;;
# esac

# have a look at gthread

# add pkg_config_dep:
# VISUAL_CFLAGS=""
# if test x"$LV_THREAD_CFLAGS" == xgthread2; then
#   VISUAL_PKG_CONFIG_DEP="$VISUAL_PKG_CONFIG_DEP gthread-2.0"
# elif test x"$LV_THREAD_CFLAGS" != x; then
#   VISUAL_CFLAGS=$LV_THREAD_CFLAGS
# fi
# AC_SUBST(VISUAL_PKG_CONFIG_DEP)

set(VISUAL_LIBS "${LIBS} -lvisual-${LIBVISUAL_VERSION_SUFFIX}")

# replace pkg_config cflags / libs
# if test x"$LV_THREAD_LIBS" != x; then
#   VISUAL_LIBS="$LV_THREAD_LIBS $VISUAL_LIBS"
# fi
# AC_SUBST(VISUAL_CFLAGS)		 
# AC_SUBST(VISUAL_LIBS)


# INSTALLING (static files)
# copy input-file over to build-dir, (limited to replacing @varname@ placeholders via @ONLY)

# TODO: add to config.h: synap@nerds-incorporated.org ; fix .pc and config.h (lv_config.h?)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/libvisual.pc.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/libvisual.pc"
  IMMEDIATE @ONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/config.h
  IMMEDIATE @ONLY)

# TODO: fix libvisual/lvconfig.h / libvisual/lvconfig.h.cmake.in:
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/libvisual/lvconfig.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/libvisual/lvconfig.h
  IMMEDIATE @ONLY)

# BUILDING SUBDIRECTORIES
add_subdirectory(libvisual)


install(FILES "${CMAKE_CURRENT_BINARY_DIR}/libvisual.pc" DESTINATION lib/pkgconfig)

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

# DEBUG_CFLAGS=""
# AC_ARG_ENABLE([profiling],
#   AC_HELP_STRING([--enable-profiling],
# 		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
# 		[profiling=$enableval],
# 		[profiling=no])
# AC_MSG_CHECKING([whether to enable profiling])
# if test x$profiling = xyes; then
#   AC_MSG_RESULT([yes])
#   if test x"$GCC" = xyes; then
#   DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
# else
#     AC_MSG_WARN([Profiling only works when using a gcc compiler.])
#   fi
# else
#   AC_MSG_RESULT([no])
# fi

# AC_ARG_ENABLE([fast-fp-rng],
#            AC_HELP_STRING([--enable-fast-fp-rng],
#                           [Enable faster random floating point generator (enabled by default if --enable-extra-optimization is passed) ]),
#            [fastfprng=$enableval],
#            [fastfprng=maybe])
# AC_MSG_CHECKING([whether to enable fast FP RNG])

# if  test x$fastfprng = xyes; then
#   AC_MSG_RESULT([yes])
# elif test x$fastfprng = xmaybe; then
#   if test x$extra_opt = xyes; then
#     AC_MSG_RESULT([yes])
#     fastfprng=yes
#   else
#     AC_MSG_RESULT([no])
#     fastfprng=no
#   fi
# else
#   AC_MSG_RESULT([no])
#   fastfprng=no
# fi

