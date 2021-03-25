cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_FLAGS "                   -Wall -Ofast -march=native -mtune=native -Wno-unknown-pragmas")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/opt/hackrf/include")            # HackRF
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/Cellar/boost/1.74.0/include")   # Boost
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/Cellar/uhd/4.0.0.0/include -Wno-#pragma-messages")    # UHD
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Rpass=loop-vectorize")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include")


# x86 part
set(CMAKE_EXE_LINKER_FLAGS "-L/usr/local/lib") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "                          /usr/local/lib/libboost_filesystem-mt.dylib") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /usr/local/lib/libboost_system-mt.dylib") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /usr/local/opt/hackrf/lib/libhackrf.a") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /usr/local/opt/libusb/lib/libusb-1.0.a") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /usr/local/Cellar/uhd/4.0.0.0/lib/libuhd.dylib") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreFoundation -framework IOKit") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lncurses") # UHD
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/lib -lSoapySDR") # SoapySDR

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -luhd") # UHD
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lhackrf") # UHD
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lSoapySDR") # SoapySDR

set(CMAKE_CXX_STANDARD 17)

########################################################################################################################

project(receiver_adsb)

set(SOURCE_FILES
        src/Processing/CRC32b/CRC/CRC32b.cpp

        src/Frame/MLType/MLType.cpp
        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/RingBuff/RingBuff.cpp
        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Tools/ExportVector/ExportVector.cpp
        src/Tools/Conversion/cvt_float_i8.cpp
        src/Tools/Conversion/cvt_float_u8.cpp
        src/Tools/Conversion/cvt_u8_float.cpp
        src/Tools/Conversion/cvt_i8_float.cpp

        src/Processing/CplxModule/CplxModule.cpp
        src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        src/Radio/Receiver/Receiver.cpp
        src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
        src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
        src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
        src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp

        src/Radio/Receiver/File/ReceiverFileRAW.cpp
        src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
        src/Radio/Receiver/File/ReceiverFileUHD.cpp
        src/Radio/Receiver/Library/ReceiverLibrary.cpp

        src/Processing/PPM/demod/PPM_demod.cpp
        src/Processing/IQ/Removing/IQ_Removing.cpp
        src/Processing/Sampling/Down/DownSampling.cpp
        src/Processing/Detector/Detector.cpp
        src/Processing/Detector/Detector_x86/Detector_x86.cpp
        src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        src/Processing/Detector/Library/DetectorLibrary.cpp

        src/receiver_adsb.cpp
        )

add_executable(receiver_adsb ${SOURCE_FILES})

target_link_libraries(receiver_adsb uhd)
target_link_libraries(receiver_adsb hackrf)
target_link_libraries(receiver_adsb SoapySDR)


########################################################################################################################

project(receiver_image)

set(SOURCE_FILES
        src/Radio/Receiver/Receiver.cpp
        src/Radio/Receiver/File/ReceiverFileRAW.cpp
        src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
        src/Radio/Receiver/File/ReceiverFileUHD.cpp
        src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
        src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
        src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
        src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp
        src/Radio/Receiver/Library/ReceiverLibrary.cpp

        src/Backend/Backend.cpp
        src/Backend/BMPFileDest/BMPFileDest.cpp
        src/Backend/BinaryFileDest/BinaryFileDest.cpp
        src/Backend/FPGADest/FPGADest.cpp
        src/Backend/NullDest/NullDest.cpp
        src/Backend/TerminalDest/TerminalDest.cpp
        src/Backend/Library/BackendLibrary.cpp

        src/Processing/CplxModule/CplxModule.cpp
        src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        src/Processing/CRC32b/CRC/CRC32b.cpp
        src/Frame/MLType/MLType.cpp

        src/Processing/Detector/Detector.cpp
        src/Processing/Detector/Detector_x86/Detector_x86.cpp
        src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        src/Processing/Detector/Library/DetectorLibrary.cpp

        src/Tools/Conversion/cvt_float_i8.cpp
        src/Tools/Conversion/cvt_float_u8.cpp
        src/Tools/Conversion/cvt_u8_float.cpp
        src/Tools/Conversion/cvt_i8_float.cpp

        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/Chains/template/Decoder/Decoder_chain.cpp
        src/Chains/ADBS-like/Decoder/Decoder_ADBS_like_chain.cpp
        src/Chains/ADBS-like-fec/Decoder/Decoder_ADBS_FEC_chain.cpp

        src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
        src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp

        src/Processing/DataPacking/BitPacking/BitPacking.cpp

        src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp

        src/Processing/PPM/demod/PPM_demod.cpp

        src/Processing/IQ/Removing/IQ_Removing.cpp

        src/Processing/Sampling/Down/DownSampling.cpp

        src/Processing/LDPC/Decoder/LDPCDecoder.cpp
        src/Processing/LDPC/Decoder/802_16e_1152x576/802_16e_1152x576_1f_m256i.cpp

        src/Tools/Resizing/resize_vect_u8.cpp



        src/Processing/IQ/Removing/IQ_Removing.cpp

        src/Processing/PPM/demod/PPM_demod.cpp
        src/RingBuff/RingBuff.cpp

        src/Processing/Sampling/Down/DownSampling.cpp

        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Tools/ExportVector/ExportVector.cpp

        src/receiver_image.cpp
        )

