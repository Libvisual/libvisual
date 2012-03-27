# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.57)

m4_define([lv_avs_version_major], [0])
m4_define([lv_avs_version_minor], [5])
m4_define([lv_avs_version_micro], [0])
m4_define([lv_avs_version], [lv_avs_version_major.lv_avs_version_minor.lv_avs_version_micro])
m4_define([lv_avs_version_suffix], [lv_avs_version_major.lv_avs_version_minor])

AC_INIT(Libvisual avs, 0.5.0, ds@nerds-incorporated.org)
AM_INIT_AUTOMAKE([1.7.0 dist-bzip2])

AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])

m4_define([libvisual_required_version], [0.5.0])

m4_define([libvisual_pkg], [libvisual-lv_avs_version_suffix])
LIBVISUAL_PKG=libvisual_pkg

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LN_S

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

PKG_CHECK_MODULES(GLIB, [glib-2.0 >= 2.6.0],
    [ADD_PC_REQUIRES([glib-2.0 >= 2.6.0])],
    [AC_MSG_ERROR([Cannot find glib2 >= 2.6.0])]
)


PKG_CHECK_MODULES(XML, [libxml-2.0 >= 2.0.0],
    [ADD_PC_REQUIRES([glib-2.0 >= 2.6.0])],
	[AC_MSG_ERROR([Cannot find xml2])]
)

# Checks for libraries.
build_actor=""
build_trans=""

