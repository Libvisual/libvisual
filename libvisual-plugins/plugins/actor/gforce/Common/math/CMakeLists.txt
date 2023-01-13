FILE(GLOB gforce_math_HEADERS "Headers/*")

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${GFORCE_SOURCE_DIR}/Common
  ${GFORCE_SOURCE_DIR}/Common/UI/Headers
  ${GFORCE_SOURCE_DIR}/Common/GeneralTools/Headers
  ${GFORCE_SOURCE_DIR}/Common/math/Headers
  ${GFORCE_SOURCE_DIR}/Common/io/Headers
  ${GFORCE_SOURCE_DIR}/unix/Headers
  ${GFORCE_SOURCE_DIR}/unix/libmfl
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

TARGET_COMPILE_DEFINITIONS(gforce_math PRIVATE ${GFORCE_COMPILE_DEFS})

TARGET_COMPILE_OPTIONS(gforce_math PRIVATE ${GFORCE_COMPILE_OPTIONS})

TARGET_LINK_LIBRARIES(gforce_math
  Libvisual::Libvisual
)
