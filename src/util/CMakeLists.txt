
set(my_lib "util")

include_directories(
      .
      inc
)

file(GLOB_RECURSE srcs */*.cpp)

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
)
