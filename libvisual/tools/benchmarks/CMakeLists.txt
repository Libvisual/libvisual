SET(BENCHMARK_PROGRAMS
  actor_bench.cpp
  morph_bench.cpp
  video_alpha_blend_bench.cpp
  video_convert_depth_bench.cpp
  video_scale_bench.cpp
  dft_bench.cpp
  math_simd_bench.cpp
)

ADD_LIBRARY(benchmark STATIC
  benchmark.cpp
)

FOREACH(BENCHMARK IN LISTS BENCHMARK_PROGRAMS)
  GET_FILENAME_COMPONENT(EXECUTABLE ${BENCHMARK} NAME_WE)

  ADD_EXECUTABLE(${EXECUTABLE} ${BENCHMARK})

  TARGET_LINK_LIBRARIES(${EXECUTABLE} libvisual benchmark)
ENDFOREACH()
