
set(my_prog "gtest")

file(GLOB_RECURSE srcs */*.cpp)

add_executable(
      ${my_prog}
      test_main.cpp
      ${srcs}
)

target_link_libraries(
      ${my_prog}
      ${KFJ_GOOGLE_TEST_LIBRARY}
      ${OpenCV_LIBS}
      of
      util
      match_cost
      st
)

if(UNIX AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
   set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
   find_package(Threads)
   target_link_libraries(${my_prog} ${CMAKE_THREAD_LIBS_INIT})
endif()
