## Process this file with automake to generate a Makefile.in

bin_PROGRAMS = $(TOOLS_LV_TOOL)

EXTRA_PROGRAMS = lv-tool

lv_tool_SOURCES = lv-tool.cpp
lv_tool_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/libvisual
lv_tool_CXXFLAGS = $(SDL_CFLAGS)
lv_tool_LDADD = $(SDL_LIBS) ../../libvisual/libvisual-@LIBVISUAL_VERSION_SUFFIX@.la