add_executable(receiver_image ${SOURCE_FILES})

target_link_libraries(receiver_image uhd)
target_link_libraries(receiver_image hackrf)
target_link_libraries(receiver_image SoapySDR)


########################################################################################################################

#project(tty_histo)
#
#SET(SOURCE_FILES
#        src/RingBuff/RingBuff.cpp
#        src/tty_histo.cpp
#        )
#
#add_executable(tty_histo ${SOURCE_FILES})
#target_link_libraries(tty_histo)

########################################################################################################################

project(emitter_adsb)

set(SOURCE_FILES
        src/Processing/CRC32b/CRC/CRC32b.cpp
        src/Frame/MLType/MLType.cpp

        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/RingBuff/RingBuff.cpp
        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Radio/Receiver/Receiver.cpp

        src/Radio/Emitter/Emitter.cpp
        src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
        src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
        src/Radio/Emitter/File/EmitterFileRAW.cpp
        src/Radio/Emitter/Library/EmitterLibrary.cpp

        src/Processing/PPM/mod/PPM_mod.cpp

        src/Processing/IQ/Insertion/IQ_Insertion.cpp
        src/Processing/Sampling/Up/UpSampling.cpp
        src/emitter_adsb.cpp
        )

add_executable(emitter_adsb ${SOURCE_FILES})

target_link_libraries(emitter_adsb uhd)
target_link_libraries(emitter_adsb hackrf)
target_link_libraries(emitter_adsb SoapySDR)


########################################################################################################################

project(emitter_image)

set(SOURCE_FILES
        src/Processing/CRC32b/CRC/CRC32b.cpp
        src/Frame/MLType/MLType.cpp

        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/RingBuff/RingBuff.cpp
        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Radio/Receiver/Receiver.cpp

        src/Frontend/Frontend.cpp
        src/Frontend/BinaryFileSource/BinaryFileSource.cpp
        src/Frontend/BMPSource/BMPSource.cpp
        src/Frontend/HexSource/HexSource.cpp
        src/Frontend/Library/FrontendLibrary.cpp

        src/Radio/Emitter/Emitter.cpp
        src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
        src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
        src/Radio/Emitter/File/EmitterFileRAW.cpp
        src/Radio/Emitter/Library/EmitterLibrary.cpp

        src/Chains/template/Encoder/Encoder_chain.cpp
        src/Chains/ADBS-like/Encoder/Encoder_ADBS_like_chain.cpp
        src/Chains/ADBS-like-fec/Encoder/Encoder_ADBS_FEC_chain.cpp

        src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
        src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
        src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp
        src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp
        src/Processing/PPM/mod/PPM_mod.cpp
        src/Processing/IQ/Insertion/IQ_Insertion.cpp
        src/Processing/Sampling/Up/UpSampling.cpp
        src/Processing/LDPC/Encoder/LDPCEncoder.cpp
        src/Tools/Resizing/resize_vect_u8.cpp


        src/Processing/PPM/mod/PPM_mod.cpp
        src/Processing/IQ/Insertion/IQ_Insertion.cpp
        src/Processing/Sampling/Up/UpSampling.cpp
        src/Processing/PPM/demod/PPM_demod.cpp
        src/Processing/IQ/Removing/IQ_Removing.cpp
        src/Processing/Sampling/Down/DownSampling.cpp
        src/emitter_image.cpp
        )

