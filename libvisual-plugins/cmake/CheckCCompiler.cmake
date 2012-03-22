# Taken from: http://diet-ter.googlecode.com/svn/trunk/diet/Cmake/CheckCCompiler.cmake

#
# Check the C compiler for unstable features
#
# Translation to cmake of some of GNU's autoconf macros as taken from c.m4.
#

#### Inline support and associated keyword.
# The following CHECK_C_COMPILER_SUPPORTS_INLINE macro tries to provide
# a functional equivalent of autoconf's AC_C_INLINE macro.
# Usage: CHECK_C_COMPILER_SUPPORTS_INLINE( INLINE_VALUE INLINE_SUPPORTED )
#       - INLINE_VALUE is a string containing the keyword used for inlining
#         by the C compiler. When inlining is supported INLINE_VALUE is one
#         of "inline", "__inline" or "__inline__", otherwise INLINE_VALUE is
#         set to be empty.
#       - INLINE_VALUE is set to true when inlining is supported and false
#         otherwise.
#
MACRO( CHECK_C_COMPILER_SUPPORTS_INLINE_COMPILE TO_TEST RESULT )
  TRY_COMPILE( ${RESULT}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/CheckCCompilerInline.c
    COMPILE_DEFINITIONS "-DPOSSIBLE_INLINE_KEYWORD=${TO_TEST}"
    OUTPUT_VARIABLE DUMMY_OUTPUT_FOR_DEBUG )
ENDMACRO( CHECK_C_COMPILER_SUPPORTS_INLINE_COMPILE )

MACRO( CHECK_C_COMPILER_SUPPORTS_INLINE INLINE_VALUE INLINE_SUPPORTED )
  MESSAGE( STATUS "Checking C compiler for inlining support.")
  SET( SUPPORTED FALSE )

  MESSAGE( STATUS "Checking C compiler for inline.")
  CHECK_C_COMPILER_SUPPORTS_INLINE_COMPILE( "inline" TEST_INLINE )
  IF( TEST_INLINE )
    SET( SUPPORTED TRUE )
    SET( ${INLINE_VALUE} "inline" )
  ENDIF( TEST_INLINE )

  IF( NOT SUPPORTED )
    MESSAGE( STATUS "Checking C compiler for __inline__.")
    CHECK_C_COMPILER_SUPPORTS_INLINE_COMPILE( "__inline__" TEST_INLINE )
    IF( TEST_INLINE )
      SET( SUPPORTED TRUE )
      SET( ${INLINE_VALUE} "__inline__" )
    ENDIF( TEST_INLINE )
  ENDIF( NOT SUPPORTED )

  IF( NOT SUPPORTED )
    MESSAGE( STATUS "Checking C compiler for __inline.")
    CHECK_C_COMPILER_SUPPORTS_INLINE_COMPILE( "__inline" TEST_INLINE )
    IF( TEST_INLINE )
      SET( SUPPORTED TRUE )
      SET( ${INLINE_VALUE} "__inline" )
    ENDIF( TEST_INLINE )
  ENDIF( NOT SUPPORTED )

  IF( SUPPORTED )
    SET( ${INLINE_SUPPORTED} TRUE CACHE INTERNAL "C compiler inlining support")
    FILE( APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
          "The C compiler was found to support inlining with keyword: "
          "${${INLINE_VALUE}}\n\n" )
  ELSE( SUPPORTED )
    SET( ${INLINE_SUPPORTED} "" CACHE INTERNAL "C compiler inlining support")
    FILE( APPEND ${CMAKE_BINARY_DIR}/CMakeOutput.log
          "The C compiler was found not to support inlining.\n\n" )
  ENDIF( SUPPORTED )
ENDMACRO( CHECK_C_COMPILER_SUPPORTS_INLINE )

#### Const support
# The following CHECK_C_COMPILER_SUPPORTS_CONST macro tries to provide
# a functional equivalent of autoconf's AC_C_CONST macro.
# Usage: CHECK_C_COMPILER_SUPPORTS_CONST( CONST_SUPPORTED )
#     CONST_VALUE is set to true when const is supported and false otherwise.
#
MACRO( CHECK_C_COMPILER_SUPPORTS_CONST CONST_SUPPORTED )
  MESSAGE( STATUS "Checking C compiler for ANSI C-conforming const.")
  TRY_COMPILE( ${CONST_SUPPORTED}
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/CheckCCompilerConst.c
    OUTPUT_VARIABLE DUMMY_OUTPUT_FOR_DEBUG )
ENDMACRO( CHECK_C_COMPILER_SUPPORTS_CONST )
