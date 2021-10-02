cmake_minimum_required (VERSION 3.16)

project (draw-curves)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                       "${CMAKE_SOURCE_DIR}/cmake/Modules/"
)
message ("${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (CMAKE_CXX_STANDARD 14)

set (CMAKE_CXX_FLAGS "-Ofast -march=native -mtune=native")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/opt/hackrf/include")

set (
  CMAKE_EXE_LINKER_FLAGS
  "-L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio -lhackrf"
)
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
# /usr/local/opt/hackrf/lib/libhackrf.a") SET(CMAKE_EXE_LINKER_FLAGS
# "${CMAKE_EXE_LINKER_FLAGS} /usr/local/opt/libusb/lib/libusb-1.0.a")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework
# CoreFoundation") SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
# -framework IOKit")

set (
  SOURCE_FILES
  src/Acquisition/Radio.cpp
  src/Acquisition/RadioFichierRAW.cpp
  src/Acquisition/RadioHackRF.cpp
  src/TemporalHistogram.cpp
  src/RawSound.cpp
  src/SoundSystem.cpp
  src/SoundView.cpp
  src/main.cpp
)

add_executable (draw-curves ${SOURCE_FILES})

find_package (UHD)
if (UHD_FOUND)
  include_directories (${UHD_INCLUDE_DIRS})
  target_link_libraries (draw-curves ${UHD_LIBRARIES})
endif (UHD_FOUND)

find_package (
  Boost
  COMPONENTS system filesystem
  REQUIRED
)

target_link_libraries (
  draw-curves ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
)

cmake_minimum_required (VERSION 3.16)

project (draw-curves-rt)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                       "${CMAKE_SOURCE_DIR}/cmake/Modules/"
)
message ("${CMAKE_SOURCE_DIR}/cmake/Modules/")

set (CMAKE_CXX_STANDARD 14)

set (CMAKE_CXX_FLAGS "-Ofast -march=native -mtune=native")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/opt/hackrf/include")

set (
  CMAKE_EXE_LINKER_FLAGS
  "-L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network -lsfml-audio -lhackrf"
)
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
# /usr/local/opt/hackrf/lib/libhackrf.a") SET(CMAKE_EXE_LINKER_FLAGS
# "${CMAKE_EXE_LINKER_FLAGS} /usr/local/opt/libusb/lib/libusb-1.0.a")
# SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework
# CoreFoundation") SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}
# -framework IOKit")

set (
  SOURCE_FILES
  src/Acquisition/Radio.cpp
  src/Acquisition/RadioFichierRAW.cpp
  src/Acquisition/RadioHackRF.cpp
  src/TemporalHistogram.cpp
  src/RawSound.cpp
  src/SoundSystem.cpp
  src/SoundView.cpp
  src/main-rt.cpp
)

add_executable (draw-curves-rt ${SOURCE_FILES})

find_package (UHD)
if (UHD_FOUND)
  include_directories (${UHD_INCLUDE_DIRS})
  target_link_libraries (draw-curves-rt ${UHD_LIBRARIES})
endif (UHD_FOUND)

find_package (
  Boost
  COMPONENTS system filesystem
  REQUIRED
)

target_link_libraries (
  draw-curves-rt ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY}
)
