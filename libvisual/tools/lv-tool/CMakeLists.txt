ADD_EXECUTABLE(lv-tool
  lv-tool.cpp
  display/display.cpp
  display/display_driver_factory.cpp
  display/stdout_driver.cpp
)

SET_TARGET_PROPERTIES(lv-tool PROPERTIES OUTPUT_NAME lv-tool-0.5)

TARGET_INCLUDE_DIRECTORIES(lv-tool
  PRIVATE
  ${PROJECT_BINARY_DIR}
)

TARGET_LINK_LIBRARIES(lv-tool PRIVATE libvisual)

# OpenGL
IF(HAVE_GL)
  TARGET_LINK_LIBRARIES(lv-tool PRIVATE OpenGL::GL)
ENDIF()

# SDL driver
IF(HAVE_SDL)
  TARGET_SOURCES(lv-tool
    PRIVATE
    display/sdl_driver.cpp
    display/stdout_sdl_driver.cpp
  )
  TARGET_LINK_LIBRARIES(lv-tool PRIVATE SDL::SDL)
ENDIF()

INSTALL(TARGETS lv-tool RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
