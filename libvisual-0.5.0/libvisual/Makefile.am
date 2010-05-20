## Process this file with automake to generate a Makefile.in

library_includedir = $(LIBVISUAL_INCLUDE_DIR)/libvisual
library_include_HEADERS = lvconfig.h libvisual.h lv_actor.h \
			  lv_audio.h lv_bin.h lv_common.h lv_fourier.h \
			  lv_input.h lv_event.h lv_keysym.h \
			  lv_list.h lv_log.h lv_palette.h lv_plugin.h \
			  lv_video.h lv_libvisual.h lv_songinfo.h \
			  lv_morph.h lv_bmp.h lv_param.h lv_mem.h \
			  lv_bits.h lv_cpu.h lv_color.h \
			  lv_time.h lv_random.h lv_error.h lv_ui.h \
			  lv_types.h lv_thread.h lv_object.h \
			  lv_transform.h lv_rectangle.h lv_ringbuffer.h \
			  lv_buffer.h lv_config.h lv_hashmap.h \
			  lv_hashlist.h lv_cache.h lv_collection.h \
			  lv_utils.h lv_os.h lv_math.h lv_gl.h lv_defines.h

lib_LTLIBRARIES = libvisual-@LIBVISUAL_VERSION_SUFFIX@.la

DEFS = @DEFS@

LIBS += -L. -L$(libdir) $(LV_THREAD_LIBS) @LTLIBINTL@

PLUGPATH = $(libdir)/libvisual

AM_CFLAGS = -DPLUGPATH="\"$(LIBVISUAL_PLUGINS_BASE_DIR)\"" \
	$(LV_THREAD_CFLAGS)

INCLUDES = -I$(top_srcdir) -I$(top_srcdir)/libvisual \
	   -I$(top_builddir) -I$(top_builddir)/libvisual

libvisual_@LIBVISUAL_VERSION_SUFFIX@_la_LDFLAGS = -export-dynamic -no-undefined

libvisual_@LIBVISUAL_VERSION_SUFFIX@_la_SOURCES = lv_actor.c lv_input.c lv_event.c \
		       lv_bin.c lv_plugin.c lv_video.c lv_video_simd.c lv_mem.c \
		       lv_audio.c lv_fourier.c lv_list.c lv_log.c lv_palette.c \
		       lv_libvisual.c lv_songinfo.c lv_morph.c lv_bmp.c lv_param.c \
		       lv_cpu.c lv_color.c lv_time.c lv_random.c lv_error.c lv_ui.c \
		       lv_thread.c lv_object.c lv_transform.c lv_rectangle.c lv_config.c \
		       lv_ringbuffer.c lv_buffer.c lv_cache.c lv_hashmap.c lv_hashlist.c \
		       lv_collection.c lv_utils.c lv_os.c lv_math.c lv_gl.c gettext.h

