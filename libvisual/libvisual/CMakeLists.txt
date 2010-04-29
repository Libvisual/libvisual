# DEPENDENCIES
# include(FindPkgConfig)
# pkg_search_module(XML REQUIRED  libxml-2.0>=2.4 libxml2>=2.4 libxml>=2.4)

include("${CMAKE_SOURCE_DIR}/cmake/FindLibDL.cmake")
if(NOT LIBDL_FOUND)

endif(NOT LIBDL_FOUND)

include_directories(
    ${LIBDL_INCLUDE_DIR}
)


set(libvisual_HEADERS
  lvconfig.h libvisual.h lv_actor.h
  lv_audio.h lv_bin.h lv_common.h lv_fourier.h
  lv_input.h lv_event.h lv_keysym.h
  lv_list.h lv_log.h lv_palette.h lv_plugin.h
  lv_video.h lv_libvisual.h lv_songinfo.h
  lv_morph.h lv_bmp.h lv_param.h lv_mem.h
  lv_mem_ops.h lv_mem_pool.h lv_mem_utils.h
  lv_bits.h lv_cpu.h lv_color.h
  lv_time.h lv_random.h lv_error.h lv_ui.h
  lv_types.h lv_thread.h lv_object.h
  lv_transform.h lv_rectangle.h lv_ringbuffer.h
  lv_buffer.h lv_config.h lv_hashmap.h
  lv_hashlist.h lv_queue.h lv_cache.h lv_collection.h
  lv_utils.h lv_os.h lv_math.h lv_gl.h
  lv_defines.h lv_beat.c)

set(lib_LTLIBRARIES
  libvisual-@LIBVISUAL_VERSION_SUFFIX@.la)

# TODO: check + link to libintl
# LIBS += -L. -L$(libdir) $(LV_THREAD_LIBS) @LTLIBINTL@


ADD_DEFINITIONS("-DPLUGPATH=\"$(LIBVISUAL_PLUGINS_BASE_DIR)\"")

include_directories(
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/libvisual
  ${CMAKE_BINARY_DIR}
  ${CMAKE_BINARY_DIR}/libvisual)


# TODO: do we still need  -no-undefined ?
ADD_DEFINITIONS(-export-dynamic)


set(libvisual_SRCS
  lv_actor.c lv_input.c lv_event.c
  lv_bin.c lv_plugin.c lv_video.c lv_video_simd.c lv_mem.c
  lv_mem_ops.c lv_mem_pool.c lv_mem_utils.c
  lv_audio.c lv_fourier.c lv_list.c lv_log.c lv_palette.c
  lv_libvisual.c lv_songinfo.c lv_morph.c lv_bmp.c lv_param.c
  lv_cpu.c lv_color.c lv_time.c lv_random.c lv_error.c lv_ui.c
  lv_thread.c lv_object.c lv_transform.c lv_rectangle.c lv_config.c
  lv_ringbuffer.c lv_buffer.c lv_cache.c lv_hashmap.c lv_hashlist.c
  lv_queue.c lv_collection.c lv_utils.c lv_os.c lv_math.c lv_gl.c
  gettext.h lv_beat.h)

set(lib_TARGET ${LIBRARY_NAME}-${LIBVISUAL_VERSION_SUFFIX})

add_library(${lib_TARGET} SHARED ${libvisual_SRCS} )

set_target_properties(${lib_TARGET}
PROPERTIES
    SOVERSION ${LIBRARY_SOVERSION}
    VERSION ${LIBRARY_VERSION}
)

target_link_libraries(${lib_TARGET}
  ${CMAKE_THREAD_LIBS_INIT}
  ${LIBDL_LIBRARIES})

# windows links to libm automatically
if(UNIX)
  target_link_libraries(${lib_TARGET} m)
endif(UNIX)


# INSTALL

install(TARGETS ${lib_TARGET} DESTINATION lib)
install(FILES ${libvisual_HEADERS} DESTINATION include/libvisual-${LIBVISUAL_VERSION_SUFFIX})


