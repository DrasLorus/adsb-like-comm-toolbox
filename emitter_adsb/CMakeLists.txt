cmake_minimum_required (VERSION 3.16)

project (
  emitter_adsb
  VERSION 0.1
  LANGUAGES CXX
)

set (
  SOURCE_FILES
  ../src/Processing/CRC32b/CRC/CRC32b.cpp
  ../src/Frame/MLType/MLType.cpp
  ../src/Frame/Frame.cpp
  ../src/Frame/FECFrame.cpp
  ../src/Frame/FrameTools.cpp
  ../src/RingBuff/RingBuff.cpp
  ../src/Tools/Parameters/Parameters.cpp
  ../src/Tools/CTickCounter/CTickCounter.cpp
  ../src/Radio/Receiver/Receiver.cpp
  ../src/Radio/Emitter/Emitter.cpp
  ../src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
  ../src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
  ../src/Radio/Emitter/File/EmitterFileRAW.cpp
  ../src/Radio/Emitter/Library/EmitterLibrary.cpp
  ../src/Processing/PPM/mod/PPM_mod.cpp
  ../src/Processing/IQ/Insertion/IQ_Insertion.cpp
  ../src/Processing/Sampling/Up/UpSampling.cpp
  ../src/emitter_adsb.cpp
)

add_executable (emitter_adsb ${SOURCE_FILES})

target_link_libraries (emitter_adsb uhd)
target_link_libraries (emitter_adsb hackrf)
target_link_libraries (emitter_adsb SoapySDR)
