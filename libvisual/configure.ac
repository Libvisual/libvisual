dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.57)

dnl Version numbers
m4_define([libvisual_version_major], [0])
m4_define([libvisual_version_minor], [4])
m4_define([libvisual_version_micro], [0])

m4_define([libvisual_version], [libvisual_version_major.libvisual_version_minor.libvisual_version_micro])

AC_INIT([Libvisual Library], [libvisual_version], [synap@nerds-incorporated.org], [libvisual])
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

dnl TODO: Add these into lv_config.h (or maybe lv_version.h)
LIBVISUAL_VERSION_MAJOR=libvisual_version_major
LIBVISUAL_VERSION_MINOR=libvisual_version_minor
LIBVISUAL_VERSION_MICRO=libvisual_version_micro
LIBVISUAL_VERSION=libvisual_version

LIBVISUAL_VERSION_SUFFIX="$LIBVISUAL_VERSION_MAJOR.$LIBVISUAL_VERSION_MINOR"
AC_SUBST(LIBVISUAL_VERSION_SUFFIX)

AC_CONFIG_HEADER(config.h)

AC_CANONICAL_HOST

lv_mips=no
lv_alpha=no
lv_sparc=no
lv_ix86=no
lv_x86_64=no
lv_powerpc=no

case "$host_cpu" in
	mips)           lv_mips=yes;;
	alpha)          lv_alpha=yes;;
	sparc)          lv_sparc=yes;;
	i*86)           lv_ix86=yes;;
	x86_64*)	lv_x86_64=yes;;
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
lv_freebsd=no
lv_netbsd=no
lv_openbsd=no
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
	freebsd*)	lv_freebsd=yes;;
	netbsd*)	lv_netbsd=yes;;
	openbsd*)	lv_openbsd=yes;;
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
AC_CHECK_HEADERS([fcntl.h sys/types.h stdlib.h string.h sys/time.h unistd.h sched.h sys/sched.h])

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
AC_CHECK_FUNCS([gettimeofday usleep nanosleep select memset sqrt strdup strndup sysconf])

AC_CHECK_LIB([dl], [dlsym], [LIBS="-ldl $LIBS"],
	     AC_MSG_WARN([*** You need dlsym() to make use of Libvisual.]))
AC_CHECK_LIB([m], [sqrt], [LIBS="-lm $LIBS"],
	     AC_MSG_ERROR([*** You need the Standard C Math Library to build Libvisual.]))

# Internationalization
GETTEXT_PACKAGE="libvisual-${LIBVISUAL_VERSION_SUFFIX}"
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [GETTEXT package name])
AC_DEFINE_UNQUOTED(LOCALEDIR, "${prefix}/share/locale", [String catalog location])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.14.1])

dnl -------------------------------
dnl Check for native thread support
dnl -------------------------------

dnl
dnl Test program for basic POSIX threads functionality (taken from Glib)
dnl
m4_define([lv_thread_test],[
#include <pthread.h> 
int check_me = 0;
void* func(void* data) {check_me = 42; return &check_me;}
int main()
 { pthread_t t; 
   void *ret;
   pthread_create (&t, $1, func, 0);
   pthread_join (t, &ret);
   exit (check_me != 42 || ret != &check_me);
}])

LIBS_NOT_FOUND_1="I can't find the libraries for the thread implementation
		"

LIBS_NOT_FOUND_2=". Please choose another thread implementation or
		provide information on your thread implementation.
		You can also run 'configure --disable-threads' 
		to compile without thread support."

AIX_COMPILE_INFO="AIX's C compiler needs to be called by a different name, when
		linking threaded applications. As Libvisual cannot do that 
		automatically, you will get an linkg error everytime you are 
		not using the right compiler. In that case you have to relink 
		with the right compiler. Ususally just '_r' is appended 
		to the compiler name."

