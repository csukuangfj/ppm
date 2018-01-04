# parameteric patch match for optical flow
# ppm flow
set(my_lib "ppm_flow")

add_definitions(-DKFJ_DEBUG)

file(GLOB srcs src/*.cpp)

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
      match_cost
      of
      util
      sift_flow_descriptor
)

#========================================
#    cmd
#----------------------------------------
add_executable(ppm_cmd cmd/ppm_commandline.cpp)
target_link_libraries(ppm_cmd ${OpenCV_LIBS} ${my_lib})

add_executable(ppm_cmd_with_epic cmd/ppm_commandline_with_epic_flow.cpp)
target_link_libraries(ppm_cmd_with_epic ${OpenCV_LIBS} ${my_lib} epic_flow)
