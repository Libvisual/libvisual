SET(INCLUDE_DIRS
  ${GFORCE_SOURCE_DIR}/GForceCommon/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

SET(LIBS
  gforce_common
  gforce_io
  gforce_math
  gforce_ui
  gforce_gt
  gforce_unix_mfl
)

SET(SOURCES
  actor_gforce.cpp
)

LV_BUILD_ACTOR_PLUGIN(gforce
  SOURCES         ${SOURCES}
  COMPILE_DEFS    ${GFORCE_COMPILE_DEFS}
  COMPILE_OPTIONS ${GFORCE_COMPILE_OPTIONS}
  INCLUDE_DIRS    ${INCLUDE_DIRS}
  LINK_LIBS       ${LIBS}
)
