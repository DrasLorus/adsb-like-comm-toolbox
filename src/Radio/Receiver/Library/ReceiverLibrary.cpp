#include "ReceiverLibrary.hpp"

#include "../File/ReceiverFileRAW.hpp"
#include "../File/ReceiverFileUHD.hpp"
#include "../File/ReceiverFileStreamRAW.hpp"
#include "../File/ReceiverFileBZ2.hpp"

#include "../Radio/HackRF/ReceiverHackRF.hpp"
#include "../Radio/SoapyHackRF/ReceiverSoapyHackRF.hpp"
#include "../Radio/SoapyUHD/ReceiverSoapyUHD.hpp"
#include "../Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.hpp"
#include "../Radio/USRP/ReceiverUSRP.hpp"
#include "../Radio/ThreadUSRP/ReceiverThreadUSRP.hpp"

#include <algorithm>

inline bool iequals(const string& a, const string& b)
{
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}



ReceiverLibrary::ReceiverLibrary()
{

}


ReceiverLibrary::~ReceiverLibrary()
{

}

Receiver* ReceiverLibrary::allocate(Parameters& param)
{
    std::string type   = param.toString("mode_radio");
    std::string module = param.toString("filename");

    transform(type.begin(), type.end(), type.begin(), ::tolower);

    Receiver* radio;
    //
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    if(
            (type == "radio" && iequals(module, "hackrf"))
    ) {
        ReceiverHackRF* r = new ReceiverHackRF(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("hackrf_amplifier") != -1 ) r->set_amp_enable( param.toInt("hackrf_amplifier") );
        if( param.toInt("receiver_gain"   ) != -1 ) r->set_vga_gain  ( param.toInt("receiver_gain") );
        if( param.toInt("receiver_gain"   ) != -1 ) r->set_lna_gain  ( param.toInt("receiver_gain") );
        radio = r;

    } else if(
            (type == "radio" && iequals(module, "soapyhackrf" )) ||
            (type == "radio" && iequals(module, "soapy_hackrf"))
    ) {
        ReceiverSoapyHackRF* r = new ReceiverSoapyHackRF(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("hackrf_amplifier") != -1 ) r->set_amp_enable( param.toInt("hackrf_amplifier") );
        if( param.toInt("receiver_gain")    != -1 ) r->set_vga_gain  ( param.toInt("receiver_gain") );
        if( param.toInt("receiver_gain")    != -1 ) r->set_lna_gain  ( param.toInt("receiver_gain") );
        radio = r;
    //
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    } else if(
            (type == "radio" && iequals(module, "soapyrtlsdr" )) ||
            (type == "radio" && iequals(module, "soapy_rtlsdr"))
    ) {
        ReceiverSoapyRTLSdr* r = new ReceiverSoapyRTLSdr(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("receiver_gain")  != -1 )
            r->set_tuner_gain( param.toDouble("receiver_gain") );
        radio = r;
    //
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    } else if(
            (type == "radio" && iequals(module, "usrp"))
            ) {
        ReceiverUSRP* r = new ReceiverUSRP(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("receiver_gain")  != -1 )
            r->set_rx_gain( param.toInt("receiver_gain") );
        radio = r;

    } else if(
            (type == "radio" && iequals(module, "threadusrp") ) ||
            (type == "radio" && iequals(module, "thread_usrp"))
            ) {
        ReceiverThreadUSRP* r = new ReceiverThreadUSRP(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("receiver_gain")  != -1 )
            r->set_rx_gain( param.toInt("receiver_gain") );
        radio = r;

    } else if(
            (type == "radio" && iequals(module, "soapyusrp") ) ||
            (type == "radio" && iequals(module, "soapy_usrp"))
            ) {
        ReceiverSoapyUHD* r = new ReceiverSoapyUHD(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("receiver_gain")  != -1 )
            r->set_gain( param.toInt("receiver_gain") );
        radio = r;
    //
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    } else if( type == "file" && (module.find(".raw") != std::string::npos) ) {
        radio = new ReceiverFileRAW(param.toString("filename"));                    // SIGNED 8b

    } else if( type == "file" && (module.find(".cs8") != std::string::npos) ) {
        radio = new ReceiverFileRAW(param.toString("filename"));                    // SIGNED 8b

    } else if( type == "file" && (module.find(".cu8") != std::string::npos) ) {
        radio = new ReceiverFileRAW(param.toString("filename"), true);    // UNSIGNED 8b

    } else if( type == "file" && (module.find(".txt") != std::string::npos) ) {
        radio = new ReceiverFileUHD(param.toString("filename"));
    //
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    } else if( type == "file-stream" && (module.find(".raw") != std::string::npos) ) {
        ReceiverFileStreamRAW* r = new ReceiverFileStreamRAW(param.toString("filename"));
        radio = r;
    } else if( type == "file-stream" && (module.find(".cs8.bz2") != std::string::npos) ) {
        ReceiverFileBZ2* r = new ReceiverFileBZ2(param.toString("filename"), false);
        radio = r;
    } else if( type == "file-stream" && (module.find(".cu8.bz2") != std::string::npos) ) {
        ReceiverFileBZ2* r = new ReceiverFileBZ2(param.toString("filename"), true);
        radio = r;
    } else if( type == "file-stream" && (module.find(".cs8") != std::string::npos) ) {
        ReceiverFileStreamRAW* r = new ReceiverFileStreamRAW(param.toString("filename"), false);
        radio = r;
    } else if( type == "file-stream" && (module.find(".cu8") != std::string::npos) ) {
        ReceiverFileStreamRAW* r = new ReceiverFileStreamRAW(param.toString("filename"), true);
        radio = r;
    }
    //
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //
    else
    {
        cout << "Error in file (" << __FILE__ << ")" << " at line (" << __LINE__ << ")" << endl;
        cout << "mode_radio = " << type << endl;
        cout << "filename   = " << param.toString("filename")      << endl;
        exit( EXIT_FAILURE );
    }
    return radio;
}
