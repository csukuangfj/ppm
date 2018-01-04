
set(my_lib "match_cost")

include_directories(
      inc
)

set(
      srcs
      src/MatchCost.cpp
      src/SadCost.cpp
      src/SsdCost.cpp
      src/HammingCost.cpp
      )

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
)