add_executable(emitter_image ${SOURCE_FILES})

target_link_libraries(emitter_image uhd)
target_link_libraries(emitter_image hackrf)
target_link_libraries(emitter_image SoapySDR)


########################################################################################################################
project(emitter)

set(SOURCE_FILES
        src/Processing/CRC32b/CRC/CRC32b.cpp
        src/Frame/MLType/MLType.cpp

        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/RingBuff/RingBuff.cpp
        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Radio/Receiver/Receiver.cpp

        src/Frontend/Frontend.cpp
        src/Frontend/BinaryFileSource/BinaryFileSource.cpp
        src/Frontend/BMPSource/BMPSource.cpp
        src/Frontend/HexSource/HexSource.cpp
        src/Frontend/Library/FrontendLibrary.cpp

        src/Radio/Emitter/Emitter.cpp
        src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
        src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
        src/Radio/Emitter/File/EmitterFileRAW.cpp
        src/Radio/Emitter/Library/EmitterLibrary.cpp

        src/Chains/template/Encoder/Encoder_chain.cpp
        src/Chains/ADBS-like/Encoder/Encoder_ADBS_like_chain.cpp
        src/Chains/ADBS-like-fec/Encoder/Encoder_ADBS_FEC_chain.cpp

        src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
        src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
        src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp
        src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp
        src/Processing/PPM/mod/PPM_mod.cpp
        src/Processing/IQ/Insertion/IQ_Insertion.cpp
        src/Processing/Sampling/Up/UpSampling.cpp
        src/Processing/LDPC/Encoder/LDPCEncoder.cpp
        src/Tools/Resizing/resize_vect_u8.cpp


        src/Processing/PPM/mod/PPM_mod.cpp
        src/Processing/IQ/Insertion/IQ_Insertion.cpp
        src/Processing/Sampling/Up/UpSampling.cpp
        src/Processing/PPM/demod/PPM_demod.cpp
        src/Processing/IQ/Removing/IQ_Removing.cpp
        src/Processing/Sampling/Down/DownSampling.cpp
        src/emitter.cpp
        )

add_executable(emitter ${SOURCE_FILES})

target_link_libraries(emitter uhd)
target_link_libraries(emitter hackrf)
target_link_libraries(emitter SoapySDR)


########################################################################################################################
project(receiver)

set(SOURCE_FILES
        src/Radio/Receiver/Receiver.cpp
        src/Radio/Receiver/File/ReceiverFileRAW.cpp
        src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
        src/Radio/Receiver/File/ReceiverFileUHD.cpp
        src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
        src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
        src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
        src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp
        src/Radio/Receiver/Library/ReceiverLibrary.cpp

        src/Backend/Backend.cpp
        src/Backend/BMPFileDest/BMPFileDest.cpp
        src/Backend/BinaryFileDest/BinaryFileDest.cpp
        src/Backend/FPGADest/FPGADest.cpp
        src/Backend/NullDest/NullDest.cpp
        src/Backend/TerminalDest/TerminalDest.cpp
        src/Backend/Library/BackendLibrary.cpp

        src/Processing/CplxModule/CplxModule.cpp
        src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        src/Processing/CRC32b/CRC/CRC32b.cpp
        src/Frame/MLType/MLType.cpp

        src/Processing/Detector/Detector.cpp
        src/Processing/Detector/Detector_x86/Detector_x86.cpp
        src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        src/Processing/Detector/Library/DetectorLibrary.cpp

        src/Tools/Conversion/cvt_float_i8.cpp
        src/Tools/Conversion/cvt_float_u8.cpp
        src/Tools/Conversion/cvt_u8_float.cpp
        src/Tools/Conversion/cvt_i8_float.cpp

        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/Chains/template/Decoder/Decoder_chain.cpp
        src/Chains/ADBS-like/Decoder/Decoder_ADBS_like_chain.cpp
        src/Chains/ADBS-like-fec/Decoder/Decoder_ADBS_FEC_chain.cpp

        src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
        src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp

        src/Processing/DataPacking/BitPacking/BitPacking.cpp

        src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp

        src/Processing/PPM/demod/PPM_demod.cpp

        src/Processing/IQ/Removing/IQ_Removing.cpp

        src/Processing/Sampling/Down/DownSampling.cpp

        src/Processing/LDPC/Decoder/LDPCDecoder.cpp
        src/Processing/LDPC/Decoder/802_16e_1152x576/802_16e_1152x576_1f_m256i.cpp

        src/Tools/Resizing/resize_vect_u8.cpp



        src/Processing/IQ/Removing/IQ_Removing.cpp

        src/Processing/PPM/demod/PPM_demod.cpp
        src/RingBuff/RingBuff.cpp

        src/Processing/Sampling/Down/DownSampling.cpp

        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Tools/ExportVector/ExportVector.cpp

        src/receiver.cpp
        )

