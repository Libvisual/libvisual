INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${GFORCE_SOURCE_DIR}/GForceCommon/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_gforce_SOURCES
  actor_gforce.cpp
)

ADD_LIBRARY(actor_gforce MODULE ${actor_gforce_SOURCES})
#-avoid-version

SET_TARGET_PROPERTIES(actor_gforce
  PROPERTIES COMPILE_FLAGS ${GFORCE_COMPILE_FLAGS}
)

TARGET_LINK_LIBRARIES(actor_gforce
  ${LIBVISUAL_LIBRARIES}
  gforce_common
  gforce_io
  gforce_math
  gforce_ui
  gforce_gt
  gforce_unix_mfl
)

INSTALL(TARGETS actor_gforce LIBRARY DESTINATION ${LV_ACTOR_PLUGIN_DIR})
