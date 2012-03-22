INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_corona_SOURCES
  actor_corona.cpp
  autopal.h
  corona.cpp
  corona.h
  corona_types.h
  palette.cpp
  palette.h
)

ADD_LIBRARY(actor_corona MODULE ${actor_corona_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(actor_corona
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS actor_corona LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