add_executable(receiver ${SOURCE_FILES})

target_link_libraries(receiver uhd)
target_link_libraries(receiver hackrf)
target_link_libraries(receiver SoapySDR)


########################################################################################################################


#project(tty_emitter)
#
#SET(SOURCE_FILES
#        src/CRC/CRC32b.cpp
#        src/Frame/Frame.cpp
#        src/RingBuff/RingBuff.cpp
#        src/tty_emitter.cpp
#        )
#
#add_executable(tty_emitter ${SOURCE_FILES})

########################################################################################################################

project(test_vecto)

SET(SOURCE_FILES
        src/Processing/CRC32b/CRC/CRC32b.cpp
        src/Frame/MLType/MLType.cpp

        src/Frame/Frame.cpp
        src/Frame/FECFrame.cpp
        src/Frame/FrameTools.cpp

        src/RingBuff/RingBuff.cpp
        src/Tools/Parameters/Parameters.cpp
        src/Tools/CTickCounter/CTickCounter.cpp
        src/Tools/ExportVector/ExportVector.cpp

        src/Chains/template/Encoder/Encoder_chain.cpp
        src/Chains/template/Decoder/Decoder_chain.cpp

        src/Chains/ADBS-like/Encoder/Encoder_ADBS_like_chain.cpp
        src/Chains/ADBS-like/Decoder/Decoder_ADBS_like_chain.cpp

        src/Chains/ADBS-like-fec/Encoder/Encoder_ADBS_FEC_chain.cpp
        src/Chains/ADBS-like-fec/Decoder/Decoder_ADBS_FEC_chain.cpp

        src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
        src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
        src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp

        src/Processing/DataPacking/BitPacking/BitPacking.cpp
        src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp

        src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp
        src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp

        src/Processing/PPM/mod/PPM_mod.cpp
        src/Processing/PPM/demod/PPM_demod.cpp

        src/Processing/IQ/Insertion/IQ_Insertion.cpp
        src/Processing/IQ/Removing/IQ_Removing.cpp

        src/Processing/Sampling/Up/UpSampling.cpp
        src/Processing/Sampling/Down/DownSampling.cpp

        src/Processing/LDPC/Encoder/LDPCEncoder.cpp
        src/Processing/LDPC/Decoder/LDPCDecoder.cpp
        src/Processing/LDPC/Decoder/802_16e_1152x576/802_16e_1152x576_1f_m256i.cpp

        src/Tools/Resizing/resize_vect_u8.cpp

        src/Tools/Conversion/cvt_float_i8.cpp
        src/Tools/Conversion/cvt_float_u8.cpp
        src/Tools/Conversion/cvt_u8_float.cpp
        src/Tools/Conversion/cvt_i8_float.cpp

        src/Processing/CplxModule/CplxModule.cpp
        src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        src/Processing/PPM/demod/PPM_demod.cpp
        src/Processing/IQ/Removing/IQ_Removing.cpp
        src/Processing/Sampling/Down/DownSampling.cpp
        src/Processing/Detector/Detector.cpp
        src/Processing/Detector/Detector_x86/Detector_x86.cpp
        src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        src/Processing/Detector/Library/DetectorLibrary.cpp

        src/Tools/Conversion/cvt_float_i8.cpp
        src/Tools/Conversion/cvt_float_u8.cpp
        src/Tools/Conversion/cvt_u8_float.cpp
        src/Tools/Conversion/cvt_i8_float.cpp

        src/test_vecto.cpp
        )

add_executable(test_vecto ${SOURCE_FILES})

########################################################################################################################
