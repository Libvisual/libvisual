LV_BUILD_TEST(video_check_test
  SOURCES video_check_test.cpp
  LINK_LIBS video_test_common
)

LV_BUILD_TEST(video_blit_test
  SOURCES video_blit_test.cpp
  LINK_LIBS video_test_common
)

LV_BUILD_TEST(video_load_test
  SOURCES video_load_test.cpp
  LINK_LIBS video_test_common
)

LV_BUILD_TEST(video_save_test
  SOURCES video_save_test.cpp
  LINK_LIBS video_test_common
)

ADD_LIBRARY(video_test_common STATIC common.cpp)
TARGET_LINK_LIBRARIES(video_test_common PUBLIC libvisual)

IF(HAVE_SDL)
  LV_BUILD_TEST(video_scale_test
    SOURCES   video_scale_test.cpp
    LINK_LIBS SDL::SDL
  )
ENDIF()
