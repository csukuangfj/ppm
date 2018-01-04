
set(my_lib "st")

include_directories(
      inc
      StereoFormat/inc
)

set(
      srcs
      src/StErrorStatistics.cpp
      StereoFormat/src/StereoFormat.cpp
      StereoFormat/src/StereoFormatPfm.cpp
)

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
)
