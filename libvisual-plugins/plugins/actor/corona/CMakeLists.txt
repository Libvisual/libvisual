SET(SOURCES
  actor_corona.cpp
  autopal.h
  corona.cpp
  corona.h
  corona_types.h
  palette.cpp
  palette.h
)

LV_BUILD_ACTOR_PLUGIN(corona
  SOURCES   ${SOURCES}
  LINK_LIBS m
)
