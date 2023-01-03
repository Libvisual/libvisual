## Process this file with automake to generate a Makefile.in

input_plugin_LTLIBRARIES = input_pulseaudio.la

input_pulseaudio_la_SOURCES = input_pulseaudio.c
input_pulseaudio_la_CFLAGS = $(LIBVISUAL_CFLAGS) $(LIBPULSE_CFLAGS)
input_pulseaudio_la_LIBADD = $(LIBVISUAL_LIBS) $(LIBPULSE_LIBS)
input_pulseaudio_la_LDFLAGS = -module -avoid-version
