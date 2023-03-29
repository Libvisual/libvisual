FILE(GLOB gforce_common_HEADERS "Headers/*.h")

SET(DATADIR ${LV_PLUGIN_DATA_DIR}/actor/actor_gforce)

SET(gforce_common_SOURCES
  DeltaField.cpp
  G-Force.cpp
  GF_Palette.cpp
  GForcePixPort.cpp
  ParticleGroup.cpp
  WaveShape.cpp
)

ADD_LIBRARY(gforce_common STATIC ${gforce_common_SOURCES})

TARGET_COMPILE_DEFINITIONS(gforce_common
  PRIVATE
  ${GFORCE_COMPILE_DEFS}
  DATADIR="${DATADIR}"
)

TARGET_COMPILE_OPTIONS(gforce_common PRIVATE ${GFORCE_COMPILE_OPTIONS})

TARGET_INCLUDE_DIRECTORIES(gforce_common
  PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/GForceCommon/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

TARGET_LINK_LIBRARIES(gforce_common
  Libvisual::Libvisual
)
