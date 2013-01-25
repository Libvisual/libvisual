IF(HAVE_SDL)
  LV_BUILD_TEST(blit_rectangle_test
    SOURCES      blit_rectangle_test.cpp
    INCLUDE_DIRS ${SDL_INCLUDE_DIR}
    LINK_LIBS    ${SDL_LIBRARY}
  )
ENDIF()
