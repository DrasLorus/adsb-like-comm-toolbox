#ifndef _ReceiverSoapy_
#define _ReceiverSoapy_

#include "../../Emitter.hpp"
#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

/*
  freq=<float>    Desired tune frequency in Hz. Valid range from 1M to 6G. (default 100M: 100000000)
  srate=<float>   Device sample rate (default 5000000). Valid values from 1M to 20M. In fact rates lower than 10M are not specified in the datasheets of the ADC chip however a rate of 1000000 (1M) still works well with SDRdaemon.
  ppmp=<float>    Argument is positive. Positive LO correction in ppm. LO is corrected by this value in ppm
  ppmn=<float>    Argument is positive. Negative LO correction in ppm. LO is corrected by minus this value in ppm. If ppmp is also specified ppmp takes precedence.
  lgain=<x>      (Rx only) LNA gain in dB. Valid values are: 0, 8, 16, 24, 32, 40, list. list lists valid values and exits. (default 16)
  vgain=<x>       VGA gain in dB. Valid values are: 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, list. list lists valid values and exits. (default 22)
  bwfilter=<x>    RF (IF) filter bandwidth in MHz. Actual value is taken as the closest to the following values: 1.75, 2.5, 3.5, 5, 5.5, 6, 7, 8, 9, 10, 12, 14, 15, 20, 24, 28, list. list lists valid values and exits. (default 2.5)
  extamp=<int>    Turn on (1) or off (0) the extra amplifier (default 0: off)
  antbias=<int>   Turn on (1) or off (0) the antenna bias for remote LNA (default 0: off)
  pwidle=<float> (Tx only) Value in negative dB of I/Q constant carrier power when idle (default 0: silent)
*/

class EmitterSoapy : public Emitter{

private :
    SoapySDR::Device *sdr;
    SoapySDR::Stream *tx_stream;

public :
    EmitterSoapy( float s_fc, float s_fe);
    ~EmitterSoapy();

    void initialize();
    void start_engine();
    void stop_engine ();

    bool reception(vector<complex<float> >& cbuffer);

    void reset();

    void   set_freq(const double value);
    double get_freq( );

    void   set_sample_rate(const double value);
    double get_sample_rate( );

    void set_amp_enable(const bool value);
    bool get_amp_enable( );

    void     set_vga_gain(uint32_t value);
    uint32_t get_vga_gain( );

    void set_antenna_enable(const bool value);
    bool get_antenna_enable( );
};

#endif