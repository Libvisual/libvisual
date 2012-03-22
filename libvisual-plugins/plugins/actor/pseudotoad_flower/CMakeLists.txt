INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_pseudotoad_flower_SOURCES
  actor_flower.c
  main.c
  main.h
  notch.c
  notch.h
)

ADD_LIBRARY(actor_pseudotoad_flower MODULE ${actor_pseudotoad_flower_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_pseudotoad_flower
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_pseudotoad_flower LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
