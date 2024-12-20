INCLUDE(LVBuild)

IF(VISUAL_OS_POSIX)
  SET(PLATFORM_SPECIFIC_LIBS dl rt m)
  SET(PLATFORM_SPECIFIC_SOURCE_DIR private/posix)
ELSEIF(VISUAL_OS_WIN32)
  SET(PLATFORM_SPECIFIC_LIBS m)
  SET(PLATFORM_SPECIFIC_SOURCE_DIR private/windows)
ENDIF()

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
  lv_log.h
  lv_palette.h
  lv_plugin.h
  lv_plugin_registry.h
  lv_video.h
  lv_libvisual.h
  lv_songinfo.h
  lv_morph.h
  lv_param.h
  lv_param_validators.h
  lv_param_value.h
  lv_mem.h
  lv_bits.h
  lv_cpu.h
  lv_color.h
  lv_time.h
  lv_random.h
  lv_error.h
  lv_checks.h
  lv_types.h
  lv_rectangle.h
  lv_buffer.h
  lv_math.h
  lv_gl.h
  lv_defines.h
  lv_alpha_blend.h
  lv_util.h

  lv_aligned_allocator.hpp
  lv_module.hpp
  lv_intrusive_ptr.hpp
  lv_singleton.hpp
  lv_util.hpp

  ${CMAKE_CURRENT_BINARY_DIR}/lvconfig.h
)

SET(libvisual_SOURCES
  lv_mem.c
  lv_log.c
  lv_param_value.c
  lv_param_validators.c
  lv_cpu.c
  lv_error.c
  lv_math.c
  lv_gl.c
  lv_alpha_blend.c
  lv_util.c

  lv_actor.cpp
  lv_audio.cpp
  lv_bin.cpp
  lv_buffer.cpp
  lv_color.cpp
  lv_event.cpp
  lv_fourier.cpp
  lv_input.cpp
  lv_libvisual.cpp
  lv_morph.cpp
  lv_param.cpp
  lv_plugin.cpp
  lv_random.cpp
  lv_palette.cpp
  lv_plugin_registry.cpp
  lv_rectangle.cpp
  lv_songinfo.cpp
  lv_time.cpp
  lv_util.cpp
  lv_video.cpp

  lv_actor_c.cpp
  lv_audio_c.cpp
  lv_bin_c.cpp
  lv_buffer_c.cpp
  lv_color_c.cpp
  lv_event_c.cpp
  lv_fourier_c.cpp
  lv_input_c.cpp
  lv_libvisual_c.cpp
  lv_morph_c.cpp
  lv_random_c.cpp
  lv_palette_c.cpp
  lv_plugin_registry_c.cpp
  lv_rectangle_c.cpp
  lv_songinfo_c.cpp
  lv_time_c.cpp
  lv_video_c.cpp

  private/lv_audio_convert.cpp
  private/lv_video_convert.cpp
  private/lv_audio_stream.cpp
  private/lv_video_fill.cpp
  private/lv_video_scale.cpp
  private/lv_video_blit.cpp
  private/lv_video_rotate.cpp
  private/lv_video_blit_simd.cpp
  private/lv_video_scale_simd.cpp
  private/lv_video_bmp.cpp
  private/lv_video_png.cpp

  ${PLATFORM_SPECIFIC_SOURCE_DIR}/lv_mem.cpp
  ${PLATFORM_SPECIFIC_SOURCE_DIR}/lv_module.cpp
  ${PLATFORM_SPECIFIC_SOURCE_DIR}/lv_time_system.cpp

  ${CMAKE_CURRENT_BINARY_DIR}/lv_alpha_blend_orc.h
  ${CMAKE_CURRENT_BINARY_DIR}/lv_math_orc.h
)

LV_GENERATE_ORC_INLINE_SOURCE(
  ${CMAKE_CURRENT_SOURCE_DIR}/lv_alpha_blend.orc
  ${CMAKE_CURRENT_BINARY_DIR}/lv_alpha_blend_orc.h
  HEADER
)

LV_GENERATE_ORC_INLINE_SOURCE(
  ${CMAKE_CURRENT_SOURCE_DIR}/lv_math.orc
  ${CMAKE_CURRENT_BINARY_DIR}/lv_math_orc.h
  HEADER
  INCLUDES math.h
)

ADD_LIBRARY(libvisual SHARED ${libvisual_SOURCES})

ADD_LIBRARY(Libvisual::Libvisual ALIAS libvisual)

SET_TARGET_PROPERTIES(libvisual
  PROPERTIES
  EXPORT_NAME Libvisual
  OUTPUT_NAME visual-${LV_VERSION_SUFFIX}
  VERSION 0.0.0
  SOVERSION 0
)

TARGET_COMPILE_OPTIONS(libvisual PRIVATE -fvisibility=hidden)

IF(ENABLE_TESTS)
  TARGET_COMPILE_DEFINITIONS(libvisual PRIVATE _LV_BUILD_PRIVATE_TEST_FUNCS)
ENDIF()

TARGET_INCLUDE_DIRECTORIES(libvisual
  PUBLIC
  "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
  "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
  "$<INSTALL_INTERFACE:${LV_INCLUDE_DIR}>"
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

IF(ENABLE_SO_UNRESOLVED_SYMBOL_CHECKS)
  TARGET_LINK_OPTIONS(libvisual PRIVATE -Wl,--no-undefined)
ENDIF()

TARGET_LINK_LIBRARIES(libvisual
  PRIVATE
  PkgConfig::ORC
  PNG::PNG
  Threads::Threads
  ZLIB::ZLIB
)

IF(WIN32)
  TARGET_LINK_LIBRARIES(libvisual PRIVATE winmm)
ENDIF()

INSTALL(TARGETS libvisual
  EXPORT libvisual
  DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

INSTALL(
  FILES ${libvisual_HEADERS}
  DESTINATION ${LV_INCLUDE_DIR}/libvisual
)
