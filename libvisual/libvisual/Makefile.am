## Process this file with automake to generate a Makefile.in

library_includedir = $(includedir)/libvisual
library_include_HEADERS = lvconfig.h libvisual.h lv_actor.h \
			  lv_audio.h lv_bin.h lv_common.h lv_fft.h \
			  lv_input.h lv_event.h lv_keysym.h \
			  lv_list.h lv_log.h lv_palette.h lv_plugin.h \
			  lv_video.h lv_libvisual.h lv_songinfo.h \
			  lv_morph.h lv_bmp.h lv_param.h lv_mem.h \
			  lv_endianess.h lv_cpu.h lv_color.h \
			  lv_time.h lv_random.h

lib_LTLIBRARIES = libvisual.la

DEFS = @DEFS@

LIBS += -L. -L$(libdir)

PLUGPATH = $(libdir)/libvisual

AM_CFLAGS = -Wall -Wno-unused-variable -Wmissing-prototypes -Wstrict-prototypes\
	    -DPLUGPATH="\"$(PLUGPATH)\"" @DEBUG_CFLAGS@ -O3

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/libvisual \
	   -I$(top_builddir) -I$(top_builddir)/libvisual

libvisual_la_LDFLAGS = -export-dynamic -no-undefined

libvisual_la_SOURCES = lv_actor.c lv_input.c lv_event.c lv_keysym.c \
		       lv_bin.c lv_plugin.c lv_video.c lv_mem.c\
		       lv_audio.c lv_fft.c lv_list.c lv_log.c lv_palette.c \
		       lv_libvisual.c lv_songinfo.c lv_morph.c lv_bmp.c lv_param.c \
		       lv_cpu.c lv_color.c lv_time.c lv_random.c