PKG_CHECK_MODULES(LIBVISUAL, libvisual_pkg = libvisual_required_version, [libvis="yes"], [libvis="no"])
if test x$libvis = xno; then
	AC_MSG_ERROR([*** libvisual not found.
			libvisual is needed to build this package.
			You can download libvisual at http://libvisual.sf.net/])
fi
AC_SUBST(LIBVISUAL_LIBS)
AC_SUBST(LIBVISUAL_CFLAGS)
	
# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
# Checks for header files.
AC_HEADER_TIME
AC_CHECK_HEADERS([fcntl.h unistd.h stdlib.h string.h sys/time.h])
AC_C_INLINE

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([floor gettimeofday memmove memset pow sqrt])
AC_CHECK_FUNC([munmap], lv_have_munmap=yes, lv_have_munmap=no)
AC_FUNC_MMAP

# The main plugin.
AC_ARG_ENABLE([avs],
	      AC_HELP_STRING([--disable-avs],
			     [Do not build AVS plugin @<:@default=enabled@:>@]),
	      [avs=$enableval],
	      [avs=yes])
AC_MSG_CHECKING([whether to build AVS plugin])
if test x$avs = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor avs"
  AC_SUBST([AVS], ['actor_AVS.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_stars],
	      AC_HELP_STRING([--disable-stars],
			     [Do not build the AVS render stars plugin @<:@default=enabled@:>@]),
	      [avs_stars=$enableval],
	      [avs_stars=yes])
AC_MSG_CHECKING([whether to build AVS render stars plugin])
if test x$avs_stars = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor stars"
  AC_SUBST([AVS_STARS], ['actor_avs_stars.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_ring],
	      AC_HELP_STRING([--disable-ring],
			     [Do not build the AVS render ring plugin @<:@default=enabled@:>@]),
	      [avs_ring=$enableval],
	      [avs_ring=yes])
AC_MSG_CHECKING([whether to build AVS render ring plugin])
if test x$avs_ring = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor ring"
  AC_SUBST([AVS_RING], ['actor_avs_ring.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_superscope],
	      AC_HELP_STRING([--disable-superscope],
			     [Do not build the AVS render superscope plugin @<:@default=enabled@:>@]),
	      [avs_superscope=$enableval],
	      [avs_superscope=yes])
AC_MSG_CHECKING([whether to build AVS render superscope plugin])
if test x$avs_superscope = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor superscope"
  AC_SUBST([AVS_SUPERSCOPE], ['actor_avs_superscope.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_timescope],
	      AC_HELP_STRING([--disable-timescope],
			     [Do not build the AVS render timescope plugin @<:@default=enabled@:>@]),
	      [avs_timescope=$enableval],
	      [avs_timescope=yes])
AC_MSG_CHECKING([whether to build AVS render timescope plugin])
if test x$avs_timescope = xyes; then
  AC_MSG_RESULT([yes])
  build_actor="$build_actor timescope"
  AC_SUBST([AVS_TIMESCOPE], ['actor_avs_timescope.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_multiplier],
	      AC_HELP_STRING([--disable-multiplier],
			     [Do not build the AVS trans multiplier plugin @<:@default=enabled@:>@]),
	      [avs_multiplier=$enableval],
	      [avs_multiplier=yes])
AC_MSG_CHECKING([whether to build AVS trans multiplier plugin])
if test x$avs_multiplier = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans multiplier"
  AC_SUBST([AVS_MULTIPLIER], ['transform_avs_multiplier.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_channelshift],
	      AC_HELP_STRING([--disable-channelshift],
			     [Do not build the AVS trans channelshift plugin @<:@default=enabled@:>@]),
	      [avs_channelshift=$enableval],
	      [avs_channelshift=yes])
AC_MSG_CHECKING([whether to build AVS trans channelshift plugin])
if test x$avs_channelshift = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans channelshift"
  AC_SUBST([AVS_CHANNELSHIFT], ['transform_avs_channelshift.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_bump],
	      AC_HELP_STRING([--disable-bump],
			     [Do not build the AVS trans bump plugin @<:@default=enabled@:>@]),
	      [avs_bump=$enableval],
	      [avs_bump=yes])
AC_MSG_CHECKING([whether to build AVS trans bump plugin])
if test x$avs_bump = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans bump"
  AC_SUBST([AVS_BUMP], ['transform_avs_bump.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_movement],
	      AC_HELP_STRING([--disable-movement],
			     [Do not build the AVS trans movement plugin @<:@default=enabled@:>@]),
	      [avs_movement=$enableval],
	      [avs_movement=yes])
AC_MSG_CHECKING([whether to build AVS trans movement plugin])
if test x$avs_movement = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans movement"
  AC_SUBST([AVS_MOVEMENT], ['transform_avs_movement.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_invert],
	      AC_HELP_STRING([--disable-invert],
			     [Do not build the AVS trans invert plugin @<:@default=enabled@:>@]),
	      [avs_invert=$enableval],
	      [avs_invert=yes])
AC_MSG_CHECKING([whether to build AVS trans invert plugin])
if test x$avs_invert = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans invert"
  AC_SUBST([AVS_INVERT], ['transform_avs_invert.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_onetone],
	      AC_HELP_STRING([--disable-onetone],
			     [Do not build the AVS trans onetone plugin @<:@default=enabled@:>@]),
	      [avs_onetone=$enableval],
	      [avs_onetone=yes])
AC_MSG_CHECKING([whether to build AVS trans onetone plugin])
if test x$avs_onetone = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans onetone"
  AC_SUBST([AVS_ONETONE], ['transform_avs_onetone.la'])
else
  AC_MSG_RESULT([no])
fi

# The AVS elements.
AC_ARG_ENABLE([avs_grain],
	      AC_HELP_STRING([--disable-grain],
			     [Do not build the AVS trans grain plugin @<:@default=enabled@:>@]),
	      [avs_grain=$enableval],
	      [avs_grain=yes])
AC_MSG_CHECKING([whether to build AVS trans grain plugin])
if test x$avs_grain = xyes; then
  AC_MSG_RESULT([yes])
  build_trans="$build_trans grain"
  AC_SUBST([AVS_GRAIN], ['transform_avs_grain.la'])
else
  AC_MSG_RESULT([no])
fi


# The AVS elements
AC_ARG_ENABLE([avs_dmovement],
        AC_HELP_STRING([--disable-dmovement],
            [Do not build the AVS dynamic movement plugin @<:@default=enabled@:>@]),
        [avs_dmovement=$enableval],
        [avs_dmovement=yes])
AC_MSG_CHECKING([Whether to build AVS dynamic movement plugin])
if test x$avs_dmovement = xyes; then
    AC_MSG_RESULT([yes])
    build_trans="$build_trans dmovement"
    AC_SUBST([AVS_DMOVEMENT], ['transform_avs_dmovement.la'])
else
    AC_MSG_RESULT([no])
fi

# The AVS elements
AC_ARG_ENABLE([avs_blur],
        AC_HELP_STRING([--disable-blur],
            [Do not build the AVS blur plugin @<:@default=enabled@:>@]),
        [avs_blur=$enableval],
        [avs_blur=yes])
AC_MSG_CHECKING([Whether to build AVS blur plugin])
if test x$avs_blur = xyes; then
    AC_MSG_RESULT([yes])
    build_trans="$build_trans blur"
    AC_SUBST([AVS_BLUR], ['transform_avs_blur.la'])
else
    AC_MSG_RESULT([no])
fi

# The AVS elements
AC_ARG_ENABLE([avs_water],
        AC_HELP_STRING([--disable-water],
            [Do not build the AVS water plugin @<:@default=enabled@:>@]),
        [avs_water=$enableval],
        [avs_water=yes])
AC_MSG_CHECKING([Whether to build AVS water plugin])
if test x$avs_water = xyes; then
    AC_MSG_RESULT([yes])
    build_trans="$build_trans water"
    AC_SUBST([AVS_WATER], ['transform_avs_water.la'])
else
    AC_MSG_RESULT([no])
fi

AC_ARG_ENABLE([avs_clear],
        AC_HELP_STRING([--disable-clear],
            [Do not build the AVS clear plugin @<:@default=enabled@:>@]),
        [avs_clear=$enableval],
        [avs_clear=yes])
AC_MSG_CHECKING([Whether to build AVS clear plugin])
if test x$avs_clear = xyes; then
    AC_MSG_RESULT([yes])
    build_trans="$build_trans clear"
    AC_SUBST([AVS_CLEAR], ['transform_avs_clear.la'])
else
    AC_MSG_RESULT([no])
fi

#
# Build paramaters
#
DEBUG_CFLAGS=""

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
AC_MSG_CHECKING([whether to enable debug])
if test "$debug" = yes; then
  AC_MSG_RESULT([yes])
  DEBUG_CFLAGS="$DEBUG_CFLAGS -ggdb3"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wall -Wmissing-braces -Wimplicit -Wunused"
  DEBUG_CFLAGS="$DEBUG_CFLAGS -Wmissing-prototypes -Wno-unused-variable"
else
  AC_MSG_RESULT([no])
fi
AC_SUBST(DEBUG_CFLAGS)

AC_ARG_ENABLE([extra-optimization],
              AC_HELP_STRING([--enable-extra-optimization],
                             [Enable extra optimizations @<:@default=disabled@:>@]),
              [extra_opt=$enableval],
              [extra_opt=no])
AC_MSG_CHECKING([wheter to enable extra optimizations])
if test "$extra_opt" = yes; then
  AC_MSG_RESULT([yes])
  OPT_CFLAGS="-O3 -fexpensive-optimizations"
else
  AC_MSG_RESULT([no])
  if test "$debug" = yes; then
	OPT_CFLAGS="-O0"
  else
	OPT_CFLAGS=""
  fi
fi
AC_SUBST(OPT_CFLAGS)

dnl use pretty build output with automake >= 1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
  [AM_DEFAULT_VERBOSITY=1
   AC_SUBST(AM_DEFAULT_VERBOSITY)])


AC_OPENMP()
AC_SUBST(OPENMP_CFLAGS)
AC_CHECK_LIB(gomp, omp_get_num_threads, HAVE_OPENMP="yes")
AC_SUBST(HAVE_OPENMP)
if test $HAVE_OPENMP = "yes"; then
  LIBVISUAL_LIBS+=-lgomp
fi

AC_SUBST(CFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${OPENMP_CFLAGS}")
AC_SUBST(CXXFLAGS, "${CFLAGS} ${DEBUG_CFLAGS} ${OPT_CFLAGS} ${OPENMP_CXXFLAGS}")

AC_ARG_ENABLE([mmx],
            AC_HELP_STRING([--enable-mmx],
                        [Enable mmx support @<:@default=disabled@:>@]),
            [with_mmx=$enableval],
            [with_mmx=no])
AC_MSG_CHECKING([Whether to enable mmx support])
if test "$with_mmx" = yes; then
    AC_MSG_RESULT([yes])
else
    AC_DEFINE(NO_MMX, 1, "User doesn't want mmx support")
    AC_MSG_RESULT([no])
fi

AC_ARG_WITH([plugins_base_dir],
            AC_HELP_STRING([--with-plugins_base_dir="dir"],
                           [Install plugins to this base directory. Defaults to Libvisual's base dir ]),
dnl ' # fix string coloring ;)
	    [plugins_base_dir=$withval],
	    [plugins_base_dir=])

AC_MSG_CHECKING([where to install plugins])

AC_ARG_WITH([plugins-base-dir],
  AS_HELP_STRING([--with-plugins-base-dir=DIR],
    [Install plugins to this base directory. Defaults to Libvisual's base dir]),
  [plugins_base_dir=$withval],
  [plugins_base_dir=`$PKG_CONFIG --variable=pluginsbasedir $LIBVISUAL_PKG`])

LIBVISUAL_PLUGINS_BASE_DIR="$plugins_base_dir"
AC_SUBST([LIBVISUAL_PLUGINS_BASE_DIR])
AC_SUBST([actor_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/actor])
AC_SUBST([input_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/input])
AC_SUBST([morph_plugindir], [$LIBVISUAL_PLUGINS_BASE_DIR/morph])

AC_SUBST(LIBVISUAL_PLUGINS_DATA_DIR, [$datadir/libvisual-plugins-lv_avs_version_suffix])

libs_dynamic_loader="-ldl"
AC_SUBST(LIBS_DYNAMIC_LOADER, "$libs_dynamic_loader")

AC_SUBST(DATADIR, "$datadir")

AC_CONFIG_FILES([
	Makefile
	common/Makefile
	src/Makefile
	plugins/Makefile
	plugins/actor/Makefile
	plugins/actor/ring/Makefile
	plugins/actor/superscope/Makefile
	plugins/transform/Makefile
	plugins/transform/movement/Makefile
	plugins/transform/dmovement/Makefile
        plugins/transform/blur/Makefile
        plugins/transform/clear/Makefile
        plugins/transform/water/Makefile
        plugins/actor/timescope/Makefile
        plugins/actor/stars/Makefile
	visscript/Makefile
])


AC_OUTPUT

echo "
================================================
LIBVISUAL-AVS, ADVANCED VISUAL STUDIO FOR LIBVISUAL $VERSION
================================================

Packages found:
libvisual:                            : ${libvis}

Plugins being build:
actor:                                : ${build_actor}
transform:                            : ${build_trans}

Libvisual plugin data dir:            : ${prefix}/share

Build options:
install path                          : $plugins_base_dir
source code location:                 : ${srcdir}
compiler:                             : ${CC}

Other options:
Debug support:                        : ${debug}
Extra optimization                    : ${extra_opt}
OpenMP support (multicore):           : ${HAVE_OPENMP}
"

