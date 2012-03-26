FILE(GLOB gforce_math_HEADERS "Headers/*")

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(gforce_math_SOURCES
  ExprArray.cpp
  ExprVirtualMachine.cpp
  Expression.cpp
  ExpressionDict.cpp
  FourierAnalyzer.cpp
  R3Matrix.cpp
  V3.cpp
)

ADD_LIBRARY(gforce_math STATIC ${gforce_math_SOURCES})

TARGET_LINK_LIBRARIES(gforce_math
  ${LIBVISUAL_LIBRARIES}
)

SET_TARGET_PROPERTIES(gforce_math
  PROPERTIES COMPILE_FLAGS "-fPIC ${GFORCE_COMPILE_FLAGS}"
)
