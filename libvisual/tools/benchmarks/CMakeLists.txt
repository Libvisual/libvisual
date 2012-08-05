INCLUDE_DIRECTORIES(
  ${PROJECT_SOURCE_DIR}
  ${PROJECT_BINARY_DIR}
)

SET(BENCHMARK_PROGRAMS
  actor_throughput_bench.c
  alphablend_bench.c
  #blit_bench
  depth_transform_bench.c
  morph_throughput_bench.c
  scale_bench.c
  math_simd_bench.cpp
)

FOREACH(BENCHMARK IN LISTS BENCHMARK_PROGRAMS)
  GET_FILENAME_COMPONENT(EXECUTABLE ${BENCHMARK} NAME_WE)
  ADD_EXECUTABLE(${EXECUTABLE} ${BENCHMARK})
  TARGET_LINK_LIBRARIES(${EXECUTABLE} libvisual)
ENDFOREACH()
