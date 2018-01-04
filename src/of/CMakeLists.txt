
set(my_lib "of")

include_directories(
      FlowFormat/inc
      FlowVisualization/inc
      FlowErrorStatistics/inc
)

set(
      srcs
      FlowFormat/src/FlowFormat.cpp
      FlowFormat/src/FlowFormat_F.cpp
      FlowFormat/src/FlowFormat_FLO.cpp
      FlowFormat/src/FlowFormat_KITTI.cpp

      FlowVisualization/src/FlowVisualization.cpp
      FlowVisualization/src/FlowVisualization_FLO.cpp
      FlowVisualization/src/FlowVisualization_KITTI.cpp

      FlowErrorStatistics/src/FlowErrorStatistics.cpp

      src/OpticalFlowKfj.cpp
)

add_library(
      ${my_lib}
      ${srcs}
)

target_link_libraries(
      ${my_lib}
      ${OpenCV_LIBS}
)
