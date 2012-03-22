FILE(GLOB gforce_common_HEADERS "Headers/*.h")

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/GForceCommon/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(DATADIR ${LV_PLUGINS_DATA_DIR_FULL}/actor/actor_gforce)

ADD_DEFINITIONS(-DDATADIR="${DATADIR}")
#$(LIBVISUAL_CFLAGS)

SET(gforce_common_SOURCES
  DeltaField.cpp
  G-Force.cpp
  GF_Palette.cpp
  GForcePixPort.cpp
  ParticleGroup.cpp
  WaveShape.cpp
)

ADD_LIBRARY(gforce_common STATIC ${gforce_common_SOURCES})

TARGET_LINK_LIBRARIES(gforce_common
  ${LIBVISUAL_LIBRARIES}
)

SET_TARGET_PROPERTIES(gforce_common
  PROPERTIES COMPILE_FLAGS "-fPIC ${GFORCE_COMPILE_FLAGS}"
)
