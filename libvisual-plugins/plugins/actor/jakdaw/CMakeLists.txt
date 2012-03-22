INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_jakdaw_SOURCES
  actor_jakdaw.c
  actor_jakdaw.h
  feedback.c
  feedback.h
  plotter.c
  plotter.h
)

ADD_LIBRARY(actor_jakdaw MODULE ${actor_jakdaw_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_jakdaw
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_jakdaw LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
