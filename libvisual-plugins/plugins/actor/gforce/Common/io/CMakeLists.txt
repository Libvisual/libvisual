FILE(GLOB gforce_io_HEADERS "Headers/*.h")

INCLUDE_DIRECTORIES(
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

SET(gforce_io_SOURCES
  CEgErr.cpp
  CEgFileSpec.cpp
  CEgIFile.cpp
  CEgIOFile.cpp
  CEgIStream.cpp
  CEgOStream.cpp
  ConfigFile.cpp
  Prefs.cpp
)

ADD_LIBRARY(gforce_io STATIC ${gforce_io_SOURCES})

SET_TARGET_PROPERTIES(gforce_io
  PROPERTIES COMPILE_FLAGS "-fPIC ${GFORCE_COMPILE_FLAGS}"
)
