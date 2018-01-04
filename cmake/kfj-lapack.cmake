# Ubuntu:
#  apt-cache search liblapack
#  sudo apt-get install liblapack-dev
#  It will install blas automatically

find_package(LAPACK)

# macOS: LAPACK_LIBRARIES: /System/Library/Frameworks/Accelerate.framework;/System/Library/Frameworks/Accelerate.framework
# Ubuntu: LAPACK_LIBRARIES: /usr/lib/liblapack.so;/usr/lib/libblas.so

if (NOT LAPACK_FOUND)
   message(STATUS "Lapack is not found by find_package()")
   message(STATUS "try to find it manually")
   find_library(
         LAPACK_LIBRARIES
         NAMES lapack
         PATHS
         /usr/lib
         /usr/lib/x86_64-linux-gnu

         DOC "Path to the lapack library"
   )
   if (NOT LAPACK_LIBRARIES)
      message(FATAL_ERROR "Cannot find lapack library in this computer!\n"
                          "Lapack is needed by EpicFlow.")
   endif()
endif ()

message(STATUS "LAPACK_LIBRARIES: ${LAPACK_LIBRARIES}")
