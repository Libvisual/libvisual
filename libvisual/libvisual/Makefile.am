lib_LTLIBRARIES = libvisual.la

libdir = $(prefix)/lib
library_includedir = $(includedir)/libvisual
library_include_HEADERS = libvisual.h lv_actor.h \
			  lv_audio.h lv_bin.h lv_common.h lv_fft.h \
			  lv_input.h lv_event.h lv_keysym.h \
			  lv_list.h lv_log.h lv_palette.h lv_plugin.h \
			  lv_video.h lv_libvisual.h lv_songinfo.h \
			  lv_morph.h lv_bmp.h

DEFS = @DEFS@

LIBS = -L. -L$(prefix)/lib

PLUGPATH = $(prefix)/lib/libvisual

AM_CFLAGS = -ggdb -DPLUGPATH="\"$(PLUGPATH)\""

INCLUDES = $(all_includes) \
	-I$(top_srcdir)

libvisual_la_LDFLAGS = -export-dynamic

libvisual_la_SOURCES = lv_actor.c lv_input.c lv_event.c lv_keysym.c \
		       lv_bin.c lv_plugin.c lv_video.c \
		       lv_audio.c lv_fft.c lv_list.c lv_log.c lv_palette.c \
		       lv_libvisual.c lv_songinfo.c lv_morph.c lv_bmp.c

