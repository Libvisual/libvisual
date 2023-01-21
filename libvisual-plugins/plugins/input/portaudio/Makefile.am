## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_portaudio.la

input_portaudio_la_SOURCES = input_portaudio.c
input_portaudio_la_CPPFLAGS = -I$(top_srcdir)
input_portaudio_la_CFLAGS = $(LIBVISUAL_CFLAGS) $(PORTAUDIO_CFLAGS)
input_portaudio_la_LIBADD = $(LIBVISUAL_LIBS) $(PORTAUDIO_LIBS)
input_portaudio_la_LDFLAGS = -module -avoid-version
