INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_infinite_SOURCES
  compute.c
  compute.h
  display.c
  display.h
  renderer.c
  renderer.h
  file.c
  lv_infinite.c
  main.h
)

ADD_LIBRARY(actor_infinite MODULE ${actor_infinite_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_infinite
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_infinite LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
