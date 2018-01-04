
set(my_lib "sift_flow_descriptor")

# enable OpenMP for SIFT flow descriptor
add_definitions(-DSIFT_OPENMP)

include_directories(
      inc
)

set(
      srcs
      src/SiftDescriptor.cpp
)

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
)

#========================================
#    cmd
#----------------------------------------
set(
      cmd_srcs
      cmd/test_sift_descriptor.cpp
)

add_executable(sift_flow_cmd ${cmd_srcs})
target_link_libraries(
      sift_flow_cmd
      ${OpenCV_LIBS}
      ${my_lib}
)
