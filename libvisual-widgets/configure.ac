# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual-widgets Library, 0.2.0, ds@nerds-incorporated.org, libvisual-widgets)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_HEADER(config.h)

AC_PREFIX_DEFAULT(/usr)

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_WARN([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/.
	       *** You will need pkg-config to compile against Libvisual Library])
fi
if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_WARN([*** pkg-config too old; version 0.14 or better recommended.])
fi

# Checks for header files.
AC_HEADER_STDC

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

# Arguments to specify certain features.
AC_ARG_ENABLE([debug],
            AC_HELP_STRING([--enable-debug],
                           [Enable debug @<:@default=disabled@:>@]),
	    [debug=$enableval],
	    [debug=no])
AC_MSG_CHECKING([wheter to enable debug])
if test x$debug = xyes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="-ggdb3"
else
  AC_MSG_RESULT([no])
  DEBUG_CFLAGS=""
fi
AC_SUBST(DEBUG_CFLAGS)

PKG_CHECK_MODULES(LIBVISUAL, libvisual = 0.2.0, [libvis="yes"], [libvis="no"])
if test x"$libvis" = xno; then
	AC_MSG_ERROR([libvisual not found, libvisual is needed to build this package,
you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

PKG_CHECK_MODULES(LIBGTK2, gtk+-2.0, [libgtk2="yes"], [libgtk2="no"])
if test x"$libgtk2" = xno; then
	AC_MSG_ERROR([gtk2 not found, libvisual is needed to build this package,
you can download libvisual at http://www.gtk.org/])
fi
AC_SUBST(LIBGTK2_LIBS)
AC_SUBST(LIBGTK2_CFLAGS)

AC_CONFIG_FILES([
	Makefile
	libvisual-widgets-gtk1.pc
	libvisual-widgets-gtk2.pc
	libvisual-widgets-qt.pc
	gtk1/Makefile
	gtk2/Makefile
	qt/Makefile])

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-WIDGETS, WIDGET LIBRARY FOR LIBVISUAL, VERSION $VERSION
=====================================================================

install path                         : ${prefix}/lib
compiler                             : ${CC}
debug enabled                        : ${debug}
"

