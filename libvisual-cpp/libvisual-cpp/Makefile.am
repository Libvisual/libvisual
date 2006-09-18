lib_LTLIBRARIES = libvisual-cpp.la

libvisual_cpp_includedir = $(includedir)/libvisual-cpp
libvisual_cpp_include_HEADERS = \
	libvisual_cpp.hpp	\
	lv_color.hpp		\
	lv_config.hpp		\
	lv_cpu.hpp		\
	lv_error.hpp		\
	lv_flags_wrapper.hpp	\
	lv_fourier.hpp		\
	lv_math.hpp		\
	lv_object.hpp		\
	lv_os.hpp		\
	lv_random.hpp		\
	lv_rectangle.hpp 	\
	lv_palette.hpp		\
	lv_thread.hpp		\
	lv_time.hpp		\
	lv_utils.hpp		\
	lv_video.hpp

libvisual_cpp_la_LDFLAGS = -export-dynamic -version-info $(LVCPP_ABI_VERSION)
libvisual_cpp_la_CXXFLAGS = $(LV_CFLAGS) -I $(top_srcdir)
libvisual_cpp_la_LIBADD = $(LV_LIBS)

libvisual_cpp_la_SOURCES =	\
	libvisual_cpp.cpp	\
	lv_thread.cpp
