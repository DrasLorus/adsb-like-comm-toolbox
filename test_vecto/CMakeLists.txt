cmake_minimum_required (VERSION 3.16)

project (
  test_vecto
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
  ../src/Tools/ExportVector/ExportVector.cpp
  ../src/Chains/template/Encoder/Encoder_chain.cpp
  ../src/Chains/template/Decoder/Decoder_chain.cpp
  ../src/Chains/ADBS-like/Encoder/Encoder_ADBS_like_chain.cpp
  ../src/Chains/ADBS-like/Decoder/Decoder_ADBS_like_chain.cpp
  ../src/Chains/ADBS-like-fec/Encoder/Encoder_ADBS_FEC_chain.cpp
  ../src/Chains/ADBS-like-fec/Decoder/Decoder_ADBS_FEC_chain.cpp
  ../src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
  ../src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
  ../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
  ../src/Processing/DataPacking/BitPacking/BitPacking.cpp
  ../src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp
  ../src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp
  ../src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp
  ../src/Processing/PPM/mod/PPM_mod.cpp
  ../src/Processing/PPM/demod/PPM_demod.cpp
  ../src/Processing/IQ/Insertion/IQ_Insertion.cpp
  ../src/Processing/IQ/Removing/IQ_Removing.cpp
  ../src/Processing/Sampling/Up/UpSampling.cpp
  ../src/Processing/Sampling/Down/DownSampling.cpp
  ../src/Processing/LDPC/Encoder/LDPCEncoder.cpp
  ../src/Processing/LDPC/Decoder/LDPCDecoder.cpp
  ../src/Processing/LDPC/Decoder/802_16e_1152x576/802_16e_1152x576_1f_m256i.cpp
  ../src/Tools/Resizing/resize_vect_u8.cpp
  ../src/Tools/Conversion/cvt_float_i8.cpp
  ../src/Tools/Conversion/cvt_float_u8.cpp
  ../src/Tools/Conversion/cvt_u8_float.cpp
  ../src/Tools/Conversion/cvt_i8_float.cpp
  ../src/Processing/CplxModule/CplxModule.cpp
  ../src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
  ../src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
  ../src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
  ../src/Processing/CplxModule/Library/CplxModuleLibrary.cpp
  ../src/Processing/PPM/demod/PPM_demod.cpp
  ../src/Processing/IQ/Removing/IQ_Removing.cpp
  ../src/Processing/Sampling/Down/DownSampling.cpp
  ../src/Processing/Detector/Detector.cpp
  ../src/Processing/Detector/Detector_x86/Detector_x86.cpp
  ../src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
  ../src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
  ../src/Processing/Detector/Library/DetectorLibrary.cpp
  ../src/Tools/Conversion/cvt_float_i8.cpp
  ../src/Tools/Conversion/cvt_float_u8.cpp
  ../src/Tools/Conversion/cvt_u8_float.cpp
  ../src/Tools/Conversion/cvt_i8_float.cpp
  ../src/test_vecto.cpp
)

add_executable (test_vecto ${SOURCE_FILES})
