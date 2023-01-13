FILE(GLOB gforce_io_HEADERS "Headers/*.h")

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

TARGET_COMPILE_DEFINITIONS(gforce_io PRIVATE ${GFORCE_COMPILE_DEFS})

TARGET_COMPILE_OPTIONS(gforce_io PRIVATE ${GFORCE_COMPILE_OPTIONS})

TARGET_INCLUDE_DIRECTORIES(gforce_io
  PRIVATE
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)
