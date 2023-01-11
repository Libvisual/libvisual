SET(INCLUDE_DIRS
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)

SET(SOURCES
  lv-tool.cpp
  display/display.cpp
  display/display_driver_factory.cpp
  display/stdout_driver.cpp
)

SET(LINK_LIBS "")
SET(LINK_DIRS "")

# SDL driver
IF(HAVE_SDL)
  LIST(APPEND SOURCES display/sdl_driver.cpp display/stdout_sdl_driver.cpp)
  LIST(APPEND LINK_LIBS SDL::SDL)
ENDIF()

# OpenGL
IF(HAVE_GL)
  LIST(APPEND LINK_LIBS OpenGL::GL)
ENDIF()

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})
LINK_DIRECTORIES(${LINK_DIRS})

ADD_EXECUTABLE(lv-tool ${SOURCES})

TARGET_LINK_LIBRARIES(lv-tool
  libvisual
  ${LINK_LIBS}
)

INSTALL(TARGETS lv-tool RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
