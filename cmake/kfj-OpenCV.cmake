#============================================================
#    Set OpenCV
#
# Set the environment variables
#   MY_OPENCV=/Users/fangjun/software/opencv/master/install-release
#   export OpenCV_DIR=${MY_OPENCV}
#   export DYLD_LIBRARY_PATH=${DYLD_LIBRARY_PATH}:${MY_OPENCV}/lib
#   export CPLUS_INCLUDE_PATH=${CPLUS_INCLUDE_PATH}:${MY_OPENCV}/include
#   export C_INCLUDE_PATH=${C_INCLUDE_PATH}:${MY_OPENCV}/include
#------------------------------------------------------------
find_package(OpenCV REQUIRED)
if(CMAKE_VERSION VERSION_LESS "2.8.11")
    # Add OpenCV headers location to your include paths
    include_directories(${OpenCV_INCLUDE_DIRS})
endif()

message(STATUS "OpenCV library status:")
message(STATUS "    Version: ${OpenCV_VERSION}")
message(STATUS "    Libraries: ${OpenCV_LIBS}")
message(STATUS "    Include path: ${OpenCV_INCLUDE_DIRS}")
#============================================================
