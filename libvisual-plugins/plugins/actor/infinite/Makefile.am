## Process this file with automake to generate a Makefile.in

pkglib_LTLIBRARIES = $(INFINITE)

EXTRA_LTLIBRARIES = actor_infinite.la

pkglibdir = $(prefix)/lib/libvisual/actor

LIBS += -L. -L$(prefix)/lib @LIBVISUAL_LIBS@

AM_CFLAGS = -Wall -rdynamic @DEBUG_CFLAGS@ @LIBVISUAL_CFLAGS@ -O3

INCLUDES = $(all_includes) -I$(top_srcdir) 

actor_infinite_la_LDFLAGS = -rpath '$(pkglibdir)' -module -avoid-version 

actor_infinite_la_SOURCES = compute.c compute.h\
			    display.c display.h\
			    renderer.c renderer.h\
			    file.c lv_infinite.c\
			    main.h

