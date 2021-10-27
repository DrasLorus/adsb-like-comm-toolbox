cmake_minimum_required (VERSION 3.16)

# set ( CMAKE_CXX_FLAGS "-Wall -Ofast -march=native -mtune=native
# -Wno-unknown-pragmas" )

# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include")

# x86 part

# set (CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib") # UHD

# set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -luhd") # UHD set
# (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lhackrf") # UHD set
# (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lSoapySDR") # SoapySDR

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_EXTENSIONS OFF)

set (CMAKE_EXPORT_COMPILE_COMMANDS true)

project (
  adsb-like-comm-toolbox
  VERSION 0.1
  LANGUAGES CXX
)

option (fast_opt "enable fast code at the expense of stability" ON)

find_package (UHD REQUIRED)

find_library (
  HACKRF
  NAME
  hackrf
  libhackrf
  REQUIRED
)
find_library (
  SOAPYSDR
  NAME
  SoapySDR
  libSoapySDR
  REQUIRED
)

if (fast_opt)
  add_compile_options (-Ofast)
endif ()

add_compile_options (-Wall -Werror -Wno-unknown-pragmas)

if (fast_opt)
  add_compile_options (-Ofast)
  include (CheckCXXCompilerFlag)
  check_cxx_compiler_flag ("-march=native" MARCH)
  check_cxx_compiler_flag ("-mtune=native" MTUNE)
  if ((NOT MARCH) OR (NOT MTUNE))
    check_cxx_compiler_flag ("-mcpu=native" MCPU)
    if (MCPU)
      add_compile_options (-mcpu=native)
    else ()
      message (
        WARNING
          "Can't use native detection, neither -march, -mtune nor -mcpu. Disabled."
      )
    endif ()
  else ()
    add_compile_options (-march=native -mtune=native)
  endif ()
endif ()

if (APPLE)
  include_directories (SYSTEM /usr/local/include)
  link_directories (/usr/local/lib)
endif ()

add_subdirectory (receiver_adsb EXCLUDE_FROM_ALL)

# ##############################################################################

add_subdirectory (receiver_image EXCLUDE_FROM_ALL)

# ##############################################################################

# project(tty_histo)
#
# SET(SOURCE_FILES src/RingBuff/RingBuff.cpp src/tty_histo.cpp )
#
# add_executable(tty_histo ${SOURCE_FILES}) target_link_libraries(tty_histo)
#
# add_subdirectory (tty-histo)

# ##############################################################################

add_subdirectory (emitter_adsb EXCLUDE_FROM_ALL)

# ##############################################################################

add_subdirectory (emitter_image EXCLUDE_FROM_ALL)

# ##############################################################################

add_subdirectory (emitter EXCLUDE_FROM_ALL)

# ##############################################################################

add_subdirectory (receiver EXCLUDE_FROM_ALL)

# ##############################################################################

# project(tty_emitter)
#
# SET(SOURCE_FILES src/CRC/CRC32b.cpp src/Frame/Frame.cpp
# src/RingBuff/RingBuff.cpp src/tty_emitter.cpp )
#
# add_executable(tty_emitter ${SOURCE_FILES})

# ##############################################################################

add_subdirectory (test_vecto EXCLUDE_FROM_ALL)

# ##############################################################################

add_subdirectory (examples)

add_library (commToolbox STATIC)
target_sources (
  commToolbox
  PRIVATE src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp
          src/Processing/CRC32b/CRC/CRC32b.cpp
          src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
          src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
          src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
          src/Processing/Detector/Detector.cpp
          src/Processing/Detector/Detector_x86/Detector_x86.cpp
          src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
          src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
          src/Processing/Detector/Library/DetectorLibrary.cpp
          src/Processing/CplxModule/CplxModule.cpp
          src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
          src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
          src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
          src/Processing/CplxModule/Library/CplxModuleLibrary.cpp
          src/Processing/DataPacking/BitPacking/BitPacking.cpp
          src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp
          src/Processing/IQ/Insertion/IQ_Insertion.cpp
          src/Processing/IQ/Removing/IQ_Removing.cpp
          src/Processing/PPM/mod/PPM_mod.cpp
          src/Processing/PPM/demod/PPM_demod.cpp
          src/Processing/Sampling/Up/UpSampling.cpp
          src/Processing/Sampling/Down/DownSampling.cpp
          src/Radio/Emitter/Emitter.cpp
          src/Radio/Emitter/Library/EmitterLibrary.cpp
          src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
          src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
          src/Radio/Emitter/File/EmitterFileRAW.cpp
          src/Radio/Receiver/Receiver.cpp
          src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
          src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
          src/Radio/Receiver/Radio/SoapyUHD/ReceiverSoapyUHD.cpp
          src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
          src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp
          src/Radio/Receiver/File/ReceiverFileRAW.cpp
          src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
          src/Radio/Receiver/File/ReceiverFileUHD.cpp
          src/Radio/Receiver/File/ReceiverFileBZ2.cpp
          src/Radio/Receiver/Library/ReceiverLibrary.cpp
          src/RingBuff/RingBuff.cpp
          src/Tools/Conversion/cvt_float_i8.cpp
          src/Tools/Conversion/cvt_float_u8.cpp
          src/Tools/Conversion/cvt_u8_float.cpp
          src/Tools/Conversion/cvt_i8_float.cpp
          src/Tools/CTickCounter/CTickCounter.cpp
          src/Tools/ExportVector/ExportVector.cpp
          src/Tools/Parameters/Parameters.cpp
)
target_link_libraries (commToolbox PUBLIC ${UHD_LIBRARIES})
target_include_directories (commToolbox PUBLIC src)
