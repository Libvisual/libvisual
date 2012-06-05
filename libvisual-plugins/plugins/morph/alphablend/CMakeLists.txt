INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(morph_alphablend_SOURCES
  morph_alphablend.c
)

ADD_LIBRARY(morph_alphablend MODULE ${morph_alphablend_SOURCES})

SET_TARGET_PROPERTIES(morph_alphablend PROPERTIES
  LINK_FLAGS -Wl,--unresolved-symbols,ignore-in-shared-libs
)

TARGET_LINK_LIBRARIES(morph_alphablend
  ${LIBVISUAL_LIBRARIES}
)

INSTALL(TARGETS morph_alphablend LIBRARY DESTINATION ${LV_MORPH_PLUGIN_DIR})
