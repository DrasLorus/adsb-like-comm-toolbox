#include "ReceiverLibrary.hpp"

#include "../File/ReceiverFileRAW.hpp"
#include "../File/ReceiverFileUHD.hpp"
#include "../File/ReceiverFileStreamRAW.hpp"

#include "../Radio/HackRF/ReceiverHackRF.hpp"
#include "../Radio/SoapyHackRF/ReceiverSoapyHackRF.hpp"
#include "../Radio/SoapyRTLSdr/ReceiverSoapyRTLSdr.hpp"
#include "../Radio/USRP/ReceiverUSRP.hpp"

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
    if(
            (type == "radio" && module == "hackrf") ||
            (type == "radio" && module == "HackRF")
    ) {
        ReceiverHackRF* r = new ReceiverHackRF(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("hackrf_amplifier") != -1 ) r->set_amp_enable( param.toInt("hackrf_amplifier") );
        if( param.toInt("receiver_gain")  != -1 ) r->set_vga_gain  ( param.toInt("receiver_gain") );
        if( param.toInt("receiver_gain")  != -1 ) r->set_lna_gain  ( param.toInt("receiver_gain") );
        radio = r;

    } else if(
            (type == "radio" && module == "SoapyHackRF") ||
            (type == "radio" && module == "soapyhackrf")
    ) {
        ReceiverSoapyHackRF* r = new ReceiverSoapyHackRF(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("hackrf_amplifier") != -1 ) r->set_amp_enable( param.toInt("hackrf_amplifier") );
//        if( param.toInt("hackrf_vga_gain")  != -1 ) r->set_vga_gain  ( param.toInt("hackrf_vga_gain") );
//        if( param.toInt("hackrf_lna_gain")  != -1 ) r->set_lna_gain  ( param.toInt("hackrf_lna_gain") );
        if( param.toInt("receiver_gain")  != -1 ) r->set_vga_gain  ( param.toInt("receiver_gain") );
        if( param.toInt("receiver_gain")  != -1 ) r->set_lna_gain  ( param.toInt("receiver_gain") );
        radio = r;

    } else if(
            (type == "radio" && module == "SoapyRTLSdr") ||
            (type == "radio" && module == "soapyrtlsdr")
    ) {
        ReceiverSoapyRTLSdr* r = new ReceiverSoapyRTLSdr(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("receiver_gain")  != -1 )
            r->set_tuner_gain( param.toDouble("receiver_gain") );
        radio = r;

    } else if(
            (type == "radio" && module == "usrp")
    ) {
        ReceiverUSRP* r = new ReceiverUSRP(param.toDouble("fc"), param.toDouble("fe"));
        if( param.toInt("receiver_gain")  != -1 )
            r->set_rx_gain( param.toInt("receiver_gain") );

        radio = r;

    //
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
    //
    } else if( type == "file-stream" && (module.find(".raw") != std::string::npos) ) {
        ReceiverFileStreamRAW* r = new ReceiverFileStreamRAW(param.toString("filename"));
        radio = r;
    } else if( type == "file-stream" && (module.find(".cs8") != std::string::npos) ) {
        ReceiverFileStreamRAW* r = new ReceiverFileStreamRAW(param.toString("filename"));
        radio = r;
    }
    else
    {
        cout << "Error in file (" << __FILE__ << ")" << " at line (" << __LINE__ << ")" << endl;
        cout << "mode_radio = " << type << endl;
        cout << "filename   = " << param.toString("filename")      << endl;
        exit( EXIT_FAILURE );
    }
    return radio;
}
