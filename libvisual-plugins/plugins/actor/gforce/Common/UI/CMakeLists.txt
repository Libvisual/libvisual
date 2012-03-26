FILE(GLOB gforce_ui_HEADERS "Headers/*")

INCLUDE_DIRECTORIES(
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
  ${GFORCE_SOURCE_DIR}/unix/libvisual
)

SET(DATADIR $(LV_PLUGINS_DATA_DIR_FULL))
ADD_DEFINITIONS(-DDATADIR="${DATADIR}")

# Missing DrawXX.cpp and LineXX.cpp
SET(gforce_ui_SOURCES
  EgOSUtils.cpp
  PixPort.cpp
  ScreenDevice.cpp
)
#EXTRA_DIST = DrawXX.cpp LineXX.cpp

ADD_LIBRARY(gforce_ui STATIC ${gforce_ui_SOURCES})

SET_TARGET_PROPERTIES(gforce_ui
  PROPERTIES COMPILE_FLAGS "-fPIC ${GFORCE_COMPILE_FLAGS}"
)
