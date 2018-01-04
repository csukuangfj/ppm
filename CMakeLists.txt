cmake_minimum_required(VERSION 3.0)
project(ppm)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(cmake/kfj-rpath-config.cmake)
include(cmake/kfj-compiler-flags.cmake)
include(cmake/kfj-OpenCV.cmake)
include(cmake/kfj-googletest.cmake)
include(cmake/kfj-lapack.cmake)

set(BUILD_SHARED_LIBS ON)

set(KFJ_DATA_PATH "${CMAKE_SOURCE_DIR}/data")
configure_file(${CMAKE_SOURCE_DIR}/inc/common.hpp ${CMAKE_BINARY_DIR}/common.hpp @ONLY)
include_directories(${CMAKE_BINARY_DIR})

#============================================================
#  of lib
#------------------------------------------------------------
include_directories(src/of/FlowFormat/inc)
include_directories(src/of/FlowVisualization/inc)
include_directories(src/of/FlowErrorStatistics/inc)
include_directories(src/of/inc)
add_subdirectory(src/of)

#============================================================
#  util lib
#------------------------------------------------------------
include_directories(src/util/inc)
add_subdirectory(src/util)

#============================================================
#  match cost lib
#------------------------------------------------------------
include_directories(src/match_cost/inc)
add_subdirectory(src/match_cost)

#============================================================
#  sift flow descriptor lib
#------------------------------------------------------------
include_directories(src/sift_flow_descriptor/inc)
add_subdirectory(src/sift_flow_descriptor)

#============================================================
#  epic flow
#------------------------------------------------------------
include_directories(src/epic_flow/inc)
add_subdirectory(src/epic_flow)

#============================================================
#  ppm flow
#------------------------------------------------------------
include_directories(src/ppm_flow/inc)
add_subdirectory(src/ppm_flow)


#////////////////////////////////////////////////////////////
#////////////////////////////////////////////////////////////

#============================================================
#  st lib
#------------------------------------------------------------
include_directories(src/st/inc)
include_directories(src/st/StereoFormat/inc)
add_subdirectory(src/st)

#============================================================
#  ppm stereo
#------------------------------------------------------------
include_directories(src/ppm_stereo/inc)
add_subdirectory(src/ppm_stereo)

#////////////////////////////////////////////////////////////
#////////////////////////////////////////////////////////////

#============================================================
#  goolge test
#------------------------------------------------------------
if (kfj_enable_test)
   add_subdirectory(test)
else()
   message(STATUS "Google test is disabled.")
endif()
