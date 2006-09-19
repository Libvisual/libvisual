# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

m4_define([lv_widgets_version_major], [0])
m4_define([lv_widgets_version_minor], [4])
m4_define([lv_widgets_version_micro], [0])
m4_define([lv_widgets_version], [lv_widgets_version_major.lv_widgets_version_minor.lv_widgets_version_micro])
m4_define([lv_widgets_version_suffix], [lv_widgets_version_major.lv_widgets_version_minor])

AC_INIT(Libvisual widgets Library, lv_widgets_version, ds@nerds-incorporated.org)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_HEADER(config.h)

m4_define([libvisual_required_version], [0.4.0])
m4_define([gtk1_required_version], [1.2.0])

m4_define([libvisual_pkg], [libvisual-lv_widgets_version_suffix])
LIBVISUAL_PKG=libvisual_pkg

AC_PATH_PROG(PKG_CONFIG, [pkg-config], [no])
if test x$PKG_CONFIG = xno ; then
  AC_MSG_ERROR([*** pkg-config not found. See http://www.freedesktop.org/software/pkgconfig/.
	       *** You will need pkg-config to compile against Libvisual Library])
fi
if $PKG_CONFIG --atleast-pkgconfig-version 0.14 ; then
  :
else
  AC_MSG_WARN([*** pkg-config too old; version 0.14 or better recommended.])
fi

## Initial sanity check, done here so that users get told they 
## have the wrong dependencies as early in the process as possible.
## Later on we actually use the cflags/libs from separate pkg-config
## calls.

PKG_CHECK_MODULES([LIBVISUAL], [libvisual_pkg >= libvisual_required_version],
  [],
  [AC_MSG_ERROR([*** libvisual libvisual_required_version not found.
 		It is needed to build this package.
		You can download libvisual at http://libvisual.sf.net/])]
)

PKG_CHECK_MODULES(BASE_DEPENDENCIES,
  [ gtk+ >= gtk1_required_version dnl
    gtk+-2.0])
dnl Check for libvisual here so that users get told they have the
dnl wrong dependencies as early in the process as possible.
PKG_CHECK_MODULES([LIBVISUAL], [libvisual_pkg >= libvisual_required_version],
  [],
  [AC_MSG_ERROR([*** libvisual libvisual_required_version not found.
 		It is needed to build this package.
		You can download libvisual at http://libvisual.sf.net/])]
)
AC_PREFIX_DEFAULT(/usr)

LVWMSG_GTK1_NOT_FOUND="
	gtk+ library not found, gtk1 widgets will not be built.
	If you have installed from binary packages, you need to
	install also the development packages of gtk+ library.
	You can download gtk+ at http://www.gtk.org/
"
LVWMSG_GTK1_TOO_OLD="
	gtk+ library is too old, gtk1 widgets will not be built.
	you can download a newer gtk+ at http://www.gtk.org/
"

LVWMSG_GTK2_NOT_FOUND="
	gtk+-2.0 library not found, gtk1 widgets will not be built.
	If you have installed from binary packages, you need to
	install also the development packages of gtk+-2.0 library.
	You can download gtk+-2.0 at http://www.gtk.org/
"

LVWMSG_GTK2_TOO_OLD="
	gtk+-2.0 library is too old, gtk1 widgets will not be built.
	you can download a newer gtk+-2.0 at http://www.gtk.org/
"

AC_CANONICAL_HOST

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

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

PKG_CHECK_MODULES(LIBVISUAL, libvisual = libvisual_required_version, [libvis="yes"], [libvis="no"])
if test x"$libvis" = xno; then
	AC_MSG_ERROR([libvisual not found, libvisual is needed to build this package,
you can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)

AC_ARG_ENABLE([gtk1widgets],
            AC_HELP_STRING([--disable-gtk1widgets],
                           [Do not build gtk+ widgets @<:@default=enabled@:>@]),
	    [gtk1widgets=$enableval],
	    [gtk1widgets=yes])
AC_MSG_CHECKING([whether to build gtk1 widgets])
if test x"$gtk1widgets" = xyes; then
	AC_MSG_RESULT([yes])
	if $PKG_CONFIG --exists gtk+ ; then
		PKG_CHECK_MODULES(LIBGTK1, gtk+ >= gtk1_required_version, [libgtk1="yes"], [libgtk1="no"])
		if test x"$libgtk1" = xyes; then
			AC_SUBST(LIBGTK1_LIBS)
			AC_SUBST(LIBGTK1_CFLAGS)
			AC_SUBST([LVW_GTK1], ['liblvwidget_gtk1.la'])
		else
			AC_MSG_WARN($LVWMSG_GTK1_TOO_OLD)
		fi
	else	
		AC_MSG_WARN($LVWMSG_GTK1_NOT_FOUND)
	fi
else
	AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([gtk2widgets],
            AC_HELP_STRING([--disable-gtk2widgets],
                           [Do not build gtk+-2.0 widgets @<:@default=enabled@:>@]),
	    [gtk2widgets=$enableval],
	    [gtk2widgets=yes])
AC_MSG_CHECKING([whether to build gtk2 widgets])
if test x"$gtk2widgets" = xyes; then
   AC_MSG_RESULT([yes])
   PKG_CHECK_MODULES(LIBGTK2, gtk+-2.0, [libgtk2="yes"], [libgtk2="no"])
   if test x"$libgtk2" = xyes; then
	   AC_MSG_CHECKING([whether gtk+-2.0 is at least 2.4.0])
	   if $PKG_CONFIG --exists 'gtk+-2.0 >= 2.4.0'; then
	      AC_MSG_RESULT([yes])
	      AC_DEFINE([HAVE_GTK_AT_LEAST_2_4_X], [1], [gtk2 version >= 2.4.x])
	   else
	      AC_MSG_RESULT([no])
	      AC_DEFINE([HAVE_GTK_AT_LEAST_2_4_X], [0], [gtk2 version < 2.4.x])
	   fi
	   AC_SUBST(LIBGTK2_LIBS)
	   AC_SUBST(LIBGTK2_CFLAGS)
	   AC_SUBST([LVW_GTK2], ['liblvwidget_gtk2.la'])
   else
	   AC_MSG_WARN($LVWMSG_GTK2_NOT_FOUND)
   fi
else
   AC_MSG_RESULT([no])
fi

AC_CONFIG_FILES([
	Makefile
	libvisual-widgets-gtk1.pc
	libvisual-widgets-gtk2.pc
	gtk1/Makefile
	gtk2/Makefile])

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL-WIDGETS, WIDGET LIBRARY FOR LIBVISUAL, VERSION $VERSION
=====================================================================

install path                         : ${prefix}/lib
compiler                             : ${CC}
debug enabled                        : ${debug}
"

