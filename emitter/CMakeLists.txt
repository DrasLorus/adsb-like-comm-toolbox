cmake_minimum_required (VERSION 3.16)

project (
  emitter
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
  ../src/Frontend/Frontend.cpp
  ../src/Frontend/BinaryFileSource/BinaryFileSource.cpp
  ../src/Frontend/BMPSource/BMPSource.cpp
  ../src/Frontend/HexSource/HexSource.cpp
  ../src/Frontend/Library/FrontendLibrary.cpp
  ../src/Radio/Emitter/Emitter.cpp
  ../src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
  ../src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
  ../src/Radio/Emitter/File/EmitterFileRAW.cpp
  ../src/Radio/Emitter/Library/EmitterLibrary.cpp
  ../src/Chains/template/Encoder/Encoder_chain.cpp
  ../src/Chains/ADBS-like/Encoder/Encoder_ADBS_like_chain.cpp
  ../src/Chains/ADBS-like-fec/Encoder/Encoder_ADBS_FEC_chain.cpp
  ../src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
  ../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
  ../src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp
  ../src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp
  ../src/Processing/PPM/mod/PPM_mod.cpp
  ../src/Processing/IQ/Insertion/IQ_Insertion.cpp
  ../src/Processing/Sampling/Up/UpSampling.cpp
  ../src/Processing/LDPC/Encoder/LDPCEncoder.cpp
  ../src/Tools/Resizing/resize_vect_u8.cpp
  ../src/Processing/PPM/mod/PPM_mod.cpp
  ../src/Processing/IQ/Insertion/IQ_Insertion.cpp
  ../src/Processing/Sampling/Up/UpSampling.cpp
  ../src/Processing/PPM/demod/PPM_demod.cpp
  ../src/Processing/IQ/Removing/IQ_Removing.cpp
  ../src/Processing/Sampling/Down/DownSampling.cpp
  ../src/emitter.cpp
)

add_executable (emitter ${SOURCE_FILES})

target_link_libraries (emitter uhd)
target_link_libraries (emitter hackrf)
target_link_libraries (emitter SoapySDR)
