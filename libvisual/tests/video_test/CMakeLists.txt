LV_BUILD_TEST(video_check_test
  SOURCES video_check_test.cpp
)

LV_BUILD_TEST(video_blit_test
  SOURCES video_blit_test.cpp
)

LV_BUILD_TEST(video_load_test
  SOURCES video_load_test.cpp
)

IF(HAVE_SDL)
  LV_BUILD_TEST(video_scale_test
    SOURCES   video_scale_test.cpp
    LINK_LIBS SDL::SDL
  )
ENDIF()
