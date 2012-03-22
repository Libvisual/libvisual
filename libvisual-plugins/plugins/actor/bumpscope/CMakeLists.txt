INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_bumpscope_SOURCES
  actor_bumpscope.c
  actor_bumpscope.h
  bump_scope.c
  bump_scope.h
)

ADD_LIBRARY(actor_bumpscope MODULE ${actor_bumpscope_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_bumpscope
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_bumpscope LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
