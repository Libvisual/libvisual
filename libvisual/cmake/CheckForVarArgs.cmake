MACRO(CHECK_FOR_ISO_C_VARARGS RESULT)
  MESSAGE(STATUS "Check for ISO C99 varargs macros in C")
  TRY_COMPILE(${RESULT} ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/CheckForIsoCVarArgs.c
    OUTPUT_VARIABLE OUTPUT)
  IF(${RESULT})
    MESSAGE(STATUS "Check for ISO C99 varargs macros in C - found")
  ELSE()
    MESSAGE(STATUS "Check for ISO C99 varargs macros in C - not found")
  ENDIF()
ENDMACRO(CHECK_FOR_ISO_C_VARARGS)

MACRO(CHECK_FOR_GNU_C_VARARGS RESULT)
  MESSAGE(STATUS "Check for GNU C varargs macros in C")
  TRY_COMPILE(${RESULT} ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/cmake/CheckForGnuCVarArgs.c
    OUTPUT_VARIABLE OUTPUT)
  IF(${RESULT})
    MESSAGE(STATUS "Check for GNU C varargs macros in C - found")
  ELSE()
    MESSAGE(STATUS "Check for GNU C varargs macros in C - not found")
  ENDIF()
ENDMACRO(CHECK_FOR_GNU_C_VARARGS)
