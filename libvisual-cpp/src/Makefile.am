lib_LTLIBRARIES = libvisual-cpp.la

libvisual_cpp_includedir = $(includedir)/libvisual-cpp
libvisual_cpp_include_HEADERS = lv_thread.hpp lv_function.hpp

libvisual_cpp_la_LDFLAGS = -export-dynamic -version-info $(LVCPP_ABI_VERSION)
libvisual_cpp_la_CXXFLAGS = $(LV_CFLAGS)
libvisual_cpp_la_LIBADD = $(LV_LIBS)

libvisual_cpp_la_SOURCES = lv_thread.cpp lv_function.cpp
