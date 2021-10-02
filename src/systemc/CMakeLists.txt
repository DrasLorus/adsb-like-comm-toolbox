cmake_minimum_required (VERSION 3.7)

project (LDPC_decoder)

set (CMAKE_CXX_FLAGS "")
set (
  CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-label -I/usr/local/systemc-2.3.1/include"
)
set (CMAKE_CXX_FLAGS
     "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -fmessage-length=0"
)
set (CMAKE_CXX_FLAGS
     "${CMAKE_CXX_FLAGS} -O0 -g -std=c++11 -march=native -mtune=native"
)
set (
  CMAKE_CXX_FLAGS
  "${CMAKE_CXX_FLAGS} -D_SIMULATION_ -Wno-unknown-pragmas -Wno-unused-parameter"
)
set (CMAKE_EXE_LINKER_FLAGS
     "-L/usr/local/systemc-2.3.1/lib-macosx64 -lsystemc -lm"
)

set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS}")

set (
  SOURCE_FILES
  \\
  ./src/module/my_module.cpp
  \\
  ./src/module/my_module_fifo.cpp
  \\
  ./src/main.cpp
)

add_executable (LDPC_decoder ${SOURCE_FILES})
