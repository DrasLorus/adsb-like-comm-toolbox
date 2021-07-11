#ifndef _ReceiverSoapyRTLSdr_
#define _ReceiverSoapyRTLSdr_

#include "../../Receiver.hpp"
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

class ReceiverSoapyRTLSdr : public Receiver{
private :
    SoapySDR::Device *sdr;
    SoapySDR::Stream *rx_stream;

public :
    ReceiverSoapyRTLSdr( const float s_fc, const float s_fe);
    ~ReceiverSoapyRTLSdr();

    void initialize();
    void start_engine();
    void stop_engine ();

    bool reception(vector<complex<float> >& cbuffer, const uint32_t coverage = 0);

    void reset();

    void   set_freq(const double value);
    double get_freq( );

    void   set_sample_rate(const double value);
    double get_sample_rate( );

    void   set_tuner_gain(const double value);
    double get_tuner_gain( );
};

#endif