## Process this file with automake to generate a Makefile.in

lib_LTLIBRARIES = $(INFINITE)

EXTRA_LTLIBRARIES = actor_infinite.la

libdir = @LIBVISUAL_PLUGINS_BASE_DIR@/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = @LIBVISUAL_CFLAGS@

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_infinite_la_LDFLAGS = -rpath $(libdir) -module -avoid-version 

actor_infinite_la_SOURCES = compute.c compute.h\
			    display.c display.h\
			    renderer.c renderer.h\
			    file.c lv_infinite.c\
			    main.h

