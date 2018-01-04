
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -O2 -DNDEBUG")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -g")
if (true)
    # refer to "Useful GCC flags for C", http://stackoverflow.com/questions/3375697/useful-gcc-flags-for-c
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wpointer-arith -Wunreachable-code")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-align" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-result" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings ")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-overflow=5")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wswitch-default -Wswitch-enum")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshadow")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Waggregate-return")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-prototypes")
endif()

if(UNIX AND NOT APPLE AND NOT ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    set(my_c_flags "${my_c_flags} -fopenmp") # enable openmp
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${my_c_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${my_c_flags}")
