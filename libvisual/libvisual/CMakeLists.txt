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
  lv_hashmap.h
  lv_hashlist.h
  lv_cache.h
  lv_collection.h
  lv_os.h
  lv_math.h
  lv_gl.h
  lv_defines.h
  lv_alpha_blend.h
  ${PROJECT_BINARY_DIR}/libvisual/lvconfig.h
)

SET(libvisual_SOURCES
  lv_actor.c
  lv_input.c
  lv_event.c
  lv_bin.c
  lv_plugin.c
  lv_plugin_registry.c
  lv_video.c
  lv_video_simd.c
  lv_mem.c
  lv_audio.c
  lv_fourier.c
  lv_list.c
  lv_log.c
  lv_palette.c
  lv_beat.c
  lv_libvisual.c
  lv_songinfo.c
  lv_morph.c
  lv_bmp.c
  lv_param.c
  lv_cpu.c
  lv_color.c
  lv_time.c
  lv_random.c
  lv_error.c
  lv_thread.c
  lv_object.c
  lv_transform.c
  lv_rectangle.c
  lv_config.c
  lv_ringbuffer.c
  lv_buffer.c
  lv_cache.c
  lv_hashmap.c
  lv_hashlist.c
  lv_collection.c
  lv_os.c
  lv_math.c
  lv_gl.c
  lv_alpha_blend.c

  private/lv_video_convert.c
  private/lv_video_fill.c
  private/lv_video_scale.c
)

SET(LINK_LIBS
  ${THREAD_LIBS}
  m
)

IF(NOT VISUAL_OS_WIN32)
  LIST(APPEND LINK_LIBS dl)
ENDIF(NOT VISUAL_OS_WIN32)

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
