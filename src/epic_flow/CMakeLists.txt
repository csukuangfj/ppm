
set(my_c_flags " -DUSE_OPENMP ") # enable openmp
set(my_c_flags "${my_c_flags} -DFINTEGER=int -msse4")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${my_c_flags}")

set(my_cpp_flags "${my_c_flags} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${my_cpp_flags}")

set(my_lib "epic_flow")

set(
      epic_flow_srcs_cpp
      src/epic.cpp
      src/epic_aux.cpp
)

set(
      epic_flow_srcs_c
      src/image.c
      src/io.c
      src/solver.c
      src/variational.c
      src/variational_aux.c
)

set(
      epic_flow_wrapper_srcs
      src/EpicFlowConfig.cpp
      src/EpicFlowInterface.cpp
      src/EpicFlowWrapper.cpp
)

add_library(
      ${my_lib}
      ${epic_flow_srcs_cpp}
      ${epic_flow_srcs_c}
      ${epic_flow_wrapper_srcs}
)

target_link_libraries(
      ${my_lib}
      ${LAPACK_LIBRARIES}
      ${OpenCV_LIBS}
      util
      -ljpeg
      -lpng
)
