INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}/libvisual
  ${PROJECT_BINARY_DIR}/libvisual
  ${THREAD_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${PROJECT_BINARY_DIR}/libvisual
)

SET(libvisual_HEADERS
  libvisual.h
  lv_actor.h
  lv_audio.h
  lv_bin.h
  lv_common.h
  lv_event.h
  lv_fourier.h
  lv_input.h
  lv_keysym.h
  lv_list.h
  lv_log.h
  lv_palette.h
  lv_plugin.h
  lv_plugin_registry.h
  lv_video.h
  lv_libvisual.h
  lv_songinfo.h
  lv_morph.h
  lv_bmp.h
  lv_param.h
  lv_mem.h
  lv_bits.h
  lv_cpu.h
  lv_color.h
  lv_beat.h
  lv_time.h
  lv_random.h
  lv_error.h
  lv_checks.h
  lv_types.h
  lv_thread.h
  lv_object.h
  lv_transform.h
  lv_rectangle.h
  lv_ringbuffer.h
  lv_buffer.h
  lv_config.h
  lv_collection.h
  lv_os.h
  lv_math.h
  lv_gl.h
  lv_defines.h
  lv_alpha_blend.h
  lv_util.h

  lv_module.hpp
  lv_intrusive_ptr.hpp
  lv_scoped_ptr.hpp
  lv_singleton.hpp
  lv_util.hpp

  ${PROJECT_BINARY_DIR}/libvisual/lvconfig.h
)

SET(libvisual_SOURCES
  lv_bin.c
  lv_video.c
  lv_video_simd.c
  lv_mem.c
  lv_audio.c
  lv_list.c
  lv_log.c
  lv_beat.c
  lv_bmp.c
  lv_param.c
  lv_cpu.c
  lv_error.c
  lv_object.c
  lv_config.c
  lv_ringbuffer.c
  lv_buffer.c
  lv_collection.c
  lv_os.c
  lv_math.c
  lv_gl.c
  lv_alpha_blend.c
  lv_util.c

  lv_actor.cpp
  lv_color.cpp
  lv_event.cpp
  lv_fourier.cpp
  lv_input.cpp
  lv_libvisual.cpp
  lv_morph.cpp
  lv_plugin.cpp
  lv_random.cpp
  lv_palette.cpp
  lv_plugin_registry.cpp
  lv_rectangle.cpp
  lv_songinfo.cpp
  lv_time.cpp
  lv_transform.cpp

  lv_color_c.cpp
  lv_event_c.cpp
  lv_fourier_c.cpp
  lv_libvisual_c.cpp
  lv_random_c.cpp
  lv_palette_c.cpp
  lv_plugin_registry_c.cpp
  lv_rectangle_c.cpp
  lv_songinfo_c.cpp
  lv_time_c.cpp

  private/lv_video_convert.c
  private/lv_video_fill.c
  private/lv_video_scale.c
  private/lv_video_blit.c
)

SET(LINK_LIBS
  ${THREAD_LIBS}
  m
)


# FIXME: Refactor this; and also define a POSIX flag so we can stop
# saying 'not Windows'
IF(NOT VISUAL_OS_WIN32)
  LIST(APPEND LINK_LIBS dl rt)

  LIST(APPEND libvisual_SOURCES
	private/posix/lv_module_dlfcn.cpp
  )

  IF(ENABLE_THREADS)
    LIST(APPEND libvisual_SOURCES
      private/posix/lv_thread_posix.c
    )
  ELSE(ENABLE_THREADS)
    LIST(APPEND libvisual_SOURCES
      lv_thread_disabled.c
    )
  ENDIF(ENABLE_THREADS)

ELSE(NOT VISUAL_OS_WIN32)

  LIST(APPEND libvisual_SOURCES
	private/windows/lv_module_win32.cpp
  )

  IF(ENABLE_THREADS)
	LIST(APPEND libvisual_SOURCES
      private/windows/lv_thread_win32.c
    )
  ELSE(ENABLE_THREADS)
    LIST(APPEND libvisual_SOURCES
      lv_thread_disabled.c
    )
  ENDIF(ENABLE_THREADS)

ENDIF()


ADD_LIBRARY(libvisual SHARED ${libvisual_SOURCES})

SET_TARGET_PROPERTIES(libvisual
  PROPERTIES
  OUTPUT_NAME visual-${LV_VERSION_SUFFIX}
  VERSION ${LV_VERSION}
  SOVERSION ${LV_VERSION_SUFFIX}
  LINK_FLAGS -Wl,--no-undefined
)

TARGET_LINK_LIBRARIES(libvisual ${LINK_LIBS})

INSTALL(TARGETS libvisual DESTINATION ${CMAKE_INSTALL_LIBDIR})

INSTALL(
  FILES ${libvisual_HEADERS}
  DESTINATION ${LV_INCLUDE_DIR}/libvisual
)
