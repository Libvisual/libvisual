# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)
AC_INIT(Libvisual Library, 0.1.7, synap@nerds-incorporated.org, libvisual)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_HEADER(config.h)

AC_PREFIX_DEFAULT(/usr)

AC_CANONICAL_HOST

lv_mips=no
lv_alpha=no
lv_sparc=no
lv_ix86=no
lv_powerpc=no

case "$host_cpu" in
	mips)           lv_mips=yes;;
	alpha)          lv_alpha=yes;;
	sparc)          lv_sparc=yes;;
	i*86)           lv_ix86=yes;;
       	powerpc|ppc)    lv_powerpc=yes;;
	*)		lv_arch_unknown=yes;;
esac

lv_hpux=no
lv_aix=no
lv_beos=no
lv_osf=no
lv_irix=no
lv_sunos=no
lv_solaris=no
lv_linux=no
lv_sco=no
lv_bsd=no
lv_darwin=no
lv_cygwin=no
lv_mingw=no
lv_os_unknown=no

case "$host_os" in
	hpux9* | hpux10* | hpux11*)	lv_hpux=yes;;
	aix*)		lv_aix=yes;;
	beos*)		lv_beos=yes;;
	*osf*)		lv_osf=yes;;
	irix*)          lv_irix=yes;;
	sunos*)         lv_sunos=yes;;
	solaris*)       lv_solaris=yes;;
	linux*)         lv_linux=yes;;
	*sco*)          lv_sco=yes;;
	*bsd*)		lv_bsd=yes;;
	*darwin*)	lv_darwin=yes;;
	cygwin*)	lv_cygwin=yes;;
  	mingw*)		lv_mingw=yes;;
	*)		lv_os_unknown=yes;;
esac

AC_MSG_CHECKING([for Win32])
if test x$lv_cygwin = xyes || test x$lv_mingw = xyes ; then
  lv_win32=yes
else
  lv_win32=no
fi
AC_MSG_RESULT([$lv_win32])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_DISABLE_STATIC
AC_LIBTOOL_WIN32_DLL
AC_LIBTOOL_DLOPEN
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
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h sys/types.h stdlib.h string.h sys/time.h unistd.h])

lv_have_opengl=no
AC_CHECK_HEADERS([GL/gl.h GL/glx.h],opengl="yes",opengl="no")
if test x"$opengl" = xno; then
	AC_MSG_WARN([*** GL headers not found.
			The library will be built without GL support.])
else
	lv_have_opengl=yes
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_HEADER_TIME

AC_MSG_CHECKING(for ISO C99 varargs macros in C)
AC_TRY_COMPILE([],[
		int a(int p1, int p2, int p3);
		#define call_a(...) a(1,__VA_ARGS__)
		call_a(2,3);
		],lv_have_iso_c_varargs=yes,lv_have_iso_c_varargs=no)
AC_MSG_RESULT($lv_have_iso_c_varargs)

AC_MSG_CHECKING(for GNUC varargs macros)
AC_TRY_COMPILE([],[
		int a(int p1, int p2, int p3);
		#define call_a(params...) a(1,params)
		call_a(2,3);
		],lv_have_gnuc_varargs=yes,lv_have_gnuc_varargs=no)
AC_MSG_RESULT($lv_have_gnuc_varargs)

AC_C_BIGENDIAN(bigendian=yes, bigendian=no, )

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gettimeofday usleep nanosleep select memset sqrt strdup strndup])

if test x"$lv_have_opengl" = xyes; then
  AC_CHECK_LIB([GL], [glBegin], [],
	       [AC_MSG_WARN([*** GL headers has been found, but we cannot link against GL Library.
			Libvisual will be built without OpenGL support.
			If you have already installed GL Library, probably you need to edit
			your /etc/ld.so.conf and run ldconfig before to try to configure Libvisual.
			Other option could be to set and export the LDFLAGS environment variable
			to the correct flags (see ./configure --help).])
	       lv_have_opengl=no])
else
  :
fi

AC_CHECK_LIB([dl], [dlsym], [LIBS="-ldl $LIBS"],
	     AC_MSG_WARN([*** You need dlsym() to make use of Libvisual.]))
AC_CHECK_LIB([m], [sqrt], [LIBS="-lm $LIBS"],
	     AC_MSG_ERROR([*** You need the Standard C Math Library to build Libvisual.]))

# check for type sizes
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(size_t)
                                                                                                               
dnl Try to figure out whether visual_size should be long or int
AC_MSG_CHECKING([for the appropriate definition for size_t])
                                                                                                               
case $ac_cv_sizeof_size_t in
$ac_cv_sizeof_short)
  visual_size_type=short
  ;;
$ac_cv_sizeof_int)
  visual_size_type=int
  ;;
$ac_cv_sizeof_long)
  visual_size_type=long
  ;;
*)  AC_MSG_ERROR([No type matching size_t in size])
	;;
esac

AC_MSG_RESULT(unsigned $visual_size_type)

# Arguments to specify certain features.
build_subdirs="libvisual"
VISUAL_CFLAGS=""
VISUAL_LIBS="$LIBS -lvisual"
AC_SUBST(VISUAL_CFLAGS)		 
AC_SUBST(VISUAL_LIBS)

