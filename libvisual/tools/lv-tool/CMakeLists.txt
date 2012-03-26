INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${SDL_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${X11_INCLUDE_DIR}
)

LINK_DIRECTORIES(
)

SET(SOURCES
  lv-tool.c
  display/display.c
  display/stdout_driver.c
  display/sdl_driver.c
  display/glx_driver.c
  display/lv_x11_key.c
)

ADD_EXECUTABLE(lv-tool ${SOURCES})

TARGET_LINK_LIBRARIES(lv-tool
  libvisual
  ${SDL_LIBRARY}
  ${OPENGL_LIBRARY}
  ${X11_LIBRARY}
)

INSTALL(TARGETS lv-tool RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
