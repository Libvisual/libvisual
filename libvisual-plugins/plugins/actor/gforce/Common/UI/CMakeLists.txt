FILE(GLOB gforce_ui_HEADERS "Headers/*")

SET(DATADIR ${LV_PLUGIN_DATA_DIR}/actor/actor_gforce)

# Missing DrawXX.cpp and LineXX.cpp
SET(gforce_ui_SOURCES
  EgOSUtils.cpp
  PixPort.cpp
  ScreenDevice.cpp
)
#EXTRA_DIST = DrawXX.cpp LineXX.cpp

ADD_LIBRARY(gforce_ui STATIC ${gforce_ui_SOURCES})

TARGET_COMPILE_DEFINITIONS(gforce_ui PRIVATE ${GFORCE_COMPILE_DEFS} DATADIR="${DATADIR}")

TARGET_COMPILE_OPTIONS(gforce_ui PRIVATE ${GFORCE_COMPILE_OPTIONS})

TARGET_INCLUDE_DIRECTORIES(gforce_ui
  PRIVATE
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
  ${GFORCE_SOURCE_DIR}/unix/libvisual
)

TARGET_LINK_LIBRARIES(gforce_ui
  Libvisual::Libvisual
)
