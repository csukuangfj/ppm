
add_definitions(-DKFJ_USE_OPENMP)

set(my_lib "ppm_stereo")

include_directories(
      inc
)

file(GLOB_RECURSE srcs src/*.cpp)

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
      st
      util
      match_cost
)

#========================================
#    cmd
#----------------------------------------
add_executable(ppm_st_cmd cmd/ppm_st_cmd.cpp)
target_link_libraries(ppm_st_cmd ${my_lib})
