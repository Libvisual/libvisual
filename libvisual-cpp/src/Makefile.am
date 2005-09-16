lib_LTLIBRARIES = libvisual-cpp.la

libvisual_cpp_includedir = $(includedir)/libvisual-cpp
libvisual_cpp_include_HEADERS = \
	libvisual_cpp.hpp	\
	lv_thread.hpp		\
	lv_function.hpp		\
	lv_object.hpp		\
	lv_color.hpp		\
	lv_rectangle.hpp 	\
	lv_cpu.hpp		\
	lv_random.hpp		\
	lv_fft.hpp		\
	lv_time.hpp

libvisual_cpp_la_LDFLAGS = -export-dynamic -version-info $(LVCPP_ABI_VERSION)
libvisual_cpp_la_CXXFLAGS = $(LV_CFLAGS)
libvisual_cpp_la_LIBADD = $(LV_LIBS)

libvisual_cpp_la_SOURCES =	\
	libvisual_cpp.cpp	\
	lv_thread.cpp		\
	lv_function.cpp		\
	lv_object.cpp		\
	lv_color.cpp		\
	lv_rectangle.cpp	\
	lv_cpu.cpp		\
	lv_random.cpp		\
	lv_fft.cpp		\
	lv_time.cpp
