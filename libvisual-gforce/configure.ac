# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(GForce Libvisual plugin, 0.1.0, ds@nerds-incorporated.org, libvisual-gforce)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_PREFIX_DEFAULT(/usr)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LN_S
AC_PROG_RANLIB

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
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lm':
AC_CHECK_LIB([m], [main])
# FIXME: Replace `main' with a function in `-lvisual':
AC_CHECK_LIB([visual], [main])

# Checks for header files.
AC_HEADER_DIRENT
AC_CHECK_HEADERS([fcntl.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE # reserved for future
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_MALLOC
AC_FUNC_STAT
AC_CHECK_FUNCS([floor gettimeofday memmove memset pow sqrt])

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.1.7, [libvis="yes"], [libvis="no"])
if test x"$libvis" = xno; then
	AC_MSG_ERROR([libvisual not found, libvisual is needed to build this package,
you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

# cflags, ld flags
libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

CXXFLAGS="$CXXFLAGS -DUNIX_X -D_REENTRANT"
CFLAGS="$CFLAGS -DUNIX_X -D_REENTRANT"

AC_CONFIG_FILES([
	Makefile
	Common/Makefile
	Common/GeneralTools/Makefile
	Common/GeneralTools/Headers/Makefile
	Common/io/Makefile
	Common/io/Headers/Makefile
	Common/math/Makefile
	Common/math/Headers/Makefile
	Common/UI/Makefile
	Common/UI/Headers/Makefile
	GForceCommon/Makefile
	GForceCommon/Headers/Makefile
	unix/Makefile
	unix/Headers/Makefile
	unix/libmfl/Makefile
	unix/libvisual/Makefile
	GForceColorMaps/Makefile
	GForceDeltaFields/Makefile
	GForceParticles/Makefile
	GForceWaveShapes/Makefile
	NotWorkingWaveShapes/Makefile
])

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-GFORCE, GFORCE ACTOR PLUGIN FOR LIBVISUAL $VERSION
=====================================================================

Packages found:
libvisual:                            : ${libvis}

Build options:
prefix:                               : ${prefix}
source code location:                 : ${srcdir}
compiler:                             : ${CC}
cflags:                               : ${CFLAGS}
cxxflags:                             : ${CXXFLAGS}
"

