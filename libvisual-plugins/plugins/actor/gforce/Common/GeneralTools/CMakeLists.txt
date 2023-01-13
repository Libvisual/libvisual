FILE(GLOB gforce_gt_HEADERS "Headers/*.h")

SET(gforce_gt_SOURCES
  Arg.cpp
  ArgList.cpp
  FileSpecList.cpp
  Hashtable.cpp
  RectUtils.cpp
  TempMem.cpp
  UtilStr.cpp
  XFloatList.cpp
  XLongList.cpp
  XPtrList.cpp
  XPtrMatrix.cpp
  XStrList.cpp
  nodeClass.cpp
)

ADD_LIBRARY(gforce_gt STATIC ${gforce_gt_SOURCES})

TARGET_COMPILE_DEFINITIONS(gforce_gt PRIVATE ${GFORCE_COMPILE_DEFS})

TARGET_COMPILE_OPTIONS(gforce_gt PRIVATE ${GFORCE_COMPILE_OPTIONS})

TARGET_INCLUDE_DIRECTORIES(gforce_gt
  PRIVATE
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/GForceCommon/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

TARGET_LINK_LIBRARIES(gforce_gt
  Libvisual::Libvisual
)
