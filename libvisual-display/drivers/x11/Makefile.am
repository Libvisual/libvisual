## Process this file with automake to generate a Makefile.in
libdir=@LIBVISUAL_PLUGINS_BASE_DIR@/display

lib_LTLIBRARIES = x11_new.la x11.la

if HAVE_XF86VMODE
lib_LTLIBRARIES += x11_fullscreen.la
endif

LIBS = -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir)

x11_la_LDFLAGS = -module -avoid-version -lX11 -lXext
x11_new_la_LDFLAGS = -module -avoid-version -lX11
x11_fullscreen_la_LDFLAGS = -module -avoid-version -lX11 @XF86VM_LIB@

x11_la_SOURCES = x11.c
x11_new_la_SOURCES = x11_new.c
x11_fullscreen_la_SOURCES = x11_fullscreen.c

