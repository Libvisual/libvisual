IF(HAVE_SDL)
  LV_BUILD_TEST(scale_test
    SOURCES      scale_test.cpp
    LINK_LIBS    SDL::SDL
  )
ENDIF()
