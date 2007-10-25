
ADD_DEFINITIONS(-DLINUX -DVERSION)

set (libvisual_version 0.4)

set(libvisual_headers

libvisual.h      lv_cache.h       lv_defines.h     lv_input.h       lv_morph.h       lv_rectangle.h   lv_ui.h
lv_actor.h       lv_collection.h  lv_error.h       lv_keysym.h      lv_object.h      lv_ringbuffer.h  lv_utils.h
lv_audio.h       lv_color.h       lv_event.h       lv_libvisual.h   lv_os.h          lv_songinfo.h    lv_video.h
lv_bin.h         lv_common.h      lv_fourier.h     lv_list.h        lv_palette.h     lv_thread.h
lv_bits.h        lv_config.h      lv_gl.h          lv_log.h         lv_param.h       lv_time.h
lv_bmp.h         lvconfig.h       lv_hashlist.h    lv_math.h        lv_plugin.h      lv_transform.h
lv_buffer.h      lv_cpu.h         lv_hashmap.h     lv_mem.h         lv_random.h      lv_types.h

)

set(libvisual_sources 

gettext.h    lv_bmp.h         lv_config.h   lv_fourier.h   lv_libvisual.h  lv_object.h   lv_rectangle.h   lv_types.h
libvisual.h  lv_buffer.h      lvconfig.h    lv_gl.h        lv_list.h       lv_os.h       lv_ringbuffer.h  lv_ui.h
lv_actor.h   lv_cache.h       lv_cpu.h      lv_hashlist.h  lv_log.h        lv_palette.h  lv_songinfo.h    lv_utils.h
lv_audio.h   lv_collection.h  lv_defines.h  lv_hashmap.h   lv_math.h       lv_param.h    lv_thread.h      lv_video.h
lv_bin.h     lv_color.h       lv_error.h    lv_input.h     lv_mem.h        lv_plugin.h   lv_time.h
lv_bits.h    lv_common.h      lv_event.h    lv_keysym.h    lv_morph.h      lv_random.h   lv_transform.h

lv_actor.c   lv_cache.c       lv_error.c     lv_hashmap.c    lv_math.c    lv_palette.c    lv_ringbuffer.c  lv_ui.c
lv_audio.c   lv_collection.c  lv_event.c     lv_input.c      lv_mem.c     lv_param.c      lv_songinfo.c    lv_utils.c
lv_bin.c     lv_color.c       lv_fourier.c   lv_libvisual.c  lv_morph.c   lv_plugin.c     lv_thread.c      lv_video.c
lv_bmp.c     lv_config.c      lv_gl.c        lv_list.c       lv_object.c  lv_random.c     lv_time.c        lv_video_simd.c
lv_buffer.c  lv_cpu.c         lv_hashlist.c  lv_log.c        lv_os.c      lv_rectangle.c  lv_transform.c

)

ADD_LIBRARY(visual-${libvisual_version} SHARED ${libvisual_sources})

#LINK_DIRECTORIES(${PROJECTM_LDFLAGS})

include_directories(../ .)
# todo: to be more portable, condition on including math lib
target_link_libraries(visual-${libvisual_version} m)

install(FILES ${libvisual_headers} DESTINATION include/libvisual-${libvisual_version})
install(TARGETS visual-${libvisual_version} DESTINATION lib)
