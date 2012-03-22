FILE(GLOB gforce_gt_HEADERS "Headers/*.h")

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/GForceCommon/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

#AM_CXXFLAGS = $(LIBVISUAL_CFLAGS)
#CXXFLAGS = "$CXXFLAGS -DUNIX_X -D_REENTRANT"

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

TARGET_LINK_LIBRARIES(gforce_gt
  ${LIBVISUAL_LIBRARIES}
)

SET_TARGET_PROPERTIES(gforce_gt
  PROPERTIES COMPILE_FLAGS "-fPIC ${GFORCE_COMPILE_FLAGS}"
)