AC_ARG_ENABLE([enable_threads],
  AC_HELP_STRING([--enable-threads],
		[Turn on basic thread support @<:@default=enabled@:>@]),
		[enable_threads=$enableval],
		[enable_threads=yes])
AC_MSG_CHECKING([whether to enable thread support])
if test x$enable_threads = xyes; then
  AC_MSG_RESULT([yes])
  have_threads=none
  AC_MSG_CHECKING([for thread implementation available])
  case $host in
  	*-*-mingw*)
		have_threads=win32
		;;
	*)
		;;
	esac
  if test "x$have_threads" = xnone; then
    AC_TRY_COMPILE([#include <pthread.h>],
		[pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;],
		have_threads=posix)
  fi
  if test "x$have_threads" = xnone; then
    AC_TRY_COMPILE([#include <pthread.h>],
		[pthread_mutex_t m; 
		pthread_mutex_init (&m, pthread_mutexattr_default);],
		have_threads=dce)
  fi
  if test "x$have_threads" = xnone; then
    PKG_CHECK_MODULES(GTHREAD2, gthread-2.0 >= 2.2.0, [have_gthread2="yes"], [have_gthread2="no"])
    if test x"$have_gthread2" = xyes; then
      have_threads=gthread2
      AC_SUBST(GTHREAD2_CFLAGS)
      AC_SUBST(GTHREAD2_LIBS)
      AC_DEFINE([LV_HAVE_GTHREAD2],[1],[glib-2.0 thread implementation available])
    fi
  fi
  # In future we will check for more thread models...

  if test "x$have_threads" = xnone; then
    AC_MSG_RESULT(none available)
    AC_MSG_WARN([You do not have any supported thread system available. Libvisual will be built without thread support.])
  else
    AC_MSG_RESULT($have_threads)
  fi
fi


dnl ------------------------------------------------
dnl Determination of linker flags for thread support
dnl ------------------------------------------------

LV_THREAD_LIBS=""
if test "x$enable_threads" = xyes && test "x$have_threads" != xnone; then
  case $have_threads in
        posix|dce)
          case $host in
            *-aix*)
              # We are not using gcc (would have set LV_THREAD_LIBS) and thus 
              # probably using the aix compiler.
		AC_MSG_WARN($AIX_COMPILE_INFO)
                ;;
            *)
              LV_THREAD_LIBS=error
	      lv_save_LIBS="$LIBS"
	      for thread_lib in "" pthread pthread32 pthreads thread dce; do
		if test x"$thread_lib" = x; then
			add_thread_lib=""
			IN=""
else
			add_thread_lib="-l$thread_lib"
			IN=" in -l$thread_lib"
		fi
		if test x"$have_threads" = xposix; then
			defattr=0
		else
			defattr=pthread_attr_default
		fi
		
		LIBS="$add_thread_lib $lv_save_LIBS"
		
		AC_MSG_CHECKING(for pthread_create/pthread_join$IN)
		AC_TRY_RUN(lv_thread_test($defattr),
                                  lv_result=yes,
                                  lv_result=no,
                                  [AC_LINK_IFELSE(lv_thread_test($defattr),
                                                  lv_result=yes,
                                                  lv_result=no)])
                AC_MSG_RESULT($lv_result)
			
              	if test "$lv_result" = "yes" ; then
			LV_THREAD_LIBS="$add_thread_lib"
                	break
              	fi
	      done

	      if test "x$LV_THREAD_LIBS" = xerror; then
		AC_MSG_ERROR($LIBS_NOT_FOUND_1$have_threads$LIBS_NOT_FOUND_2)
fi

	      LIBS="$lv_save_LIBS"
              ;;
            esac
            ;;
	  gthread2)
            LV_THREAD_LIBS="$GTHREAD2_LIBS"
            ;;
          *)
            ;;
  esac
else
  # Thread support disabled
  :
fi

AC_MSG_CHECKING(thread related libraries)
AC_MSG_RESULT($LV_THREAD_LIBS)
AC_SUBST(LV_THREAD_LIBS)

