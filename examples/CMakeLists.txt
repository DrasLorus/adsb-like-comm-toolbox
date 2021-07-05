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

project(debug_rcv_usrp)

set(SOURCE_FILES
        ../src/Processing/CRC32b/CRC/CRC32b.cpp
        ../src/RingBuff/RingBuff.cpp
        ../src/Tools/Parameters/Parameters.cpp
        ../src/Tools/CTickCounter/CTickCounter.cpp
        ../src/Tools/ExportVector/ExportVector.cpp
        ../src/Tools/Conversion/cvt_float_i8.cpp
        ../src/Tools/Conversion/cvt_float_u8.cpp
        ../src/Tools/Conversion/cvt_u8_float.cpp
        ../src/Tools/Conversion/cvt_i8_float.cpp

        ../src/Processing/CplxModule/CplxModule.cpp
        ../src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        ../src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        ../src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        ../src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        ../src/Radio/Receiver/Receiver.cpp
        ../src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
        ../src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
        ../src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
        ../src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp

        ../src/Radio/Receiver/File/ReceiverFileRAW.cpp
        ../src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
        ../src/Radio/Receiver/File/ReceiverFileUHD.cpp
        ../src/Radio/Receiver/Library/ReceiverLibrary.cpp

        ../src/Processing/PPM/demod/PPM_demod.cpp
        ../src/Processing/DataPacking/BitPacking/BitPacking.cpp
        ../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
        ../src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
        ../src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp
        ../src/Processing/IQ/Removing/IQ_Removing.cpp
        ../src/Processing/Sampling/Down/DownSampling.cpp
        ../src/Processing/Detector/Detector.cpp
        ../src/Processing/Detector/Detector_x86/Detector_x86.cpp
        ../src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        ../src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        ../src/Processing/Detector/Library/DetectorLibrary.cpp

        rcv_usrp/rcv_usrp.cpp
        )

add_executable(debug_rcv_usrp ${SOURCE_FILES})

target_link_libraries(debug_rcv_usrp uhd)
target_link_libraries(debug_rcv_usrp hackrf)
target_link_libraries(debug_rcv_usrp SoapySDR)

########################################################################################################################

project(rcv_adsb)

set(SOURCE_FILES
        ../src/Processing/CRC32b/CRC/CRC32b.cpp
        ../src/RingBuff/RingBuff.cpp
        ../src/Tools/Parameters/Parameters.cpp
        ../src/Tools/CTickCounter/CTickCounter.cpp
        ../src/Tools/ExportVector/ExportVector.cpp
        ../src/Tools/Conversion/cvt_float_i8.cpp
        ../src/Tools/Conversion/cvt_float_u8.cpp
        ../src/Tools/Conversion/cvt_u8_float.cpp
        ../src/Tools/Conversion/cvt_i8_float.cpp

        ../src/Processing/CplxModule/CplxModule.cpp
        ../src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        ../src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        ../src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        ../src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        ../src/Radio/Receiver/Receiver.cpp
        ../src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
        ../src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
        ../src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
        ../src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp

        ../src/Radio/Receiver/File/ReceiverFileRAW.cpp
        ../src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
        ../src/Radio/Receiver/File/ReceiverFileUHD.cpp
        ../src/Radio/Receiver/Library/ReceiverLibrary.cpp

        ../src/Processing/PPM/demod/PPM_demod.cpp
        ../src/Processing/DataPacking/BitPacking/BitPacking.cpp
        ../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
        ../src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
        ../src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp
        ../src/Processing/IQ/Removing/IQ_Removing.cpp
        ../src/Processing/Sampling/Down/DownSampling.cpp
        ../src/Processing/Detector/Detector.cpp
        ../src/Processing/Detector/Detector_x86/Detector_x86.cpp
        ../src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        ../src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        ../src/Processing/Detector/Library/DetectorLibrary.cpp

        rcv_adsb/rcv_adsb.cpp
        )

add_executable(rcv_adsb ${SOURCE_FILES})

target_link_libraries(rcv_adsb uhd)
target_link_libraries(rcv_adsb hackrf)
target_link_libraries(rcv_adsb SoapySDR)

########################################################################################################################

project(rcv_calibration)

