INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(morph_slide_left_SOURCES  morph_slide_left.c  common.c)
SET(morph_slide_right_SOURCES morph_slide_right.c common.c)
SET(morph_slide_up_SOURCES    morph_slide_up.c    common.c)
SET(morph_slide_down_SOURCES  morph_slide_down.c  common.c)

ADD_LIBRARY(morph_slide_left  MODULE ${morph_slide_left_SOURCES})
ADD_LIBRARY(morph_slide_right MODULE ${morph_slide_right_SOURCES})
ADD_LIBRARY(morph_slide_up    MODULE ${morph_slide_up_SOURCES})
ADD_LIBRARY(morph_slide_down  MODULE ${morph_slide_down_SOURCES})
#-avoid-version

TARGET_LINK_LIBRARIES(morph_slide_left  ${LIBVISUAL_LIBRARIES})
TARGET_LINK_LIBRARIES(morph_slide_right ${LIBVISUAL_LIBRARIES})
TARGET_LINK_LIBRARIES(morph_slide_up    ${LIBVISUAL_LIBRARIES})
TARGET_LINK_LIBRARIES(morph_slide_down  ${LIBVISUAL_LIBRARIES})

INSTALL(
  TARGETS morph_slide_left
          morph_slide_right
          morph_slide_up
          morph_slide_down
  LIBRARY DESTINATION ${LV_MORPH_PLUGIN_DIR}
)