dnl --------------------------------------------------
dnl Determination of compiler flags for thread support
dnl --------------------------------------------------

LV_THREAD_CFLAGS=""
if test "x$enable_threads" = xyes && test "x$have_threads" != xnone; then
  case $have_threads in
	  gthread2)
            LV_THREAD_CFLAGS="$GTHREAD2_CFLAGS"
            ;;
          *)
	    ;;
  esac
fi
AC_SUBST(LV_THREAD_CFLAGS)

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

build_subdirs="libvisual"

VISUAL_PKG_CONFIG_DEP=""

VISUAL_CFLAGS=""
if test x"$LV_THREAD_CFLAGS" == xgthread2; then
  VISUAL_PKG_CONFIG_DEP="$VISUAL_PKG_CONFIG_DEP gthread-2.0"
elif test x"$LV_THREAD_CFLAGS" != x; then
  VISUAL_CFLAGS=$LV_THREAD_CFLAGS
fi
AC_SUBST(VISUAL_PKG_CONFIG_DEP)

VISUAL_LIBS="$LIBS -lvisual-${LIBVISUAL_VERSION_SUFFIX}"
if test x"$LV_THREAD_LIBS" != x; then
  VISUAL_LIBS="$LV_THREAD_LIBS $VISUAL_LIBS"
fi
AC_SUBST(VISUAL_CFLAGS)		 
AC_SUBST(VISUAL_LIBS)

dnl ******************************
dnl Arguments to specify certain features.
dnl ******************************

dnl ******************************
dnl Debugging stuff 
dnl ******************************
DEBUG_CFLAGS=""
AC_ARG_ENABLE([profiling],
  AC_HELP_STRING([--enable-profiling],
		[Enable emision of profile data to be used by gprof @<:@default=disabled@:>@]),
		[profiling=$enableval],
		[profiling=no])
AC_MSG_CHECKING([whether to enable profiling])
if test x$profiling = xyes; then
  AC_MSG_RESULT([yes])
  if test x"$GCC" = xyes; then
  DEBUG_CFLAGS="$DEBUG_CFLAGS -pg"
else
    AC_MSG_WARN([Profiling only works when using a gcc compiler.])
  fi
else
  AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([debug],
            AC_HELP_STRING([--enable-debug],
                           [Enable debug @<:@default=disabled@:>@]),
	    [debug=$enableval],
	    [debug=no])
AC_MSG_CHECKING([whether to enable debug])
if test x$debug = xyes; then
  AC_MSG_RESULT([yes])
  if test x"$GCC" = xyes; then
    DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3 -Wall"
    DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-braces -Wimplicit -Wunused -Wmissing-prototypes"
  # IMHO -Werror should be here also. --VVB
    lv_save_CFLAGS=$CFLAGS
    CFLAGS="$CFLAGS -Wno-unused-variable"
    AC_MSG_CHECKING([whether gcc version is greater than 2.95])
    AC_TRY_COMPILE([#include <stdlib.h>],
		   [exit(0);],
		   [lv_gcc_gt_2_95=yes],
		   [lv_gcc_gt_2_95=no])
    CFLAGS=$lv_save_CFLAGS
    if test x$lv_gcc_gt_2_95 = xyes ; then
      AC_MSG_RESULT([yes])
      DEBUG_CFLAGS="$DEBUG_CFLAGS -Wno-unused-variable"
    else
      AC_MSG_RESULT([no])
    fi
  fi
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(DEBUG_CFLAGS)

AC_ARG_ENABLE([extra-optimization],
           AC_HELP_STRING([--enable-extra-optimization],
                          [Enable extra optimizations @<:@default=disabled@:>@]),
           [extra_opt=$enableval],
           [extra_opt=no])
AC_MSG_CHECKING([whether to enable extra optimizations])
if test x$extra_opt = xyes; then
  AC_MSG_RESULT([yes])
  if test x"$GCC" = xyes; then
  OPT_CFLAGS="-O3 -fexpensive-optimizations"
else
    AC_MSG_WARN([Extra optimizations only works with a gcc compiler.])
  fi
else
  AC_MSG_RESULT([no])
  if test x$debug = xyes && test x"$GCC" = xyes; then
    OPT_CFLAGS="-O0"
  else
    OPT_CFLAGS=""
  fi
fi

AC_ARG_ENABLE([fast-fp-rng],
           AC_HELP_STRING([--enable-fast-fp-rng],
                          [Enable faster random floating point generator (enabled by default if --enable-extra-optimization is passed) ]),
           [fastfprng=$enableval],
           [fastfprng=maybe])
AC_MSG_CHECKING([whether to enable fast FP RNG])

if  test x$fastfprng = xyes; then
  AC_MSG_RESULT([yes])
elif test x$fastfprng = xmaybe; then
  if test x$extra_opt = xyes; then
    AC_MSG_RESULT([yes])
    fastfprng=yes
  else
    AC_MSG_RESULT([no])
    fastfprng=no
  fi
else
  AC_MSG_RESULT([no])
  fastfprng=no
fi


#define VISUAL_RANDOM_FAST_FP_RND 1

if test x$lv_mingw = xyes ; then
OPT_CFLAGS="-mwindows"
fi
AC_SUBST(OPT_CFLAGS)

AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS}")

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

