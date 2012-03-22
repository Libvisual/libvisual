SET(GFORCE_SOURCE_DIR ${PROJECT_SOURCE_DIR}/plugins/actor/G-Force)

# FIXME: Get rid of -DUNIX_X
SET(GFORCE_COMPILE_FLAGS "-DUNIX_X -D_REENTRANT")

ADD_SUBDIRECTORY(Common)
ADD_SUBDIRECTORY(GForceCommon)
#ADD_SUBDIRECTORY(GForceColorMaps)
#ADD_SUBDIRECTORY(GForceDeltaFields)
#ADD_SUBDIRECTORY(GForceParticles)
#ADD_SUBDIRECTORY(GForceWaveShapes)
#ADD_SUBDIRECTORY(NotWorkingWaveShapes)
#ADD_SUBDIRECTORY(unix)
#ADD_SUBDIRECTORY(docs)

INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
  ${LIBVISUAL_INCLUDE_DIRS}
)

LINK_DIRECTORIES(
  ${LIBVISUAL_LIBRARY_DIRS}
)

SET(actor_gforce_SOURCES
  null.cpp
)

ADD_LIBRARY(actor_gforce MODULE ${actor_gforce_SOURCES})
# -avoid-version

TARGET_LINK_LIBRARIES(actor_gforce
  ${LIBVISUAL_LIBRARIES}
  gforce_common # GForceCommon/libgforce.so
  gforce_io     # Common/io/libio.so
  gforce_math   # Common/math/libmath.so
  gforce_ui     # Common/UI/libUI.so
  gforce_gt     # Common/GeneralTools/libgt.so
  gforce_mfl    # unix/libmfl/libmfl.so
  gforce_lv     # unix/libvisual/libactor_gforce.so
)

SET_TARGET_PROPERTIES(actor_gforce
  PROPERTIES COMPILE_FLAGS ${GFORCE_COMPILE_FLAGS}
)

#EXTRA_DIST = deffont