AC_ARG_ENABLE([profiling],
  AC_HELP_STRING([--enable-profiling],
		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
		[profiling=$enableval],
		[profiling=no])
AC_MSG_CHECKING([wheter to enable profiling])
if test x$profiling = xyes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
else
  AC_MSG_RESULT([no])
fi

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

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(BUILD_SUBDIRS, "libvisual")

# Terrible hack, but otherwise the CFLAGS are passed as last arguments
# to compiler, and this way these arguments (probably -g -O2) overrides
# our -O3 flag on libvisual/Makefile.am
AC_SUBST(CFLAGS, "")

dnl Output the LibVisual config file
AC_CONFIG_COMMANDS([libvisual/lvconfig.h],
[
	outfile=libvisual/lvconfig.h-tmp
	cat > $outfile <<\_______EOF
/* lvconfig.h
 *
 * This is a generated file.  Please modify 'configure.ac'
 */

#ifndef __LV_CONFIG_H__
#define __LV_CONFIG_H__

_______EOF

        if test x$lv_have_iso_c_varargs = xyes ; then
                cat >>$outfile <<_______EOF
#ifndef __cplusplus
# define LV_HAVE_ISO_VARARGS	(1)
#endif
_______EOF
        fi

        if test x$lv_have_gnuc_varargs = xyes ; then
                cat >>$outfile <<_______EOF

/* gcc-2.95.x supports both gnu style and ISO varargs, but if -ansi
 * is passed ISO vararg support is turned off, and there is no work
 * around to turn it on, so we unconditionally turn it off.
 */
#if __GNUC__ == 2 && __GNUC_MINOR__ == 95
#  undef LV_HAVE_ISO_VARARGS
#endif

#define LV_HAVE_GNUC_VARARGS	(1)
_______EOF
       fi

        if test x$bigendian = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_BIG_ENDIAN	(1)
#define VISUAL_LITTLE_ENDIAN 	(0)
_______EOF
	else
                cat >>$outfile <<_______EOF

#define VISUAL_BIG_ENDIAN	(0)
#define VISUAL_LITTLE_ENDIAN	(1)
_______EOF
        fi

  		cat >>$outfile <<_______EOF

typedef unsigned $visual_size_type visual_size_t;
#define VISUAL_SIZE_T_FORMAT	$visual_size_format
_______EOF

        if test x$lv_have_opengl = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_HAVE_OPENGL
_______EOF
        fi

        if test x$lv_mips = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_MIPS
_______EOF
        fi

        if test x$lv_alpha = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_ALPHA
_______EOF
        fi

        if test x$lv_sparc = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_SPARC
_______EOF
        fi

        if test x$lv_ix86 = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_Ix86
_______EOF
        fi

        if test x$lv_powerpc = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_POWERPC
_______EOF
        fi

        if test x$lv_arch_unknown = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_UNKNOWN
_______EOF
        fi

        if test x$lv_linux = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_LINUX
_______EOF
        fi

        if test x$lv_win32 = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_WIN32
_______EOF
        fi

        if test x$lv_hpux = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_HPUX
_______EOF
        fi

        if test x$lv_aix = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_AIX
_______EOF
        fi

        if test x$lv_beos = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_BEOS
_______EOF
        fi

        if test x$lv_osf = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_OSF
_______EOF
        fi

        if test x$lv_irix = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_IRIX
_______EOF
        fi

        if test x$lv_sunos = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_SUNOS
_______EOF
        fi

        if test x$lv_solaris = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_SOLARIS
_______EOF
        fi

        if test x$lv_sco = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_SCO
_______EOF
        fi

        if test x$lv_bsd = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_BSD
_______EOF
        fi

        if test x$lv_darwin = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_DARWIN
_______EOF
        fi

        if test x$lv_os_unknown = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_UNKNOWN
_______EOF
        fi

        if test x$lv_cygwin = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_WITH_CYGWIN
_______EOF
        fi

        if test x$lv_mingw = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_WITH_MINGW
_______EOF
        fi

echo >>$outfile
echo >>$outfile
echo "#endif /* LV_CONFIG_H */" >>$outfile
echo >>$outfile

        if cmp -s $outfile libvisual/lvconfig.h; then
		AC_MSG_NOTICE([lvconfig.h is unchanged])
		rm -f $outfile
        else
		mv $outfile libvisual/lvconfig.h
        fi

],[
   lv_have_iso_c_varargs=$lv_have_iso_c_varargs
   lv_have_gnuc_varargs=$lv_have_gnuc_varargs
   visual_size_type=$visual_size_type
   lv_win32=$lv_win32
   lv_have_opengl=$lv_have_opengl
 
   lv_mips=$lv_mips
   lv_alpha=$lv_alpha
   lv_sparc=$lv_sparc
   lv_ix86=$lv_ix86
   lv_powerpc=$lv_powerpc
   lv_arch_unknown=$lv_arch_unknown

   lv_hpux=$lv_hpux
   lv_aix=$lv_aix
   lv_beos=$lv_beos
   lv_osf=$lv_osf
   lv_irix=$lv_irix
   lv_sunos=$lv_sunos
   lv_solaris=$lv_solaris
   lv_linux=$lv_linux
   lv_sco=$lv_sco
   lv_bsd=$lv_bsd
   lv_darwin=$lv_darwin
   lv_cygwin=$lv_cygwin
   lv_mingw=$lv_mingw
   lv_os_unknown=$lv_os_unknown

case $visual_size_type in
short)
  visual_size_format='"hu"'
  ;;
int)
  visual_size_format='"u"'
  ;;
long)
  visual_size_format='"lu"'
  ;;
esac

])
				    
AC_CONFIG_FILES([
Makefile
libvisual.pc
libvisual/Makefile
])
#
# For now this are not included in the package
#
#tests/Makefile
#tools/Makefile

AC_OUTPUT

echo "
=====================================================================
LIBVISUAL, AN AUDIO VISUALISATION ABSTRACTION LIBRARY, VERSION $VERSION
=====================================================================

install path                         : ${prefix}/lib
compiler                             : ${CC}
debug enabled                        : ${debug}
"

