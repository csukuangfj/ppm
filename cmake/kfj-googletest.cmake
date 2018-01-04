
# set the following variable to false to disable gtest
# if gtest is not installed, set the following variable to false

option(kfj_enable_test "enable/disable google test" OFF)

#============================================================
# Find the googletest header files and lib
#   Terminate cmake if they are not found.
#------------------------------------------------------------
if (kfj_enable_test)
   find_path(
         KFJ_GOOGLE_TEST_INCLUDE_DIR gtest/gtest.h

         # add the directory where google test is installed
         $ENV{HOME}/kfj-software/googletest/include
         $ENV{HOME}/kfj-software/googletest/installation/include

         DOC "The directory where gtest/gtest.h resides"
   )

   if (NOT KFJ_GOOGLE_TEST_INCLUDE_DIR)
      message(FATAL_ERROR " Error: Cannot find the google test installation directory (include ).\n"
            "Please either disable googletest or set the installation directy of googletest above.")
   endif()

   message(STATUS "googletest include dir: ${KFJ_GOOGLE_TEST_INCLUDE_DIR}")
   include_directories(${KFJ_GOOGLE_TEST_INCLUDE_DIR})

   find_library(
         KFJ_GOOGLE_TEST_LIBRARY
         NAMES gtest
         PATHS

         # add the directory where google test is installed
         $ENV{HOME}/kfj-software/googletest/lib/
         $ENV{HOME}/kfj-software/googletest/installation/lib/

         DOC "The googletest library"
   )

   if (NOT KFJ_GOOGLE_TEST_LIBRARY)
      message(FATAL_ERROR " Error: Cannot find the google test installation directory (lib).\n"
            "Please either disable googletest or set the installation directy of googletest above.")
   endif()

   message(STATUS "googletest lib: ${KFJ_GOOGLE_TEST_LIBRARY}")

endif()