dnl Installation paths
LIBVISUAL_INCLUDE_DIR="${includedir}/libvisual-${LIBVISUAL_VERSION_SUFFIX}"
LIBVISUAL_PLUGINS_BASE_DIR="${libdir}/libvisual-${LIBVISUAL_VERSION_SUFFIX}"
AC_SUBST(LIBVISUAL_INCLUDE_DIR)
AC_SUBST(LIBVISUAL_PLUGINS_BASE_DIR)

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

        if test x$fastfprng = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_RANDOM_FAST_FP_RNG (1)
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

#define VISUAL_ARCH_X86
_______EOF
        fi

        if test x$lv_x86_64 = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_ARCH_X86_64
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

        if test x$lv_freebsd = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_FREEBSD
_______EOF
        fi

        if test x$lv_netbsd = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_NETBSD
_______EOF
        fi

	if test x$lv_openbsd = xyes ; then
                cat >>$outfile <<_______EOF

#define VISUAL_OS_OPENBSD
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

        if test x$lv_enable_threads = xyes && test x$lv_have_threads != xnone ; then
		cat >>$outfile <<_______EOF

#define VISUAL_HAVE_THREADS
_______EOF
		case $lv_have_threads in
			win32)
		                cat >>$outfile <<_______EOF

#define VISUAL_THREAD_MODEL_WIN32
_______EOF
				;;
			posix)
		                cat >>$outfile <<_______EOF

#define VISUAL_THREAD_MODEL_POSIX
_______EOF
				;;
			dce)
		                cat >>$outfile <<_______EOF

#define VISUAL_THREAD_MODEL_DCE
_______EOF
				;;
			gthread2)
				cat >>$outfile <<_______EOF
#define VISUAL_THREAD_MODEL_GTHREAD2
_______EOF
				;;
			*)
				;;
		esac
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
   fastfprng=$fastfprng
   lv_win32=$lv_win32
 
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

   lv_have_threads=$have_threads
   lv_enable_threads=$enable_threads

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
m4/Makefile
po/Makefile.in
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

plugins_base_dir=`eval echo \`eval echo ${LIBVISUAL_PLUGINS_BASE_DIR}\``

echo "
=====================================================================
LIBVISUAL, AN AUDIO VISUALISATION ABSTRACTION LIBRARY, VERSION $VERSION
=====================================================================

install path                         : ${prefix}/lib
plugins base directory               : $plugins_base_dir
compiler                             : ${CC}
debug enabled                        : ${debug}
"