set(SOURCE_FILES
        ../src/Processing/CRC32b/CRC/CRC32b.cpp
        ../src/RingBuff/RingBuff.cpp
        ../src/Tools/Parameters/Parameters.cpp
        ../src/Tools/CTickCounter/CTickCounter.cpp
        ../src/Tools/ExportVector/ExportVector.cpp
        ../src/Tools/Conversion/cvt_float_i8.cpp
        ../src/Tools/Conversion/cvt_float_u8.cpp
        ../src/Tools/Conversion/cvt_u8_float.cpp
        ../src/Tools/Conversion/cvt_i8_float.cpp

        ../src/Processing/CplxModule/CplxModule.cpp
        ../src/Processing/CplxModule/CplxModule_x86/CplxModule_x86.cpp
        ../src/Processing/CplxModule/CplxModule_NEON/CplxModule_NEON.cpp
        ../src/Processing/CplxModule/CplxModule_AVX2/CplxModule_AVX2.cpp
        ../src/Processing/CplxModule/Library/CplxModuleLibrary.cpp

        ../src/Radio/Receiver/Receiver.cpp
        ../src/Radio/Receiver/Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.cpp
        ../src/Radio/Receiver/Radio/SoapyHackRF/ReceiverSoapyHackRF.cpp
        ../src/Radio/Receiver/Radio/HackRF/ReceiverHackRF.cpp
        ../src/Radio/Receiver/Radio/USRP/ReceiverUSRP.cpp

        ../src/Radio/Receiver/File/ReceiverFileRAW.cpp
        ../src/Radio/Receiver/File/ReceiverFileStreamRAW.cpp
        ../src/Radio/Receiver/File/ReceiverFileUHD.cpp
        ../src/Radio/Receiver/Library/ReceiverLibrary.cpp

        ../src/Processing/PPM/demod/PPM_demod.cpp
        ../src/Processing/DataPacking/BitPacking/BitPacking.cpp
        ../src/Processing/CRC32b/CheckCRC32b/CheckCRC32b.cpp
        ../src/Processing/CRC32b/RemoveCRC32b/RemoveCRC32b.cpp
        ../src/Processing/ADSBSynchro/RemoveADSBSynchro/RemoveADSBSynchro.cpp
        ../src/Processing/IQ/Removing/IQ_Removing.cpp
        ../src/Processing/Sampling/Down/DownSampling.cpp
        ../src/Processing/Detector/Detector.cpp
        ../src/Processing/Detector/Detector_x86/Detector_x86.cpp
        ../src/Processing/Detector/Detector_NEON/Detector_NEON.cpp
        ../src/Processing/Detector/Detector_AVX2/Detector_AVX2.cpp
        ../src/Processing/Detector/Library/DetectorLibrary.cpp

        rcv_calibration/rcv_calibration.cpp
        )

add_executable(rcv_calibration ${SOURCE_FILES})

target_link_libraries(rcv_calibration uhd)
target_link_libraries(rcv_calibration hackrf)
target_link_libraries(rcv_calibration SoapySDR)

########################################################################################################################

project(debug_snd_usrp)

set(SOURCE_FILES
        ../src/RingBuff/RingBuff.cpp
        ../src/Tools/Parameters/Parameters.cpp

        ../src/Radio/Emitter/Emitter.cpp
        ../src/Radio/Emitter/Library/EmitterLibrary.cpp
        ../src/Radio/Emitter/Radio/HackRF/EmitterHackRF.cpp
        ../src/Radio/Emitter/Radio/USRP/EmitterUSRP.cpp
        ../src/Radio/Emitter/File/EmitterFileRAW.cpp

        ../src/Processing/CRC32b/CRC/CRC32b.cpp
        ../src/Processing/CRC32b/InsertCRC32b/InsertCRC32b.cpp
        ../src/Processing/DataPacking/BitUnpacking/BitUnpacking.cpp
        ../src/Processing/Sampling/Up/UpSampling.cpp
        ../src/Processing/IQ/Insertion/IQ_Insertion.cpp
        ../src/Processing/PPM/mod/PPM_mod.cpp
        ../src/Processing/IQ/Removing/IQ_Removing.cpp
        ../src/Processing/Sampling/Down/DownSampling.cpp
        ../src/Processing/ADSBSynchro/InsertADSBSynchro/InsertADSBSynchro.cpp

        snd_usrp/snd_usrp.cpp
        )

add_executable(debug_snd_usrp ${SOURCE_FILES})

target_link_libraries(debug_snd_usrp uhd)
target_link_libraries(debug_snd_usrp hackrf)
target_link_libraries(debug_snd_usrp SoapySDR)

