IF(HAVE_SDL)
  LV_BUILD_TEST(scale_test
    SOURCES      scale_test.cpp
    INCLUDE_DIRS ${SDL_INCLUDE_DIR}
    LINK_LIBS    ${SDL_LIBRARY}
  )
ENDIF()
