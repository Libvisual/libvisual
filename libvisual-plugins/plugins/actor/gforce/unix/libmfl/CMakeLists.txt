SET(gforce_unix_mfl_SOURCES
  mfl.c
)

ADD_LIBRARY(gforce_unix_mfl STATIC ${gforce_unix_mfl_SOURCES})

TARGET_LINK_LIBRARIES(gforce_unix_mfl
  Libvisual::Libvisual
)

SET_TARGET_PROPERTIES(gforce_unix_mfl
  PROPERTIES
  COMPILE_FLAGS ${GFORCE_COMPILE_FLAGS}
)
