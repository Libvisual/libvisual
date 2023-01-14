SET(gforce_unix_mfl_SOURCES
  mfl.c
)

ADD_LIBRARY(gforce_unix_mfl STATIC ${gforce_unix_mfl_SOURCES})

TARGET_LINK_LIBRARIES(gforce_unix_mfl
  Libvisual::Libvisual
)

TARGET_COMPILE_DEFINITIONS(gforce_unix_mfl PRIVATE ${GFORCE_COMPILE_DEFS})

TARGET_COMPILE_OPTIONS(gforce_unix_mfl PRIVATE ${GFORCE_COMPILE_OPTIONS})
